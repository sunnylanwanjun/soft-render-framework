#include "Action.h"
#include "Node.h"
#include "log.h"
NS_ENGIN_BEGIN

Action::Action() :_target(nullptr){

}

Action::~Action(){

}

void Action::start(Ref* target){
	if (_target){
		_target->release();
		_target = nullptr;
	}
	_target = target;
	_target->retain();
}

void Action::stop(){
	_target->release();
	_target = nullptr;
}

/************************************************************************/
/* 间隔动画                                                             */
/************************************************************************/

ActionInterval::ActionInterval():_dur(0){

}

ActionInterval::~ActionInterval(){

}

/************************************************************************/
/* 随机移动动画                                                         */
/************************************************************************/

RandomMoveBy::RandomMoveBy():_range(0,0,0,0){
	_xDir = rand() % 10 > 5 ? DIR_LEFT:DIR_RIGHT;
	_yDir = rand() % 10 > 5 ? DIR_UP : DIR_DOWN;
}
 
RandomMoveBy::~RandomMoveBy(){

}

void RandomMoveBy::update(float dt){
	Node* target = (Node*)_target;
	if (target == nullptr){
		return;
	}
	Vec3f pos = target->getPosition();
	Size size = target->getContentSize();
	pos.x += (int)(_xDir*_speed*dt);
	pos.y += (int)(_yDir*_speed*dt);

	if (pos.x < _range.left){
		_xDir = DIR_RIGHT;
		pos.x = 0;
	}
	else if (pos.x + size.width > _range.right){
		_xDir = DIR_LEFT;
		pos.x = _range.right - size.width;
	}

	if (pos.y < _range.top){
		_yDir = DIR_DOWN;
		pos.y = 0;
	}
	else if (pos.y + size.height > _range.bottom){
		_yDir = DIR_UP;
		pos.y = _range.bottom - size.height;
	}

	target->setPosition(pos);
}

RandomMoveBy* RandomMoveBy::create(Range& range, int speed){
	RandomMoveBy* randomMove = new RandomMoveBy;
	randomMove->autoRelease();
	randomMove->_range = range;
	randomMove->_speed = speed;
	return randomMove;
}

/************************************************************************/
/* 无限旋转动画                                                         */
/************************************************************************/

RepeatRotateBy::RepeatRotateBy() :_angleSpeed(0),_totalAngle(0){

}

RepeatRotateBy::~RepeatRotateBy(){

}

RepeatRotateBy* RepeatRotateBy::create(float angle){
	RepeatRotateBy* rotate = new RepeatRotateBy;
	rotate->_angleSpeed = angle;
	rotate->autoRelease();
	return rotate;
}

void RepeatRotateBy::update(float dt){
	Node* target = (Node*)_target;
	if (target == nullptr){
		return;
	}
	_totalAngle += _angleSpeed*dt;
	target->setRotation(_totalAngle);
}

/************************************************************************/
/* 无限缩放动画                                                         */
/************************************************************************/

RepeatScaleBy::RepeatScaleBy() :_scaleSpeed(0), _totalScale(1), _scaleDir(1), _maxScale(1), _minScale(1){

}

RepeatScaleBy::~RepeatScaleBy(){

}

RepeatScaleBy* RepeatScaleBy::create(float scale, float maxScale, float minScale){
	RepeatScaleBy* scaleBy = new RepeatScaleBy;
	scaleBy->_scaleSpeed = scale;
	scaleBy->_maxScale = maxScale;
	scaleBy->_minScale = minScale;
	scaleBy->autoRelease();
	return scaleBy;
}

void RepeatScaleBy::update(float dt){
	Node* target = (Node*)_target;
	if (target == nullptr){
		return;
	}
	_totalScale += _scaleDir*_scaleSpeed*dt;
	if (_totalScale >= _maxScale){
		_scaleDir = -1;
		_totalScale = _maxScale;
	}
	else if (_totalScale <= _minScale){
		_scaleDir = 1;
		_totalScale = _minScale;
	}
	target->setScale(_totalScale);
}
NS_ENGIN_END