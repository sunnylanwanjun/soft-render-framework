#ifndef __COORDINATE2D_H
#define __COORDINATE2D_H
#include "macros.h"
#include "Node.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Coordinate2D:public Node{
public:
	Coordinate2D();
	virtual ~Coordinate2D();
	virtual bool init();
	virtual void draw(Renderer* renderer, Mat4& transform);
	static Coordinate2D* create(float scale=1.0f);
public:
	DWORD color;
private:
	Vec3f p0,p1;
	int col,row;
	Vec3f originvList[4];
};
NS_ENGIN_END
#endif