#ifndef __MAT3_H
#define __MAT3_H
#include "macros.h"
#include "Vec.h"
#include "d3dh.h"
NS_ENGIN_BEGIN
class Quaternion;
//使用矩阵可以减少很多运算，对于单个坐标的旋转，缩放，位移，貌似运算量还更大，但从整个引擎上来说，
//由于子节点要根据父节点进行变换，如果使用直接运算，需要计算的层次就太多了，而矩阵有个优点，就是可以把运算
//进行叠加，最后使用一个矩阵来表达所有的变换。
class ENGIN_DLL Mat4{
public:
	Mat4(float m11, float m12, float m13, float m14,
		 float m21, float m22, float m23, float m24,
		 float m31, float m32, float m33, float m34,
		 float m41, float m42, float m43, float m44);
	Mat4();
	~Mat4();
	void identify();
	void transpose();
	Mat4 getInverse()const;
	//static 接口
	static void createTranslation(float xTranslation, float yTranslation, float zTranslation, Mat4&  dst);
	static void createTranslation(const Vec3f& v,Mat4& dst);
	static void createRotation(const Quaternion& q, Mat4& dst);
	static void createScale(float scaleX,float scaleY,float scaleZ, Mat4& dst);
	static void createRotation(const Vec3f& vr,Mat4& dst);
public:
	union{
		float m[16];
		struct{
			float _11; float _12; float _13; float _14;
			float _21; float _22; float _23; float _24;
			float _31; float _32; float _33; float _34;
			float _41; float _42; float _43; float _44;
		};
	};
};

inline void Mat4_Mul(const Vec4f& point ,const Mat4& mat,Vec4f& dst){
#ifdef USE_D3D
	D3DXVec4Transform((D3DXVECTOR4*)&dst,(const D3DXVECTOR4*)&point,(D3DXMATRIX*)&mat);
#else
	Vec4f res;
	res.x = point.x*mat.m[0]+point.y*mat.m[4]+point.z*mat.m[8]+ point.w*mat.m[12];
	res.y = point.x*mat.m[1]+point.y*mat.m[5]+point.z*mat.m[9]+ point.w*mat.m[13];
	res.z = point.x*mat.m[2]+point.y*mat.m[6]+point.z*mat.m[10]+point.w*mat.m[14];
	res.w = point.x*mat.m[4]+point.y*mat.m[7]+point.z*mat.m[11]+point.w*mat.m[15];
	dst=res;
#endif
}

//以下函数当目标与操作数不是同一对象时，使用
inline void Mat4_Mul(const Vec3f& src,const Mat4& mat,Vec3f& dst){
#ifdef USE_D3D
	D3DXVec3TransformCoord((D3DXVECTOR3*)&dst,(D3DXVECTOR3*)&src,(D3DXMATRIX*)&mat);
#else
	if(&dst==&src){
		Vec3f res;
		res.x = point.x*mat.m[0]+point.y*mat.m[4]+point.z*mat.m[8]+mat.m[12];
		res.y = point.x*mat.m[1]+point.y*mat.m[5]+point.z*mat.m[9]+mat.m[13];
		res.z = point.x*mat.m[2]+point.y*mat.m[6]+point.z*mat.m[10]+mat.m[14];
		dst=res;
	}else{
		dst.x =src.x*mat.m[0]+src.y*mat.m[4]+src.z*mat.m[8]+mat.m[12];
		dst.y =src.x*mat.m[1]+src.y*mat.m[5]+src.z*mat.m[9]+mat.m[13];
		dst.z =src.x*mat.m[2]+src.y*mat.m[6]+src.z*mat.m[10]+mat.m[14];
	}
#endif
}

inline void Mat4_Mul(const Mat4& mat0,const Mat4& mat1,Mat4& dst){
#ifdef USE_D3D
	D3DXMatrixMultiply((D3DXMATRIX*)&dst,(D3DXMATRIX*)&mat0,(D3DXMATRIX*)&mat1);
#else
	if((&dst!=&mat0)&&(&dst!=&mat1)){
		dst.m[0]  = mat0.m[0] *mat1.m[0]  +mat0.m[1] *mat1.m[4]  +mat0.m[2] *mat1.m[8]   +mat0.m[3] *mat1.m[12];
		dst.m[1]  = mat0.m[0] *mat1.m[1]  +mat0.m[1] *mat1.m[5]  +mat0.m[2] *mat1.m[9]   +mat0.m[3] *mat1.m[13];
		dst.m[2]  = mat0.m[0] *mat1.m[2]  +mat0.m[1] *mat1.m[6]  +mat0.m[2] *mat1.m[10]  +mat0.m[3] *mat1.m[14];
		dst.m[3]  = mat0.m[0] *mat1.m[3]  +mat0.m[1] *mat1.m[7]  +mat0.m[2] *mat1.m[11]  +mat0.m[3] *mat1.m[15];	
		dst.m[4]  = mat0.m[4] *mat1.m[0]  +mat0.m[5] *mat1.m[4]  +mat0.m[6] *mat1.m[8]   +mat0.m[7] *mat1.m[12];
		dst.m[5]  = mat0.m[4] *mat1.m[1]  +mat0.m[5] *mat1.m[5]  +mat0.m[6] *mat1.m[9]   +mat0.m[7] *mat1.m[13];
		dst.m[6]  = mat0.m[4] *mat1.m[2]  +mat0.m[5] *mat1.m[6]  +mat0.m[6] *mat1.m[10]  +mat0.m[7] *mat1.m[14];
		dst.m[7]  = mat0.m[4] *mat1.m[3]  +mat0.m[5] *mat1.m[7]  +mat0.m[6] *mat1.m[11]  +mat0.m[7] *mat1.m[15];
		dst.m[8]  = mat0.m[8] *mat1.m[0]  +mat0.m[9] *mat1.m[4]  +mat0.m[10]*mat1.m[8]   +mat0.m[11]*mat1.m[12];
		dst.m[9]  = mat0.m[8] *mat1.m[1]  +mat0.m[9] *mat1.m[5]  +mat0.m[10]*mat1.m[9]   +mat0.m[11]*mat1.m[13];
		dst.m[10] = mat0.m[8] *mat1.m[2]  +mat0.m[9] *mat1.m[6]  +mat0.m[10]*mat1.m[10]  +mat0.m[11]*mat1.m[14];
		dst.m[11] = mat0.m[8] *mat1.m[3]  +mat0.m[9] *mat1.m[7]  +mat0.m[10]*mat1.m[11]  +mat0.m[11]*mat1.m[15];
		dst.m[12] = mat0.m[12]*mat1.m[0]  +mat0.m[13]*mat1.m[4]  +mat0.m[14]*mat1.m[8]   +mat0.m[15]*mat1.m[12];
		dst.m[13] = mat0.m[12]*mat1.m[1]  +mat0.m[13]*mat1.m[5]  +mat0.m[14]*mat1.m[9]   +mat0.m[15]*mat1.m[13];
		dst.m[14] = mat0.m[12]*mat1.m[2]  +mat0.m[13]*mat1.m[6]  +mat0.m[14]*mat1.m[10]  +mat0.m[15]*mat1.m[14];
		dst.m[15] = mat0.m[12]*mat1.m[3]  +mat0.m[13]*mat1.m[7]  +mat0.m[14]*mat1.m[11]  +mat0.m[15]*mat1.m[15];
	}else{
		Mat4 ret;
		ret.m[0]  = mat0.m[0] *mat1.m[0]  +mat0.m[1] *mat1.m[4]  +mat0.m[2] *mat1.m[8]   +mat0.m[3] *mat1.m[12];
		ret.m[1]  = mat0.m[0] *mat1.m[1]  +mat0.m[1] *mat1.m[5]  +mat0.m[2] *mat1.m[9]   +mat0.m[3] *mat1.m[13];
		ret.m[2]  = mat0.m[0] *mat1.m[2]  +mat0.m[1] *mat1.m[6]  +mat0.m[2] *mat1.m[10]  +mat0.m[3] *mat1.m[14];
		ret.m[3]  = mat0.m[0] *mat1.m[3]  +mat0.m[1] *mat1.m[7]  +mat0.m[2] *mat1.m[11]  +mat0.m[3] *mat1.m[15];	
		ret.m[4]  = mat0.m[4] *mat1.m[0]  +mat0.m[5] *mat1.m[4]  +mat0.m[6] *mat1.m[8]   +mat0.m[7] *mat1.m[12];
		ret.m[5]  = mat0.m[4] *mat1.m[1]  +mat0.m[5] *mat1.m[5]  +mat0.m[6] *mat1.m[9]   +mat0.m[7] *mat1.m[13];
		ret.m[6]  = mat0.m[4] *mat1.m[2]  +mat0.m[5] *mat1.m[6]  +mat0.m[6] *mat1.m[10]  +mat0.m[7] *mat1.m[14];
		ret.m[7]  = mat0.m[4] *mat1.m[3]  +mat0.m[5] *mat1.m[7]  +mat0.m[6] *mat1.m[11]  +mat0.m[7] *mat1.m[15];
		ret.m[8]  = mat0.m[8] *mat1.m[0]  +mat0.m[9] *mat1.m[4]  +mat0.m[10]*mat1.m[8]   +mat0.m[11]*mat1.m[12];
		ret.m[9]  = mat0.m[8] *mat1.m[1]  +mat0.m[9] *mat1.m[5]  +mat0.m[10]*mat1.m[9]   +mat0.m[11]*mat1.m[13];
		ret.m[10] = mat0.m[8] *mat1.m[2]  +mat0.m[9] *mat1.m[6]  +mat0.m[10]*mat1.m[10]  +mat0.m[11]*mat1.m[14];
		ret.m[11] = mat0.m[8] *mat1.m[3]  +mat0.m[9] *mat1.m[7]  +mat0.m[10]*mat1.m[11]  +mat0.m[11]*mat1.m[15];
		ret.m[12] = mat0.m[12]*mat1.m[0]  +mat0.m[13]*mat1.m[4]  +mat0.m[14]*mat1.m[8]   +mat0.m[15]*mat1.m[12];
		ret.m[13] = mat0.m[12]*mat1.m[1]  +mat0.m[13]*mat1.m[5]  +mat0.m[14]*mat1.m[9]   +mat0.m[15]*mat1.m[13];
		ret.m[14] = mat0.m[12]*mat1.m[2]  +mat0.m[13]*mat1.m[6]  +mat0.m[14]*mat1.m[10]  +mat0.m[15]*mat1.m[14];
		ret.m[15] = mat0.m[12]*mat1.m[3]  +mat0.m[13]*mat1.m[7]  +mat0.m[14]*mat1.m[11]  +mat0.m[15]*mat1.m[15];
		dst=ret;
	}
#endif
}

inline bool Mat4_IsIdentity(const Mat4& m){
#ifdef USE_D3D
	return D3DXMatrixIsIdentity((const D3DXMATRIX*)&m)?true:false;
#else
	for(int i=0;i<16;i++){
		if(i%5==0){
			if(ABS(m[i]-1.0f)>E5)
				return false;
		}else if(ABS(m[i])>E5){
			return false;
		}
	}
	return true;
#endif
}

NS_ENGIN_END
#endif