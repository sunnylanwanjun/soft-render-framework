#ifndef __MATHUTIL_H
#define __MATHUTIL_H
#include "macros.h"
#include "Geometry.h"
#include "Polar.h"
#include "Vec.h"
NS_ENGIN_BEGIN

//点与面
enum PointInPlane{
	PIP_IN,		//点与法线同向一侧
	PIP_ON,     //点在平面内
	PIP_OUT     //点与法线异向一侧
};
//点与线
enum PointInLine{
	PIL_IN,     //点在线上
	PIL_OUT     //点在线外
};
//线与线
enum LineCrossType{
	LCT_NOT,    //两线平行
	LCT_IN,     //两线相交，且交点在线上
	LCT_OUT,    //两线相交，但交点不在线上
};
//线与面
enum LinePlaneCrossType{
	LPCT_NOT,   //线与面平行
	LPCT_ON,    //线与面相交，且交点在面上
	LPCT_IN,    //线与面不相交，但不平行，该线与面法线同侧
	LPCT_OUT,   //线与面不相交，但不平行，该线与面法线异侧
};

struct ENGIN_DLL Mat2X2;
struct ENGIN_DLL Mat3X1;
struct ENGIN_DLL Mat3X3;
struct ENGIN_DLL Mat2X1;
struct ENGIN_DLL Plane3D;

extern int ENGIN_DLL Fast_Distance_2D(int x, int y);
extern float ENGIN_DLL Fast_Distance_3D(float fx, float fy, float fz);
extern void ENGIN_DLL BoundingBox(const Vec3f veclist[],int numVec,Range& range);
extern bool ENGIN_DLL PolygonCollistion(const Vec3f veclist0[],int numVec0,const Vec3f veclist1[],int numVec1);
extern LineCrossType ENGIN_DLL LineCross2D(const Vec3f& L1p, const Vec3f& L1v, const Vec3f& L2p, const Vec3f& L2v, float *t1, float *t2, Vec3f* p);
extern void ENGIN_DLL VecToPolar2D(const Vec3f& v,Polar2D& p);
extern void ENGIN_DLL PolarToVec2D(Vec3f& v,Polar2D& p);
extern void ENGIN_DLL VecToPolar3D(Vec3f& v,Polar3D& p);
extern void ENGIN_DLL PolarToVec3D(Vec3f& v,Polar3D& p);
extern void ENGIN_DLL Vec3DToColumn(Vec3f& v,Column& c);
extern void ENGIN_DLL ColumnToVec3D(Vec3f& v,Column& c);
extern PointInPlane ENGIN_DLL GetPointInPlane(const Plane3D& plane,const Vec3f& p);
extern PointInPlane ENGIN_DLL GetPointInPlane(const Vec3f& n,const Vec3f& p,const Vec3f& p0);
extern PointInPlane ENGIN_DLL GetPointInPlane(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,const Vec3f& p3);
extern float ENGIN_DLL WrapAngle(float angle);
extern void GetPlane3D(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Plane3D& plane);

inline Mat2X1 ENGIN_DLL operator*(Mat2X2& m0,Mat2X1 m1);
inline bool ENGIN_DLL Equation2(Mat2X2& coef,Mat2X1& res,float* x,float* y);
inline bool ENGIN_DLL Equation3(Mat3X3& coef,Mat3X1& res,float* x,float* y,float *z);
inline void ENGIN_DLL FIXP16_Mul(FIXP16 f0,FIXP16 f1,FIXP16& f2);
inline void ENGIN_DLL FIXP16_Div(FIXP16 f0,FIXP16 f1,FIXP16& f2);
inline void ENGIN_DLL GetLinePoint3D(const Vec3f& Lp,const Vec3f& Lv,float t,Vec3f& p);
/*FPU指令
不要把指令写成大写，不支持。。。
首先得说，3D游戏编程大师技巧的说法有误
其次假设(operator)为ADD,SUB,MUL,DIV这四个操作
1 F(operator)P ST(n),ST
表ST(n)=ST(n)-ST,结束后弹出ST
2 F(operator);不带任何操作数
F(operator)==F(operator)P ST(1),ST
3 F(operator) [ST(n),n=1~7],ST
ST(n)=ST(n)-ST,不进行弹出操作
3 F(operator) [ST(n),n=1~7]|[memory]
ST=ST-param，不进行任何弹出操作
4 Fi(operator) [memory]
ST=ST-param，不进行任何弹出操作,与3除了表示[memory]为整数外，其它都一样
5 FLD [memory]
内存入栈
6 FILD [memory]
除了入栈，还表示memory为整数
总结：
记住两点，适用于全部，结果肯定是目标操作数，
对于有两个目标操作数的指令，目标操作数肯定是第一个，对于只有一个操作数的，目标操作
数肯定是ST,对于没有操作数的，目标操作数是ST1,有弹出操作的，永远弹出ST，而目标操作数
是不能被弹出的，所以对于操作数的顺序肯定是有要求的了，这点书上说的不对。
对于
*/

inline void FAdd(float f0,float f1,float& f2);
inline void FSub(float f0,float f1,float& f2);
inline void FMul(float f0,float f1,float& f2);
inline void FDiv(float f0,float f1,float& f2);
inline void FAdd(float f0,int   i1,float& f2);
inline void FSub(float f0,int   i1,float& f2);
inline void FMul(float f0,int   i1,float& f2);
inline void FDiv(float f0,int   i1,float& f2);
inline void FAdd(float f0,int   i1,int& i2);
inline void FSub(float f0,int   i1,int& f2);
inline void FMul(float f0,int   i1,int& f2);
inline void FDiv(float f0,int   i1,int& f2);
NS_ENGIN_END
//注意不要放在命名空间里边，因为inl已经写了命名空间，放在里边就会出现嵌套
#include "mathUtil.inl"

#endif