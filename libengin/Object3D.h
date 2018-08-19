#ifndef __OBJECT3D_H
#define __OBJECT3D_H
#include "macros.h"
#include "Vec.h"
#include "Ref.h"
#include <string>
#include "mathUtil.h"
#include <vector>
#include "Node3D.h"
#include "Light.h"
#include "ModelReader.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Polygon3D_VecIdx{
public:
	Polygon3D_VecIdx():state(0),color(0),attr(0),pVecList(0){
		memset(verIndex,0,12);
	}
	~Polygon3D_VecIdx(){}
	PointInPlane getPointInPlane(const Vec3f& p)const{
		return GetPointInPlane(n,pVecList[verIndex[0]].v3f(),p);
	}
	void updateNormVec(){
		GetNormalVec(pVecList[verIndex[0]].v3f(),pVecList[verIndex[1]].v3f(),
			pVecList[verIndex[2]].v3f(),n);
	}
public:
	int state;//״̬
	int color;//��ɫ
	int attr;//��������
	Vec4f* pVecList;//�����б�
	int verIndex[3];//�����б�����

	int shadeColor; //��ɫ��������ɫ
	Vec3f n;        //����
};

class ENGIN_DLL Polygon3D_Vec{
public:
	Polygon3D_Vec():state(0),color(0),attr(0),pPre(0),pNext(0),shadeColor(0){
	}
	Polygon3D_Vec(Polygon3D_VecIdx& poly){
		state=poly.state;
		color=poly.color;
		attr=poly.attr;
		shadeColor=poly.shadeColor;
		n=poly.n;
		pPre=pNext=nullptr;
		for(int i=0;i<3;i++){
			lVecList[i]=poly.pVecList[poly.verIndex[i]];
			gVecList[i]=lVecList[i];
		}
	}
	~Polygon3D_Vec(){}
	PointInPlane getPointInPlane(const Vec3f& p){
		//return GetPointInPlane(n,gVecList[0].v3f(),p);
		return GetPointInPlane(n,gVecList[0].v3f(),p);
	}
	void updateNormVec(){
		GetNormalVec(gVecList[0].v3f(),gVecList[1].v3f(),gVecList[2].v3f(),n);
	}
public:
	int state;//״̬
	int color;//��ɫ
	int attr;//��������
	Vec4f lVecList[3];//�ֲ����궥��
	Vec4f gVecList[3];//�任���궥��
	Polygon3D_Vec* pPre;
	Polygon3D_Vec* pNext;
	int shadeColor; //��ɫ��������ɫ
	Vec3f n;        //����
};

class ENGIN_DLL ObjNode3D:public Node3D{
public:
	ObjNode3D();
	virtual ~ObjNode3D();
	static ObjNode3D* createWithPlgfile(const std::string& fileName,const Vec3f&vs=NORMVEC);
	static ObjNode3D* createWithAscfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	static ObjNode3D* createWithCobfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	//����״̬λ
	virtual void resetState();
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void setScale( Vec3f& vs );
	virtual void computeLight( LightMgr* lightMgr,Camera* cam );
	//ˢ������İ�Χ�뾶��ƽ���뾶
	void computeObjRadius();
	void cullObj(Camera* cam,int cullType=CULL_ALL);
	void removeBackface(Camera* cam);
public:
	int id;           //ID
	char name[64];    //����
	int state;        //״̬
	int attr;         //��������

	float avgRadius;  //ƽ���뾶
	float maxRadius;  //���뾶

	int numVec;//�������
	Vec4f localVecList[OBJ_MAX_VEC];  //�ֲ�
	Vec4f worldVecList[OBJ_MAX_VEC];  //����
	Vec4f screenVecList[OBJ_MAX_VEC]; //��Ļ

	//ʵ��任��ʱ��ֻ��ı䶥�����ֵ���������붥���Ĺ�ϵ���������仯
	//��������Ķ�����б��ʼ����Ͳ��ᷢ���仯
	int numPoly;//��ĸ���
	Polygon3D_VecIdx polyList[OBJ_MAX_POLY];
};
NS_ENGIN_END
#endif