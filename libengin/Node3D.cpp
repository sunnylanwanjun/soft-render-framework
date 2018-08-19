#include "Node3D.h"
#include "Director.h"
#include "ActionManager.h"
#include "Action.h"
#include "Camera.h"
#include <algorithm>
NS_ENGIN_BEGIN
Node3D::Node3D():
_visible(true),
_parent(nullptr),
_wireMode(true),
_modelViewMatDirty(true),
_lightOn(false),
_sortOn(true),
_zOrder(0){
	_actionManager = Director::getInstance()->getActionManager();
	_scale.x=_scale.y=_scale.z=1;
}

Node3D::~Node3D(){

}

bool sortFunction(Node3D*c0,Node3D*c1){
	if(c0->_pos.z>c1->_pos.z)
		return true;
	return false;
}

void Node3D::runAction(Action* action){
	_actionManager->addAction(action, this);
}

void Node3D::setScale( Vec3f& vs ){
	_scale=vs;
	_modelViewMatDirty=true;
}

void Node3D::setRotation( Vec3f& vr ){
	_rotation=vr;
	_modelViewMatDirty=true;
}

void Node3D::setPosition( Vec3f& vt ){
	_pos=vt;
	_modelViewMatDirty=true;
}

void Node3D::updateModelViewTransMat(){
	Mat4 rm;
	Mat4::createRotation(_rotation,rm);
	Mat4::createScale(_scale.x,_scale.y,_scale.z,_modelViewTransMat);
	Mat4_Mul(rm,_modelViewTransMat,rm);
	Mat4::createTranslation(_pos,_modelViewTransMat);
	Mat4_Mul(rm,_modelViewTransMat,_modelViewTransMat);
}

void Node3D::visit(Renderer* renderer, Camera* cam, const Mat4& parentTransform){
	if(!_visible)
		return;
	bool isParentIdentity=Mat4_IsIdentity(parentTransform);
	if(_modelViewMatDirty){
		updateModelViewTransMat();
		//如果父节点的转换矩阵不是单位阵，后面会重新计算一次世界坐标
		if(isParentIdentity)
			updateLocalToWorldTrans(_modelViewTransMat);
		_modelViewMatDirty=false;
	}

	static Mat4 transMat;
	Mat4* pTransMat;
	if(isParentIdentity){
		pTransMat=&_modelViewTransMat;
	}else{
		Mat4_Mul(_modelViewTransMat,parentTransform,transMat);
		pTransMat=&transMat;
		updateLocalToWorldTrans(transMat);
	}

	int childNum = _children.size();
	if (childNum <= 0){
		draw(renderer, cam);
	}
	else{
		int i = 0;
		int childNum = _children.size();
		std::sort(_children.begin(),_children.end(),sortFunction);
		//使得zorder小于0的节点会在父节点的下面
		for (; i < childNum; i++){
			if (_children[i]->getZOrder() < 0){
				_children[i]->visit(renderer, cam, *pTransMat);
			}
			else{
				break;
			}
		}
		this->draw(renderer, cam);
		for (; i < childNum; i++){
			_children[i]->visit(renderer, cam, *pTransMat);
		}
	}
}

void Node3D::draw(Renderer* renderer, Camera* cam){

}

void Node3D::addChild(Node3D* child){
	insertChild(child);
}

void Node3D::insertChild(Node3D* child){
	_children.push_back(child);
	child->retain();
}

void Node3D::detachChild(Node3D* child, size_t index){
	child->_parent = nullptr;
	auto iter = std::next(_children.begin(), index);
	_children.erase(iter);
	child->release();
}

void Node3D::removeChild(Node3D* child){
	auto iter=std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()){
		size_t index = iter - _children.begin();
		detachChild(child, index);
	}
}

void Node3D::removeFromParent(){
	if (_parent){
		_parent->removeChild(this);
	}
}

NS_ENGIN_END