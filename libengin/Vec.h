#ifndef __VEC2_H
#define __VEC2_H
#include "macros.h"
#include "memory.h"
#include "math.h"
NS_ENGIN_BEGIN
class Vec2f;
class ENGIN_DLL Vec3f{
public:
	union{
		float m[3];
		struct{
			float x;
			float y;
			float z;
		};
	};
	Vec3f(){memset(m,0,sizeof(m));}
	Vec3f(float vx, float vy,float vz=0) :x(vx), y(vy), z(vz){}
	Vec3f(const Vec3f&v){ x = v.x; y = v.y; z = v.z;}
	Vec2f& v2f(){ return (Vec2f&)*this;}
	float getModelFast() const;
	float getModel() const {return sqrt(x*x+y*y+z*z);}
	Vec3f& identify(){
		float norm=getModel();
		if(norm>E5){
			x/=norm;
			y/=norm;
			z/=norm;
		}else{
			x=y=z=1;
		}
		return *this;
	}
	void zero(){memset(m,0,sizeof(m));}
};

class ENGIN_DLL Vec4f{
public:
	union{
		struct{
			float x;
			float y;
			float z;
			float w;
		};
		float m[4];
	};
	Vec4f():x(0.0f),y(0.0f),z(0.0f),w(1.0f){}
	Vec4f(float _x,float _y,float _z):x(_x),y(_y),z(_z),w(1.0f){}
	Vec4f(float _x,float _y,float _z,float _w):x(_x),y(_y),z(_z),w(_w){}
	Vec4f(const Vec4f&vv){memcpy(m,vv.m,sizeof m);}
	Vec3f& v3f(){return (Vec3f&)*this;}
	Vec2f& v2f(){return (Vec2f&)*this;}
	void homogeneous(){
		if(w<E5||w-1.0f<=E5){
			return;
		}
		x/=w;
		y/=w;
		z/=w;
		w=1.0f;
	}
};

class ENGIN_DLL Vec2f{
public:
	union{
		float m[2];
		struct{
			float x;
			float y;
		};
		struct{
			float u;
			float v;
		};
	};
	Vec2f(){memset(m,0,sizeof(m));}
	Vec2f(float vx, float vy) :x(vx), y(vy){}
};

inline void ENGIN_DLL Vec2f_Add(const Vec2f&v0,const Vec2f& v1,Vec2f& dst){
	dst.x=v0.x+v1.x;
	dst.y=v0.y+v1.y;
}

inline void ENGIN_DLL Vec2f_Sub(const Vec2f&v0,const Vec2f& v1,Vec2f& dst){
	dst.x=v0.x-v1.x;
	dst.y=v0.y-v1.y;
}

class ENGIN_DLL Vec2i{
public:
	union{
		int m[2];
		struct{
			int x;
			int y;
		};
	};
	Vec2i(){memset(m,0,sizeof(m));}
	Vec2i(int vx, int vy) :x(vx), y(vy){}
	Vec2i(const Vec2i& v):x(v.x),y(v.y){}
};

class ENGIN_DLL Vec2FIXP16{
public:
	union{
		FIXP16 m[2];
		struct{
			FIXP16 x;
			FIXP16 y;
		};
		struct{
			FIXP16 u;
			FIXP16 v;
		};
	};
	Vec2FIXP16(){memset(m,0,sizeof(m));}
	Vec2FIXP16(FIXP16 vx, FIXP16 vy) :x(vx), y(vy){}
	Vec2FIXP16(const Vec2i& v):x(INT_2_FIXP16(v.x)),y(INT_2_FIXP16(v.y)){}
	Vec2FIXP16(const Vec2f& v):x(FLOAT_2_FIXP16(v.x)),y(FLOAT_2_FIXP16(v.y)){}
};

inline void ENGIN_DLL Vec2i_Mul(const Vec2i&v0,float k,Vec2i& dst){
	dst.x=(int)(v0.x*k);
	dst.y=(int)(v0.y*k);
}

float ENGIN_DLL Get2VecAngle(const Vec3f& v0,const Vec3f& v1);
float ENGIN_DLL Get2VecCos(const Vec3f& v0,const Vec3f& v1);
void ENGIN_DLL GetProjVec(const Vec3f& u,const Vec3f& v,Vec3f& dst);
void ENGIN_DLL crossMultiply(const Vec3f& v0,const Vec3f& v1,Vec3f& dst);
void ENGIN_DLL GetNormalVec(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Vec3f& dst);
void ENGIN_DLL GetNormalVec_NO_IDENTIFY(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Vec3f& v);

inline void ENGIN_DLL Vec4f_Sub(const Vec4f& v0,const Vec4f& v1,Vec4f& dst){
	dst.x=v0.x-v1.x;
	dst.y=v0.y-v1.y;
	dst.z=v0.z-v1.z;
}

inline void ENGIN_DLL Vec4f_Sub(const Vec4f& v0,const Vec4f& v1,Vec3f& dst){
	dst.x=v0.x-v1.x;
	dst.y=v0.y-v1.y;
	dst.z=v0.z-v1.z;
}

inline void ENGIN_DLL Vec4f_Mul(const Vec4f& v0,const Vec3f& v1,Vec4f& dst){
	dst.x=v0.x*v1.x;
	dst.y=v0.y*v1.y;
	dst.z=v0.z*v1.z;
}

inline float ENGIN_DLL Vec3f_Mul(const Vec3f& v0,const Vec3f& v1){
	return v0.x*v1.x+
		   v0.y*v1.y+
		   v0.z*v1.z;
}

inline void ENGIN_DLL Vec3f_Mul(const Vec3f& v0,float k,Vec3f& dst){
	dst.x=v0.x*k;
	dst.y=v0.y*k;
	dst.z=v0.z*k;
}

inline void ENGIN_DLL Vec3f_Sub(const Vec3f& v0,const Vec3f& v1,Vec3f& dst){
	dst.x=v0.x-v1.x;
	dst.y=v0.y-v1.y;
	dst.z=v0.z-v1.z;
}

inline void ENGIN_DLL Vec3f_Add(const Vec3f& v0,const Vec3f& v1,Vec3f& dst){
	dst.x=v0.x+v1.x;
	dst.y=v0.y+v1.y;
	dst.z=v0.z+v1.z;
}

struct FVec4f{
	union{
		float m[12];
		struct{
			float x,y,z,w;       //位置坐标
			float nx,ny,nz,nw;   //法线
			float u,v;           //纹理坐标
			float c;             //顶点颜色
			int   attr;          //顶点属性
		};
	};
	FVec4f(){memset(m,0,48);}
	FVec4f(float _x,float _y,float _z,float _c):
	x(_x),y(_y),z(_z),c(_c){}
	Vec4f& v4f(){return (Vec4f&)(*m);}
	Vec3f& v3f(){return (Vec3f&)(*m);}
	Vec2f& v2f(){return (Vec2f&)(*m);}
	Vec4f& n4f(){return (Vec4f&)*(m+4);}
	Vec3f& n3f(){return (Vec3f&)*(m+4);}
	Vec2f& t2f(){return (Vec2f&)*(m+8);}
};

NS_ENGIN_END
#endif