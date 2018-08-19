#ifndef __POLAR_H
#define __POLAR_H
#include "macros.h"
#include "math.h"
NS_ENGIN_BEGIN
struct ENGIN_DLL Polar2D{
	float angle;
	float r;
	Polar2D():r(0),angle(0){}
};
struct ENGIN_DLL Polar3D{
	float xAngle;//��X��ļн�,0��360
	float zAngle;//��Z��ļн�
	float r;
	Polar3D():r(0),xAngle(0),zAngle(0){}
};
struct ENGIN_DLL Column{
	Polar2D polar;
	float z;
	Column():z(0){};
};
NS_ENGIN_END
#endif