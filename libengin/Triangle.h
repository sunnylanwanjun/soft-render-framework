#ifndef __TRIANGLE_H
#define __TRIANGLE_H
#include "macros.h"
#include "Node.h"
NS_ENGIN_BEGIN
class Renderer;
class ENGIN_DLL Triangle :public Node{
public:
	Triangle();
	virtual ~Triangle();
	static Triangle* create(Vec3f pos, Vec3f p0, Vec3f p1, Vec3f p2, DWORD color);
	virtual void draw(Renderer* renderer, Mat4& transform);
public:
	DWORD color;
	Vec3f p0;
	Vec3f p1;
	Vec3f p2;
	Vec3f gp0;
	Vec3f gp1;
	Vec3f gp2;
};
NS_ENGIN_END
#endif