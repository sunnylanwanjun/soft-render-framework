#include "Layer3D.h"
#include "Scene3D.h"
NS_ENGIN_BEGIN
Layer3D::Layer3D(){

}

Layer3D::~Layer3D(){
	if(_scene){
		_scene->release();
		_scene = nullptr;
	}
}

Layer3D* Layer3D::create(){
	Layer3D* layer3D = new Layer3D;
	layer3D->autoRelease();
	return layer3D;
}

void Layer3D::setScene( Scene3D* scene ){
	if(_scene){
		_scene->release();
		_scene = nullptr;
	}
	_scene=scene;
	_scene->retain();
}

void Layer3D::draw(Renderer* renderer,Mat4& transform){
	_scene->drawScene(renderer);
}

NS_ENGIN_END