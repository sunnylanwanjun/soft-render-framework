#include "Action3D.h"
#include "Node3D.h"
#include "Mat4.h"
#include "Object3D.h"
#include "Camera.h"
#include "log.h"
NS_ENGIN_BEGIN
/************************************************************************/
/* 无限旋转动画3D                                                        */
/************************************************************************/
RepeatRotate3DBy::RepeatRotate3DBy(){

}

RepeatRotate3DBy::~RepeatRotate3DBy(){

}

RepeatRotate3DBy* RepeatRotate3DBy::create(const Vec3f& rotate){
	RepeatRotate3DBy* rotateBy = new RepeatRotate3DBy;
	rotateBy->_rotate = rotate;
	rotateBy->autoRelease();
	return rotateBy;
}

void RepeatRotate3DBy::update(float dt){
	Node3D* target = (Node3D*)_target;
	if (target == nullptr){
		return;
	}
	Vec3f_Add(_totalRotate,_rotate,_totalRotate);
	target->setRotation(_totalRotate);
}

RepeatCameraMove::RepeatCameraMove():_dis(0),_angle(0),_angleSpeed(0){
	
}

RepeatCameraMove::~RepeatCameraMove(){

}

RepeatCameraMove* RepeatCameraMove::create(float dis,float angleSpeed){
	RepeatCameraMove* camMove = new RepeatCameraMove;
	camMove->_dis = dis;
	camMove->_angleSpeed = angleSpeed;
	camMove->autoRelease();
	return camMove;
}

void RepeatCameraMove::update( float dt ){
	Camera* cam = (Camera*)_target;
	if(!cam)return;
	_angle+=_angleSpeed;
	if(_angle>=d2PI)
		_angle=0;
	_pos.x=_dis*cos(_angle);
	_pos.y=_dis*sin(_angle);
	_pos.z=2*_pos.y;
	cam->setPos(_pos);
	cam->updateCamMatrix();
}

NS_ENGIN_END