#ifndef __MATERIALNODE3D_H
#define __MATERIALNODE3D_H
#include "macros.h"
#include "Object3D.h"
NS_ENGIN_BEGIN
class ENGIN_DLL MatPoly_VecIdx{
public:
	MatPoly_VecIdx(){
		memset(this,0,sizeof MatPoly_VecIdx);
	}
	~MatPoly_VecIdx(){}
	PointInPlane getPointInPlane(const Vec3f& p)const{
		return GetPointInPlane(n,pVecList[verIndex[0]].v3f(),p);
	}
	void updateNormVec(){
		GetNormalVec_NO_IDENTIFY(pVecList[verIndex[0]].v3f(),pVecList[verIndex[1]].v3f(),
			pVecList[verIndex[2]].v3f(),n);
		nLen=n.getModelFast();
	}
public:
	int state;//״̬
	int color;//��ɫ
	int attr;//��������
	int shadeColor[3];//�����������ɫ��ɫ�����Flat��ɫʱ����ֻʹ�õ�һ��
	BITMAP* pTexture;//����ָ��
	FVec4f* pVecList;//�����б�
	int verIndex[3];//�����б�����
	Vec2f* pTexList;//���������б�
	int texIndex[3];//������������
	Vec3f n;        //����
	float nLen;     //���߳���
};

class ENGIN_DLL MatPoly_Vec{
public:
	MatPoly_Vec(){		
		memset(this,0,sizeof MatPoly_Vec);
	}
	MatPoly_Vec(MatPoly_VecIdx& poly){
		state=poly.state;
		color=poly.color;
		attr=poly.attr;
		memcpy(shadeColor,poly.shadeColor,12);
		n=poly.n;
		nLen=poly.nLen;
		pPre=pNext=nullptr;
		pTexture=poly.pTexture;
		for(int i=0;i<3;i++){
			lVecList[i]=poly.pVecList[poly.verIndex[i]];
			lVecList[i].t2f()=poly.pTexList[poly.texIndex[i]];
			gVecList[i]=lVecList[i];
		}
	}
	~MatPoly_Vec(){}
	PointInPlane getPointInPlane(const Vec3f& p){
		return GetPointInPlane(n,gVecList[0].v3f(),p);
	}
	void updateNormVec(){
		//���淨�߲����е�λ��������Ϊ�󶥵㷨��ʱ����Ҫ��Ȩ�أ����淨�ߵĳ���
		//��������ΪȨ�ء�
		GetNormalVec_NO_IDENTIFY(gVecList[0].v3f(),gVecList[1].v3f(),gVecList[2].v3f(),n);
		nLen=n.getModelFast();
	}
public:
	int state;//״̬
	int color;//��ɫ
	int attr;//��������
	int shadeColor[3];//�����������ɫ��ɫ�����Flat��ɫʱ����ֻʹ�õ�һ��
	BITMAP* pTexture;//����ָ��
	FVec4f lVecList[3];//�ֲ����궥��
	FVec4f gVecList[3];//�任���궥��
	MatPoly_Vec* pPre;
	MatPoly_Vec* pNext;
	Vec3f n;        //����
	float nLen;     //���߳���
};

class ENGIN_DLL MaterialNode3D:public Node3D{
public:
	MaterialNode3D();
	virtual ~MaterialNode3D();
	void init(  int _num_vertices, 
				int _num_polys, 
				int _num_frames);
	void setFrame(int frame);
	static MaterialNode3D* createWithPlgfile(const std::string& fileName,const Vec3f&vs=NORMVEC);
	static MaterialNode3D* createWithAscfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	static MaterialNode3D* createWithCobfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	static void destroyObj(MaterialNode3D* obj);
	static void copyObj_World(MaterialNode3D* dst,const MaterialNode3D* src);
	//����״̬λ
	virtual void resetState();
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void setScale( Vec3f& vs );
	virtual void computeLight( LightMgr* lightMgr,Camera* cam );
	//ˢ������İ�Χ�뾶��ƽ���뾶
	void computeObjRadius();
	void computePolyNorm();
	void computeVecNorm();
	void cullObj(Camera* cam,int cullType=CULL_ALL);
	void removeBackface(Camera* cam);
public:
	int id;           //ID
	char name[64];    //����
	int state;        //״̬
	int attr;         //��������

	float* avgRadiuses;  //ƽ���뾶����
	float* maxRadiuses;  //���뾶����

	int numVec;//ÿ֡�������
	int totalVec;//�ܹ��������
	FVec4f* localVecList;//[OBJ_MAX_VEC];  //�ֲ�
	FVec4f* worldVecList;//[OBJ_MAX_VEC];  //����
	//��Ļ���겻��Ҫÿ֡��������
	Vec4f* screenVecList;//[OBJ_MAX_VEC]; //��Ļ

	FVec4f* curLocalVecList;	   //ָ��ÿ֡�ֲ������б��׵�ַ
	FVec4f* curWorldVecList;	   //ָ��ÿ֡���������б��׵�ַ

	int numFrame;//�ܹ�֡��
	int curFrame;//��ǰ�ڼ�֡
	int frameSize;//ÿһ֡����ռ�õ��ڴ�

	//���������б� �������*3���붥����Ŀ��ͬ��������Թ��ã���
	//����ε��������겻�ܹ���
	Vec2f* texList;
	BITMAP* texture;//����

	//ʵ��任��ʱ��ֻ��ı䶥�����ֵ���������붥���Ĺ�ϵ���������仯
	//��������Ķ�����б��ʼ����Ͳ��ᷢ���仯
	int numPoly;//��ĸ���
	MatPoly_VecIdx* polyList;//[OBJ_MAX_POLY];

	int ivar1,ivar2;
	float fvar1,fvar2;
};
NS_ENGIN_END
#endif