#ifndef __TEXTURE_H
#define __TEXTURE_H
#include "macros.h"
#include "Ref.h"
#include <string>
#include "Geometry.h"
#include "Image.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Texture:public Ref{
public:
	Texture();
	Texture(const Texture& texture);
	Texture& operator=(const Texture& texture);
	virtual ~Texture();
	static Texture* create(const std::string& fileName, float scaleX=1, float scaleY=1);
	static Texture* createWithImage(Image* image);
	void setTexture(const Texture& texture);
	bool initWithFile(const std::string& fileName, float scaleX = 1, float scaleY = 1);
	bool initWithImage(Image* image);
	bool initWithTexture(const Texture& texture);
	Size getContentSize()const{ return _image->getContentSize(); }
	BYTE* getData()const{ return _image->getData(); };
	LPDIRECTDRAWSURFACE7 getDirectDrawSurface(){ return _image->getDirectDrawSurface(); }
	void updateDirectDrawSurface(){_image->updateDirectDrawSurface();}
private:
	Image* _image;
};
NS_ENGIN_END
#endif