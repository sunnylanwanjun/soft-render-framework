#ifndef __MATHUTIL_H
#define __MATHUTIL_H
#include "macros.h"
#include "Geometry.h"
#include "Polar.h"
#include "Vec.h"
NS_ENGIN_BEGIN

//������
enum PointInPlane{
	PIP_IN,		//���뷨��ͬ��һ��
	PIP_ON,     //����ƽ����
	PIP_OUT     //���뷨������һ��
};
//������
enum PointInLine{
	PIL_IN,     //��������
	PIL_OUT     //��������
};
//������
enum LineCrossType{
	LCT_NOT,    //����ƽ��
	LCT_IN,     //�����ཻ���ҽ���������
	LCT_OUT,    //�����ཻ�������㲻������
};
//������
enum LinePlaneCrossType{
	LPCT_NOT,   //������ƽ��
	LPCT_ON,    //�������ཻ���ҽ���������
	LPCT_IN,    //�����治�ཻ������ƽ�У��������淨��ͬ��
	LPCT_OUT,   //�����治�ཻ������ƽ�У��������淨�����
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
/*FPUָ��
��Ҫ��ָ��д�ɴ�д����֧�֡�����
���ȵ�˵��3D��Ϸ��̴�ʦ���ɵ�˵������
��μ���(operator)ΪADD,SUB,MUL,DIV���ĸ�����
1 F(operator)P ST(n),ST
��ST(n)=ST(n)-ST,�����󵯳�ST
2 F(operator);�����κβ�����
F(operator)==F(operator)P ST(1),ST
3 F(operator) [ST(n),n=1~7],ST
ST(n)=ST(n)-ST,�����е�������
3 F(operator) [ST(n),n=1~7]|[memory]
ST=ST-param���������κε�������
4 Fi(operator) [memory]
ST=ST-param���������κε�������,��3���˱�ʾ[memory]Ϊ�����⣬������һ��
5 FLD [memory]
�ڴ���ջ
6 FILD [memory]
������ջ������ʾmemoryΪ����
�ܽ᣺
��ס���㣬������ȫ��������϶���Ŀ���������
����������Ŀ���������ָ�Ŀ��������϶��ǵ�һ��������ֻ��һ���������ģ�Ŀ�����
���϶���ST,����û�в������ģ�Ŀ���������ST1,�е��������ģ���Զ����ST����Ŀ�������
�ǲ��ܱ������ģ����Զ��ڲ�������˳��϶�����Ҫ����ˣ��������˵�Ĳ��ԡ�
����
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
//ע�ⲻҪ���������ռ���ߣ���Ϊinl�Ѿ�д�������ռ䣬������߾ͻ����Ƕ��
#include "mathUtil.inl"

#endif