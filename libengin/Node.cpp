#include "Node.h"
#include <algorithm>
#include "Renderer.h"
#include "Director.h"
#include "Scheduler.h"
#include "ActionManager.h"

NS_ENGIN_BEGIN
static int gArrivalOrder = 1;
//return true 排序后，参数1在前，参数2在后
bool nodeFirstDraw(Node* n0,Node* n1){
	return (n0->getZOrder() < n1->getZOrder()) ||
		(n0->getArrivalOrder() < n1->getArrivalOrder() && n0->getZOrder() == n1->getZOrder());
}

Node* Node::create(){
	Node* node = new Node();
	if (node->init()){
		node->autoRelease();
		return node;
	}
	else{
		delete node;
		return nullptr;
	}
}

Node::Node() :
_zOrder(0),
_pos(0, 0),
_arrivalOrder(0),
_contentSize(0,0),
_reorderChildDirty(false),
_scheduler(nullptr),
_scaleX(1),
_scaleY(1),
_transformDirty(true),
_anchorPoint(0,0),
_anchorPointInPoints(0,0){
	_scheduler = Director::getInstance()->getScheduler();
	_actionManager = Director::getInstance()->getActionManager();
	//初始化四元数
	setRotation(0.0f);
}

bool Node::init(){
	return true;
}

Node::~Node(){
	_scheduler->unScheduleUpdate(this);
	for (auto node : _children){
		node->release();
	}
}

void Node::visit(Renderer* renderer, Mat4& parentTransform){
	processParentFlags( parentTransform, 0 );
	int childNum = _children.size();
	if (childNum <= 0){
		draw(renderer, _modelViewTransform);
	}
	else{
		sortAllChildren();
		int i = 0;
		int childNum = _children.size();
		//使得zorder小于0的节点会在父节点的下面
		for (; i < childNum; i++){
			if (_children[i]->getZOrder() < 0){
				_children[i]->visit(renderer, _modelViewTransform);
			}
			else{
				break;
			}
		}
		this->draw(renderer, _modelViewTransform);
		for (; i < childNum; i++){
			_children[i]->visit(renderer, _modelViewTransform);
		}
	}
}

void Node::draw(Renderer* renderer, Mat4& transform){

}

void Node::addChild(Node* child){
	addChild(child, child->_zOrder);
}

void Node::addChild(Node* child, int zOrder){
	child->_parent = this;
	insertChild(child, zOrder);
	child->_arrivalOrder = gArrivalOrder++;
	_reorderChildDirty = true;
}

void Node::insertChild(Node* child, int zOrder){
	_children.push_back(child);
	child->retain();
	child->_zOrder = zOrder;
}

void Node::detachChild(Node* child, size_t index){
	child->_parent = nullptr;
	auto iter = std::next(_children.begin(), index);
	_children.erase(iter);
	child->release();
}

void Node::sortAllChildren(){
	if (_reorderChildDirty){
		std::sort(std::begin(_children), std::end(_children), nodeFirstDraw);
		_reorderChildDirty = false;
	}
}

void Node::setPosition(const Vec3f& pos){
	_pos = pos;
	_transformDirty = true;
}

void Node::removeChild(Node* child){
	auto iter=std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()){
		size_t index = iter - _children.begin();
		detachChild(child, index);
	}
}

void Node::removeFromParent(){
	if (_parent){
		_parent->removeChild(this);
	}
}

void Node::update(float dt){

}

void Node::scheduleUpdate(){
	_scheduler->scheduleUpdate(this);
}

void Node::unScheduleUpdate(){
	_scheduler->unScheduleUpdate(this);
}

void Node::setScale(float scale){
	_scaleX = _scaleY = scale;
	_transformDirty = true;
}

void Node::setScaleX(float scaleX){
	_scaleX = scaleX;
	_transformDirty = true;
}

void Node::setScaleY(float scaleY){
	_scaleY = scaleY;
	_transformDirty = true;
}

void Node::runAction(Action* action){
	_actionManager->addAction(action, this);
}

const Mat4& Node::getNodeToParentTransform(){
	if(!_transformDirty){
		return _transform;
	}
	_transformDirty=false;
	Mat4::createRotation(_rotateQuat,_transform);
	static Mat4 scaleMat;
	Mat4::createScale(_scaleX,_scaleY,1.0f,scaleMat);
	Mat4_Mul(_transform,scaleMat,_transform);
	static Vec3f anchorPoint;
	Mat4_Mul(_anchorPointInPoints,_transform,anchorPoint);
	static Vec3f newPos;
	Vec3f_Sub(_pos , anchorPoint, newPos);
	static Mat4 translate;
	Mat4::createTranslation((float)newPos.x,(float)newPos.y,(float)newPos.z,translate);
	Mat4_Mul(_transform,translate,_transform);
	return _transform;
}

void Node::setAnchorPoint( const Vec2f& point ){
	_anchorPoint = point;
	_anchorPointInPoints.x = _anchorPoint.x*_contentSize.width;
	_anchorPointInPoints.y = _anchorPoint.y*_contentSize.height;
	_transformDirty = true;
}

void Node::setContentSize( const Size& size){
	_contentSize = size;
	_anchorPointInPoints.x = _anchorPoint.x*_contentSize.width;
	_anchorPointInPoints.y = _anchorPoint.y*_contentSize.height;
	_transformDirty = true;
}

void Node::setRotation( float rotation ){
	_rotation = rotation;
	_rotateQuat.eulerToQuat(0,0,_rotation);
	_transformDirty = true;
}

uint Node::processParentFlags( const Mat4& parentTransform, uint parentFlags ){
	Mat4_Mul(parentTransform,getNodeToParentTransform(),_modelViewTransform);
	return 0;
}

NS_ENGIN_END


