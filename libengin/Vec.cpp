#include "Vec.h"
#include "math.h"
#include "mathUtil.h"

NS_ENGIN_BEGIN
//�����������ͶӰ
float Get2VecAngle(const Vec3f& v0,const Vec3f& v1){
	float model=v0.getModel()*v1.getModel();
	if(model<=E5)
		return 0.0f;
	float cosAngle = Vec3f_Mul(v0,v1)/model;
	return acos(cosAngle);
}

float Get2VecCos(const Vec3f& v0,const Vec3f& v1){
	float model=v0.getModel()*v1.getModel();
	if(model<=E5)
		return 0.0f;
	return Vec3f_Mul(v0,v1)/model;
}

//�������U��V�ϵ�ͶӰ����
void GetProjVec(const Vec3f& u,const Vec3f& v,Vec3f& dst){
	float model=v.getModel();
	if(model<=E5){
		dst.zero();
		return;
	}
	float t=Vec3f_Mul(u,v)/pow(model,2);
	Vec3f_Mul(v,t,dst);
}

void crossMultiply(const Vec3f& v0,const Vec3f& v1,Vec3f& dst){
	dst.x = v0.y*v1.z-v0.z*v1.y;
	dst.y = -v0.x*v1.z+v0.z*v1.x;
	dst.z = v0.x*v1.y-v0.y*v1.x;
	dst.identify();
}

void crossMultiply_NO_IDENTIFY(const Vec3f& v0,const Vec3f& v1,Vec3f& dst){
	dst.x = v0.y*v1.z-v0.z*v1.y;
	dst.y = -v0.x*v1.z+v0.z*v1.x;
	dst.z = v0.x*v1.y-v0.y*v1.x;
}

void GetNormalVec_NO_IDENTIFY(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Vec3f& v){
	static Vec3f v1;
	static Vec3f v2;
	Vec3f_Sub(p1,p0,v1);
	Vec3f_Sub(p2,p1,v2);
	crossMultiply_NO_IDENTIFY(v2,v1,v);
}

//����ƽ�������㣬��÷���������ʱ�������,��������ϵ��Ҫ�����ֶ����жϲ�˷���
void GetNormalVec(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Vec3f& v){
	static Vec3f v1;
	static Vec3f v2;
	Vec3f_Sub(p1,p0,v1);
	Vec3f_Sub(p2,p1,v2);
	crossMultiply(v2,v1,v);
}

float Vec3f::getModelFast() const{
	return Fast_Distance_3D(x,y,z);
}

NS_ENGIN_END