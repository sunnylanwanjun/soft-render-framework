#ifndef __T3DLIB1_H
#define __T3DLIB1_H
#include "macros.h"
#include "Geometry.h"
#include <iostream>
#include <fstream>
#include <ddraw.h>
#include <tchar.h>
#include <shlwapi.h>
#include <functional>
#include "Material.h"
NS_ENGIN_BEGIN
struct RGBA;
extern LPDIRECTDRAW7 DirectDraw7Init(HWND hwnd, bool isFullScreen, DWORD width, DWORD height, DWORD bpp);
extern LPDIRECTDRAWPALETTE RandomPaletteInit(LPDIRECTDRAW7 lpdd7);
extern LPDIRECTDRAWSURFACE7 DirectDrawSurfaceInit_Primary(LPDIRECTDRAW7 lpdd7);
extern void DirectDrawSurfaceInit_Primary(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7* lplpddsprimary, LPDIRECTDRAWSURFACE7* lplpddsback);
extern LPDIRECTDRAWSURFACE7 DirectDrawSurfaceInit_Common(LPDIRECTDRAW7 lpdd7, int width, int height, int mem_flags=DDSCAPS_VIDEOMEMORY, size_t low_color=0, size_t high_color=0);
extern LPDIRECTDRAWCLIPPER DirectDrawClipperInit(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, int numRect, LPRECT rectList);
extern LPDIRECTDRAWCLIPPER DirectDrawClipperInit(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, HWND hwnd);
extern void GetBounds(int numRect, LPRECT rectList, RECT& bounds);
extern bool GetBmp(BYTE* src, size_t srcLen, BYTE*& dst, size_t& dstLen, int x, int y, int& w, int& h);
extern void BltBmpData32(DWORD*src, int srcW, int srcH, DWORD* dest, int destW, int destH, LONG pitch32);
extern void Clean32(PDWORD pMem, UINT width, UINT height, LONG pitch32);
extern void reverseRGBA(BYTE* data, size_t size);
extern void ENGIN_DLL DrawLine(DWORD* pMem, 
							   LONG pitch32, 
							   const Vec2FIXP16& wndOrigin,
							   const RangeFIXP16& wndRange,
							   DWORD color,
							   Vec2FIXP16 p0,
							   Vec2FIXP16 p1);
extern void DrawTriangle(DWORD* pMem, 
						 LONG pitch32, 
						 const Vec2FIXP16& wndOrigin, 
						 const RangeFIXP16& wndRange, 
						 DWORD color, 
						 Vec2FIXP16 p0, 
						 Vec2FIXP16 p1, 
						 Vec2FIXP16 p2,
						 bool isWire=false);
extern void DrawPolygon2D_Line(DWORD* pMem, LONG pitch32, const Vec2FIXP16& wndOrigin, const RangeFIXP16& wndRange, DWORD color, int numVer, Vec3f* vList);
extern void DrawPolygon2D_Fill(DWORD* pMem, 
							   LONG pitch32, 
							   const Vec2FIXP16& wndOrigin, 
							   const RangeFIXP16& wndRange, 
							   DWORD color, 
							   int numVer, 
							   Vec3f* vList);
extern bool GetBmp(const char* fileName,BITMAP** ppBmp);
extern void DrawGouraudTriangle(DWORD* pMem, 
								LONG pitch32, 
								const Vec2FIXP16& wndOrigin, 
								const RangeFIXP16& wndRange, 
								Vec2FIXP16 p0, 
								Vec2FIXP16 p1, 
								Vec2FIXP16 p2,
								DWORD c0,DWORD c1,DWORD c2,
								bool isWire=false);
extern void DrawTextureTriangle(DWORD* pMem, 
								LONG pitch32, 
								const Vec2FIXP16& wndOrigin, 
								const RangeFIXP16& wndRange, 
								Vec2FIXP16 p0, 
								Vec2FIXP16 p1, 
								Vec2FIXP16 p2,
								Vec2FIXP16 t0,
								Vec2FIXP16 t1,
								Vec2FIXP16 t2,
								float mr,float mg,float mb,
								const BITMAP* texture);
extern void ENGIN_DLL DrawGouraudLine(DWORD* pMem, 
							LONG pitch32, 
							const Vec2FIXP16& wndOrigin, 
							const RangeFIXP16& wndRange, 
							Vec2FIXP16 p0, 
							Vec2FIXP16 p1,
							DWORD c0,DWORD c1);
extern void ENGIN_DLL DrawLightingLine(DWORD* pMem, 
					  LONG pitch32, 
					  const Vec2FIXP16& wndOrigin,
					  const RangeFIXP16& wndRange,
					  DWORD color,
					  Vec2f* vList,
					  int vNum,
					  int seg);
///////////////////内联函数/////////////////////

inline void ENGIN_DLL Color_Alpha(DWORD* c0,float a0,DWORD* c1,int len,DWORD* dst){
	float a1=1-a0;
	for(int i=0;i<len;i++){
		DWORD r=(DWORD)(RGB32BIT_R(c0[i])*a0+RGB32BIT_R(c1[i])*a1);
		DWORD g=(DWORD)(RGB32BIT_G(c0[i])*a0+RGB32BIT_G(c1[i])*a1);
		DWORD b=(DWORD)(RGB32BIT_B(c0[i])*a0+RGB32BIT_B(c1[i])*a1);
		dst[i]=RGB32BIT(0,r,g,b);
	}
}

inline void ENGIN_DLL Color_Add(DWORD* c0,DWORD* c1,int len,DWORD* dst){
	for(int i=0;i<len;i++){
		DWORD r=RGB32BIT_R(c0[i])+RGB32BIT_R(c1[i]);
		if(r>0xff)continue;
		DWORD g=RGB32BIT_G(c0[i])+RGB32BIT_G(c1[i]);
		if(g>0xff)continue;
		DWORD b=RGB32BIT_B(c0[i])+RGB32BIT_B(c1[i]);
		if(b>0xff)continue;
		dst[i]=RGB32BIT(0,r,g,b);
	}
}

inline void ENGIN_DLL Color_Add(DWORD* c,long color,int len,DWORD* dst){
	int k=CMP(color,0);
	color=ABS(color);
	DWORD r0=RGB32BIT_R(color);
	DWORD g0=RGB32BIT_G(color);
	DWORD b0=RGB32BIT_B(color);
	for(int i=0;i<len;i++){
		int r=RGB32BIT_R(c[i])+r0*k;
		if(r>0xff||r<0)continue;
		int g=RGB32BIT_G(c[i])+g0*k;
		if(g>0xff||g<0)continue;
		int b=RGB32BIT_B(c[i])+b0*k;
		if(b>0xff||b<0)continue;
		dst[i]=RGB32BIT(0,r,g,b);
	}
}

inline void ENGIN_DLL Color_Mul(DWORD* c0,DWORD* c1,int len,DWORD* dst){
	for(int i=0;i<len;i++){
		DWORD r=RGB32BIT_R(c0[i])*RGB32BIT_R(c1[i]);
		if(r>0xff)continue;
		DWORD g=RGB32BIT_G(c0[i])*RGB32BIT_G(c1[i]);
		if(g>0xff)continue;
		DWORD b=RGB32BIT_B(c0[i])*RGB32BIT_B(c1[i]);
		if(b>0xff)continue;
		dst[i]=RGB32BIT(0,r,g,b);
	}
}

inline DWORD ENGIN_DLL Color_Mul(DWORD ori,float r,float g,float b){
	int rr=int(RGB32BIT_R(ori)*r);
	int gg=int(RGB32BIT_G(ori)*g);
	int bb=int(RGB32BIT_B(ori)*b);
	return RGB32BIT(0,
		LIMIT(rr,255),
		LIMIT(gg,255),
		LIMIT(bb,255));
}

inline void ENGIN_DLL Color_Mul(DWORD* c,float k,int len,DWORD* dst){
	for(int i=0;i<len;i++){
		int r=(int)(RGB32BIT_R(c[i])*k);
		if(r>0xff||r<0)continue;
		int g=(int)(RGB32BIT_G(c[i])*k);
		if(g>0xff||g<0)continue;
		int b=(int)(RGB32BIT_B(c[i])*k);
		if(b>0xff||b<0)continue;
		dst[i]=RGB32BIT(0,r,g,b);
	}
}

#define FIXP16_2_WRAP_BYTE(n,temp) temp=FIXP16_2_INT(n);temp=WRAP_BYTE(temp);

inline DWORD ENGIN_DLL Wrap_Byte(DWORD b){
	return WRAP_BYTE(b);
}

inline DWORD GetBmpPixel(const BITMAP *tex,int u,int v,int shiftWidth){	
	if(u<0){
		u=0;
	}
	if(v<0){
		v=0;
	}
	if(u>=tex->width){
		u=tex->width-1;
	}
	if(v>=tex->height){
		v=tex->height-1;
	}	
	return ((DWORD*)(tex->data))[((v<<shiftWidth)+u)];
}

inline void Plot8(int x,int y,BYTE colorIdx,PBYTE pMem,LONG pitch){
	pMem[x + y*pitch] = colorIdx;
}

inline void Plot16(int x, int y, USHORT color, PUSHORT pMem, LONG pitch16){
	pMem[x + y*pitch16] = color;
}

inline void Plot24(int x, int y, BYTE r,BYTE g,BYTE b,PBYTE pMem,LONG pitch){
	DWORD addr = x + x + x + y*pitch;
	pMem[addr] = b;
	pMem[addr + 1] = g;
	pMem[addr + 2] = r;
}

inline void Plot32(int x, int y, DWORD color, PDWORD pMem, LONG pitch32){
	pMem[x + y*pitch32] = color;
}

inline void Clean32(PDWORD pMem, UINT width, UINT height, LONG pitch32){
	if (width == pitch32){
		__asm{
			pushad;
			mov ecx, dword ptr[height];
			mov eax, dword ptr[width];
			mul ecx;
			mov ecx, eax
				xor eax, eax;
			mov edi, dword ptr[pMem];
			cld;
			rep stosd;
			popad;
		}
	}
	else{
		for (UINT i = 0; i < height; i++){
			__asm{
				pushad;
				mov ecx, dword ptr[width];
				xor eax, eax;
				mov edi, dword ptr[pMem];
				cld;
				rep stosd;
				popad;
			}
			pMem += pitch32;
		}
	}
}

inline bool InRange(float x,float y,const Range& range){
	if (x >= range.left && x <= range.right && y>=range.top && y<=range.bottom){
		return true;
	}
	return false;
}

inline bool InRange(int x,int y,const Range& range){
	if (x >= range.left && x <= range.right && y>=range.top && y<=range.bottom){
		return true;
	}
	return false;
}

inline bool InRange(FIXP16 x,FIXP16 y,const RangeFIXP16& range){
	if (x >= range.left && x <= range.right && y>=range.top && y<=range.bottom){
		return true;
	}
	return false;
}

//void DrawTriangle(DWORD* pMem, LONG pitch32, const Vec2f& wndOrigin, const Range& wndRange, DWORD color, Vec2f p0, Vec2f p1, Vec2f p2,bool isWire/*=false*/){
//	//线框模式的三角形
//	if(isWire){
//		DrawTriangleWire(pMem,pitch32,wndOrigin,wndRange,color,p0,p1,p2);
//		return;
//	}
//	//舍去小数点部分，否则会造成像素填充时各种精度的问题,因为会有累积误差
//	p0.y=floor(p0.y);
//	p0.x=floor(p0.x);
//	p1.y=floor(p1.y);
//	p1.x=floor(p1.x);
//	p2.y=floor(p2.y);
//	p2.x=floor(p2.x);
//	//填充模式的三角形
//	Vec2f* minP;
//	Vec2f* maxP;
//	Vec2f* middleP;
//	if(p0.y<p1.y){
//		//0 1 2
//		if(p1.y<p2.y){
//			minP=&p0;
//			middleP=&p1;
//			maxP=&p2;
//		}
//		//0 2 1
//		else if(p0.y<p2.y){
//			minP=&p0;
//			middleP=&p2;
//			maxP=&p1;
//		}
//		//2 0 1  
//		else{
//			minP=&p2;
//			middleP=&p0;
//			maxP=&p1;
//		}
//	}else{
//		//2 1 0
//		if(p1.y>p2.y){
//			minP=&p2;
//			middleP=&p1;
//			maxP=&p0;
//		}
//		//1 2 0
//		else if(p0.y>p2.y){
//			minP=&p1;
//			middleP=&p2;
//			maxP=&p0;
//
//		}
//		//1 0 2
//		else{
//			minP=&p1;
//			middleP=&p0;
//			maxP=&p2;
//		}
//	}
//
//	static Vec2f bp;
//	static Vec2f ep;
//
//	DrawLine(pMem,pitch32,wndOrigin,wndRange,color,*minP,*maxP);
//	DrawLine(pMem,pitch32,wndOrigin,wndRange,color,*minP,*middleP);
//	DrawLine(pMem,pitch32,wndOrigin,wndRange,color,*middleP,*maxP);
//	float max_min=maxP->y-minP->y;
//	if(max_min<1){	
//		return;
//	}
//
//	float depx = (maxP->x - minP->x) / (maxP->y - minP->y);
//	float dbpx;
//	Vec2f* pMax;
//	Vec2f* pMin;
//	DWORD* tempMem;
//	int wndRangeLeft=(int)wndRange.left;
//	int wndRangeRight=(int)wndRange.right;
//	float middle_min=middleP->y - minP->y;
//	if(middle_min>=0.01){
//		dbpx = (middleP->x - minP->x)/middle_min;
//		ep.x= bp.x = minP->x;
//		if(depx<dbpx){
//			pMax=&bp;
//			pMin=&ep;
//		}else{
//			pMax=&ep;
//			pMin=&bp;
//		}
//		float y = minP->y;
//		while(true){
//			ep.y=bp.y=y;
//			int offsetY=(int)(y+wndOrigin.y);
//			if(offsetY>wndRange.bottom)break;
//			if(offsetY>=wndRange.top){
//				int offsetMinX=(int)(pMin->x+wndOrigin.x);
//				int offsetMaxX=(int)(pMax->x+wndOrigin.x);
//				if(offsetMinX<wndRangeLeft)offsetMinX=wndRangeLeft;
//				if(offsetMaxX>wndRangeRight)offsetMaxX=wndRangeRight;
//				tempMem = pMem+offsetY*pitch32+offsetMinX;
//				for (int x = offsetMinX; x <= offsetMaxX; x++){
//					*(tempMem++) = color;
//				}
//			}
//			y+=1.0f;
//			if(y>middleP->y)
//				break;
//			bp.x += dbpx;
//			ep.x += depx;
//		}
//	}
//
//	float max_middle=maxP->y-middleP->y;
//	if(max_middle>=0.01){
//		ep.x = bp.x = maxP->x;
//		dbpx = (maxP->x - middleP->x)/max_middle;
//		if(depx<dbpx){
//			pMax=&ep;
//			pMin=&bp;
//		}else{
//			pMax=&bp;
//			pMin=&ep;
//		}
//		float y=maxP->y;
//		while(true){
//			ep.y = bp.y = y;
//			int offsetY=(int)(y+wndOrigin.y);
//			if(offsetY<wndRange.top)break;
//			if(offsetY<=wndRange.bottom){
//				int offsetMinX=(int)(pMin->x+wndOrigin.x);
//				int offsetMaxX=(int)(pMax->x+wndOrigin.x);
//				if(offsetMinX<wndRangeLeft)offsetMinX=wndRangeLeft;
//				if(offsetMaxX>wndRangeRight)offsetMaxX=wndRangeRight;
//				tempMem = pMem+offsetY*pitch32+offsetMinX;
//				for (int x = offsetMinX; x <= offsetMaxX; x++){
//					*(tempMem++) = color;
//				}
//			}
//			y-=1.0f;
//			if(y<middleP->y)
//				break;
//			bp.x -= dbpx;
//			ep.x -= depx;
//		}
//	}
//}

NS_ENGIN_END
#endif