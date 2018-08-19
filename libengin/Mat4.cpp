#include "Mat4.h"
#include <propidl.h>
#include "Quaternion.h"
NS_ENGIN_BEGIN
Mat4::Mat4( float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44 ){
	m[0]  = m11; m[1]  = m12; m[2]  = m13; m[3]  = m14;
	m[4]  = m21; m[5]  = m22; m[6]  = m23; m[7]  = m24;
	m[8]  = m31; m[9]  = m32; m[10] = m33; m[11] = m34;
	m[12] = m41; m[13] = m42; m[14] = m43; m[15] = m44;
}

Mat4::Mat4(){
	identify();
}

Mat4::~Mat4(){

}

void Mat4::createTranslation( float dx, float dy, float dz, Mat4& dst ){
#ifdef USE_D3D
	D3DXMatrixTranslation((D3DXMATRIX*)&dst,dx,dy,dz);
#else
	dst._11=1;  dst._12=0;  dst._13=0;  dst._14=0;
	dst._21=0;  dst._22=1;  dst._23=0;  dst._24=0;
	dst._31=0;  dst._32=0;  dst._33=1;  dst._34=0;
	dst._41=dx; dst._42=dy; dst._43=dz; dst._44=1;
#endif
}

void Mat4::createTranslation( const Vec3f&v, Mat4& dst ){
#ifdef USE_D3D
	D3DXMatrixTranslation((D3DXMATRIX*)&dst,v.x,v.y,v.z);
#else
	dst._11=1;  dst._12=0;  dst._13=0;  dst._14=0;
	dst._21=0;  dst._22=1;  dst._23=0;  dst._24=0;
	dst._31=0;  dst._32=0;  dst._33=1;  dst._34=0;
	dst._41=v.x; dst._42=v.y; dst._43=v.z; dst._44=1;
#endif
}

/*
四元数 
w = cos(A/2)
x = sin(A/2)*Nx
y = sin(A/2)*Ny
z = sin(A/2)*Nz
矩阵
Nx*Nx*(1-cosA)+cosA		Nx*Ny*(1-cosA)+Nz*sinA		Nx*Nz*(1-cosA)-Ny*sinA	0
Nx*Ny*(1-cosA)-Nz*sinA  Ny*Ny*(1-cosA)+cosA			Ny*Nz*(1-cosA)+Nx*sinA	0
Nx*Nz*(1-cosA)-Ny*sinA  Ny*Nz*(1-cosA)-Nx*sinA      Nz*Nz*(1-cosA)+cosA		0
0						0							0						1
四元数转矩阵
cosA = 2cos(A/2)*cos(A/2)-1
1-cosA = 1 - 2cos(A/2)*cos(A/2) + 1 = 2 - 2cos(A/2)*cos(A/2)
Nx = x / sin(A/2)
Nx*Nx = x*x / (sin(A/2) * sin(A/2)) = x*x / (1 - cos(A/2)*cos(A/2))
Nx*Nx*(1-cosA) = 
(x*x / 1 - cos(A/2)*cos(A/2))*(2 - 2cos(A/2)*cos(A/2))=
2*x*x
Nx*Nx*(1-cosA)+cosA = 
2*x*x + 2cos(A/2)*cos(A/2)-1 = 
2*x*x + 2*w*w - 1 = 
2*x*x + 2*(1-x*x-y*y-z*z) - 1 = 
2 -2*y*y - 2*z*z - 1 = 
1 - 2*y*y - 2*z*z
同理可得：
1 - 2*y*y-2*z*z    2*x*y + 2*w*z		2*x*z - 2*w*y
2*x*y - 2*w*z      1 - 2*y*y-2*z*z      2*y*z + 2*w*x
2*x*z + 2*w*y	   2*y*z - 2*w*x        1-2x*x-2y*y
*/
void Mat4::createRotation( const Quaternion& q, Mat4& dst ){
#ifdef USE_D3D
	D3DXMatrixRotationQuaternion( (D3DXMATRIX*)&dst, (const D3DXQUATERNION *)&q);
#else
	float yy2 = q.y*q.y*2;
	float zz2 = q.z*q.z*2;
	float xx2 = q.x*q.x*2;
	float wx2 = q.w*q.x*2;
	float wz2 = q.w*q.z*2;
	float wy2 = q.w*q.y*2;
	float xy2 = q.x*q.y*2;
	float xz2 = q.x*q.z*2;
	float yz2 = q.y*q.z*2;
	dst.m[0]  = 1 - yy2 - zz2; 
	dst.m[1]  = xy2 + wz2; 
	dst.m[2]  = xz2 - wy2; 
	dst.m[3]  = 0;
	dst.m[4]  = xy2 - wz2; 
	dst.m[5]  = 1 - yy2 - zz2; 
	dst.m[6]  = yz2 + wx2; 
	dst.m[7]  = 0;
	dst.m[8]  = xz2 + wy2; 
	dst.m[9]  = yz2 - wx2; 
	dst.m[10] = 1- xx2 - yy2; 
	dst.m[11] = 0;
	dst.m[12] = 0; 
	dst.m[13] = 0; 
	dst.m[14] = 0; 
	dst.m[15] = 1;
#endif
}

//欧拉角转化为矩阵
void Mat4::createRotation( const Vec3f& vr,Mat4& dst ){
#ifdef USE_D3D
	D3DXMatrixRotationYawPitchRoll((D3DXMATRIX*)&dst,-vr.y,vr.x,vr.z);
#else
	float cosy=cos(vr.y);float siny=sin(vr.y);
	float cosx=cos(vr.x);float sinx=sin(vr.x);
	float cosz=cos(vr.z);float sinz=sin(vr.z);
	
	dst._11=cosz*cosy+sinz*sinx*siny;  dst._12=sinz*cosx; dst._13=cosz*siny-sinz*sinx*cosy;  dst._14=0;
	dst._21=-sinz*cosy+cosz*sinx*siny; dst._22=cosz*cosx; dst._23=-sinz*siny-cosz*sinx*cosy; dst._24=0;
	dst._31=-cosx*siny;                dst._32=sinx;      dst._33=cosx*cosy;                 dst._34=0;
	dst._41=0;                         dst._42=0;         dst._43=0;                         dst._44=1;
#endif
	//Mat4 mz,mx,my;
	//mz.identify();
	//mz._11=cosz;mz._12=sinz;
	//mz._21=-sinz;mz._22=cosz;
	//mx.identify();
	//mx._22=cosx;mx._23=-sinx;
	//mx._32=sinx;mx._33=cosx;
	//my.identify();
	//my._11=cosy;my._13=siny;
	//my._31=-siny;my._33=cosy;
	//dst=mz*mx;
	//dst=dst*my;
}
           
void Mat4::createScale( float scaleX, float scaleY, float scaleZ, Mat4& dst){
#ifdef USE_D3D
	D3DXMatrixScaling((D3DXMATRIX*)&dst,scaleX,scaleY,scaleZ);
#else
	memset(&dst,0,sizeof Mat4);
	dst.m[0]=scaleX;
	dst.m[5]=scaleY;
	dst.m[10]=scaleZ;
	dst.m[15]=1;
#endif
}

void Mat4::identify(){
#ifdef USE_D3D
	D3DXMatrixIdentity((D3DXMATRIX*)this);
#else
	memset(m,0,sizeof Mat4);
	m[0]=1;
	m[5]=1;
	m[10]=1;
	m[15]=1;
#endif
}

void Mat4::transpose(){
#ifdef USE_D3D
	D3DXMatrixTranspose((D3DXMATRIX*)this,(D3DXMATRIX*)this);
#else
	float temp;
	SWAP(temp,m[1], m[4]);
	SWAP(temp,m[2], m[8]);
	SWAP(temp,m[3], m[12]);
	SWAP(temp,m[6], m[9]);
	SWAP(temp,m[7], m[13]);
	SWAP(temp,m[11],m[14]);
#endif
}

Mat4 Mat4::getInverse()const
{
	// computes the inverse of a 4x4, assumes that the last
	// column is [0 0 0 1]t
	Mat4 mi;
	float det =  ( _11 * ( _22 * _33 - _23 * _32 ) -
		_12 * ( _21 * _33 - _23 * _31 ) +
		_13 * ( _21 * _32 - _22 * _31 ) );

	// test determinate to see if it's 0
	if (fabs(det) < E5)
		return mi;

	float det_inv  = 1.0f / det;

	mi._11 =  det_inv * ( _22 * _33 - _23 * _32 );
	mi._12 = -det_inv * ( _12 * _33 - _13 * _32 );
	mi._13 =  det_inv * ( _12 * _23 - _13 * _22 );
	mi._14 = 0.0f; // always 0

	mi._21 = -det_inv * ( _21 * _33 - _23 * _31 );
	mi._22 =  det_inv * ( _11 * _33 - _13 * _31 );
	mi._23 = -det_inv * ( _11 * _23 - _13 * _21 );
	mi._24 = 0.0f; // always 0

	mi._31 =  det_inv * ( _21 * _32 - _22 * _31 );
	mi._32 = -det_inv * ( _11 * _32 - _12 * _31 );
	mi._33 =  det_inv * ( _11 * _22 - _12 * _21 );
	mi._34 = 0.0f; // always 0

	mi._41 = -( _41 * mi._11 + _42 * mi._21 + _43 * mi._31 );
	mi._42 = -( _41 * mi._12 + _42 * mi._22 + _43 * mi._32 );
	mi._43 = -( _41 * mi._13 + _42 * mi._23 + _43 * mi._33 );
	mi._44 = 1.0f; // always 0

	// return success 
	return mi;
} 

NS_ENGIN_END