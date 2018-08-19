#include "Object3D.h"
#include "Mat4.h"
#include "PlgReader.h"
#include "Renderer.h"
#include "Director.h"
#include "Camera.h"
#include "ModelReader.h"
NS_ENGIN_BEGIN
ObjNode3D::ObjNode3D():
	id(0),
	state(0),
	attr(0),
	avgRadius(0.0f),
	maxRadius(0.0f),
	numVec(0),
	numPoly(0){
		memset(name,0,64);
}

ObjNode3D::~ObjNode3D(){

}

ObjNode3D* ObjNode3D::createWithPlgfile( const std::string& fileName ,const Vec3f&vs/*=NORMVEC*/){
	ObjNode3D* obj = new ObjNode3D;
	if(LoadPlg(obj,fileName.c_str(),vs)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

ObjNode3D* ObjNode3D::createWithAscfile(const std::string& fileName,const Vec3f&vs,int mode){
	ObjNode3D* obj = new ObjNode3D;
	if(LoadAsc(obj,fileName.c_str(),vs,mode)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

ObjNode3D* ObjNode3D::createWithCobfile(const std::string& fileName,const Vec3f&vs,int mode){
	ObjNode3D* obj = new ObjNode3D;
	if(LoadCob(obj,fileName.c_str(),vs,mode)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

void ObjNode3D::setScale( Vec3f& vs ){
	Node3D::setScale(vs);
	computeObjRadius();
}

void ObjNode3D::computeObjRadius(){
	avgRadius=0;
	maxRadius=0;
	for(int i=0;i<numVec;i++){
		Vec3f v=localVecList[i].v3f();
		v.x*=_scale.x;
		v.y*=_scale.y;
		v.z*=_scale.z;
		float vModel=v.getModel();
		if(maxRadius<vModel)
			maxRadius=vModel;
		avgRadius+=vModel;
	}
	avgRadius/=numVec;
}

/*
由于每一帧都会因为相机变换，等使得原本被裁剪，背面消除，踢除的物体变得
不被裁剪，不被消除，不被踢除，所以需要重置，这些是非代码业务逻辑使然的，
所以需要重置，但如隐藏，这种是编程人员有意使然的，就不能重置。
*/
void ObjNode3D::resetState(){
	//物体剔除属性
	RESETBIT(state,OBJ_STATE_CULLED);
	//物体的面消除，裁剪属性
	for(int i=0;i<numPoly;i++){
		Polygon3D_VecIdx& poly=polyList[i];
		//不可见的多边形，不需要重置属性
		if(!(poly.state&POLY_STATE_ACTIVE))
			continue;
		RESETBIT(poly.state,POLY_STATE_CLIPPED);
		RESETBIT(poly.state,POLY_STATE_BACKFACE);
	}
}

void ObjNode3D::updateLocalToWorldTrans(const Mat4& transMat){
	for(int i=0;i<numVec;i++){
		Mat4_Mul(localVecList[i],transMat,worldVecList[i]);
	}
	for(int i=0;i<numPoly;i++){
		Polygon3D_VecIdx& poly=polyList[i];
		poly.updateNormVec();
	}
}

void ObjNode3D::draw(Renderer* renderer, Camera* cam){
	
	cullObj(cam);
	if((state&OBJ_STATE_CULLED)||
		!(state&OBJ_STATE_ACTIVE)||
		!(state&OBJ_STATE_VISIBLE)){
			resetState();
			return;
	}

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const RangeFIXP16& wndRange_FIX = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin_FIX = Director::getInstance()->getFIXP16WindowOrigin();

	removeBackface(cam);
	if(_lightOn){
		computeLight(LightMgr::getInstance(),cam);
	}

	for(int i=0;i<numVec;i++){
		Mat4_Mul(worldVecList[i],cam->mcam,screenVecList[i]);
		Mat4_Mul(screenVecList[i],cam->mper,screenVecList[i]);
		screenVecList[i].homogeneous();
		Mat4_Mul(screenVecList[i],cam->mscr,screenVecList[i]);
	}

	for(int i=0;i<numPoly;i++){
		Polygon3D_VecIdx& poly=polyList[i];
		if(POLY_NEED_DRAW((&poly))){
			//这里使用screenVecList不使用poly中存储的顶列列表指针，
			//那个指针是指向局部顶点列表的，是为了在RenderList可以存
			//储ObjNode3D，如果指向screenVecList，那么Renderlist进行
			//坐标变换的时候会出错
			if(_lightOn){
				DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
					wndOrigin_FIX, wndRange_FIX, poly.shadeColor, 
					screenVecList[poly.verIndex[0]].v2f(), 
					screenVecList[poly.verIndex[1]].v2f(),
					screenVecList[poly.verIndex[2]].v2f(),
					_wireMode);
			}else{
				DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
					wndOrigin_FIX, wndRange_FIX, poly.color, 
					screenVecList[poly.verIndex[0]].v2f(), 
					screenVecList[poly.verIndex[1]].v2f(),
					screenVecList[poly.verIndex[2]].v2f(),
					_wireMode);
			}
		}
	}

	resetState();
	lpdds->Unlock(nullptr);
}

void ObjNode3D::cullObj( Camera* cam,int cullType/*=CULL_ALL*/ ){
	Vec3f center;
	Mat4_Mul(_pos,cam->mcam,center);
	if(cullType&CULL_Z){
		if(center.z-maxRadius>=cam->fc||
			center.z+maxRadius<=cam->nc){
				SETBIT(state,OBJ_STATE_CULLED);
		}
	}
	Vec3f check;
	if(cullType&CULL_X){
		check=center;
		check.x=center.x+maxRadius;
		if(GetPointInPlane(cam->rc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
		check.x=center.x-maxRadius;
		if(GetPointInPlane(cam->lc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
	}
	if(cullType&CULL_Y){
		check=center;
		check.y=center.y+maxRadius;
		if(GetPointInPlane(cam->bc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
		check.y=center.y-maxRadius;
		if(GetPointInPlane(cam->tc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
	}
}

void ObjNode3D::removeBackface( Camera* cam ){
	for(int i=0;i<numPoly;i++){
		if(state&OBJ_STATE_CULLED)
			continue;
		Polygon3D_VecIdx& poly=polyList[i];
		//双面，未激活，背面，裁剪的都不需要进行背面检测
		if((poly.attr&POLY_ATTR_SIDE_2)||
			!(poly.state&POLY_STATE_ACTIVE)||
			(poly.state&POLY_STATE_BACKFACE)||
			(poly.state&POLY_STATE_CLIPPED))
			continue;
		if(poly.getPointInPlane(cam->pos.v3f())==PIP_OUT){
			SETBIT(poly.state,POLY_STATE_BACKFACE);
		}
	}
}

void ObjNode3D::computeLight( LightMgr* lightMgr,Camera* cam ){
	if(lightMgr==nullptr||cam==nullptr)return;
	if((state&OBJ_STATE_CULLED)||
	   !(state&OBJ_STATE_ACTIVE)||
	   !(state&OBJ_STATE_VISIBLE)){
		return;
	}
	static Vec3f lightV;
	//不使用结构体，是因为，只在结尾的时候，作溢出判断，这样减少判断次数
	int sum_r;
	int sum_g;
	int sum_b;
	int ori_r;
	int ori_g;
	int ori_b;
	for(int i=0;i<numPoly;i++){
		Polygon3D_VecIdx& poly=polyList[i];
		ori_r=RGB32BIT_R(poly.color);
		ori_g=RGB32BIT_G(poly.color);
		ori_b=RGB32BIT_B(poly.color);
		sum_r=0;
		sum_g=0;
		sum_b=0;
		if(!POLY_NEED_DRAW((&poly)))continue;
		if(!(poly.attr&POLY_ATTR_SHADE_FLAT)&&!(poly.attr&POLY_ATTR_SHADE_GOURAUD)){
			//自发光的物体,也就是不进行光照计算的物体
			poly.shadeColor=poly.color;
			continue;
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
				Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]],lightV);
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
				Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]],lightV);
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
}

NS_ENGIN_END