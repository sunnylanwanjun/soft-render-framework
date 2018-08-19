#ifndef __LINE_H
#define __LINE_H
#include "macros.h"
#include "Node.h"
NS_ENGIN_BEGIN
class Renderer;
class ENGIN_DLL Line :public Node{
public:
	Line();
	virtual ~Line();
	static Line* create(Vec3f& pos, Vec3f& p0, Vec3f& p1, DWORD color);
	virtual void draw(Renderer* renderer, Mat4& transform);
public:
	Vec3f p0;
	Vec3f p1;
	Vec3f gp0;
	Vec3f gp1;
	Vec3f v;
	DWORD color;
};
NS_ENGIN_END
#endif