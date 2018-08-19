#ifndef __PLGREADER_H
#define __PLGREADER_H
#include "macros.h"
#include "log.h"
#include "t3dlib5.h"
#include <fstream>
NS_ENGIN_BEGIN
#define LOAD_BUFFER_LEN 255
//表面描述符屏蔽位
#define PLG_COLOR_MASK		0x8000
#define PLG_SHADE_MASK		0x6000
#define PLG_SIDE_MASK		0x1000
//表面描述符相应的值
#define PLG_COLOR_16		0x8000
#define PLG_COLOR_8			0x0000
#define PLG_SIDE_2			0x1000
#define PLG_SIDE_1			0x0000
//着色模式
#define PLG_SHADE_PURE      0x0000  // this poly is a constant color
#define PLG_SHADE_CONSTANT  0x0000  // alias
#define PLG_SHADE_FLAT      0x2000  // this poly uses flat shading
#define PLG_SHADE_GOURAUD   0x4000  // this poly used gouraud shading
#define PLG_SHADE_PHONG     0x6000  // this poly uses phong shading
#define PLG_SHADE_FASTPHONG 0x6000  // this poly uses phong shading 
class ObjNode3D;
class Vec3f;
class MaterialNode3D;
int ENGIN_DLL ReadLine(char* buffer,int bufLen,std::ifstream& in);
bool ENGIN_DLL LoadPlg(ObjNode3D* pObj,const char* plgName,const Vec3f& v);
bool ENGIN_DLL LoadPlg(MaterialNode3D* pObj,const char* plgName,const Vec3f& v);
NS_ENGIN_END
#endif