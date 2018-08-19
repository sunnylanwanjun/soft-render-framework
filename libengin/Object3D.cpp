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
����ÿһ֡������Ϊ����任����ʹ��ԭ�����ü��������������߳���������
�����ü������������������߳���������Ҫ���ã���Щ�ǷǴ���ҵ���߼�ʹȻ�ģ�
������Ҫ���ã��������أ������Ǳ����Ա����ʹȻ�ģ��Ͳ������á�
*/
void ObjNode3D::resetState(){
	//�����޳�����
	RESETBIT(state,OBJ_STATE_CULLED);
	//��������������ü�����
	for(int i=0;i<numPoly;i++){
		Polygon3D_VecIdx& poly=polyList[i];
		//���ɼ��Ķ���Σ�����Ҫ��������
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
			//����ʹ��screenVecList��ʹ��poly�д洢�Ķ����б�ָ�룬
			//�Ǹ�ָ����ָ��ֲ������б�ģ���Ϊ����RenderList���Դ�
			//��ObjNode3D�����ָ��screenVecList����ôRenderlist����
			//����任��ʱ������
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
		//˫�棬δ������棬�ü��Ķ�����Ҫ���б�����
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
	//��ʹ�ýṹ�壬����Ϊ��ֻ�ڽ�β��ʱ��������жϣ����������жϴ���
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
			//�Է��������,Ҳ���ǲ����й��ռ��������
			poly.shadeColor=poly.color;
			continue;
		} 
		//������Դ
		for(int i=0;i<lightMgr->num_lights;i++){
			Light& light=lightMgr->lights[i];
			if(!(light.state&LIGHT_STATE_ON))
				continue;
			//û���򣬲�˥�� (�����⣬�������ط��䣬�����ģ��޷�ȷ����Դ����Ĺ�)
			if(light.attr&LIGHT_ATTR_AMBIENT){
				sum_r+=(light.cAmbient.r*ori_r>>8);
				sum_g+=(light.cAmbient.g*ori_g>>8);
				sum_b+=(light.cAmbient.b*ori_b>>8);
			}//�з��򣬲�˥��(̫����)
			else if(light.attr&LIGHT_ATTR_DIRECTIONAL){
				//����ָ�������棬��������뷽���ͬ�򣬿϶��ǿ�������
				//����Ӧ����
				float dot=-Vec3f_Mul(light.dir.v3f(),poly.n);
				if(dot>0){
					sum_r+=(int(light.cDiffuse.r*ori_r*dot)>>8);
					sum_g+=(int(light.cDiffuse.g*ori_g*dot)>>8);
					sum_b+=(int(light.cDiffuse.b*ori_b*dot)>>8);
				}
			}//�з��򣬻�˥��(����)
			else if(light.attr&LIGHT_ATTR_POINT){
				//plg�ļ������ٵ��棬0�ǹ��㣿��1������ɹ�����Զ��ʱ��ͬһ
				//ƽ������������Σ���ɫ��ϸ΢�Ĳ��
				Vec4f_Sub(light.pos,poly.pVecList[poly.verIndex[0]],lightV);
				float dot=Vec3f_Mul(lightV,poly.n);
				if(dot>0){
					float dis=lightV.getModel();
					float decline=light.kc+light.kl*dis+light.kq*dis*dis;
					//�ٳ���dis����ΪlightVû�е�λ������ʡһ����ģ����
					dot/=decline*dis;
					sum_r+=(int(light.cDiffuse.r*ori_r*dot)>>8);
					sum_g+=(int(light.cDiffuse.g*ori_g*dot)>>8);
					sum_b+=(int(light.cDiffuse.b*ori_b*dot)>>8);
				}
			}//�з��򣬻�˥�����з�Χ(�ֵ�Ͳ)
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