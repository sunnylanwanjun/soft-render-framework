#ifndef __IMAGE_H
#define __IMAGE_H
#include "macros.h"
#include "Node.h"
#include <string>
#include <vector>
#include "t3dlib1.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Image :public Ref{
	
	enum class Format
	{
		JPG,
		PNG,
		TIFF,
		WEBP,
		PVR,
		ETC,
		S3TC,
		ATITC,
		TGA,
		RAW_DATA,
		UNKNOWN
	};

public:
	Image();
	virtual ~Image();
	static Image* create(const std::string& fileName, float scaleX=1, float scaleY=1,DWORD srcKey=0);
	static Image* createWithRGBData(const BYTE* data, int w, int h, float scaleX=1, float scaleY=1,DWORD srcKey=0);
	LPDIRECTDRAWSURFACE7 getDirectDrawSurface(){ return _lpdds; }
	void updateDirectDrawSurface();
	BYTE* getData(){ return _data; }
	int  getDataLen(){return _dataLen;}
	void setData(const BYTE* data,int w,int h);
	Size getContentSize(){ return Size(_width, _height); }
	Size getDataSize(){ return Size(_rawWidth, _rawHeight); }
private:
	bool initWithFile(const std::string& fileName);
	bool initWithBmpData(BYTE* data,size_t len);
	bool initWithPngData(BYTE* data,size_t len);
	bool initWithRGBData(const BYTE* data, int w, int h);
	bool initDirectDrawSurface();
	bool isPng(BYTE* data, size_t len);
	bool isBmp(BYTE* data, size_t len);
private:
	LPDIRECTDRAWSURFACE7 _lpdds;
	BYTE*    _data;
	size_t   _dataLen;
	float	 _width;
	float	 _height;
	int		_rawWidth;
	int		_rawHeight;
	float    _scaleX;
	float    _scaleY;
	DWORD   srcKey;
};

NS_ENGIN_END
#endif