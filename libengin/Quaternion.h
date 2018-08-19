#ifndef __QUATERNION_H
#define __QUATERNION_H
#include "macros.h"
#include "Vec.h"
#include "d3dh.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Quaternion{
public:
	union{
		float m[4];
		struct{
			float x;//x;
			float y;//z;
			float z;//w;
			float w;//y;
		};
		struct{
			Vec3f qV;
			float q0;
		};
	};
	Quaternion();
	virtual ~Quaternion();
	Quaternion(Vec3f& v):q0(0),qV(v){}
	//绕轴和角，旋转点
	void rotate(Vec3f& point,Vec3f& normal,float theta);
	//欧拉转四元数
	void eulerToQuat(float heading, float pitch, float bank);
	//轴，角对转四元数
	void vectorThetaToQuat(Vec3f& normal,float theta);
	//求逆
	void inverse(){
		float model2=getModel2();
		if(model2<=E5)
			return;
		model2=1/model2;
		w = w*model2;
		x = -x*model2;
		y = -y*model2;
		z = -z*model2;
	}
	//共轭
	void conjugate(){
		w = w;
		x = -x;
		y = -y;
		z = -z;
	}
	//乘以系数
	void multiply(float k){
		x*=k;
		y*=k;
		z*=k;
		w*=k;
	}
	//单位化
	void normal(){
		float _1DivModel=getModel();
		if(_1DivModel<=E5){
			return;
		}
		_1DivModel=1/_1DivModel;
		x*=_1DivModel;
		y*=_1DivModel;
		z*=_1DivModel;
		w*=_1DivModel;
	}
	//归零
	void zero(){
		x=y=z=w=0.0f;
	}
	//////////////////////////////////////////////////////
	//const接口
	Quaternion operator+(const Quaternion& q)const{
		Quaternion quat;
		quat.x = x+q.x;
		quat.y = y+q.y;
		quat.z = z+q.z;
		quat.w = w+q.w;
		return quat;
	}
	Quaternion operator*(float k)const{
		Quaternion quat;
		quat.x = x*k;
		quat.y = y*k;
		quat.z = z*k;
		quat.w = w*k;
		return quat;
	}
	void getConjugate(Quaternion& quat)const{
		quat=*this;
		quat.conjugate();
	}
	void getMultiply(float k,Quaternion& quat)const{
		quat.x = x*k;
		quat.y = y*k;
		quat.z = z*k;
		quat.w = w*k;
	}	
	Quaternion operator-(const Quaternion& q)const{
		Quaternion quat;
		quat.x = x-q.x;
		quat.y = y-q.y;
		quat.z = z-q.z;
		quat.w = w-q.w;
		return quat;
	}
	float getModel()const{
		return sqrt(w*w+x*x+y*y+z*z);
	}
	//获得模的平方
	float getModel2()const{
		return w*w+x*x+y*y+z*z;
	}
	//获得单位四元数
	void getNormal(Quaternion& quat)const{
		quat=*this;
		quat.normal();
	}
	//该四元数必须是单位四元数
	void getNormInverse(Quaternion& quat)const{
		getConjugate(quat);
	}
	void getInverse(Quaternion& quat)const{
		quat=*this;
		quat.inverse();
	}
	void getVecorTheta(Vec3f& normal,float& theta);
};



/*
两四元数相乘
实部与实部，虚部与虚部相乘
Q=Qw+Qv
P=Pw+Pv
Q*P=(Qw+Qv)*(Pw+Pv)
[Pw*Qw-(Pv(dot)Qv)]+[Pw*Qv+Qw*Pv+Pv(cross)Qv]
合并同类项之后，就是以下的四个表达式
*/
inline Quaternion operator*(const Quaternion&p,const Quaternion& q){
	Quaternion quat;
	quat.w = p.w*q.w - p.x*q.x - p.y*q.y - p.z*q.z;
	quat.x = p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y;
	quat.y = p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z;
	quat.z = p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x;
	return quat;
}

inline void Quat_Mul(const Quaternion&p,const Quaternion& q,Quaternion& dst){
	dst.w = p.w*q.w - p.x*q.x - p.y*q.y - p.z*q.z;
	dst.x = p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y;
	dst.y = p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z;
	dst.z = p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x;
}

inline void Quat_Mul(const Quaternion& s0,const Quaternion& s1,const Quaternion& s2,Quaternion& dst){
	Quaternion s01;
	Quat_Mul(s0,s1,s01);
	Quat_Mul(s01,s2,dst);
}

/*
四元数的理解

在理解四元数之前，先复习行列式的知识
什么是行列式，行列式从何而来，从二维开始
如何求一个平行四边形的面积,设向量m和n为平行四边形的两条边，
有|m||n|sin<m,n>,而sin<m,n>=sin<m>cos<n>-cos<m>sin<n>
所以|m||n|sin<m,n>=|m||n|(sin<m>cos<n>-cos<m>sin<n>)=
|m|sin<m>*|n|*cos<n>-|m|cos<m>|n|sin<n>=my*nx-mx*ny
于是有四边形的面积公式S=my*nx-mx*n，定义二维行列式
|mx,my|=my*nx-mx*n,这就是二维行列式的由来，
|nx,ny|
现在猜想，既然二维行列式是平行四边形的面积，那三维行列式是不是平行六面体的体积呢？
现已知平行六面体的三边向量分别为A,B,C，根据体积公式，V=s*h有
s = |A||B|sin<A,B>
h = |C|cos<C,n>，其中n为A,B的法向量
现在定义一种新的运算,|AxB| = |A||B|sin<A,B>，且AxB的方向为n,有|n|=|AxB|

s*h=|A||B|sin<A,B>*|C|cos<C,n>=
|AxB|*|C|cos<C,n>=(AxB)*C

而将结果定义为一种新运算
|Cx Cy Cz| = (AxB)*C
|Ax Ay Az|
|Bx By Bz|

由于n与A,B垂直，所以有方程组
n*A=0
n*B=0
联立可得，n=<AyBz-ByAz,AxBz-BxAz,AxBy-AyBz>
转成线性组合表达式有
n=(AyBz-ByAz)*i+(AxBz-BxAz)*j+(AxBy-AyBz)*k
|i  j  k | = (AyBz-ByAz)*i+(AxBz-BxAz)*j+(AxBy-AyBz)*k
|Ax Ay Az|
|Bx By Bz|

从只有一个虚部开始
n = sinθ*i+cosθ,约定这个表达式为复数，且n为一向量，θ为与x轴的夹角，
现有 z1 = sin(A)*i+cos(A) z2 = sin(B)*i+cos(B)
z1*z2 = sin(A)*sin(B)*i*i+sin(A)*cos(B)*i+cos(A)*sin(B)*i+cos(A)*cos(B)
= [sin(A)*cos(B)+cos(A)*sin(B)]*i+cos(A)*cos(B)-sin(A)*sin(B)
= sin(A+B)*i+cos(A+B)
根据约定，可知z1*z2的结果为z1旋转了B度的角,有个地方需要注意，旋转量必须为
单位复数，否则结果的就与约定不同了，会多乘以一个系数
扩展到四元数有，
n = sin(A)*V+cos(A)
z1 = <0,x,y,z> = <0,v>
z2 = sin(B)*V+cos(B) = <w,u>
z2^-1 = -sin(B)*V+cos(B) = <w,-u>
四元数不能用旋转叠加来进行证明，因为旋转公式为
Q'=P*Q*P^-1
Q为被旋转的向量，P为旋转四元数，P^-1为P的倒数，
由于P必须为单位四元数，所以P的倒数为P的共轭四元数
P^-1*P==1==P*P^-1
P^-1*P*P~==P~==P*P^-1*P~
P^-1*(P*P~)==P~
P^-1*|P|^2==P~
P^-1==P~/|P|^2
由于P为单位向量所以
P^-1==P~ 于是P的倒数为P的共轭四元数的证题，得证
Q'=P*Q*P^-1
=<w,u>*<0,v>*<w,-u>
先记算前面的式子
<w,u>*<0,v>
=(0*w-u*v)+(w*v+0*u+uxv)
=-u*v+(w*v+uxv)
<-u*v,w*v+uxv>*<w,-u>
=<-u*v*w+u*(w*v+uxv),w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
=<-u*v*w+u*w*v+u*uxv),w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
u*uxv u与v叉乘的结果肯定是与u和v都垂直的，用这个结果与u点乘，最后肯定是0
于是，接着计算
=<0,w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
现在分解虚部
分解前，需要用一个公式ax(bxc)=b(a*c)-c(a*b)
w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)=
w*(w*v+uxv)+u*v*u+ux(w*v+uxv)=
w^2*v+uxv*w+u*v*u+uxw*v+uxuxv=
w^2*v+2uxv*w+u*v*u+uxuxv=
w^2*v+2uxv*w+u*v*u+u(u*v)-v(u*u)=
(w^2-u*u)*v+2uxv*w+2*u(u*v)
由w=cos(B) u=sin(B)代入计算，有
cos(2B)v+(1-cos(2B))(n*v)n+sin(2B)(nxv)
由定义绕n轴旋转角B为
cos(B)v+(1-cos(B))(n*v)n+sin(B)(nxv)
所以Q'=P*Q*P^-1使得Q绕n旋转2B角
于是欧拉角需要减半再转四元数就是这个道理
*/

NS_ENGIN_END
#endif