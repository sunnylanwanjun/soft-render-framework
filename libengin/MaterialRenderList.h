#ifndef __MATERIALRENDERLIST_H
#define __MATERIALRENDERLIST_H
#include "macros.h"
#include "MaterialNode3D.h"
NS_ENGIN_BEGIN
class ENGIN_DLL MaterialRenderList:public Node3D{
public:
	MaterialRenderList();
	virtual ~MaterialRenderList();
	static MaterialRenderList* create();

	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void updateLocalToWorldTrans(const Mat4& transMat);
	void computeLight( LightMgr* lightMgr,Camera* cam,MatPoly_Vec& poly);
	void reset(){numPoly=0;}
	void resetState();
	void insertPoly(MatPoly_Vec& poly);
	void insertPoly(MatPoly_VecIdx& polyVecIdx);
	void insertObj(MaterialNode3D& obj);
	void sortPolyList();
public:
	int state;        //◊¥Ã¨
	int attr;         //ŒÔ¿Ì Ù–‘
	int numPoly;
	RenderSortType sortType;
	MatPoly_Vec* polyList[RENDER_MAX_POLY];
	MatPoly_Vec  polyData[RENDER_MAX_POLY];
};
NS_ENGIN_END
#endif