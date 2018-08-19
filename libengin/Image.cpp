#include "Image.h"
#include <fstream>
#include "log.h"
#include "..\external\pnglib\png.h"
#include "FileUtils.h"
#include "Director.h"
#include "Renderer.h"
NS_ENGIN_BEGIN

Image::Image() :_width(0.0f), _height(0.0f), _data(nullptr),_dataLen(0), _lpdds(nullptr),_scaleX(1),_scaleY(1),_rawHeight(0),_rawWidth(0),srcKey(0x00000000){
	
}

Image::~Image(){
	delete[] _data;
	_data = nullptr;
	if (_lpdds){
		_lpdds->Release();
		_lpdds = nullptr;
	}
}

Image* Image::create(const std::string& fileName, float scaleX, float scaleY,DWORD srcKey){
	Image* image = new Image;
	image->_scaleX = scaleX;
	image->_scaleY = scaleY;
	image->srcKey=srcKey;
	if (image->initWithFile(fileName)){
		image->autoRelease();
		return image;
	}
	else{
		delete image;
		return nullptr;
	}
}

Image* Image::createWithRGBData(const BYTE* data, int w, int h, float scaleX, float scaleY,DWORD srcKey){
	Image* image = new Image;
	image->_scaleX = scaleX;
	image->_scaleY = scaleY;
	image->srcKey=srcKey;
	if (image->initWithRGBData(data, w, h)){
		image->autoRelease();
		return image;
	}
	else{
		delete image;
		return nullptr;
	}
}

bool Image::initWithFile(const std::string& fileName){
	Data data=FileUtils::getInstance()->getDataFromFile(fileName);
	if (data.isNull()){
		return false;
	}
	BYTE* bytes = data.getBytes();
	size_t size = data.getSize();
	bool ret = false;
	if (isPng(bytes,size)){
		ret = initWithPngData(bytes, size);
	}
	else if (isBmp(bytes, size)){
		ret = initWithBmpData(bytes, size);
	}
	if (ret){
		ret = initDirectDrawSurface();
	}
	return ret;
}
 
bool Image::initWithRGBData(const BYTE* data, int w, int h){
	_rawHeight = h;
	_rawWidth = w;
	_dataLen = _rawWidth*_rawHeight*4;
	_data = new BYTE[_dataLen];
	memcpy(_data, data, _dataLen);
	return initDirectDrawSurface();
}

void Image::setData(const BYTE* data,int w,int h){
	if(_data){
		delete[] _data;
		_data = nullptr;
	}
	initWithRGBData(data,w,h);
}

bool Image::initWithBmpData(BYTE* data,size_t len){
	DWORD* bmpData = (DWORD*)data;
	bool ret = GetBmp(data, len, _data, _dataLen, 0, 0, _rawWidth, _rawHeight);
	if (ret){
		ret = initDirectDrawSurface();
	}
	return ret;
}

void Image::updateDirectDrawSurface(){
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(_lpdds->Lock(nullptr, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))){
		Error("Image::updateDirectDrawSurface Lock failed");
		return;
	}
	BltBmpData32((DWORD*)_data, _rawWidth, _rawHeight, (DWORD*)ddsd.lpSurface, (int)_width, (int)_height, ddsd.lPitch >> 2);
	_lpdds->Unlock(nullptr);
}

bool Image::initDirectDrawSurface(){
	if (_lpdds){
		_lpdds->Release();
		_lpdds = nullptr;
	}
	LPDIRECTDRAW7 lpdd7 = Director::getInstance()->getRenderer()->getDirectDraw();
	_width = _rawWidth*_scaleX;
	_height = _rawHeight*_scaleY;
	_lpdds = DirectDrawSurfaceInit_Common(lpdd7, (int)_width, (int)_height, DDSCAPS_VIDEOMEMORY, srcKey, srcKey);
	if (_lpdds == nullptr){
		return false;
	}
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(_lpdds->Lock(nullptr, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))){
		Error("Image::initDirectDrawSurface Lock failed");
		return false;
	}
	BltBmpData32((DWORD*)_data, _rawWidth, _rawHeight, (DWORD*)ddsd.lpSurface, (int)_width, (int)_height, ddsd.lPitch >> 2);
	_lpdds->Unlock(nullptr);
	return true;
}

struct custom_read_struct{
	size_t offset;
	size_t totalLen;
	BYTE*  src;
};

void custom_read_func(png_structp png_ptr, png_bytep data, png_size_t length){
	if (png_ptr == NULL)
		return;
	custom_read_struct* crs = (custom_read_struct*)png_get_io_ptr(png_ptr);
	if (crs->offset + length <= crs->totalLen){
		memcpy(data, crs->src + crs->offset, length);
		crs->offset += length;
	}
	else{
		Error("Read Png Error");
	}
}

bool Image::initWithPngData(BYTE* data, size_t len){
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr){
		return false;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return false;
	}

	custom_read_struct crs;
	crs.totalLen = len;
	crs.offset = 0;
	crs.src = data;
	png_set_read_fn(png_ptr, &crs, custom_read_func);
	png_read_info(png_ptr, info_ptr);
	int iColorType = 0;
	int iBitDepth = 0;
	png_get_IHDR(png_ptr, info_ptr, (size_t*)&_rawWidth, (size_t*)&_rawHeight, &iBitDepth, &iColorType, nullptr, nullptr, nullptr);

	// force palette images to be expanded to 24-bit RGB
	// it may include alpha channel
	if (iColorType == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}
	// low-bit-depth grayscale images are to be expanded to 8 bits
	if (iColorType == PNG_COLOR_TYPE_GRAY && iBitDepth < 8)
	{
		iBitDepth = 8;
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	// expand any tRNS chunk data into a full alpha channel
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}
	// reduce images with 16-bit samples to 8 bits
	if (iBitDepth == 16)
	{
		png_set_strip_16(png_ptr);
	}

	// Expanded earlier for grayscale, now take care of palette and rgb
	if (iBitDepth < 8)
	{
		png_set_packing(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (size_t*)&_rawWidth, (size_t*)&_rawHeight, &iBitDepth, &iColorType, nullptr, nullptr, nullptr);

	size_t rowBytes=png_get_rowbytes(png_ptr, info_ptr);
	_dataLen = _rawHeight*rowBytes;
	_data = new BYTE[_rawHeight*rowBytes];
	BYTE** row_ptr = (BYTE**)malloc(4*_rawHeight);
	for (int i = 0; i < _rawHeight; i++){
		*(row_ptr + i) = _data + i*rowBytes;
	}
	png_read_image(png_ptr, row_ptr);
	png_read_end(png_ptr, info_ptr);
	free(row_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	reverseRGBA(_data, _dataLen);
	return true;
}

bool Image::isPng(BYTE* data, size_t len){
	static BYTE png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	static BYTE buffer_sign[8] = {0};
	memcpy(buffer_sign, data, sizeof buffer_sign);
	return (memcmp(buffer_sign, png_signature, sizeof buffer_sign))==0;
}

bool Image::isBmp(BYTE* data, size_t len){
	BITMAPFILEHEADER bmpfh;
	memcpy(&bmpfh, data, sizeof BITMAPFILEHEADER);
	if (bmpfh.bfType != 0x4d42){
		Log("Error:%s%x\r\n", "GetBmp:not a bitmap file,type is ", bmpfh.bfType);
		return false;
	}
	BITMAPINFOHEADER bmpih;
	memcpy(&bmpih, data + sizeof BITMAPFILEHEADER, sizeof BITMAPINFOHEADER);
	WORD bc = bmpih.biBitCount;
	if (bc != 24){
		Log("Error:%s%x\r\n", "GetBmp:not a 24 bitmap file,bitCount is ", bc);
		return false;
	}
	return true;
}

NS_ENGIN_END