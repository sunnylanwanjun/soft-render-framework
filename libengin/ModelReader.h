#ifndef __ASCREADER_H
#define __ASCREADER_H
#include "macros.h"
NS_ENGIN_BEGIN
class ObjNode3D;
class Vec3f;
class MaterialNode3D;
//读取模型时，作为参数传递
#define LOAD_SWAP_XZ				  0x0001
#define LOAD_SWAP_XY				  0x0002
#define LOAD_SWAP_YZ				  0x0004
#define LOAD_INVER_X				  0x0008
#define LOAD_INVER_Y				  0x0010
#define LOAD_INVER_Z				  0x0020
#define LOAD_SHADE_CONSTANT           0x0040//外部指定多边形着色方式，因为asc文件不支持着色方式
#define LOAD_SHADE_FLAT               0x0080//外部指定多边形着色方式，因为asc文件不支持着色方式
#define LOAD_SHADE_GOURAUD            0x0100//外部指定多边形着色方式，因为asc文件不支持着色方式
#define LOAD_SHADE_PHONG              0x0200//外部指定多边形着色方式，因为asc文件不支持着色方式
#define LOAD_TEXTURE_SWAP_XY          0x0400
#define LOAD_TEXTURE_INVER_X          0x0800
#define LOAD_TEXTURE_INVER_Y          0x1000

bool ENGIN_DLL LoadAsc(ObjNode3D* pObj,const char* ascName,const Vec3f& vs,int mode=0);
bool ENGIN_DLL LoadCob(ObjNode3D* pObj,const char* cobName,const Vec3f& vs,int mode=0);
bool ENGIN_DLL LoadAsc(MaterialNode3D* pObj,const char* ascName,const Vec3f& vs,int mode=0);
bool ENGIN_DLL LoadCob(MaterialNode3D* pObj,const char* cobName,const Vec3f& vs,int mode=0);
NS_ENGIN_END
#endif