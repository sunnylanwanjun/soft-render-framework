#ifndef __RENDERLIST_H
#define __RENDERLIST_H
#include "macros.h"
#include "Ref.h"
#include "t3dlib5.h"
#include "Node3D.h"
#include "Object3D.h"
NS_ENGIN_BEGIN
class ENGIN_DLL RenderList:public Node3D{
public:
	RenderList();
	virtual ~RenderList();
	static RenderList* create();
	
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	void computeLight( LightMgr* lightMgr,Camera* cam,Polygon3D_Vec& poly);
	void reset(){numPoly=0;}
	void resetState();
	void insertPoly(Polygon3D_Vec& poly);
	void insertPoly(Polygon3D_VecIdx& polyVecIdx);
	void insertObj(ObjNode3D& obj);
	void sortPolyList();
public:
	int state;        //◊¥Ã¨
	int attr;         //ŒÔ¿Ì Ù–‘
	int numPoly;
	RenderSortType sortType;
	Polygon3D_Vec* polyList[RENDER_MAX_POLY];
	Polygon3D_Vec  polyData[RENDER_MAX_POLY];
};
NS_ENGIN_END
#endif