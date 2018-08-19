#include "Texture.h"
#include "t3dlib1.h"
NS_ENGIN_BEGIN

Texture::Texture() :_image(nullptr){

}

Texture::Texture(const Texture& texture){
	setTexture(texture);
}

bool Texture::initWithTexture(const Texture& texture){
	Size contentSize=texture.getContentSize();
	_image = Image::createWithRGBData(texture.getData(),(int)contentSize.width,(int)contentSize.height);
	if(_image==nullptr){
		return false;
	}
	_image->retain();
	return true;
}

void Texture::setTexture(const Texture& texture){
	if(_image){
		_image->release();
		_image=nullptr;
	}
	initWithTexture(texture);
}

Texture::~Texture(){
	if (_image){
		_image->release();
		_image = nullptr;
	}
}

Texture* Texture::create(const std::string& fileName,float scaleX,float scaleY){
	Texture* tex = new Texture;
	if (tex->initWithFile(fileName,scaleX,scaleY)){
		tex->autoRelease();
		return tex;
	}
	else{
		delete tex;
		return nullptr;
	}
}

bool Texture::initWithFile(const std::string& fileName, float scaleX, float scaleY){
	_image = Image::create(fileName, scaleX, scaleY);
	if (_image == nullptr){
		return false;
	}
	_image->retain();
	return true;
}

Texture* Texture::createWithImage(Image* image){
	Texture* tex = new Texture;
	if (tex->initWithImage(image)){
		tex->autoRelease();
		return tex;
	}
	else{
		delete tex;
		return nullptr;
	}
}

bool Texture::initWithImage(Image* image){
	if (image == nullptr){
		return false;
	}
	_image = image;
	_image->retain();
	return true;
}

Texture& Texture::operator=( const Texture& texture ){
	setTexture(texture);
	return *this;
}

NS_ENGIN_END