#ifndef __DRAWFUNC_H
#define __DRAWFUNC_H
#include "macros.h"
#include "Node.h"
#include "Vec.h"
#include <functional>
#include <vector>
NS_ENGIN_BEGIN
typedef std::function<bool(float,float&)> DRAWFUNC;
class ENGIN_DLL DrawFunc:public Node{
public:
	DrawFunc();
	virtual ~DrawFunc();
	virtual bool init();
	void setMinX(int min);
	void setMaxX(int max);
	virtual void draw(Renderer* renderer, Mat4& transform);
	static DrawFunc* create( DRAWFUNC func,float scale=1.0f);
public:
	DWORD color;
private:
	int _minX;
	int _maxX;
	bool _setMinX;
	bool _setMaxX;
	std::vector<Vec2i> _vecs;
	DRAWFUNC _func;
};
NS_ENGIN_END
#endif