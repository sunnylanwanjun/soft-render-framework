#include "MaterialNode3D.h"
#include "Mat4.h"
#include "Renderer.h"
#include "Director.h"
#include "Camera.h"
#include "ModelReader.h"
#include "PlgReader.h"
NS_ENGIN_BEGIN
MaterialNode3D::MaterialNode3D()
:id(0),
state(0),
attr(0),
avgRadiuses(nullptr),
maxRadiuses(nullptr),
numVec(0),
totalVec(0),
localVecList(nullptr),
worldVecList(nullptr),
screenVecList(nullptr),
curLocalVecList(nullptr),
curWorldVecList(nullptr),
numFrame(0),
curFrame(0),
texList(nullptr),
texture(nullptr),
numPoly(0),
polyList(nullptr)
{

}

MaterialNode3D::~MaterialNode3D(){
	delete[] localVecList;
	delete[] worldVecList;
	delete[] screenVecList;
	delete[] texList;
	delete[] avgRadiuses;
	delete[] maxRadiuses;
	delete[] polyList;
	delete   texture;
}

MaterialNode3D* MaterialNode3D::createWithPlgfile(const std::string& fileName ,
												  const Vec3f&vs/*=NORMVEC*/){
	MaterialNode3D* obj = new MaterialNode3D;
	if(LoadPlg(obj,fileName.c_str(),vs)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

MaterialNode3D* MaterialNode3D::createWithAscfile(const std::string& fileName,
												  const Vec3f&vs,
												  int mode){
	MaterialNode3D* obj = new MaterialNode3D;
	if(LoadAsc(obj,fileName.c_str(),vs,mode)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

MaterialNode3D* MaterialNode3D::createWithCobfile(const std::string& fileName,
												  const Vec3f&vs,
												  int mode){
	MaterialNode3D* obj = new MaterialNode3D;
	if(LoadCob(obj,fileName.c_str(),vs,mode)){
		obj->autoRelease();
		return obj;
	}else{
		delete obj;
		return nullptr;
	}
}

void MaterialNode3D::destroyObj(MaterialNode3D* obj){
	delete[] obj->avgRadiuses;
	delete[] obj->maxRadiuses;
	delete[] obj->texList;
	delete   obj->texture;
	delete[] obj->worldVecList;
	delete[] obj->screenVecList;
	delete[] obj->localVecList;
	delete[] obj->polyList;
}

//不使用共用点,不拷贝局部坐标
void MaterialNode3D::copyObj_World(MaterialNode3D* dst,const MaterialNode3D* src){
	
	dst->attr=src->attr;
	dst->id=src->id;
	strcpy(dst->name,src->name);
	dst->state=OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	dst->numVec=src->numPoly*3;
	dst->numPoly=src->numPoly;
	dst->numFrame=src->numFrame;
	dst->totalVec=dst->numVec*dst->numFrame;
	dst->frameSize=dst->numVec*sizeof Vec4f;

	//与源相同的部分，简单内存拷贝即可
	dst->avgRadiuses=new float[dst->numFrame];
	memcpy(dst->avgRadiuses,src->avgRadiuses,dst->numFrame*sizeof(float));
	dst->maxRadiuses=new float[dst->numFrame];
	memcpy(dst->maxRadiuses,src->maxRadiuses,dst->numFrame*sizeof(float));
	dst->texList = new Vec2f[dst->numVec];
	memcpy(dst->texList,src->texList,dst->numVec*sizeof(Vec2f));
	if(src->texture){
		dst->texture = new BITMAP(src->texture);
	}else{
		dst->texture;
	}
	//不需要使用局部坐标点
	dst->curLocalVecList=dst->localVecList=nullptr;
	dst->screenVecList=new Vec4f[dst->numVec];

	dst->curWorldVecList=dst->worldVecList=new FVec4f[dst->totalVec];
	dst->polyList = new MatPoly_VecIdx[dst->numPoly];
	memcpy(dst->polyList,src->polyList,dst->numPoly*sizeof(MatPoly_VecIdx));
	int j=0;
	for(int i=0;i<dst->numPoly;i++){
		MatPoly_VecIdx *poly=dst->polyList+i;
		poly->attr=src->polyList[i].attr;
		poly->state=src->polyList[i].state;
		poly->pTexList=dst->texList;
		poly->pTexture=dst->texture;
		poly->pVecList=dst->worldVecList;
		poly->verIndex[0]=j;
		poly->verIndex[1]=j+1;
		poly->verIndex[2]=j+2;
		dst->worldVecList[j]=src->worldVecList[src->polyList[i].verIndex[0]];
		dst->worldVecList[j+1]=src->worldVecList[src->polyList[i].verIndex[1]];
		dst->worldVecList[j+2]=src->worldVecList[src->polyList[i].verIndex[2]];
		j+=3;
	}
}

void MaterialNode3D::init(int _num_vertices, 
						  int _num_polys, 
						  int _num_frames)
{
	numVec=_num_vertices;
	numPoly=_num_polys;
	numFrame=_num_frames;
	totalVec=numVec*numFrame;
	frameSize=numVec*sizeof Vec4f;
	curLocalVecList=localVecList=new FVec4f[totalVec];
	curWorldVecList=worldVecList=new FVec4f[totalVec];
	screenVecList=new Vec4f[numVec];
	avgRadiuses=new float[numFrame];
	memset(avgRadiuses,0,sizeof(numFrame*4));
	maxRadiuses=new float[numFrame];
	memset(maxRadiuses,0,sizeof(numFrame*4));
	texList = new Vec2f[numPoly*3];
	polyList = new MatPoly_VecIdx[numPoly];
}

void MaterialNode3D::setFrame(int frame){
	curFrame=frame;
	if(curFrame<0)
		curFrame=0;
	else if(curFrame>numFrame)
		curFrame=numFrame-1;
	int perFrameSize=curFrame*numVec;
	curLocalVecList=localVecList+perFrameSize;
	curWorldVecList=worldVecList+perFrameSize;
	computePolyNorm();
	computeVecNorm();
}

void MaterialNode3D::computePolyNorm(){
	//更新法向量
	for(int i=0;i<numPoly;i++){
		MatPoly_VecIdx& poly=polyList[i];
		poly.updateNormVec();
	}
}

void MaterialNode3D::computeVecNorm(){
	static int vecUseTime[OBJ_MAX_VEC];
	memset(vecUseTime, 0, sizeof(int)*OBJ_MAX_VEC); 
	bool hasGouraud=false;
	for(int i=0;i<numPoly;i++){
		MatPoly_VecIdx& poly=polyList[i];
		if(poly.attr&POLY_ATTR_SHADE_GOURAUD){
			hasGouraud=true;
			vecUseTime[poly.verIndex[0]]++;
			vecUseTime[poly.verIndex[1]]++;
			vecUseTime[poly.verIndex[2]]++;
			Vec3f_Add(poly.pVecList[poly.verIndex[0]].n3f(),
				poly.n,poly.pVecList[poly.verIndex[0]].n3f());
			Vec3f_Add(poly.pVecList[poly.verIndex[1]].n3f(),
				poly.n,poly.pVecList[poly.verIndex[1]].n3f());
			Vec3f_Add(poly.pVecList[poly.verIndex[2]].n3f(),
				poly.n,poly.pVecList[poly.verIndex[2]].n3f());
		}
	}
	//没有gouraud着色的多边形，直接返回
	if(!hasGouraud)
		return;
	for(int i=0;i<numVec;i++){
		if(vecUseTime[i]>1){
			Vec3f_Mul(curWorldVecList[i].n3f(),1.0f/vecUseTime[i],
				curWorldVecList[i].n3f());
			curWorldVecList[i].n3f().identify();
		}
	}
}

void MaterialNode3D::setScale( Vec3f& vs ){
	Node3D::setScale(vs);
	computeObjRadius();
}

void MaterialNode3D::computeObjRadius(){
	float _1_numVec=1.0f/numVec;
	FVec4f* head=localVecList;
	for(int f=0;f<numFrame;f++){
		for(int i=0;i<numVec;i++){
			Vec3f v=head[i].v3f();
			v.x*=_scale.x;
			v.y*=_scale.y;
			v.z*=_scale.z;
			float vModel=v.getModel();
			if(maxRadiuses[f]<vModel)
				maxRadiuses[f]=vModel;
			avgRadiuses[f]+=vModel;
		}
		head+=frameSize;
		avgRadiuses[f]*=_1_numVec;
	}
}

void MaterialNode3D::resetState(){
	//物体剔除属性
	RESETBIT(state,OBJ_STATE_CULLED);
	//物体的面消除，裁剪属性
	for(int i=0;i<numPoly;i++){
		MatPoly_VecIdx& poly=polyList[i];
		//不可见的多边形，不需要重置属性
		if(!(poly.state&POLY_STATE_ACTIVE))
			continue;
		RESETBIT(poly.state,POLY_STATE_CLIPPED);
		RESETBIT(poly.state,POLY_STATE_BACKFACE);
	}
}

void MaterialNode3D::updateLocalToWorldTrans(const Mat4& transMat){
	FVec4f* headlocal=localVecList;
	FVec4f* headWorld=worldVecList;
	for(int f=0;f<numFrame;f++){
		for(int i=0;i<numVec;i++){
			Mat4_Mul(headlocal[i].v4f(),transMat,headWorld[i].v4f());
			//顶点法线归零
			headWorld[i].n3f().zero();
		}
		headlocal+=frameSize;
		headWorld+=frameSize;
	}
	computePolyNorm();
	computeVecNorm();
}

void MaterialNode3D::draw(Renderer* renderer, Camera* cam){

	cullObj(cam);
	if(state&OBJ_STATE_CULLED){
		resetState();
		return;
	}

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const Range& wndRange = Director::getInstance()->getClipRange();
	const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
	const RangeFIXP16& wndRange_FIX = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin_FIX = Director::getInstance()->getFIXP16WindowOrigin();

	removeBackface(cam);
	if(_lightOn){
		computeLight(LightMgr::getInstance(),cam);
	}

	for(int i=0;i<numVec;i++){
		Mat4_Mul(curWorldVecList[i].v4f(),cam->mcam,screenVecList[i]);
		Mat4_Mul(screenVecList[i],cam->mper,screenVecList[i]);
		screenVecList[i].homogeneous();
		Mat4_Mul(screenVecList[i],cam->mscr,screenVecList[i]);
	}

	for(int i=0;i<numPoly;i++){
		MatPoly_VecIdx& poly=polyList[i];
		if(POLY_NEED_DRAW((&poly))){
			//这里使用screenVecList不使用poly中存储的顶列列表指针，
			//那个指针是指向局部顶点列表的，是为了在RenderList可以存
			//储ObjNode3D，如果指向screenVecList，那么Renderlist进行
			//坐标变换的时候会出错

			if(_lightOn){
				if(poly.attr&POLY_ATTR_SHADE_TEXTURE){
					DrawTextureTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
						wndOrigin_FIX, wndRange_FIX,
						screenVecList[poly.verIndex[0]].v2f(), 
						screenVecList[poly.verIndex[1]].v2f(),
						screenVecList[poly.verIndex[2]].v2f(),
						texList[poly.texIndex[0]],
						texList[poly.texIndex[1]],
						texList[poly.texIndex[2]],
						RGB32BIT_R(poly.shadeColor[0])*0.003922f,
						RGB32BIT_G(poly.shadeColor[0])*0.003922f,
						RGB32BIT_B(poly.shadeColor[0])*0.003922f,
						texture);
				}
				else if(poly.attr&POLY_ATTR_SHADE_CONSTANT||poly.attr&POLY_ATTR_SHADE_FLAT){
					DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
						wndOrigin_FIX, wndRange_FIX, poly.shadeColor[0], 
						screenVecList[poly.verIndex[0]].v2f(), 
						screenVecList[poly.verIndex[1]].v2f(),
						screenVecList[poly.verIndex[2]].v2f(),
						_wireMode);
				}else if(poly.attr&POLY_ATTR_SHADE_GOURAUD){
					DrawGouraudTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
						wndOrigin_FIX, wndRange_FIX, 
						screenVecList[poly.verIndex[0]].v2f(), 
						screenVecList[poly.verIndex[1]].v2f(),
						screenVecList[poly.verIndex[2]].v2f(),
						poly.shadeColor[0], 
						poly.shadeColor[1],
						poly.shadeColor[2], 
						_wireMode);
				}
			}else{
				if(poly.attr&POLY_ATTR_SHADE_TEXTURE){
					DrawTextureTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
						wndOrigin_FIX, wndRange_FIX,
						screenVecList[poly.verIndex[0]].v2f(), 
						screenVecList[poly.verIndex[1]].v2f(),
						screenVecList[poly.verIndex[2]].v2f(),
						texList[poly.texIndex[0]],
						texList[poly.texIndex[1]],
						texList[poly.texIndex[2]],
						1.0f,
						1.0f,
						1.0f,
						texture);
				}
				else {
					DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, 
						wndOrigin_FIX, wndRange_FIX, poly.color, 
						screenVecList[poly.verIndex[0]].v2f(), 
						screenVecList[poly.verIndex[1]].v2f(),
						screenVecList[poly.verIndex[2]].v2f(),
						_wireMode);
				}
			}
		}
	}

	resetState();
	lpdds->Unlock(nullptr);
}

void MaterialNode3D::cullObj( Camera* cam,int cullType/*=CULL_ALL*/ ){
	Vec3f center;
	Mat4_Mul(_pos,cam->mcam,center);
	if(cullType&CULL_Z){
		if(center.z-maxRadiuses[curFrame]>=cam->fc||
			center.z+maxRadiuses[curFrame]<=cam->nc){
				SETBIT(state,OBJ_STATE_CULLED);
		}
	}
	Vec3f check;
	if(cullType&CULL_X){
		check=center;
		check.x=center.x+maxRadiuses[curFrame];
		if(GetPointInPlane(cam->rc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
		check.x=center.x-maxRadiuses[curFrame];
		if(GetPointInPlane(cam->lc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
	}
	if(cullType&CULL_Y){
		check=center;
		check.y=center.y+maxRadiuses[curFrame];
		if(GetPointInPlane(cam->bc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
		check.y=center.y-maxRadiuses[curFrame];
		if(GetPointInPlane(cam->tc,check)==PIP_OUT)
			SETBIT(state,OBJ_STATE_CULLED);
	}
}

void MaterialNode3D::removeBackface( Camera* cam ){
	for(int i=0;i<numPoly;i++){
		if(state&OBJ_STATE_CULLED)
			continue;
		MatPoly_VecIdx& poly=polyList[i];
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

void MaterialNode3D::computeLight( LightMgr* lightMgr,Camera* cam ){
	
	if(lightMgr==nullptr||cam==nullptr)return;
	if((state&OBJ_STATE_CULLED)||
		!(state&OBJ_STATE_ACTIVE)||
		!(state&OBJ_STATE_VISIBLE)){
			return;
	}
	static Vec3f lightV;
	//不使用结构体，是因为，只在结尾的时候，作溢出判断，这样减少判断次数
	int sum_r[3];
	int sum_g[3];
	int sum_b[3];
	int ori_r;		//原始颜色
	int ori_g;		//原始颜色
	int ori_b;		//原始颜色
	int Lr;			//光强
	int Lg;			//光强
	int Lb;			//光强

	for(int i=0;i<numPoly;i++){
		MatPoly_VecIdx& poly=polyList[i];
		ori_r=RGB32BIT_R(poly.color);
		ori_g=RGB32BIT_G(poly.color);
		ori_b=RGB32BIT_B(poly.color);
		memset(sum_r,0,sizeof sum_r);
		memset(sum_g,0,sizeof sum_g);
		memset(sum_b,0,sizeof sum_b);
		if(!POLY_NEED_DRAW((&poly)))continue;
		if(poly.attr&POLY_ATTR_SHADE_CONSTANT){
			//自发光的物体,也就是不进行光照计算的物体
			poly.shadeColor[0]=poly.color;
		} 
		//////////////////////////////////////////////////////////
		//恒定光照
		else if(poly.attr&POLY_ATTR_SHADE_FLAT||poly.attr&POLY_ATTR_SHADE_TEXTURE){
			for(int i=0;i<lightMgr->num_lights;i++){
				Light& light=lightMgr->lights[i];
				if(!(light.state&LIGHT_STATE_ON))
					continue;
				//没方向，不衰减 (环境光，经过多重反射，折射后的，无法确定光源方向的光)
				if(light.attr&LIGHT_ATTR_AMBIENT){
					sum_r[0]+=(light.cAmbient.r*ori_r>>8);
					sum_g[0]+=(light.cAmbient.g*ori_g>>8);
					sum_b[0]+=(light.cAmbient.b*ori_b>>8);
				}//有方向，不衰减(太阳光)
				else if(light.attr&LIGHT_ATTR_DIRECTIONAL){
					//法线指向是正面，如果法线与方向光同向，肯定是看不见的
					//所以应反向
					float dot=-Vec3f_Mul(light.dir.v3f(),poly.n);
					if(dot>0){
						dot/=poly.nLen;
						sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}
				}//有方向，会衰减(灯泡)
				else if(light.attr&LIGHT_ATTR_POINT){
					//plg文件，相临的面，0是共点？用1，会造成光照在远处时，同一
					//平面的两个三角形，颜色有细微的差别
					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]].v4f(),lightV);
					float dot=Vec3f_Mul(lightV,poly.n);
					if(dot>0){
						float dis=lightV.getModel();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						//再除以dis是因为lightV没有单位化，节省一次求模运算
						dot/=decline*dis*poly.nLen;
						sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}
				}//有方向，会衰减，有范围(手电筒)
				else if((light.attr&LIGHT_ATTR_SPOT1)||(light.attr&LIGHT_ATTR_SPOT2)){
					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]].v4f(),lightV);
					float dot=Vec3f_Mul(lightV,poly.n);
					if(dot>0){
						float dis=lightV.getModelFast();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						float cosVal=-Vec3f_Mul(lightV,light.dir.v3f());
						if(cosVal>0){
							cosVal=pow(cosVal,light.pf);
							dot*=cosVal/(decline*dis*dis*poly.nLen);
							sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
							sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
							sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
						}					
					}
				}
			}
			poly.shadeColor[0]=RGB32BIT(0,LIMIT(sum_r[0],0xff),
										  LIMIT(sum_g[0],0xff),
										  LIMIT(sum_b[0],0xff));
		}
		//////////////////////////////////////////////////////////
		//过渡光照
		else if(poly.attr&POLY_ATTR_SHADE_GOURAUD){
			for(int i=0;i<lightMgr->num_lights;i++){
				Light& light=lightMgr->lights[i];
				if(!(light.state&LIGHT_STATE_ON))
					continue;
				//没方向，不衰减 (环境光，经过多重反射，折射后的，无法确定光源方向的光)
				if(light.attr&LIGHT_ATTR_AMBIENT){
					Lr=(light.cAmbient.r*ori_r>>8);
					Lg=(light.cAmbient.g*ori_g>>8);
					Lb=(light.cAmbient.b*ori_b>>8);
					sum_r[0]+=Lr;
					sum_g[0]+=Lg;
					sum_b[0]+=Lb;
					sum_r[1]+=Lr;
					sum_g[1]+=Lg;
					sum_b[1]+=Lb;
					sum_r[2]+=Lr;
					sum_g[2]+=Lg;
					sum_b[2]+=Lb;
				}//有方向，不衰减(太阳光)
				else if(light.attr&LIGHT_ATTR_DIRECTIONAL){
					//法线指向是正面，如果法线与方向光同向，肯定是看不见的
					//所以应反向
					float dot=-Vec3f_Mul(light.dir.v3f(),poly.pVecList[0].n3f());
					if(dot>0){
						sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}
					dot=-Vec3f_Mul(light.dir.v3f(),poly.pVecList[1].n3f());
					if(dot>0){
						sum_r[1]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[1]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[1]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}
					dot=-Vec3f_Mul(light.dir.v3f(),poly.pVecList[2].n3f());
					if(dot>0){
						sum_r[2]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[2]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[2]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}
				}//有方向，会衰减(灯泡)
				else if(light.attr&LIGHT_ATTR_POINT){
					//plg文件，相临的面，0是共点？用1，会造成光照在远处时，同一
					//平面的两个三角形，颜色有细微的差别
					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]].v4f(),lightV);
					float dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[0]].n3f());
					if(dot>0){
						float dis=lightV.getModel();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						//再除以dis是因为lightV没有单位化，节省一次求模运算
						dot/=decline*dis;
						sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}

					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[1]].v4f(),lightV);
					dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[1]].n3f());
					if(dot>0){
						float dis=lightV.getModel();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						//再除以dis是因为lightV没有单位化，节省一次求模运算
						dot/=decline*dis;
						sum_r[1]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[1]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[1]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}

					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[2]].v4f(),lightV);
					dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[2]].n3f());
					if(dot>0){
						float dis=lightV.getModel();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						//再除以dis是因为lightV没有单位化，节省一次求模运算
						dot/=decline*dis;
						sum_r[2]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
						sum_g[2]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
						sum_b[2]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
					}

				}//有方向，会衰减，有范围(手电筒)
				else if((light.attr&LIGHT_ATTR_SPOT1)||(light.attr&LIGHT_ATTR_SPOT2)){
					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]].v4f(),lightV);
					float dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[0]].n3f());
					if(dot>0){
						float dis=lightV.getModelFast();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						float cosVal=-Vec3f_Mul(lightV,light.dir.v3f());
						if(cosVal>0){
							cosVal=pow(cosVal,light.pf);
							dot*=cosVal/(decline*dis*dis);
							sum_r[0]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
							sum_g[0]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
							sum_b[0]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
						}					
					}

					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[1]].v4f(),lightV);
					dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[1]].n3f());
					if(dot>0){
						float dis=lightV.getModelFast();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						float cosVal=-Vec3f_Mul(lightV,light.dir.v3f());
						if(cosVal>0){
							cosVal=pow(cosVal,light.pf);
							dot*=cosVal/(decline*dis*dis);
							sum_r[1]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
							sum_g[1]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
							sum_b[1]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
						}					
					}

					Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[2]].v4f(),lightV);
					dot=Vec3f_Mul(lightV,poly.pVecList[poly.verIndex[2]].n3f());
					if(dot>0){
						float dis=lightV.getModelFast();
						float decline=light.kc+light.kl*dis+light.kq*dis*dis;
						float cosVal=-Vec3f_Mul(lightV,light.dir.v3f());
						if(cosVal>0){
							cosVal=pow(cosVal,light.pf);
							dot*=cosVal/(decline*dis*dis);
							sum_r[2]+=(int(light.cDiffuse.r*ori_r*dot)>>8);
							sum_g[2]+=(int(light.cDiffuse.g*ori_g*dot)>>8);
							sum_b[2]+=(int(light.cDiffuse.b*ori_b*dot)>>8);
						}					
					}

				}
			}
			poly.shadeColor[0]=RGB32BIT(0,LIMIT(sum_r[0],0xff),
				LIMIT(sum_g[0],0xff),
				LIMIT(sum_b[0],0xff));
			poly.shadeColor[1]=RGB32BIT(0,LIMIT(sum_r[1],0xff),
				LIMIT(sum_g[1],0xff),
				LIMIT(sum_b[1],0xff));
			poly.shadeColor[2]=RGB32BIT(0,LIMIT(sum_r[2],0xff),
				LIMIT(sum_g[2],0xff),
				LIMIT(sum_b[2],0xff));
		}
	}
}

NS_ENGIN_END