#ifndef __STARS_H
#define __STARS_H
#include "engin.h"
#define NUM_STARS  256
class Stars:public engin::Node{
public:
	Stars();
	~Stars();
	void draw(engin::Renderer* renderer, engin::Mat4& transform);
	engin::Vec3f stars[NUM_STARS];
};
#endif