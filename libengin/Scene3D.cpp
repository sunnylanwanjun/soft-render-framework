#include "Scene3D.h"
NS_ENGIN_BEGIN
Scene3D::Scene3D():_cam(nullptr){
	
}

Scene3D::~Scene3D(){
	if(_cam){
		_cam->release();
		_cam=nullptr;
	}
}

Scene3D* Scene3D::create(){
	Scene3D* scene = new Scene3D;
	scene->autoRelease();
	return scene;
}

void Scene3D::setCamera(Camera* cam){
	if(_cam){
		_cam->release();
		_cam = nullptr;
	}
	_cam=cam;
	_cam->retain();
}

void Scene3D::drawScene(Renderer* renderer){
	visit(renderer,_cam,_modelViewTransMat);
}

NS_ENGIN_END