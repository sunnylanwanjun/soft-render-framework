#include "RenderList.h"
#include "Mat4.h"
#include "Object3D.h"
#include "Renderer.h"
#include "Director.h"
#include "Camera.h"
#include <algorithm>
NS_ENGIN_BEGIN

RenderList::RenderList():state(0),attr(0),numPoly(0),sortType(BY_AVG_Z){
	memset(polyList,0,RENDER_MAX_POLY*4);
}

RenderList::~RenderList(){

}

RenderList* RenderList::create(){
	RenderList* renderList = new RenderList;
	renderList->autoRelease();
	return renderList;
}

void RenderList::updateLocalToWorldTrans(const Mat4& transMat){
	for(int i=0;i<numPoly;i++){
		Polygon3D_Vec* poly=polyList[i];
		for(int j=0;j<3;j++){
			Mat4_Mul(poly->lVecList[j],transMat,poly->gVecList[j]);
		}
		poly->updateNormVec();
	}
}

bool sort_by_avgz(Polygon3D_Vec* p0,Polygon3D_Vec* p1){
	if(p0==nullptr||p1==nullptr)
		return true;
	float avgz0=(p0->gVecList[0].z+p0->gVecList[1].z+p0->gVecList[2].z)*0.3333f;
	float avgz1=(p1->gVecList[0].z+p1->gVecList[1].z+p1->gVecList[2].z)*0.3333f;
	if(avgz0>avgz1)
		return true;
	return false;
}

bool sort_by_maxz(Polygon3D_Vec* p0,Polygon3D_Vec* p1){
	if(p0==nullptr||p1==nullptr)
		return true;
	float avgz0=MAX(p0->gVecList[0].z,p0->gVecList[1].z);
	avgz0=MAX(avgz0,p0->gVecList[2].z);
	float avgz1=MAX(p1->gVecList[0].z,p1->gVecList[1].z);
	avgz1=MAX(avgz0,p1->gVecList[2].z);
	if(avgz0>avgz1)
		return true;
	return false;
}

bool sort_by_minz(Polygon3D_Vec* p0,Polygon3D_Vec* p1){
	if(p0==nullptr||p1==nullptr)
		return true;
	float avgz0=MIN(p0->gVecList[0].z,p0->gVecList[1].z);
	avgz0=MIN(avgz0,p0->gVecList[2].z);
	float avgz1=MIN(p1->gVecList[0].z,p1->gVecList[1].z);
	avgz1=MIN(avgz0,p1->gVecList[2].z);
	if(avgz0>avgz1)
		return true;
	return false;
}

void RenderList::sortPolyList(){
	if(numPoly<=1)
		return;
	switch(sortType){
	case BY_AVG_Z:
		std::sort(&polyList[0],&polyList[numPoly-1],sort_by_avgz);
		break;
	case BY_MAX_Z:
		std::sort(&polyList[0],&polyList[numPoly-1],sort_by_maxz);
		break;
	case BY_MIN_Z:
		std::sort(&polyList[0],&polyList[numPoly-1],sort_by_minz);
		break;
	}
}

void RenderList::draw( Renderer* renderer, Camera* cam ){
	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const RangeFIXP16& wndRange_FIX = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin_FIX = Director::getInstance()->getFIXP16WindowOrigin();
	const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
	LightMgr* lightMgr=LightMgr::getInstance();

	for(int i=0;i<numPoly;i++){
		Polygon3D_Vec* poly=polyList[i];
		if(!POLY_NEED_DRAW(poly))
			continue;

		if(!(poly->attr&POLY_ATTR_SIDE_2)){
			if(poly->getPointInPlane(cam->pos.v3f())==PIP_OUT){
				SETBIT(poly->state,POLY_STATE_BACKFACE);
			}
		}

		for(int j=0;j<3;j++){
			Mat4_Mul(poly->gVecList[j],cam->mcam,poly->gVecList[j]);
		}
	}

	sortPolyList();
	for(int i=0;i<numPoly;i++){
		Polygon3D_Vec* poly=polyList[i];
		if(!POLY_NEED_DRAW(poly))
			continue;

		//渲染列表的优点在于在进行变换时，可以过滤背面消除的多边形
		//这个ObjNode3D做不到，因为ObjNode3D是对整个物体的点进行变换
		//所以不能对面进行过滤，当然这样渲染列表的代价是浪费一些内存
		//执行光照计算
		if(_lightOn){
			computeLight(lightMgr,cam,*poly);
		}

		static Vec4f vList[3];
		bool needDraw=true;
		for(int j=0;j<3;j++){
			Mat4_Mul(poly->gVecList[j],cam->mper,vList[j]);
			vList[j].homogeneous();
			Mat4_Mul(vList[j],cam->mscr,vList[j]);
			if(FIXP16_OVER(vList[j].x+wndOrigin.x)||FIXP16_OVER(vList[j].y+wndOrigin.y)){
				needDraw=false;
				break;
			}
		}
		if(!needDraw)
			continue;
		if(_lightOn){
			DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
				wndOrigin_FIX, wndRange_FIX, poly->shadeColor, 
				vList[0].v2f(), 
				vList[1].v2f(),
				vList[2].v2f(),
				_wireMode);
		}else{
			DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
				wndOrigin_FIX, wndRange_FIX, poly->color, 
				vList[0].v2f(), 
				vList[1].v2f(),
				vList[2].v2f(),
				_wireMode);
		}
	}

	resetState();
	lpdds->Unlock(nullptr);
}

void RenderList::insertObj(ObjNode3D& obj){
	if( (obj.state&OBJ_STATE_CULLED)||
		!(obj.state&OBJ_STATE_ACTIVE)||
		!(obj.state&OBJ_STATE_VISIBLE))
		return;
	for(int i=0;i<obj.numPoly;i++){
		insertPoly(obj.polyList[i]);
	}
}

void RenderList::insertPoly(Polygon3D_Vec& poly){
	polyData[numPoly]=poly;
	Polygon3D_Vec& tempPoly=polyData[numPoly];
	polyList[numPoly]=&tempPoly;
	tempPoly.pNext=nullptr;
	if(numPoly>0){
		tempPoly.pPre=&polyData[numPoly-1];
	}else{
		tempPoly.pPre=nullptr;
	}
	numPoly++;
}

void RenderList::insertPoly(Polygon3D_VecIdx& polyVecIdx){
	Polygon3D_Vec poly(polyVecIdx);
	insertPoly(poly);
}

void RenderList::resetState(){
	//物体的面消除，裁剪属性
	for(int i=0;i<numPoly;i++){
		Polygon3D_Vec* poly=polyList[i];
		//不可见的多边形，不需要重置属性
		if(poly==nullptr||!(poly->state&POLY_STATE_ACTIVE))
			continue;
		RESETBIT(poly->state,POLY_STATE_CLIPPED);
		RESETBIT(poly->state,POLY_STATE_BACKFACE);
	}
}

void RenderList::computeLight( LightMgr* lightMgr,Camera* cam,Polygon3D_Vec& poly){
	if(lightMgr==nullptr||cam==nullptr)return;
	if(!POLY_NEED_DRAW((&poly)))return;
	static Vec3f lightV;
	//不使用结构体，是因为，只在结尾的时候，作溢出判断，这样减少判断次数
	int ori_r=RGB32BIT_R(poly.color);
	int ori_g=RGB32BIT_G(poly.color);
	int ori_b=RGB32BIT_B(poly.color);
	int sum_r=0;
	int sum_g=0;
	int sum_b=0;
	
	if(!(poly.attr&POLY_ATTR_SHADE_FLAT)&&!(poly.attr&POLY_ATTR_SHADE_GOURAUD)){
		//自发光的物体,也就是不进行光照计算的物体,固定着色
		poly.shadeColor=poly.color;
		return;
	}

	//遍历光源
	for(int i=0;i<lightMgr->num_lights;i++){
		Light& light=lightMgr->lights[i];
		if(!(light.state&LIGHT_STATE_ON))
			continue;
		//没方向，不衰减 (环境光，经过多重反射，折射后的，无法确定光源方向的光)
		if(light.attr&LIGHT_ATTR_AMBIENT){
			sum_r+=(light.cAmbient.r*ori_r>>8);
			sum_g+=(light.cAmbient.g*ori_g>>8);
			sum_b+=(light.cAmbient.b*ori_b>>8);
		}//有方向，不衰减(太阳光)
		else if(light.attr&LIGHT_ATTR_DIRECTIONAL){
			//法线指向是正面，如果法线与方向光同向，肯定是看不见的
			//所以应反向
			float dot=-Vec3f_Mul(light.dir.v3f(),poly.n);
			if(dot>0){
				sum_r+=(int(light.cDiffuse.r*ori_r*dot)>>8);
				sum_g+=(int(light.cDiffuse.g*ori_g*dot)>>8);
				sum_b+=(int(light.cDiffuse.b*ori_b*dot)>>8);
			}
		}//有方向，会衰减(灯泡)
		else if(light.attr&LIGHT_ATTR_POINT){
			//plg文件，相临的面，0是共点？用1，会造成光照在远处时，同一
			//平面的两个三角形，颜色有细微的差别
			Vec4f_Sub(light.pos,poly.gVecList[0],lightV);
			float dot=Vec3f_Mul(lightV,poly.n);
			if(dot>0){
				float dis=lightV.getModel();
				float decline=light.kc+light.kl*dis+light.kq*dis*dis;
				//再除以dis是因为lightV没有单位化，节省一次求模运算
				dot/=decline*dis;
				sum_r+=(int(light.cDiffuse.r*ori_r*dot)>>8);
				sum_g+=(int(light.cDiffuse.g*ori_g*dot)>>8);
				sum_b+=(int(light.cDiffuse.b*ori_b*dot)>>8);
			}
		}//有方向，会衰减，有范围(手电筒)
		else if((light.attr&LIGHT_ATTR_SPOT1)||(light.attr&LIGHT_ATTR_SPOT2)){
			Vec4f_Sub(light.pos,poly.gVecList[0],lightV);
			float dot=Vec3f_Mul(lightV,poly.n);
			if(dot>0){
				float dis=lightV.getModelFast();
				float decline=light.kc+light.kl*dis+light.kq*dis*dis;
				float cosVal=-Vec3f_Mul(lightV,light.dir.v3f());
				if(cosVal>0){
					cosVal=pow(cosVal,light.pf);
					dot*=cosVal/(decline*dis*dis);
					sum_r+=(int(light.cDiffuse.r*ori_r*dot)>>8);
					sum_g+=(int(light.cDiffuse.g*ori_g*dot)>>8);
					sum_b+=(int(light.cDiffuse.b*ori_b*dot)>>8);
				}					
			}
		}
	}
	poly.shadeColor=RGB32BIT(0,LIMIT(sum_r,0xff),
							  LIMIT(sum_g,0xff),
							  LIMIT(sum_b,0xff));
}

NS_ENGIN_END