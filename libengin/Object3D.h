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
	int state;//状态
	int color;//颜色
	int attr;//物理属性
	Vec4f* pVecList;//顶点列表
	int verIndex[3];//顶点列表索引

	int shadeColor; //着色计算后的颜色
	Vec3f n;        //法线
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
	int state;//状态
	int color;//颜色
	int attr;//物理属性
	Vec4f lVecList[3];//局部坐标顶点
	Vec4f gVecList[3];//变换坐标顶点
	Polygon3D_Vec* pPre;
	Polygon3D_Vec* pNext;
	int shadeColor; //着色计算后的颜色
	Vec3f n;        //法线
};

class ENGIN_DLL ObjNode3D:public Node3D{
public:
	ObjNode3D();
	virtual ~ObjNode3D();
	static ObjNode3D* createWithPlgfile(const std::string& fileName,const Vec3f&vs=NORMVEC);
	static ObjNode3D* createWithAscfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	static ObjNode3D* createWithCobfile(const std::string& fileName,const Vec3f&vs=NORMVEC,int mode=LOAD_SWAP_YZ);
	//重置状态位
	virtual void resetState();
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void setScale( Vec3f& vs );
	virtual void computeLight( LightMgr* lightMgr,Camera* cam );
	//刷新物体的包围半径和平均半径
	void computeObjRadius();
	void cullObj(Camera* cam,int cullType=CULL_ALL);
	void removeBackface(Camera* cam);
public:
	int id;           //ID
	char name[64];    //名字
	int state;        //状态
	int attr;         //物理属性

	float avgRadius;  //平均半径
	float maxRadius;  //最大半径

	int numVec;//顶点个数
	Vec4f localVecList[OBJ_MAX_VEC];  //局部
	Vec4f worldVecList[OBJ_MAX_VEC];  //世界
	Vec4f screenVecList[OBJ_MAX_VEC]; //屏幕

	//实体变换的时候，只会改变顶点的数值，但顶点与顶点间的关系并不发生变化
	//所以下面的多边形列表初始化后就不会发生变化
	int numPoly;//面的个数
	Polygon3D_VecIdx polyList[OBJ_MAX_POLY];
};
NS_ENGIN_END
#endif