#ifndef __GEOMETRY_H
#define __GEOMETRY_H
#include "macros.h"
#include "Vec.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Size{
public:
	float width;
	float height;
	Size():width(0.0f),height(0.0f){}
	Size(int w, int h){ width = (float)w; height = (float)h; }
	Size(float w, float h){ width = w; height = h; }
	Size(const Size& s){ width = s.width; height = s.height; }
};
class ENGIN_DLL Rect{
public:
	Size size;
	Vec3f origin;
	Rect() :size(0.0f,0.0f),origin(0.0f,0.0f){}
	Rect(float x, float y, float w, float h) :size(w, h), origin(x, y){}
	Rect(const Rect&r){ size = r.size; origin = r.origin; }
	Rect(const Vec3f&v, const Size&s){ origin = v; size = s; }
};
typedef Rect* LPRect;
class ENGIN_DLL Range{
public:
	float    left;
	float    top;
	float    right;
	float    bottom;
	Range() :left(0), top(0), right(0), bottom(0){}
	Range(float l, float t, float r, float b) :left(l), top(t), right(r), bottom(b){}
	Range(const Range&range){ left = range.left; top = range.top; right = range.right; bottom = range.bottom; }
};
class ENGIN_DLL RangeFIXP16{
public:
	FIXP16    left;
	FIXP16    top;
	FIXP16    right;
	FIXP16    bottom;
	RangeFIXP16() :left(0), top(0), right(0), bottom(0){}
};
struct ENGIN_DLL iRange{
	int left;
	int top;
	int right;
	int bottom;
	iRange(int l, int t, int r, int b) :left(l), top(t), right(r), bottom(b){}
};
NS_ENGIN_END
#endif