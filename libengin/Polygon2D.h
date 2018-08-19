#ifndef __POLYGON2D_H
#define __POLYGON2D_H
#include "macros.h"
#include "Node.h"
NS_ENGIN_BEGIN
//#define SHOW_BOUNDING_BOX
class Renderer;
class ENGIN_DLL Polygon2D :public Node{
public:
	Polygon2D();
	virtual ~Polygon2D();
	static Polygon2D* Polygon2D::create(Vec3f& pos, int numVer, Vec3f* vList, DWORD color, bool isFill);
	virtual void draw(Renderer* renderer, Mat4& transform);
public:
	int numVer;
	DWORD color;
	Vec3f* globalVertexList;
private:
	bool _isFill;
	Vec3f* _originVertexList;
#ifdef SHOW_BOUNDING_BOX
	Vec3f  _bList[4];
	Range  _bBox;
#endif
	//float* _radiusList;
};
NS_ENGIN_END
#endif