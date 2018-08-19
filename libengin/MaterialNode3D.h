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
	int state;//状态
	int color;//颜色
	int attr;//物理属性
	int shadeColor[3];//三个顶点的着色颜色，如果Flat着色时，则只使用第一个
	BITMAP* pTexture;//材质指针
	FVec4f* pVecList;//顶点列表
	int verIndex[3];//顶点列表索引
	Vec2f* pTexList;//纹理坐标列表
	int texIndex[3];//纹理坐标索引
	Vec3f n;        //法线
	float nLen;     //法线长度
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
		//对面法线不进行单位化，是因为求顶点法线时，需要加权重，而面法线的长度
		//就正好作为权重。
		GetNormalVec_NO_IDENTIFY(gVecList[0].v3f(),gVecList[1].v3f(),gVecList[2].v3f(),n);
		nLen=n.getModelFast();
	}
public:
	int state;//状态
	int color;//颜色
	int attr;//物理属性
	int shadeColor[3];//三个顶点的着色颜色，如果Flat着色时，则只使用第一个
	BITMAP* pTexture;//材质指针
	FVec4f lVecList[3];//局部坐标顶点
	FVec4f gVecList[3];//变换坐标顶点
	MatPoly_Vec* pPre;
	MatPoly_Vec* pNext;
	Vec3f n;        //法线
	float nLen;     //法线长度
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
	//重置状态位
	virtual void resetState();
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void setScale( Vec3f& vs );
	virtual void computeLight( LightMgr* lightMgr,Camera* cam );
	//刷新物体的包围半径和平均半径
	void computeObjRadius();
	void computePolyNorm();
	void computeVecNorm();
	void cullObj(Camera* cam,int cullType=CULL_ALL);
	void removeBackface(Camera* cam);
public:
	int id;           //ID
	char name[64];    //名字
	int state;        //状态
	int attr;         //物理属性

	float* avgRadiuses;  //平均半径数组
	float* maxRadiuses;  //最大半径数组

	int numVec;//每帧顶点个数
	int totalVec;//总共顶点个数
	FVec4f* localVecList;//[OBJ_MAX_VEC];  //局部
	FVec4f* worldVecList;//[OBJ_MAX_VEC];  //世界
	//屏幕坐标不需要每帧保存数据
	Vec4f* screenVecList;//[OBJ_MAX_VEC]; //屏幕

	FVec4f* curLocalVecList;	   //指向每帧局部坐标列表首地址
	FVec4f* curWorldVecList;	   //指向每帧世界坐标列表首地址

	int numFrame;//总共帧数
	int curFrame;//当前第几帧
	int frameSize;//每一帧顶点占用的内存

	//纹理坐标列表 多边形数*3，与顶点数目不同，顶点可以共用，但
	//多边形的纹理坐标不能共用
	Vec2f* texList;
	BITMAP* texture;//纹理

	//实体变换的时候，只会改变顶点的数值，但顶点与顶点间的关系并不发生变化
	//所以下面的多边形列表初始化后就不会发生变化
	int numPoly;//面的个数
	MatPoly_VecIdx* polyList;//[OBJ_MAX_POLY];

	int ivar1,ivar2;
	float fvar1,fvar2;
};
NS_ENGIN_END
#endif