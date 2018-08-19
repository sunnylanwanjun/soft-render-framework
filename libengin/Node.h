#ifndef __NODE_H
#define __NODE_H
#include "macros.h"
#include "Ref.h"
#include "Vec.h"
#include <vector>
#include "Geometry.h"
#include "Mat4.h"
#include "Quaternion.h"
NS_ENGIN_BEGIN
class Renderer;
class Scheduler;
class ActionManager;
class Action;
class ENGIN_DLL Node :public Ref{
public:
	Node();
	virtual ~Node();
	static Node* create();
	virtual void visit(Renderer* renderer, Mat4& parentTransform);
	virtual void draw(Renderer* renderer, Mat4& transform);
	virtual void addChild(Node* child);
	virtual void addChild(Node* child, int zOrder);
	virtual void removeChild(Node* child);
	virtual void setPosition(const Vec3f& pos);
	virtual void setRotation(float rotation);
	virtual void setScale(float scale);
	virtual void setScaleX(float scaleX);
	virtual void setScaleY(float scaleY);

	void runAction(Action* action);
	void removeFromParent();
	int getZOrder(){ return _zOrder; }
	int getArrivalOrder(){ return _arrivalOrder; }
	Vec3f getPosition(){ return _pos; };
	Size getContentSize(){ return _contentSize; }
	void setContentSize( const Size& size);
	virtual void update(float dt);
	void scheduleUpdate();
	void unScheduleUpdate();
	void setAnchorPoint(const Vec2f& point);
protected:
	virtual bool init();
	const Mat4& Node::getNodeToParentTransform();
	virtual uint processParentFlags(const Mat4& parentTransform, uint parentFlags);
private:
	void insertChild(Node* child, int zOrder);
	void detachChild(Node* child, size_t index);
	void sortAllChildren();
////////////////////////////property//////////////////////////////
private:
	bool _reorderChildDirty;
protected:
	Vec3f _anchorPointInPoints;
	Vec2f _anchorPoint;  
	float _rotation;
	Vec3f _pos;
	int  _zOrder;
	int  _arrivalOrder;
	Size _contentSize;
	Node* _parent;
	std::vector<Node*> _children;
	Scheduler* _scheduler;
	float _scaleX;
	float _scaleY;
	ActionManager* _actionManager;
	Mat4   _transform;
	Mat4   _modelViewTransform;
	Quaternion _rotateQuat;
	bool _transformDirty;
};
NS_ENGIN_END
#endif