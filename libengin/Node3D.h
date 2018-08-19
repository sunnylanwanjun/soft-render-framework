#ifndef __NODE3D_H
#define __NODE3D_H
#include "macros.h"
#include "t3dlib5.h"
#include <vector>
#include "Ref.h"
#include "Mat4.h"
NS_ENGIN_BEGIN
class Vec3f;
class Node3D;
class Action;
class ActionManager;
class Renderer;
class LightMgr;
typedef std::vector<Node3D*> NODE3D_VECTOR;
extern bool sortFunction(Node3D*c0,Node3D*c1);
class ENGIN_DLL Node3D:public Ref{
	friend bool sortFunction(Node3D*c0,Node3D*c1);
public:
	Node3D();
	virtual ~Node3D();
	virtual void setScale(Vec3f& vs);
	virtual void setRotation(Vec3f& vr);
	virtual void setPosition(Vec3f& vt);
	virtual void visit(Renderer* renderer, Camera* cam, const Mat4& parentTransform);
	virtual void draw(Renderer* renderer, Camera* cam);
	virtual void updateLocalToWorldTrans(const Mat4& transMat){}
	virtual void updateModelViewTransMat();
	virtual void setLight(LightMgr* lightMgr,Camera* cam,int maxLight){}
	virtual void addChild(Node3D* child);
	virtual void removeChild(Node3D* child);
	virtual void removeFromParent();
	virtual void setLight(bool lightOn){_lightOn=lightOn;}
	virtual void setSort(bool sortOn){_sortOn=sortOn;}

	void runAction(Action* action);
	void setVisible(bool visible){_visible=visible;}
	const Mat4& getModelViewMat(){return _modelViewTransMat;}
	bool isVisible(){return _visible;}
	NODE3D_VECTOR& getChildren(){return _children;}
	void insertChild(Node3D* child);
	void detachChild(Node3D* child, size_t index);
	int getZOrder(){ return _zOrder; }
	void setZOrder(int zOrder){ _zOrder=zOrder; }
protected:
	bool _lightOn;
	bool _sortOn;
	bool _visible;
	bool _wireMode;    //是否采用线框模式
	Vec3f _scale;
	Vec3f _pos;        //在世界坐标中的位置
	Vec3f _rotation;
	bool _modelViewMatDirty;
	Mat4 _modelViewTransMat;  //根据模型，进行平移，旋转，缩放的矩阵
	Node3D* _parent;
	int  _zOrder;
	NODE3D_VECTOR _children;
	ActionManager* _actionManager;
};
NS_ENGIN_END
#endif