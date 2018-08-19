#include "Quaternion.h"
#include "math.h"
#include "mathUtil.h"
NS_ENGIN_BEGIN

Quaternion::Quaternion():x(0.0f),y(0.0f),z(0.0f),w(1.0f){

}

Quaternion::~Quaternion(){

}

/*
  由四元数的轴角对公式进行推导
  p = cos(A/2)+sin(A/2)*i+sin(A/2)*j+sin(A/2)*k;
  其中ijk是任意旋转轴分解为xyz轴后的分量
  而欧拉角是绕三个轴的角度，于是绕y轴的四元数为
  py = cos(h/2)   +0*i         +sin(h/2)*j    +0*k;
  px = cos(h/2)   +sin(h/2)*i  +0*j           +0*k;
  pz = cos(h/2)   +0*i         +0*j           +sin(h/2)*k;
  由两四元数相乘公式，
  w = w1*w2 - x1*x2 - y1*y2 - z1*z2
  x = w1*x2 + x1*w2 + y1*z2 - z1*y2
  y = w1*y2 + y1*w2 + z1*x2 - x1*z2
  z = w1*z2 + z1*w2 + x1*y2 - y1*x2
  可得
  py*px 中
  w = cos(h/2)*cos(h/2)
  x = cos(h/2)*sin(h/2)
  y = sin(h/2)*cos(h/2)
  z = sin(h/2)*sin(h/2)
  再乘上pz可得
  w = cos(h/2)*cos(p/2)*cos(b/2) - sin(h/2)*sin(p/2)*sin(b/2)
  x = cos(h/2)*sin(p/2)*cos(b/2) - sin(h/2)*cos(p/2)*sin(b/2)
  y = sin(h/2)*cos(p/2)*cos(b/2) - cos(h/2)*sin(p/2)*sin(b/2)
  z = cos(h/2)*cos(p/2)*sin(b/2) - sin(h/2)*sin(p/2)*cos(b/2) 
  顺序 heading - pitch -bank 也就是 yxz
*/
void Quaternion::eulerToQuat( float heading, float pitch, float bank ){
#ifdef USE_D3D
	D3DXQuaternionRotationYawPitchRoll( (D3DXQUATERNION*)this, heading,pitch, bank);
#else
	float halfh = heading/2;float halfp = pitch/2;float halfb = bank/2;
	float cosh = cos(halfh);float sinh = sin(halfh);
	float cosp = cos(halfp);float sinp = sin(halfp);
	float cosb = cos(halfb);float sinb = sin(halfb);
	w = cosh*cosp*cosb+sinh*sinp*sinb;
	x = cosh*sinp*cosb+sinh*cosp*sinb;
	y = sinh*cosp*cosb-cosh*sinp*sinb;
	z = cosh*cosp*sinb-sinh*sinp*cosb;
#endif
}

void Quaternion::vectorThetaToQuat( Vec3f& normal,float theta ){
	FDiv(theta,2,theta);
	float sinVal=sin(theta);
	w = cos(theta);
	x = sinVal*normal.x;
	y = sinVal*normal.y;
	z = sinVal*normal.z;
}

void Quaternion::getVecorTheta( Vec3f& normal,float& theta ){
	theta=acos(w);
	float sinTheta=sin(theta);
	if(ABS(sinTheta)<=E5){
		normal.zero();
	}
	else{
		normal.x=x/sinTheta;
		normal.y=y/sinTheta;
		normal.z=z/sinTheta;
	}
	theta*=2;
}

void Quaternion::rotate( Vec3f& point,Vec3f& normal,float theta ){
	vectorThetaToQuat(normal,theta);
	this->normal();
	Quaternion quatc;
	getConjugate(quatc);
	Quaternion rotateP=point;
	Quaternion dst;
	Quat_Mul(*this,rotateP,quatc,dst);
	point = dst.qV;
}

NS_ENGIN_END