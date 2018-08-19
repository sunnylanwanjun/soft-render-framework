#include "t3dlib1.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <ddraw.h>
#include <vector>
#include "mathUtil.h"
#include "log.h"
#include "Geometry.h"
#include "Data.h"
#include "FileUtils.h"
using namespace std;
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib, "dxguid.lib ")//没有这一句，会报error LNK2001: 无法解析的外部符号 _IID_IDirectDraw7
//添加包含目录的时候，请注意，不要把dx8的include目录放在最前面，否则会报错
//dx常量标志的前缀一般都使用方法名的简写作为前缀，如DDSCL:SetCooperativeLevel,协作等级,如果该常量标志是结构体的标志，那就以结构体的简写作为前缀
//如DDPF_XX，像素格式DDPixelFormat
//非常多的地方有CAPS的字眼，如创建表面，创建调色板，创建双缓冲，都会见到CAPS，全称Capabilities能力
//可以理解成，创建COM接口的时候，赋予了怎样的的能力，那么该接口就拥有怎样的能力，而针对不同的接口命名前缀也不一样
//如调色板能力，DDPCAPS_XX,表面能力，DDSCAPS_XX,创建一个接口的时候，要么需要有描述符，要么就是给予能力，
//唯一不用指定描述符和能力的就是DirectDraw接口，因为它是由硬件决定的，代表着每一张显卡
//而由DirectDraw创建出来的接口，如表面，调色板，由主表面创建的从表面，都需要描述符或能力
//如表面:DDSurfaceDesc2，这个描述符中也包含了能力字段ddscaps
//调色板就直接使用DDPCAPS_XX作为其标志位了

NS_ENGIN_BEGIN
#define ROUND(num) (int((num+0.5)))
#define FLAG_FULLSCREEN DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT
#define FLAG_WINDOW DDSCL_NORMAL 
#define MAKE_PALETTEENTRY(pe,r,g,b) pe.peRed=r;pe.peGreen=g;pe.peBlue=b;pe.peFlags=PC_NOCOLLAPSE;//PC_NOCOLLAPSE 不需要DX进行优化
#define PALETTE_FLAGS DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE //DDPCAPS_ALLOW256 调色板数组第0，255个代码黑色和白色，DDPCAPS_INITIALIZE 数组初始化硬件设置

const char* GetErrType(HRESULT res){
	static const char* errStr[] = {
		"DD_OK 完全成功",
		"DDERR_DIRECTDRAWALREADYCREATED DirectDraw对象已经创建",
		"DDERR_GENERIC	DirectDraw不知道什么出错了",
		"DDERR_INVALIDDIRECTDRAWGUID 无效的设备GUID",
		"DDERR_INVALIDPARAMS 无效的参数",
		"DDERR_NODIRECTDRAWHW 没有硬件",
		"DDERR_OUTOFMEMORY 随便猜猜看" };
	switch (res){
	case DD_OK:
		return errStr[0];
		break;
	case DDERR_DIRECTDRAWALREADYCREATED:
		return errStr[1];
		break;
	case DDERR_GENERIC:
		return errStr[2];
		break;
	case DDERR_INVALIDDIRECTDRAWGUID:
		return errStr[3];
		break;
	case DDERR_INVALIDPARAMS:
		return errStr[4];
		break;
	case DDERR_NODIRECTDRAWHW:
		return errStr[5];
		break;
	case DDERR_OUTOFMEMORY:
		return errStr[6];
		break;
	}
	return errStr[2];
}

//************************************
// Function:  初始化DirectDraw7
// Returns:   LPDIRECTDRAW7
// Parameter: HWND hwnd            窗口名柄
// Parameter: bool isFullScreen    是否全屏
// 显示模式，即该应用程序将会占居多少显存
// Parameter: DWORD width          显示模式的宽
// Parameter: DWORD height         显示模式的高
// Parameter: DWORD bpp            显示模式的位深
//************************************
LPDIRECTDRAW7 DirectDraw7Init(HWND hwnd,bool isFullScreen,DWORD width,DWORD height,DWORD bpp){
	LPDIRECTDRAW7 lpdd7 = nullptr;
	HRESULT res=DirectDrawCreateEx(nullptr, (LPVOID*)&lpdd7, IID_IDirectDraw7, nullptr);
	if (FAILED(res)){
		Log("Error:%s,ErrorType:%s","DirectDraw7Init(DirectDrawCreateEx)ErrType:", GetErrType(res));
		return nullptr;
	}
	DWORD flag = FLAG_FULLSCREEN;
	if (!isFullScreen)
		flag = FLAG_WINDOW;
	if (FAILED(lpdd7->SetCooperativeLevel(hwnd, flag))){
		Error("DirectDraw7Init(SetCooperativeLevel)Failed");
		lpdd7->Release();
		return nullptr;
	}
	if (isFullScreen){
		if (FAILED(lpdd7->SetDisplayMode(width, height, bpp, 0, 0))){
			Error("DirectDraw7Init(SetDisplayMode)Failed");
			lpdd7->Release();
			return nullptr;
		}
	}
	return lpdd7;
}

LPDIRECTDRAWPALETTE RandomPaletteInit(LPDIRECTDRAW7 lpdd7){
	if (lpdd7 == nullptr)
		return nullptr;
	PALETTEENTRY colorTable[256];
	MAKE_PALETTEENTRY(colorTable[0],0,0,0);
	MAKE_PALETTEENTRY(colorTable[255],255,255,255);
	for (int i = 1; i < 255; i++){
		MAKE_PALETTEENTRY(colorTable[i],rand()%256,rand()%256,rand()%256);
	}
	LPDIRECTDRAWPALETTE lpddpal = nullptr;
	if (FAILED(lpdd7->CreatePalette(PALETTE_FLAGS, colorTable, &lpddpal, nullptr))){
		Error("RandomPaletteInit(CreatePalette)Failed");
		return nullptr;
	}
	return lpddpal;
}

LPDIRECTDRAWSURFACE7 DirectDrawSurfaceInit_Primary(LPDIRECTDRAW7 lpdd7){
	if (lpdd7 == nullptr)
		return nullptr;
	LPDIRECTDRAWSURFACE7 lpddsprimary = nullptr;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, nullptr))){
		Error("DirectDrawSurfaceInit_Primary(CreateSurface)Failed");
		return nullptr;
	}
	return lpddsprimary;
}

void DirectDrawSurfaceInit_Primary(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7* lplpddsprimary, LPDIRECTDRAWSURFACE7* lplpddsback){
	if (lpdd7 == nullptr)
		return;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.dwBackBufferCount = 1;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	if (FAILED(lpdd7->CreateSurface(&ddsd, lplpddsprimary, nullptr))){
		Error("DirectDrawSurfaceInit(CreateSurface)Failed");
		return;
	}
	DDSCAPS2 ddsCaps;
	ZeroMemory(&ddsCaps, sizeof DDSCAPS2);
	ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	if (FAILED((*lplpddsprimary)->GetAttachedSurface(&ddsCaps, lplpddsback))){
		Error("DirectDrawSurfaceInit_Primary(GetAttachedSurface)Failed");
		return;
	}
}

LPDIRECTDRAWSURFACE7 DirectDrawSurfaceInit_Common(LPDIRECTDRAW7 lpdd7, int width, int height, int mem_flags,size_t low_color,size_t high_color){
	if (lpdd7 == nullptr)
		return nullptr;
	LPDIRECTDRAWSURFACE7 lpdds = nullptr;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_CKSRCBLT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	//[low_color,high_color]表示一个闭区间，在这个区域内的源色彩都会被过滤
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = low_color;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = high_color;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|mem_flags;
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpdds, nullptr))){
		Error("DirectDrawSurfaceInit_Common(CreateSurface)Failed");
		return nullptr;
	}

	//DDCOLORKEY ddck;
	//ddck.dwColorSpaceLowValue = low_color;
	//ddck.dwColorSpaceHighValue = high_color;
	//lpdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
	return lpdds;
}

LPDIRECTDRAWCLIPPER DirectDrawClipperInit(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, int numRect, LPRECT rectList){
	if (lpdd7 == nullptr || lpdds == nullptr){
		return nullptr;
	}
	LPDIRECTDRAWCLIPPER lpddclipper;
	if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, 0))){
		return nullptr;
	}
	RGNDATA* lpRGNData = (RGNDATA*)malloc(sizeof(RGNDATAHEADER)+numRect*sizeof(RECT));
	lpRGNData->rdh.dwSize = sizeof RGNDATAHEADER;
	lpRGNData->rdh.iType = RDH_RECTANGLES;
	lpRGNData->rdh.nCount = numRect;
	lpRGNData->rdh.nRgnSize = numRect*sizeof(RECT);
	memcpy(lpRGNData->Buffer, rectList, lpRGNData->rdh.nRgnSize);
	GetBounds(numRect, rectList, lpRGNData->rdh.rcBound);
	if (FAILED(lpddclipper->SetClipList(lpRGNData, 0))){
		free(lpRGNData);
		lpddclipper->Release();
		return nullptr;
	}
	if (FAILED(lpdds->SetClipper(lpddclipper))){
		free(lpRGNData);
		lpddclipper->Release();
		return nullptr;
	}
	free(lpRGNData);
	return lpddclipper;
}

LPDIRECTDRAWCLIPPER DirectDrawClipperInit(LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, HWND hwnd){
	if (lpdd7 == nullptr || lpdds == nullptr){
		return nullptr;
	}
	LPDIRECTDRAWCLIPPER lpddclipper;
	if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, 0))){
		return nullptr;
	}
	if (FAILED(lpddclipper->SetHWnd(0, hwnd))){
		lpddclipper->Release();
		return nullptr;
	}
	if (FAILED(lpdds->SetClipper(lpddclipper))){
		lpddclipper->Release();
		return nullptr;
	}
	lpddclipper->Release();
	return lpddclipper;
}

void GetBounds(int numRect,LPRECT rectList,RECT& bounds){
	memcpy(&bounds, rectList, sizeof RECT);
	for (int i = 1; i < numRect; i++){
		if (bounds.top > rectList[0].top)
			bounds.top = rectList[0].top;
		if (bounds.bottom < rectList[0].bottom)
			bounds.bottom = rectList[0].bottom;
		if (bounds.left > rectList[0].left)
			bounds.left = rectList[0].left;
		if (bounds.right < rectList[0].right)
			bounds.right = rectList[0].right;
	}
}

bool GetBmp(const char* fileName,BITMAP** ppBmp){
	*ppBmp=new BITMAP;
	Data data=FileUtils::getInstance()->getDataFromFile(fileName);
	if (data.isNull()){
		return false;
	}
	BYTE* bytes = data.getBytes();
	size_t size = data.getSize();
	return GetBmp(bytes, size, (*ppBmp)->data, (*ppBmp)->dataLen,
		0, 0, (*ppBmp)->width, (*ppBmp)->height);
}

bool GetBmp(BYTE* src, size_t srcLen, BYTE*& dst, size_t& dstLen, int x, int y, int& w, int& h){
	dst = nullptr;
	dstLen = 0;
	w = 0;
	h = 0;

	if (x < 0 || y < 0 || w < 0 || h < 0)
		return false;
	BITMAPFILEHEADER bmpfh;
	memcpy(&bmpfh, src, sizeof BITMAPFILEHEADER);
	BITMAPINFOHEADER bmpih;
	memcpy(&bmpih, src + sizeof BITMAPFILEHEADER, sizeof BITMAPINFOHEADER);
	if (x >= bmpih.biWidth || y >= bmpih.biHeight )
		return false;
	if (x + w>bmpih.biWidth||w==0)
		w = bmpih.biWidth - x;
	if (y + h>bmpih.biHeight||h==0)
		h = bmpih.biHeight - y;
	dstLen = w*h * 4;
	dst = new BYTE[dstLen];
	BYTE* pDst = dst;
	BYTE* pSrc = src;
	for (LONG r = y; r < y+h; r++){
		BYTE* pRowData = pSrc + srcLen + (-r*bmpih.biWidth * 3 - x * 3);
		for (LONG c = x; c < x+w; c++){
			pRowData -= 3;
			memcpy(pDst, pRowData, 3);
			pDst[3]=0xff;
			pDst += 4;
		}
	}
	return true;
}

void BltBmpData32(DWORD*src, int srcW, int srcH, DWORD* dest, int destW, int destH, LONG pitch32){
	int cbx = 0;
	int cby = 0;
	int cw = srcW;
	int ch = srcH;
	if (cw <= 0 || ch <= 0){
		return;
	}
	src += cbx + cby*srcW;

	float scaleX = (float)srcW / destW;
	float scaleY = (float)srcH / destH;
	DWORD* pRowData = src;
	for (float r = (float)cby; r < (float)ch;){
		DWORD* pColMem = dest;
		DWORD* pColData = pRowData;
		for (float c = (float)cbx; c < (float)cw;){
			*pColMem = *pColData;
			pColMem++;
			c += scaleX;
			pColData = pRowData + (int)(c - cbx);
		}
		r += scaleY;
		pRowData = src + srcW*(int)(r - cby);
		dest += pitch32;
	}
}

void reverseRGBA(BYTE* data,size_t size){
	for (size_t i = 0; i < size; i+=4){
		data[i] = data[i] ^ data[i + 2];
		data[i + 2] = data[i] ^ data[i + 2];
		data[i] = data[i] ^ data[i + 2];
	}
}

void DrawLine(DWORD* pMem, 
			  LONG pitch32, 
			  const Vec2FIXP16& wndOrigin,
			  const RangeFIXP16& wndRange,
			  DWORD color,
			  Vec2FIXP16 p0,
			  Vec2FIXP16 p1){
	//没有画的空间
	if (wndRange.left == wndRange.right || wndRange.top == wndRange.bottom){
		return;
	}
	
	p0.x+=wndOrigin.x;
	p0.y+=wndOrigin.y;
	p1.x+=wndOrigin.x;
	p1.y+=wndOrigin.y;
	p0.x=FIXP16_FLOOR(p0.x);
	p0.y=FIXP16_FLOOR(p0.y);
	p1.x=FIXP16_FLOOR(p1.x);
	p1.y=FIXP16_FLOOR(p1.y);

	static Vec2FIXP16 bp;
	int wndRangeLeft=FIXP16_2_INT(wndRange.left);
	int wndRangeRight=FIXP16_2_INT(wndRange.right);
	int wndRangeTop=FIXP16_2_INT(wndRange.top);
	int wndRangeBottom=FIXP16_2_INT(wndRange.bottom);

	//填充模式的三角形
	Vec2FIXP16* y_pMin;
	Vec2FIXP16* y_pMax;
	if(p0.y>p1.y){
		y_pMax=&p0;
		y_pMin=&p1;
	}else{
		y_pMax=&p1;
		y_pMin=&p0;
	}

	DWORD* tempMem;
	FIXP16 middle_min=y_pMax->y - y_pMin->y;
	//横线
	if (middle_min<FIXP16_1){
		if(p0.y<wndRange.top||p0.y>wndRange.bottom)
			return;
		int minx = 0;
		int maxx = 0;
		if(p0.x<p1.x){
			minx=p0.x;
			maxx=p1.x;
		}else{
			maxx=p0.x;
			minx=p1.x;
		}
		if(minx<wndRange.left)minx=wndRange.left;
		if(maxx>wndRange.right)maxx=wndRange.right;
		minx=FIXP16_2_INT(minx);
		maxx=FIXP16_2_INT(maxx);
		tempMem = pMem + FIXP16_2_INT(y_pMin->y)*pitch32 + minx;
		for (int x = minx; x <= maxx; x++){
			*(tempMem++) = color;
		}
		return;
	}

	FIXP16 dbpx;

	bool preBpIn=false;
	bool nowBpIn=false;
	int intY;
	int intBpx=0;
	int preBpx=0;
	int midpx=0;

	if(middle_min>FIXP16_1){
		FIXP16_Div(y_pMax->x - y_pMin->x,middle_min,dbpx);
	}
	else{
		dbpx = y_pMax->x - y_pMin->x;
	}

	bp.x = y_pMin->x;

	FIXP16 y = y_pMin->y;
	FIXP16 edgeY=y_pMax->y;
	FIXP16 fixTemp;

	bool needScan;
	FIXP16 dis;
	if(y<wndRange.top){
		y=wndRange.top;
	}
	if(y<=edgeY){
		dis=y-y_pMin->y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x += fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	while(needScan){
		bp.y=y;
		intY=FIXP16_2_INT(y);
		if(intY>wndRangeBottom)break;
		preBpx=intBpx;
		intBpx=FIXP16_ROUND(bp.x);
		preBpIn=nowBpIn;
		nowBpIn=true;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			nowBpIn=false;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			nowBpIn=false;
		}

		if(nowBpIn||preBpIn){
			tempMem = pMem+intY*pitch32+intBpx;
			*tempMem = color;
			//处理起始点不连续的情况
			if(ABS(intBpx-preBpx)>1){
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = color;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = color;
						}
					}
				}
			}
		}

		y+=FIXP16_1;
		if(y>edgeY)
			break;

		bp.x += dbpx;
	}
}

void DrawTriangle(DWORD* pMem, 
				  LONG pitch32, 
				  const Vec2FIXP16& wndOrigin, 
				  const RangeFIXP16& wndRange, 
				  DWORD color, 
				  Vec2FIXP16 p0, 
				  Vec2FIXP16 p1, 
				  Vec2FIXP16 p2,
				  bool isWire/*=false*/){
	//舍去小数点部分，否则会造成像素填充时各种精度的问题,因为会有累积误差
	p0.y=FIXP16_FLOOR(p0.y);
	p0.x=FIXP16_FLOOR(p0.x);
	p1.y=FIXP16_FLOOR(p1.y);
	p1.x=FIXP16_FLOOR(p1.x);
	p2.y=FIXP16_FLOOR(p2.y);
	p2.x=FIXP16_FLOOR(p2.x);

	if(isWire){
		DrawLine(pMem,pitch32,wndOrigin,wndRange,color,p0,p1);
		DrawLine(pMem,pitch32,wndOrigin,wndRange,color,p1,p2);
		DrawLine(pMem,pitch32,wndOrigin,wndRange,color,p2,p0);
		return;
	}

	//填充模式的三角形
	Vec2FIXP16* minP;
	Vec2FIXP16* maxP;
	Vec2FIXP16* middleP;
	if(p0.y<p1.y){
		//0 1 2
		if(p1.y<p2.y){
			minP=&p0;
			middleP=&p1;
			maxP=&p2;
		}
		//0 2 1
		else if(p0.y<p2.y){
			minP=&p0;
			middleP=&p2;
			maxP=&p1;
		}
		//2 0 1  
		else{
			minP=&p2;
			middleP=&p0;
			maxP=&p1;
		}
	}else{
		//2 1 0
		if(p1.y>p2.y){
			minP=&p2;
			middleP=&p1;
			maxP=&p0;
		}
		//1 2 0
		else if(p0.y>p2.y){
			minP=&p1;
			middleP=&p2;
			maxP=&p0;
		}
		//1 0 2
		else{
			minP=&p1;
			middleP=&p0;
			maxP=&p2;
		}
	}

	static Vec2FIXP16 bp;
	static Vec2FIXP16 ep;
	int wndRangeLeft=FIXP16_2_INT(wndRange.left);
	int wndRangeRight=FIXP16_2_INT(wndRange.right);
	int wndRangeTop=FIXP16_2_INT(wndRange.top);
	int wndRangeBottom=FIXP16_2_INT(wndRange.bottom);

	FIXP16 max_min=maxP->y-minP->y;
	FIXP16 depx;
	if(max_min<FIXP16_1){
		depx=maxP->x - minP->x;
	}else{
		FIXP16_Div(maxP->x - minP->x,max_min,depx);
	}

	FIXP16 dbpx;

	bool bpOutLeft=false;
	bool bpOutRight=false;
	bool epOutLeft=false;
	bool epOutRight=false;
	int intY;
	int intBpx=0;
	int intEpx=0;
	int preBpx=0;
	int preEpx=0;
	int midpx=0;

	///////////////////////////////////////////////////////
	//从最小到中间
	DWORD* tempMem;
	FIXP16 middle_min=middleP->y - minP->y;
	if(middle_min>FIXP16_1){
		FIXP16_Div(middleP->x - minP->x,middle_min,dbpx);
	}
	else{
		dbpx = middleP->x - minP->x;
	}

	ep.x = bp.x = minP->x+wndOrigin.x;

	FIXP16 y = minP->y+wndOrigin.y;
	FIXP16 edgeY=middleP->y+wndOrigin.y;
	FIXP16 fixTemp;
	
	bool needScan;
	bool needSlerp=false;
	FIXP16 dis;
	if(y<wndRange.top){
		y=wndRange.top;
	}
	if(y<=edgeY){
		dis=y-minP->y-wndOrigin.y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x += fixTemp;
		FIXP16_Mul(dis,depx,fixTemp);
		ep.x += fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);

	while(needScan){
		ep.y=bp.y=y;
		intY=FIXP16_2_INT(y);
		if(intY>wndRangeBottom)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);
		bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if((!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			tempMem = pMem+intY*pitch32+intBpx;

			//test-----------------------------
			//*tempMem = RGB32BIT(0,(int)br,(int)bg,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)er,(int)eg,(int)eb);
			//test-----------------------------
			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = color;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = color;
				}
			}
			else{
				*tempMem = color;
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = color;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = color;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = color;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = color;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = color;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}
		
		y+=FIXP16_1;
		if(y>edgeY)
			break;

		bp.x += dbpx;
		ep.x += depx;
	}

	///////////////////////////////////////////////////////
	//从中间到最大
	FIXP16 max_middle=maxP->y-middleP->y;
	if(max_middle>FIXP16_1){
		FIXP16_Div(maxP->x - middleP->x,max_middle,dbpx);
	}else{
		dbpx = maxP->x - middleP->x;
	}

	ep.x = bp.x = maxP->x+wndOrigin.x;
	y=maxP->y+wndOrigin.y;

	if(y>wndRange.bottom){
		y=wndRange.bottom;
	}
	if(y>=edgeY){
		dis=maxP->y+wndOrigin.y-y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x-= fixTemp;
		FIXP16_Mul(dis,depx,fixTemp);
		ep.x-= fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);
	needSlerp=false;

	while(needScan){
		ep.y = bp.y = y;
		intY=FIXP16_2_INT(y);
		if(intY<wndRangeTop)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);
		bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if((!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			
			tempMem = pMem+intY*pitch32+intBpx;

			//test----------------------------------------
			//*tempMem = RGB32BIT(0,(int)br,(int)bg,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)er,(int)eg,(int)eb);
			//test----------------------------------------
			
			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = color;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = color;
				}
			}
			else{
				*tempMem = color;
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = color;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = color;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = color;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = color;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = color;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = color;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = color;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = color;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}

		y-=FIXP16_1;
		if(y<edgeY)
			break;

		bp.x -= dbpx;
		ep.x -= depx;
	}

}

void DrawGouraudLine(DWORD* pMem, 
					 LONG pitch32, 
					 const Vec2FIXP16& wndOrigin, 
					 const RangeFIXP16& wndRange, 
					 Vec2FIXP16 p0, 
					 Vec2FIXP16 p1,
					 DWORD c0,DWORD c1){
	if (wndRange.left == wndRange.right || wndRange.top == wndRange.bottom){
		return;
	}

	p0.x+=wndOrigin.x;
	p0.y+=wndOrigin.y;
	p1.x+=wndOrigin.x;
	p1.y+=wndOrigin.y;
	p0.x=FIXP16_FLOOR(p0.x);
	p0.y=FIXP16_FLOOR(p0.y);
	p1.x=FIXP16_FLOOR(p1.x);
	p1.y=FIXP16_FLOOR(p1.y);

	static Vec2FIXP16 bp;
	int wndRangeLeft=FIXP16_2_INT(wndRange.left);
	int wndRangeRight=FIXP16_2_INT(wndRange.right);
	int wndRangeTop=FIXP16_2_INT(wndRange.top);
	int wndRangeBottom=FIXP16_2_INT(wndRange.bottom);

	//填充模式的三角形
	Vec2FIXP16* y_pMin;
	Vec2FIXP16* y_pMax;
	ARGB y_pMinC;
	ARGB y_pMaxC;
	if(p0.y>p1.y){
		y_pMax=&p0;
		y_pMin=&p1;
		y_pMaxC.argb=c0;
		y_pMinC.argb=c1;
	}else{
		y_pMax=&p1;
		y_pMin=&p0;
		y_pMaxC.argb=c1;
		y_pMinC.argb=c0;
	}

	static FIXP16 dbpx;
	
	//起点颜色
	static FIXP16 dbr;
	static FIXP16 dbg;
	static FIXP16 dbb;
	static FIXP16 br;
	static FIXP16 bg;
	static FIXP16 bb;

	static FIXP16 tr;
	static FIXP16 tg;
	static FIXP16 tb;

	//随x轴改变的颜色 在x轴位移不连续的时候使用
	static FIXP16 dbrx;
	static FIXP16 dbgx;
	static FIXP16 dbbx;

	bool preBpIn=false;
	bool nowBpIn=false;
	int intY;
	int intBpx=0;
	int preBpx=0;
	int midpx=0;

	DWORD* tempMem;
	int itr;
	int itg;
	int itb;
	FIXP16 middle_min=y_pMax->y - y_pMin->y;

	//横线
	if (middle_min<FIXP16_1){
		if(p0.y<wndRange.top||p0.y>wndRange.bottom)
			return;
		int minx = 0;
		int maxx = 0;
		ARGB minc;
		ARGB maxc;
		if(p0.x<p1.x){
			minx=p0.x;
			maxx=p1.x;
			minc.argb=c0;
			maxc.argb=c1;
		}else{
			maxx=p0.x;
			minx=p1.x;
			minc.argb=c1;
			maxc.argb=c0;
		}

		middle_min=maxx-minx;
		br=INT_2_FIXP16(minc.r);
		bg=INT_2_FIXP16(minc.g);
		bb=INT_2_FIXP16(minc.b);
		if(middle_min>FIXP16_1){
			FIXP16_Div(INT_2_FIXP16(maxc.r - minc.r),middle_min,  dbr );
			FIXP16_Div(INT_2_FIXP16(maxc.g - minc.g),middle_min,  dbg );
			FIXP16_Div(INT_2_FIXP16(maxc.b - minc.b),middle_min,  dbb );
		}else{
			dbr=0;
			dbg=0;
			dbb=0;
		}

		if(minx<wndRange.left)minx=wndRange.left;
		if(maxx>wndRange.right)maxx=wndRange.right;
		minx=FIXP16_2_INT(minx);
		maxx=FIXP16_2_INT(maxx);
		tempMem = pMem + FIXP16_2_INT(y_pMin->y)*pitch32 + minx;

		for (int x = minx; x <= maxx; x++){
			FIXP16_2_WRAP_BYTE(br,itr);
			FIXP16_2_WRAP_BYTE(bg,itg);
			FIXP16_2_WRAP_BYTE(bb,itb);
			*tempMem++ = RGB32BIT(0,itr,itg,itb);
			br+=dbr;
			bg+=dbg;
			bb+=dbb;
		}
		return;
	}

	///////////////////////////////////////////////////////
	//从最小到中间

	bp.x = y_pMin->x;
	br=INT_2_FIXP16(y_pMinC.r);
	bg=INT_2_FIXP16(y_pMinC.g);
	bb=INT_2_FIXP16(y_pMinC.b);
	FIXP16 y = y_pMin->y;
	FIXP16 edgeY=y_pMax->y;
	FIXP16 fixTemp;

	if(middle_min>FIXP16_1){
		/*
		FIXP16 _1_middle_min;
		FIXP16_Div(FIXP16_1,middle_min,_1_middle_min);
		FIXP16_Mul(y_pMax->x - y_pMin->x,_1_middle_min,dbpx);
		FIXP16_Mul(INT_2_FIXP16(y_pMaxC.r - y_pMinC.r),_1_middle_min,  dbr );
		FIXP16_Mul(INT_2_FIXP16(y_pMaxC.g - y_pMinC.g),_1_middle_min,  dbg );
		FIXP16_Mul(INT_2_FIXP16(y_pMaxC.b - y_pMinC.b),_1_middle_min,  dbb );*/
		
		FIXP16_Div(y_pMax->x - y_pMin->x,middle_min,dbpx);
		FIXP16_Div(INT_2_FIXP16(y_pMaxC.r - y_pMinC.r),middle_min,  dbr );
		FIXP16_Div(INT_2_FIXP16(y_pMaxC.g - y_pMinC.g),middle_min,  dbg );
		FIXP16_Div(INT_2_FIXP16(y_pMaxC.b - y_pMinC.b),middle_min,  dbb );
	}
	else{
		dbpx = y_pMax->x - y_pMin->x;
		dbr =  INT_2_FIXP16(y_pMaxC.r - y_pMinC.r);
		dbg =  INT_2_FIXP16(y_pMaxC.g - y_pMinC.g);
		dbb =  INT_2_FIXP16(y_pMaxC.b - y_pMinC.b);
	}

	if(ABS(dbpx)>FIXP16_1){
		FIXP16_Div(dbr,dbpx,dbrx);
		FIXP16_Div(dbg,dbpx,dbgx);
		FIXP16_Div(dbb,dbpx,dbbx);
	}
	
	bool needScan;
	FIXP16 dis;
	if(y<wndRange.top){
		y=wndRange.top;
	}
	if(y<=edgeY){
		dis=y-y_pMin->y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x += fixTemp;
		FIXP16_Mul(dis,dbr,fixTemp);
		br+=fixTemp;
		FIXP16_Mul(dis,dbg,fixTemp);
		bg+=fixTemp;
		FIXP16_Mul(dis,dbb,fixTemp);
		bb+=fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	while(needScan){
		bp.y=y;
		intY=FIXP16_2_INT(y);
		if(intY>wndRangeBottom)break;
		preBpx=intBpx;
		intBpx=FIXP16_ROUND(bp.x);
		preBpIn=nowBpIn;
		nowBpIn=true;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			nowBpIn=false;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			nowBpIn=false;
		}
		if(nowBpIn||preBpIn){

			tempMem = pMem+intY*pitch32+intBpx;
			FIXP16_2_WRAP_BYTE(br,itr);
			FIXP16_2_WRAP_BYTE(bg,itg);
			FIXP16_2_WRAP_BYTE(bb,itb);
			*tempMem = RGB32BIT(0,itr,itg,itb);
			
			//处理起始点不连续的情况
			if(ABS(intBpx-preBpx)>1){
				tr=br;
				tg=bg;
				tb=bb;
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						FIXP16_2_WRAP_BYTE(tr,itr);
						FIXP16_2_WRAP_BYTE(tg,itg);
						FIXP16_2_WRAP_BYTE(tb,itb);
						*tempMem = RGB32BIT(0,itr,itg,itb);
						tr-=dbrx;
						tg-=dbgx;
						tb-=dbbx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							FIXP16_2_WRAP_BYTE(tr,itr);
							FIXP16_2_WRAP_BYTE(tg,itg);
							FIXP16_2_WRAP_BYTE(tb,itb);
							*tempMem = RGB32BIT(0,itr,itg,itb);
							tr-=dbrx;
							tg-=dbgx;
							tb-=dbbx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						FIXP16_2_WRAP_BYTE(tr,itr);
						FIXP16_2_WRAP_BYTE(tg,itg);
						FIXP16_2_WRAP_BYTE(tb,itb);
						*tempMem = RGB32BIT(0,itr,itg,itb);
						tr+=dbrx;
						tg+=dbgx;
						tb+=dbbx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							FIXP16_2_WRAP_BYTE(tr,itr);
							FIXP16_2_WRAP_BYTE(tg,itg);
							FIXP16_2_WRAP_BYTE(tb,itb);
							*tempMem = RGB32BIT(0,itr,itg,itb);
							tr+=dbrx;
							tg+=dbgx;
							tb+=dbbx;
						}
					}
				}
			}
		}		
		y+=FIXP16_1;
		if(y>edgeY)
			break;

		bp.x += dbpx;
		br+=dbr;
		bg+=dbg;
		bb+=dbb;
	}
}

void DrawGouraudTriangle(DWORD* pMem, 
						 LONG pitch32, 
						 const Vec2FIXP16& wndOrigin, 
						 const RangeFIXP16& wndRange, 
						 Vec2FIXP16 p0, 
						 Vec2FIXP16 p1, 
						 Vec2FIXP16 p2,
						 DWORD c0,DWORD c1,DWORD c2,
						 bool isWire){
	//舍去小数点部分，否则会造成像素填充时各种精度的问题,因为会有累积误差
	p0.y=FIXP16_FLOOR(p0.y);
	p0.x=FIXP16_FLOOR(p0.x);
	p1.y=FIXP16_FLOOR(p1.y);
	p1.x=FIXP16_FLOOR(p1.x);
	p2.y=FIXP16_FLOOR(p2.y);
	p2.x=FIXP16_FLOOR(p2.x);

	if(isWire){
		DrawGouraudLine(pMem,pitch32,wndOrigin,wndRange,p0,p1,c0,c1);
		DrawGouraudLine(pMem,pitch32,wndOrigin,wndRange,p1,p2,c1,c2);
		DrawGouraudLine(pMem,pitch32,wndOrigin,wndRange,p2,p0,c2,c0);
		return;
	}

	//填充模式的三角形
	Vec2FIXP16* minP;
	Vec2FIXP16* maxP;
	Vec2FIXP16* middleP;
	ARGB minC;
	ARGB maxC;
	ARGB middleC;
	if(p0.y<p1.y){
		//0 1 2
		if(p1.y<p2.y){
			minP=&p0;
			middleP=&p1;
			maxP=&p2;

			minC.argb=c0;
			middleC.argb=c1;
			maxC.argb=c2;
		}
		//0 2 1
		else if(p0.y<p2.y){
			minP=&p0;
			middleP=&p2;
			maxP=&p1;

			minC.argb=c0;
			middleC.argb=c2;
			maxC.argb=c1;
		}
		//2 0 1  
		else{
			minP=&p2;
			middleP=&p0;
			maxP=&p1;

			minC.argb=c2;
			middleC.argb=c0;
			maxC.argb=c1;
		}
	}else{
		//2 1 0
		if(p1.y>p2.y){
			minP=&p2;
			middleP=&p1;
			maxP=&p0;

			minC.argb=c2;
			middleC.argb=c1;
			maxC.argb=c0;
		}
		//1 2 0
		else if(p0.y>p2.y){
			minP=&p1;
			middleP=&p2;
			maxP=&p0;

			minC.argb=c1;
			middleC.argb=c2;
			maxC.argb=c0;
		}
		//1 0 2
		else{
			minP=&p1;
			middleP=&p0;
			maxP=&p2;

			minC.argb=c1;
			middleC.argb=c0;
			maxC.argb=c2;
		}
	}

	static Vec2FIXP16 bp;
	static Vec2FIXP16 ep;
	int wndRangeLeft=FIXP16_2_INT(wndRange.left);
	int wndRangeRight=FIXP16_2_INT(wndRange.right);
	int wndRangeTop=FIXP16_2_INT(wndRange.top);
	int wndRangeBottom=FIXP16_2_INT(wndRange.bottom);

	FIXP16 max_min=maxP->y-minP->y;
	FIXP16 _1_max_min;
	FIXP16 depx;
	if(max_min<FIXP16_1){
		max_min=FIXP16_1;
		_1_max_min=FIXP16_1;
		depx=maxP->x - minP->x;
	}else{
		FIXP16_Div(FIXP16_1,max_min,_1_max_min);
		FIXP16_Mul(maxP->x - minP->x,_1_max_min,depx);
	}

	FIXP16 dbpx;
	
	//起点颜色
	FIXP16 dbr;
	FIXP16 dbg;
	FIXP16 dbb;
	FIXP16 br;
	FIXP16 bg;
	FIXP16 bb;

	//终点颜色
	FIXP16 der;
	FIXP16_Mul(INT_2_FIXP16(maxC.r - minC.r),_1_max_min,der);
	FIXP16 deg;
	FIXP16_Mul(INT_2_FIXP16(maxC.g - minC.g),_1_max_min,deg);
	FIXP16 deb;
	FIXP16_Mul(INT_2_FIXP16(maxC.b - minC.b),_1_max_min,deb);
	FIXP16 er;
	FIXP16 eg;
	FIXP16 eb;

	//扫描线颜色
	FIXP16 dtr;
	FIXP16 dtg;
	FIXP16 dtb;
	FIXP16 tr;
	FIXP16 tg;
	FIXP16 tb;

	//随x轴改变的颜色 在x轴位移不连续的时候使用
	FIXP16 dbrx;
	FIXP16 dbgx;
	FIXP16 dbbx;
	FIXP16 derx;
	FIXP16 degx;
	FIXP16 debx;

	bool bpOutLeft=false;
	bool bpOutRight=false;
	bool epOutLeft=false;
	bool epOutRight=false;
	int intY;
	int intBpx=0;
	int intEpx=0;
	int preBpx=0;
	int preEpx=0;
	int midpx=0;

	if(ABS(depx)>FIXP16_1){
		FIXP16_Div(der,depx,derx);
		FIXP16_Div(deg,depx,degx);
		FIXP16_Div(deb,depx,debx);
	}

	///////////////////////////////////////////////////////
	//从最小到中间
	DWORD* tempMem;
	FIXP16 middle_min=middleP->y - minP->y;
	if(middle_min>FIXP16_1){
		FIXP16 _1_middle_min;
		FIXP16_Div(FIXP16_1,middle_min,_1_middle_min);
		FIXP16_Mul(middleP->x - minP->x,_1_middle_min,dbpx);
		FIXP16_Mul(INT_2_FIXP16(middleC.r - minC.r),_1_middle_min,  dbr );
		FIXP16_Mul(INT_2_FIXP16(middleC.g - minC.g),_1_middle_min,  dbg );
		FIXP16_Mul(INT_2_FIXP16(middleC.b - minC.b),_1_middle_min,  dbb );
	}
	else{
		dbpx = middleP->x - minP->x;
		dbr =  INT_2_FIXP16(middleC.r - minC.r);
		dbg =  INT_2_FIXP16(middleC.g - minC.g);
		dbb =  INT_2_FIXP16(middleC.b - minC.b);
	}

	if(ABS(dbpx)>FIXP16_1){
		FIXP16_Div(dbr,dbpx,dbrx);
		FIXP16_Div(dbg,dbpx,dbgx);
		FIXP16_Div(dbb,dbpx,dbbx);
	}

	ep.x = bp.x = minP->x+wndOrigin.x;
	er=br=INT_2_FIXP16(minC.r);
	eg=bg=INT_2_FIXP16(minC.g);
	eb=bb=INT_2_FIXP16(minC.b);

	FIXP16 y = minP->y+wndOrigin.y;
	FIXP16 edgeY=middleP->y+wndOrigin.y;
	FIXP16 fixTemp;
	
	bool needScan;
	FIXP16 dis;
	if(y<wndRange.top){
		y=wndRange.top;
	}
	if(y<=edgeY){
		dis=y-minP->y-wndOrigin.y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x += fixTemp;
		FIXP16_Mul(dis,depx,fixTemp);
		ep.x += fixTemp;
		FIXP16_Mul(dis,dbr,fixTemp);
		br+=fixTemp;
		FIXP16_Mul(dis,der,fixTemp);
		er+=fixTemp;
		FIXP16_Mul(dis,dbg,fixTemp);
		bg+=fixTemp;
		FIXP16_Mul(dis,deg,fixTemp);
		eg+=fixTemp;
		FIXP16_Mul(dis,dbb,fixTemp);
		bb+=fixTemp;
		FIXP16_Mul(dis,deb,fixTemp);
		eb+=fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);
	bool needSlerp=false;
	while(needScan){
		ep.y=bp.y=y;
		intY=FIXP16_2_INT(y);
		if(intY>wndRangeBottom)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);
		bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if((!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			FIXP16 _1_offsetMaxX_MinX=ep.x-bp.x;
			if(ABS(_1_offsetMaxX_MinX)>FIXP16_1)
				FIXP16_Div(FIXP16_1,_1_offsetMaxX_MinX,_1_offsetMaxX_MinX);
			else
				_1_offsetMaxX_MinX=0;
			FIXP16_Mul(er-br,_1_offsetMaxX_MinX,dtr);
			FIXP16_Mul(eg-bg,_1_offsetMaxX_MinX,dtg);
			FIXP16_Mul(eb-bb,_1_offsetMaxX_MinX,dtb);

			tempMem = pMem+intY*pitch32+intBpx;

			FIXP16 offsetColor=INT_2_FIXP16(intBpx-FIXP16_ROUND(bp.x));
			FIXP16_Mul(dtr,offsetColor,tr);
			tr=br+tr;
			FIXP16_Mul(dtg,offsetColor,tg);
			tg=bg+tg;
			FIXP16_Mul(dtb,offsetColor,tb);
			tb=bb+tb;

			//test-----------------------------
			//*tempMem = RGB32BIT(0,(int)br,(int)bg,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)er,(int)eg,(int)eb);
			//test-----------------------------
			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
					tr+=dtr;
					tg+=dtg;
					tb+=dtb;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
					tr-=dtr;
					tg-=dtg;
					tb-=dtb;
				}
			}
			else{
				*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				tr=br;
				tg=bg;
				tb=bb;
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr-=dbrx;
						tg-=dbgx;
						tb-=dbbx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr-=dbrx;
							tg-=dbgx;
							tb-=dbbx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr+=dbrx;
						tg+=dbgx;
						tb+=dbbx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr+=dbrx;
							tg+=dbgx;
							tb+=dbbx;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				tr=er;
				tg=eg;
				tb=eb;
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr-=derx;
						tg-=degx;
						tb-=debx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr-=derx;
							tg-=degx;
							tb-=debx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr+=derx;
						tg+=degx;
						tb+=debx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr+=derx;
							tg+=degx;
							tb+=debx;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}
		
		y+=FIXP16_1;
		if(y>edgeY)
			break;

		bp.x += dbpx;
		ep.x += depx;
		br+=dbr;
		er+=der;
		bg+=dbg;
		eg+=deg;
		bb+=dbb;
		eb+=deb;
	}

	///////////////////////////////////////////////////////
	//从中间到最大
	FIXP16 max_middle=maxP->y-middleP->y;
	if(max_middle>FIXP16_1){
		FIXP16 _1_max_middle;
		FIXP16_Div(FIXP16_1,max_middle,_1_max_middle);
		FIXP16_Mul((maxP->x - middleP->x),_1_max_middle,dbpx);
		FIXP16_Mul(INT_2_FIXP16(maxC.r - middleC.r),_1_max_middle  ,dbr );
		FIXP16_Mul(INT_2_FIXP16(maxC.g - middleC.g),_1_max_middle  ,dbg );
		FIXP16_Mul(INT_2_FIXP16(maxC.b - middleC.b),_1_max_middle  ,dbb );
	}else{
		dbpx = maxP->x - middleP->x;
		dbr  = INT_2_FIXP16(maxC.r - middleC.r);
		dbg  = INT_2_FIXP16(maxC.g - middleC.g);
		dbb  = INT_2_FIXP16(maxC.b - middleC.b);
	}

	if(ABS(dbpx)>FIXP16_1){
		FIXP16_Div(dbr,dbpx,dbrx);
		FIXP16_Div(dbg,dbpx,dbgx);
		FIXP16_Div(dbb,dbpx,dbbx);
	}

	ep.x = bp.x = maxP->x+wndOrigin.x;
	er=br=INT_2_FIXP16(maxC.r);
	eg=bg=INT_2_FIXP16(maxC.g);
	eb=bb=INT_2_FIXP16(maxC.b);
	y=maxP->y+wndOrigin.y;

	if(y>wndRange.bottom){
		y=wndRange.bottom;
	}
	if(y>=edgeY){
		dis=maxP->y+wndOrigin.y-y;
		FIXP16_Mul(dis,dbpx,fixTemp);
		bp.x-= fixTemp;
		FIXP16_Mul(dis,depx,fixTemp);
		ep.x-= fixTemp;
		FIXP16_Mul(dis,dbr,fixTemp);
		br-=fixTemp;
		FIXP16_Mul(dis,der,fixTemp);
		er-=fixTemp;
		FIXP16_Mul(dis,dbg,fixTemp);
		bg-=fixTemp;
		FIXP16_Mul(dis,deg,fixTemp);
		eg-=fixTemp;
		FIXP16_Mul(dis,dbb,fixTemp);
		bb-=fixTemp;
		FIXP16_Mul(dis,deb,fixTemp);
		eb-=fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);
	needSlerp=false;
	while(needScan){
		ep.y = bp.y = y;
		intY=FIXP16_2_INT(y);
		if(intY<wndRangeTop)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);
		bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if((!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			FIXP16 _1_offsetMaxX_MinX=ep.x-bp.x;
			if(ABS(_1_offsetMaxX_MinX)>FIXP16_1)
				FIXP16_Div(FIXP16_1,_1_offsetMaxX_MinX,_1_offsetMaxX_MinX);
			else
				_1_offsetMaxX_MinX=0;
			FIXP16_Mul(er-br,_1_offsetMaxX_MinX,dtr);
			FIXP16_Mul(eg-bg,_1_offsetMaxX_MinX,dtg);
			FIXP16_Mul(eb-bb,_1_offsetMaxX_MinX,dtb);

			tempMem = pMem+intY*pitch32+intBpx;

			//test----------------------------------------
			//*tempMem = RGB32BIT(0,(int)br,(int)bg,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)er,(int)eg,(int)eb);
			//test----------------------------------------
			
			FIXP16 offsetColor=INT_2_FIXP16(intBpx-FIXP16_ROUND(bp.x));
			FIXP16_Mul(dtr,offsetColor,tr);
			tr=br+tr;
			FIXP16_Mul(dtg,offsetColor,tg);
			tg=bg+tg;
			FIXP16_Mul(dtb,offsetColor,tb);
			tb=bb+tb;

			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
					tr+=dtr;
					tg+=dtg;
					tb+=dtb;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
					tr-=dtr;
					tg-=dtg;
					tb-=dtb;
				}
			}
			else{
				*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				tr=br;
				tg=bg;
				tb=bb;
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr-=dbrx;
						tg-=dbgx;
						tb-=dbbx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr-=dbrx;
							tg-=dbgx;
							tb-=dbbx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr+=dbrx;
						tg+=dbgx;
						tb+=dbbx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr+=dbrx;
							tg+=dbgx;
							tb+=dbbx;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				tr=er;
				tg=eg;
				tb=eb;
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr-=derx;
						tg-=degx;
						tb-=debx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr-=derx;
							tg-=degx;
							tb-=debx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
						tr+=derx;
						tg+=degx;
						tb+=debx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = RGB32BIT(0,WRAP_BYTE((int)tr),WRAP_BYTE((int)tg),WRAP_BYTE((int)tb));*tempMem = RGB32BIT(0,Wrap_Byte(FIXP16_2_INT(tr)),Wrap_Byte(FIXP16_2_INT(tg)),Wrap_Byte(FIXP16_2_INT(tb)));
							tr+=derx;
							tg+=degx;
							tb+=debx;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}

		y-=FIXP16_1;
		if(y<edgeY)
			break;

		bp.x -= dbpx;
		ep.x -= depx;

		br-=dbr;
		er-=der;
		bg-=dbg;
		eg-=deg;
		bb-=dbb;
		eb-=deb;
	}
	
}

void DrawTextureTriangle_Const(DWORD* pMem, 
							  LONG pitch32, 
							  const Vec2f& wndOrigin, 
							  const Range& wndRange, 
							  Vec2f p0, 
							  Vec2f p1, 
							  Vec2f p2,
							  Vec2f t0,
							  Vec2f t1,
							  Vec2f t2,
							  const BITMAP* texture){
	
}

void DrawTextureTriangle(DWORD* pMem, 
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
						 const BITMAP* texture){
	
	p0.y=FIXP16_FLOOR(p0.y);
	p0.x=FIXP16_FLOOR(p0.x);
	p1.y=FIXP16_FLOOR(p1.y);
	p1.x=FIXP16_FLOOR(p1.x);
	p2.y=FIXP16_FLOOR(p2.y);
	p2.x=FIXP16_FLOOR(p2.x);

	//填充模式的三角形
	Vec2FIXP16* minP;
	Vec2FIXP16* maxP;
	Vec2FIXP16* middleP;
	Vec2FIXP16* minT;
	Vec2FIXP16* maxT;
	Vec2FIXP16* middleT;
	if(p0.y<p1.y){
		//0 1 2
		if(p1.y<p2.y){
			minP=&p0;
			middleP=&p1;
			maxP=&p2;

			minT=&t0;
			middleT=&t1;
			maxT=&t2;
		}
		//0 2 1
		else if(p0.y<p2.y){
			minP=&p0;
			middleP=&p2;
			maxP=&p1;

			minT=&t0;
			middleT=&t2;
			maxT=&t1;
		}
		//2 0 1  
		else{
			minP=&p2;
			middleP=&p0;
			maxP=&p1;

			minT=&t2;
			middleT=&t0;
			maxT=&t1;
		}
	}else{
		//2 1 0
		if(p1.y>p2.y){
			minP=&p2;
			middleP=&p1;
			maxP=&p0;

			minT=&t2;
			middleT=&t1;
			maxT=&t0;
		}
		//1 2 0
		else if(p0.y>p2.y){
			minP=&p1;
			middleP=&p2;
			maxP=&p0;

			minT=&t1;
			middleT=&t2;
			maxT=&t0;
		}
		//1 0 2
		else{
			minP=&p1;
			middleP=&p0;
			maxP=&p2;

			minT=&t1;
			middleT=&t0;
			maxT=&t2;
		}
	}

	static Vec2FIXP16 bp;
	static Vec2FIXP16 ep;
	int wndRangeLeft=FIXP16_2_INT(wndRange.left);
	int wndRangeRight=FIXP16_2_INT(wndRange.right);
	int wndRangeTop=FIXP16_2_INT(wndRange.top);
	int wndRangeBottom=FIXP16_2_INT(wndRange.bottom);

	FIXP16 max_min=maxP->y-minP->y;
	FIXP16 _1_max_min;
	FIXP16 depx;
	if(max_min<FIXP16_1){
		max_min=FIXP16_1;
		_1_max_min=FIXP16_1;
		depx=maxP->x - minP->x;
	}else{
		FIXP16_Div(FIXP16_1,max_min,_1_max_min);
		FIXP16_Mul(maxP->x - minP->x,_1_max_min,depx);
	}

	FIXP16 dbpx;
	
	//起点颜色
	FIXP16 dbu;
	FIXP16 dbv;
	FIXP16 bu;
	FIXP16 bv;

	//终点颜色
	FIXP16 deu;
	FIXP16_Mul(maxT->u - minT->u,_1_max_min,deu);
	FIXP16 dev;
	FIXP16_Mul(maxT->v - minT->v,_1_max_min,dev);
	FIXP16 eu;
	FIXP16 ev;

	//扫描线颜色
	FIXP16 dtu;
	FIXP16 dtv;
	FIXP16 tu;
	FIXP16 tv;

	//随x轴改变的颜色 在x轴位移不连续的时候使用
	FIXP16 dbux;
	FIXP16 dbvx;
	FIXP16 deux;
	FIXP16 devx;

	bool bpOutLeft=false;
	bool bpOutRight=false;
	bool epOutLeft=false;
	bool epOutRight=false;
	int intY;
	int intBpx=0;
	int intEpx=0;
	int preBpx=0;
	int preEpx=0;
	int midpx=0;

	if(ABS(depx)>FIXP16_1){
		FIXP16_Div(deu,depx,deux);
		FIXP16_Div(dev,depx,devx);
	}

	///////////////////////////////////////////////////////
	//从最小到中间
	DWORD* tempMem;
	FIXP16 middle_min=middleP->y - minP->y;
	if(middle_min>FIXP16_1){
		FIXP16 _1_middle_min;
		FIXP16_Div(FIXP16_1,middle_min,_1_middle_min);
		FIXP16_Mul(middleP->x - minP->x,_1_middle_min,dbpx);
		FIXP16_Mul(middleT->u - minT->u,_1_middle_min,dbu);
		FIXP16_Mul(middleT->v - minT->v,_1_middle_min,dbv);
	}
	else{
		dbpx = middleP->x - minP->x;
		dbu =  middleT->u - minT->u;
		dbv =  middleT->v - minT->v;
	}

	if(ABS(dbpx)>FIXP16_1){
		FIXP16_Div(dbu,dbpx,dbux);
		FIXP16_Div(dbv,dbpx,dbvx);
	}

	FIXP16 y = minP->y+wndOrigin.y;
	FIXP16 edgeY=middleP->y+wndOrigin.y;
	ep.x = bp.x = minP->x+wndOrigin.x;
	eu=bu=minT->u;
	ev=bv=minT->v;

	bool needScan;
	FIXP16 dis;
	FIXP16 fixTemp;
	if(y<wndRange.top){
		y=wndRange.top;
	}
	if(y<=edgeY){
		dis=y-minP->y-wndOrigin.y;
		FIXP16_Mul(dbpx,dis,fixTemp);
		bp.x+=fixTemp;
		FIXP16_Mul(depx,dis,fixTemp);
		ep.x+=fixTemp;
		FIXP16_Mul(dbu,dis,fixTemp);
		bu+=fixTemp;
		FIXP16_Mul(deu,dis,fixTemp);
		eu+=fixTemp;
		FIXP16_Mul(dbv,dis,fixTemp);
		bv+=fixTemp;
		FIXP16_Mul(dev,dis,fixTemp);
		ev+=fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);
	bool needSlerp=false;
	extern int log2Table[];
	int shiftWidth=log2Table[texture->width];
	while(needScan){
		ep.y=bp.y=y;
		intY=FIXP16_2_INT(y);
		if(intY>wndRangeBottom)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);
		bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if((!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			FIXP16 _1_offsetMaxX_MinX=ep.x-bp.x;
			if(ABS(_1_offsetMaxX_MinX)>FIXP16_1)
				FIXP16_Div(FIXP16_1,_1_offsetMaxX_MinX,_1_offsetMaxX_MinX);
			else
				_1_offsetMaxX_MinX=0;
			FIXP16_Mul(eu-bu,_1_offsetMaxX_MinX,dtu);
			FIXP16_Mul(ev-bv,_1_offsetMaxX_MinX,dtv);

			tempMem = pMem+intY*pitch32+intBpx;

			FIXP16 offsetColor=INT_2_FIXP16(intBpx-FIXP16_ROUND(bp.x));
			FIXP16_Mul(dtu,offsetColor,tu);
			tu=bu+tu;
			FIXP16_Mul(dtv,offsetColor,tv);
			tv=bv+tv;

			//test-----------------------------
			//*tempMem = RGB32BIT(0,(int)bu,(int)bv,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)eu,(int)ev,(int)eb);
			//test-----------------------------
			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
					tu+=dtu;
					tv+=dtv;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
					tu-=dtu;
					tv-=dtv;
				}
			}
			else{
				*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				tu=bu;
				tv=bv;
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu-=dbux;
						tv-=dbvx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu-=dbux;
							tv-=dbvx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu+=dbux;
						tv+=dbvx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu+=dbux;
							tv+=dbvx;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				tu=eu;
				tv=ev;
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu-=deux;
						tv-=devx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu-=deux;
							tv-=devx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu+=deux;
						tv+=devx;
					}
					if(intY>wndRangeTop){
						tempMem = pMem+(intY-1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu+=deux;
							tv+=devx;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}
		
		y+=FIXP16_1;
		if(y>edgeY)
			break;

		bp.x += dbpx;
		ep.x += depx;
		bu+=dbu;
		eu+=deu;
		bv+=dbv;
		ev+=dev;
	}
	
	///////////////////////////////////////////////////////
	//从中间到最大
	FIXP16 max_middle=maxP->y-middleP->y;
	if(max_middle>FIXP16_1){
		FIXP16 _1_max_middle;
		FIXP16_Div(FIXP16_1,max_middle,_1_max_middle);
		FIXP16_Mul(maxP->x - middleP->x,_1_max_middle,dbpx);
		FIXP16_Mul(maxT->u - middleT->u,_1_max_middle,dbu );
		FIXP16_Mul(maxT->v - middleT->v,_1_max_middle,dbv );
	}else{
		dbpx = maxP->x - middleP->x;
		dbu  = maxT->u - middleT->u;
		dbv  = maxT->v - middleT->v;
	}

	if(ABS(dbpx)>FIXP16_1){
		FIXP16_Div(dbu,dbpx,dbux);
		FIXP16_Div(dbv,dbpx,dbvx);
	}

	ep.x = bp.x = maxP->x+wndOrigin.x;
	eu=bu=maxT->u;
	ev=bv=maxT->v;
	y=maxP->y+wndOrigin.y;

	if(y>wndRange.bottom){
		y=wndRange.bottom;
	}
	if(y>=edgeY){
		dis=maxP->y+wndOrigin.y-y;
		FIXP16_Mul(dbpx,dis,fixTemp);
		bp.x-=fixTemp;
		FIXP16_Mul(depx,dis,fixTemp);
		ep.x-=fixTemp;
		FIXP16_Mul(dbu,dis,fixTemp);
		bu-=fixTemp;
		FIXP16_Mul(deu,dis,fixTemp);
		eu-=fixTemp;
		FIXP16_Mul(dbv,dis,fixTemp);
		bv-=fixTemp;
		FIXP16_Mul(dev,dis,fixTemp);
		ev-=fixTemp;
		needScan=true;
	}
	else{
		needScan=false;
	}

	preBpx=intBpx=FIXP16_ROUND(bp.x);
	preEpx=intEpx=FIXP16_ROUND(ep.x);
	needSlerp=false;
	while(needScan){
		ep.y = bp.y = y;
		intY=FIXP16_2_INT(y);
		if(intY<wndRangeTop)break;
		preBpx=intBpx;
		preEpx=intEpx;
		intBpx=FIXP16_ROUND(bp.x);
		intEpx=FIXP16_ROUND(ep.x);bpOutLeft=false;
		bpOutRight=false;
		epOutLeft=false;
		epOutRight=false;

		if(intBpx<wndRangeLeft){
			intBpx=wndRangeLeft;
			bpOutLeft=true;
		}
		if(intBpx>wndRangeRight){
			intBpx=wndRangeRight;
			bpOutRight=true;
		}
		if(intEpx<wndRangeLeft){
			intEpx=wndRangeLeft;
			epOutLeft=true;
		}
		if(intEpx>wndRangeRight){
			intEpx=wndRangeRight;
			epOutRight=true;
		}
		if(intY<=wndRangeBottom&&
			(!(bpOutLeft&&epOutLeft))&&
			(!(bpOutRight&&epOutRight))){
			FIXP16 _1_offsetMaxX_MinX=ep.x-bp.x;
			if(ABS(_1_offsetMaxX_MinX)>FIXP16_1)
				FIXP16_Div(FIXP16_1,_1_offsetMaxX_MinX,_1_offsetMaxX_MinX);
			else
				_1_offsetMaxX_MinX=0;
			FIXP16_Mul(eu-bu,_1_offsetMaxX_MinX,dtu);
			FIXP16_Mul(ev-bv,_1_offsetMaxX_MinX,dtv);

			tempMem = pMem+intY*pitch32+intBpx;

			//test----------------------------------------
			//*tempMem = RGB32BIT(0,(int)bu,(int)bv,(int)bb);
			//tempMem = pMem+intY*pitch32+intEpx;
			//*tempMem = RGB32BIT(0,(int)eu,(int)ev,(int)eb);
			//test----------------------------------------
			
			FIXP16 offsetColor=INT_2_FIXP16(intBpx-FIXP16_ROUND(bp.x));
			FIXP16_Mul(dtu,offsetColor,tu);
			FIXP16_Mul(dtv,offsetColor,tv);
			tu=bu+tu;
			tv=bv+tv;

			int sign=intEpx-intBpx;
			if(sign>0){
				for (int x = intBpx; x <= intEpx; x++,tempMem++){
					*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
					tu+=dtu;
					tv+=dtv;
				}
			}
			else if(sign<0){
				for (int x = intBpx; x >= intEpx; x--,tempMem--){
					*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
					tu-=dtu;
					tv-=dtv;
				}
			}
			else{
				*tempMem =  Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
			}

			//处理起始点不连续的情况
			if(needSlerp&&!bpOutLeft&&!bpOutRight&&ABS(intBpx-preBpx)>1){
				tu=bu;
				tv=bv;
				midpx=intBpx-((intBpx-preBpx)>>1);
				if(intBpx-preBpx>1){
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x>midpx;x--,tempMem--){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu-=dbux;
						tv-=dbvx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preBpx;x--,tempMem--){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu-=dbux;
							tv-=dbvx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intBpx;
					for(int x=intBpx;x<midpx;x++,tempMem++){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu+=dbux;
						tv+=dbvx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<=preBpx;x++,tempMem++){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu+=dbux;
							tv+=dbvx;
						}
					}
				}
			}

			//处理终点不连续的情况
			if(needSlerp&&!epOutLeft&&!epOutRight&&ABS(intEpx-preEpx)>1){
				tu=eu;
				tv=ev;
				midpx=intEpx-((intEpx-preEpx)>>1);
				if(intEpx-preEpx>1){
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x>midpx;x--,tempMem--){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu-=deux;
						tv-=devx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x>=preEpx;x--,tempMem--){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu-=deux;
							tv-=devx;
						}
					}
				}else{
					tempMem = pMem+intY*pitch32+intEpx;
					for(int x=intEpx;x<midpx;x++,tempMem++){
						*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
						tu+=deux;
						tv+=devx;
					}
					if(intY<wndRangeBottom){
						tempMem = pMem+(intY+1)*pitch32+midpx;
						for(int x=midpx;x<preEpx;x++,tempMem++){
							*tempMem = Color_Mul(GetBmpPixel(texture,FIXP16_2_INT(tu),FIXP16_2_INT(tv),shiftWidth),mr,mg,mb);
							tu+=deux;
							tv+=devx;
						}
					}
				}
			}
			needSlerp=true;
		}else{
			needSlerp=false;
		}

		y-=FIXP16_1;
		if(y<edgeY)
			break;

		bp.x -= dbpx;
		ep.x -= depx;

		bu-=dbu;
		eu-=deu;
		bv-=dbv;
		ev-=dev;
	}
}

void DrawPolygon2D_Line(DWORD* pMem, LONG pitch32, const Vec2FIXP16& wndOrigin, const RangeFIXP16& wndRange, DWORD color, int numVer, Vec3f* vList){
	for (int i = 0; i < numVer; i++){
		int next = i + 1 >= numVer ? 0 : i + 1;
		DrawLine(pMem,pitch32,wndOrigin,wndRange,color, vList[i].v2f(), vList[next].v2f());
	}
}

void DrawPolygon2D_Fill(DWORD* pMem, 
						LONG pitch32, 
						const Vec2FIXP16& wndOrigin, 
						const RangeFIXP16& wndRange, 
						DWORD color, 
						int numVer, 
						Vec3f* vList){
	int second=1;
	int third=2;
	for(;third<numVer;third++,second++){
		DrawTriangle(pMem, pitch32, wndOrigin, wndRange, color, vList[0].v2f(), vList[second].v2f(), vList[third].v2f());
	}
}

void DrawLightingLine(DWORD* pMem, 
					  LONG pitch32, 
					  const Vec2FIXP16& wndOrigin,
					  const RangeFIXP16& wndRange,
					  DWORD color,
					  Vec2f* vList,
					  int vNum,
					  int seg){
	static Vec2f pre;
	static Vec2f now;
	for(int i=0;i<vNum-1;i++){
		float dx=(vList[i+1].x-vList[i].x)/seg;
		float dy=(vList[i+1].y-vList[i].y)/seg;
		float rx=ABS(dy/100.0f)+1.0f;
		float ry=ABS(dx/100.0f)+1.0f;
		pre=vList[i];
		now=pre;
		float tx=now.x;
		float ty=now.y;
		for(int j=1;j<seg;j++){
			tx+=dx;
			ty+=dy;
			now.y=ty+ry*Rand_Range(-10,10);
			now.x=tx+rx*Rand_Range(-10,10);
			DrawLine(pMem, 
					 pitch32, 
					 wndOrigin,
				     wndRange,
				     color,pre,now);
			pre=now;
		}
		now.x=tx+dx;
		now.y=ty+dy;
		DrawLine(pMem, 
			pitch32, 
			wndOrigin,
			wndRange,
			color,pre,now);
	}
}

NS_ENGIN_END