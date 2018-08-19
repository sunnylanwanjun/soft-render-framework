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
	//����ͽǣ���ת��
	void rotate(Vec3f& point,Vec3f& normal,float theta);
	//ŷ��ת��Ԫ��
	void eulerToQuat(float heading, float pitch, float bank);
	//�ᣬ�Ƕ�ת��Ԫ��
	void vectorThetaToQuat(Vec3f& normal,float theta);
	//����
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
	//����
	void conjugate(){
		w = w;
		x = -x;
		y = -y;
		z = -z;
	}
	//����ϵ��
	void multiply(float k){
		x*=k;
		y*=k;
		z*=k;
		w*=k;
	}
	//��λ��
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
	//����
	void zero(){
		x=y=z=w=0.0f;
	}
	//////////////////////////////////////////////////////
	//const�ӿ�
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
	//���ģ��ƽ��
	float getModel2()const{
		return w*w+x*x+y*y+z*z;
	}
	//��õ�λ��Ԫ��
	void getNormal(Quaternion& quat)const{
		quat=*this;
		quat.normal();
	}
	//����Ԫ�������ǵ�λ��Ԫ��
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
����Ԫ�����
ʵ����ʵ�����鲿���鲿���
Q=Qw+Qv
P=Pw+Pv
Q*P=(Qw+Qv)*(Pw+Pv)
[Pw*Qw-(Pv(dot)Qv)]+[Pw*Qv+Qw*Pv+Pv(cross)Qv]
�ϲ�ͬ����֮�󣬾������µ��ĸ����ʽ
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
��Ԫ�������

�������Ԫ��֮ǰ���ȸ�ϰ����ʽ��֪ʶ
ʲô������ʽ������ʽ�Ӻζ������Ӷ�ά��ʼ
�����һ��ƽ���ı��ε����,������m��nΪƽ���ı��ε������ߣ�
��|m||n|sin<m,n>,��sin<m,n>=sin<m>cos<n>-cos<m>sin<n>
����|m||n|sin<m,n>=|m||n|(sin<m>cos<n>-cos<m>sin<n>)=
|m|sin<m>*|n|*cos<n>-|m|cos<m>|n|sin<n>=my*nx-mx*ny
�������ı��ε������ʽS=my*nx-mx*n�������ά����ʽ
|mx,my|=my*nx-mx*n,����Ƕ�ά����ʽ��������
|nx,ny|
���ڲ��룬��Ȼ��ά����ʽ��ƽ���ı��ε����������ά����ʽ�ǲ���ƽ�������������أ�
����֪ƽ������������������ֱ�ΪA,B,C�����������ʽ��V=s*h��
s = |A||B|sin<A,B>
h = |C|cos<C,n>������nΪA,B�ķ�����
���ڶ���һ���µ�����,|AxB| = |A||B|sin<A,B>����AxB�ķ���Ϊn,��|n|=|AxB|

s*h=|A||B|sin<A,B>*|C|cos<C,n>=
|AxB|*|C|cos<C,n>=(AxB)*C

�����������Ϊһ��������
|Cx Cy Cz| = (AxB)*C
|Ax Ay Az|
|Bx By Bz|

����n��A,B��ֱ�������з�����
n*A=0
n*B=0
�����ɵã�n=<AyBz-ByAz,AxBz-BxAz,AxBy-AyBz>
ת��������ϱ��ʽ��
n=(AyBz-ByAz)*i+(AxBz-BxAz)*j+(AxBy-AyBz)*k
|i  j  k | = (AyBz-ByAz)*i+(AxBz-BxAz)*j+(AxBy-AyBz)*k
|Ax Ay Az|
|Bx By Bz|

��ֻ��һ���鲿��ʼ
n = sin��*i+cos��,Լ��������ʽΪ��������nΪһ��������Ϊ��x��ļнǣ�
���� z1 = sin(A)*i+cos(A) z2 = sin(B)*i+cos(B)
z1*z2 = sin(A)*sin(B)*i*i+sin(A)*cos(B)*i+cos(A)*sin(B)*i+cos(A)*cos(B)
= [sin(A)*cos(B)+cos(A)*sin(B)]*i+cos(A)*cos(B)-sin(A)*sin(B)
= sin(A+B)*i+cos(A+B)
����Լ������֪z1*z2�Ľ��Ϊz1��ת��B�ȵĽ�,�и��ط���Ҫע�⣬��ת������Ϊ
��λ�������������ľ���Լ����ͬ�ˣ�������һ��ϵ��
��չ����Ԫ���У�
n = sin(A)*V+cos(A)
z1 = <0,x,y,z> = <0,v>
z2 = sin(B)*V+cos(B) = <w,u>
z2^-1 = -sin(B)*V+cos(B) = <w,-u>
��Ԫ����������ת����������֤������Ϊ��ת��ʽΪ
Q'=P*Q*P^-1
QΪ����ת��������PΪ��ת��Ԫ����P^-1ΪP�ĵ�����
����P����Ϊ��λ��Ԫ��������P�ĵ���ΪP�Ĺ�����Ԫ��
P^-1*P==1==P*P^-1
P^-1*P*P~==P~==P*P^-1*P~
P^-1*(P*P~)==P~
P^-1*|P|^2==P~
P^-1==P~/|P|^2
����PΪ��λ��������
P^-1==P~ ����P�ĵ���ΪP�Ĺ�����Ԫ����֤�⣬��֤
Q'=P*Q*P^-1
=<w,u>*<0,v>*<w,-u>
�ȼ���ǰ���ʽ��
<w,u>*<0,v>
=(0*w-u*v)+(w*v+0*u+uxv)
=-u*v+(w*v+uxv)
<-u*v,w*v+uxv>*<w,-u>
=<-u*v*w+u*(w*v+uxv),w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
=<-u*v*w+u*w*v+u*uxv),w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
u*uxv u��v��˵Ľ���϶�����u��v����ֱ�ģ�����������u��ˣ����϶���0
���ǣ����ż���
=<0,w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)>
���ڷֽ��鲿
�ֽ�ǰ����Ҫ��һ����ʽax(bxc)=b(a*c)-c(a*b)
w*(w*v+uxv)+u*v*u+(w*v+uxv)x(-u)=
w*(w*v+uxv)+u*v*u+ux(w*v+uxv)=
w^2*v+uxv*w+u*v*u+uxw*v+uxuxv=
w^2*v+2uxv*w+u*v*u+uxuxv=
w^2*v+2uxv*w+u*v*u+u(u*v)-v(u*u)=
(w^2-u*u)*v+2uxv*w+2*u(u*v)
��w=cos(B) u=sin(B)������㣬��
cos(2B)v+(1-cos(2B))(n*v)n+sin(2B)(nxv)
�ɶ�����n����ת��BΪ
cos(B)v+(1-cos(B))(n*v)n+sin(B)(nxv)
����Q'=P*Q*P^-1ʹ��Q��n��ת2B��
����ŷ������Ҫ������ת��Ԫ�������������
*/

NS_ENGIN_END
#endif