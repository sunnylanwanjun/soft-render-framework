#include "Sprite.h"
#include "t3dlib1.h"
#include "Renderer.h"
NS_ENGIN_BEGIN

Sprite::Sprite():_texture(nullptr){

}

Sprite::~Sprite(){
	if (_texture){
		_texture->release();
		_texture = nullptr;
	}
}

Sprite* Sprite::create(const std::string& fileName){
	Sprite* sp = new Sprite;
	if (sp->initWithFile(fileName)){
		sp->autoRelease();
		return sp;
	}
	else{
		delete sp;
		return nullptr;
	}
}

Sprite* Sprite::createWithTexture(Texture* texture){
	Sprite* sp = new Sprite;
	if (sp->initWithTexture(texture)){
		sp->autoRelease();
		return sp;
	}
	else{
		delete sp;
		return nullptr;
	}
}

bool Sprite::initWithTexture(Texture* texture){
	if (texture == nullptr)
		return false;
	_texture = texture;
	setContentSize(_texture->getContentSize());
	_texture->retain();
	return true;
}

void Sprite::setTexture(Texture* texture){
	if(_texture){
		_texture->release();
		_texture=nullptr;
	}
	initWithTexture(texture);
}

bool Sprite::initWithFile(const std::string& fileName){
	_texture=Texture::create(fileName);
	if (_texture==nullptr)
		return false;
	setContentSize(_texture->getContentSize());
	_texture->retain();
	return true;
}

void Sprite::draw(Renderer* renderer, Mat4& transform){
	renderer->render(_pos, _contentSize, _scaleX, _scaleY, _rotation, _texture);
}

NS_ENGIN_END