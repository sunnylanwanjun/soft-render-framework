#ifndef __MATERIAL_H
#define __MATERIAL_H
#include "macros.h"
#include "memory.h"
NS_ENGIN_BEGIN
// states for objects
#define MATL_STATE_ACTIVE			  0x0001

//多边形属性										//8 4 2 1 8 4 2 1 8 4 2 1 8 4 2 1
#define MATL_ATTR_SIDE_2				0x0001	//							  1
#define MATL_ATTR_TRANSPARENT			0x0002	//							1
#define MATL_ATTR_COLOR_8				0x0004	//						  1
#define MATL_ATTR_COLOR_16				0x0008	//						1
#define MATL_ATTR_COLOR_32				0x0010	//					  1
#define MATL_ATTR_SHADE_PURE            0x0020	//					1
#define MATL_ATTR_SHADE_CONSTANT        0x0020 	//					1
#define MATL_ATTR_SHADE_FLAT            0x0040	//				  1
#define MATL_ATTR_SHADE_GOURAUD         0x0080	//				1
#define MATL_ATTR_SHADE_PHONG           0x0100	//			  1
#define MATL_ATTR_SHADE_FASTPHONG       0x0100 	//			  1
#define MATL_ATTR_SHADE_TEXTURE         0x0200 	//			1

struct ENGIN_DLL RGBA{
	union{
		BYTE m[4];
		struct{
			BYTE a,b,g,r;
		};
		int rgba;
	};
};

struct ENGIN_DLL ARGB{
	union{
		BYTE m[4];
		struct{
			BYTE b,g,r,a;
		};
		int argb;
	};
};

inline void ENGIN_DLL RGBA_Add(RGBA& c0,RGBA& c1,RGBA& dst){
	dst.r=LIMIT(c0.r+c1.r,0xff);
	dst.g=LIMIT(c0.g+c1.g,0xff);
	dst.b=LIMIT(c0.b+c1.b,0xff);
}

inline void ENGIN_DLL RGBA_MUL(RGBA& c,float k,RGBA& dst){
	dst.r=LIMIT(int(c.r*k),0xff);
	dst.g=LIMIT(int(c.g*k),0xff);
	dst.b=LIMIT(int(c.b*k),0xff);
}

inline void ENGIN_DLL RGBA_MUL(RGBA& c0,RGBA& c1,float k,RGBA& dst){
	dst.r=LIMIT((int(c0.r*c1.r*k)>>8),0xff);
	dst.g=LIMIT((int(c0.g*c1.g*k)>>8),0xff);
	dst.b=LIMIT((int(c0.b*c1.b*k)>>8),0xff);
}

inline void ENGIN_DLL RGBA_MUL(RGBA& c0,RGBA& c1,RGBA& dst){
	dst.r=LIMIT(((c0.r*c1.r)>>8),0xff);
	dst.g=LIMIT(((c0.g*c1.g)>>8),0xff);
	dst.b=LIMIT(((c0.b*c1.b)>>8),0xff);
}

inline void ENGIN_DLL RGBA_WRAP(RGBA& c){
	c.r=LIMIT((c.r&0xff),0xff);
	c.g=LIMIT((c.g&0xff),0xff);
	c.b=LIMIT((c.b&0xff),0xff);
}

struct ENGIN_DLL BITMAP{
	int attr;
	int state;
	int x,y,width,height;
	BYTE* data;
	size_t dataLen;
	int bpp;
	BITMAP(){memset(this,0,sizeof BITMAP);}
	BITMAP(const BITMAP* obj){
		memcpy(this,obj,sizeof BITMAP);
		this->data=new BYTE[dataLen];
		memcpy(this->data,obj->data,dataLen);
	}
	~BITMAP(){delete[] data;}
};

struct ENGIN_DLL Material{
public:
	int attr;
	char name[64];
	int state;
	int id;
	RGBA color;
	float ka,kd,ks,power;//power暂时用不到
	RGBA ra,rd,rs;
	char textureName[128];
	BITMAP texture;
	Material(){memset(this,0,sizeof Material);}
	~Material(){}
	void reset();
};

#define MAX_MATERIALS                     256
class ENGIN_DLL MaterialMgr{
public:
	MaterialMgr();
	~MaterialMgr();
	Material materials[MAX_MATERIALS];
	int num_materials;
	static MaterialMgr* getInstance();
	static MaterialMgr* _instance;
	static void destroyInstance();
	void reset();
};
NS_ENGIN_END
#endif