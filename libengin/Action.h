#ifndef __ACTION_H
#define __ACTION_H
#include "macros.h"
#include "Ref.h"
#include <vector>
#include "Geometry.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Action :public Ref{
public:
	Action();
	virtual ~Action();
	void start(Ref* target);
	void stop();
	virtual bool isDone(){ return false; }
	virtual void update(float dt)=0;
protected:
	Ref* _target;
};

class ENGIN_DLL ActionInterval :public Action{
public:
	ActionInterval();
	virtual ~ActionInterval();
	void setDuration(float dur){ _dur = dur; }
protected:
	float _dur;
};

class ENGIN_DLL RandomMoveBy :public Action{
public:

	enum MOVEDIR{
		DIR_LEFT = -1,
		DIR_RIGHT = 1,
		DIR_UP = -1,
		DIR_DOWN = 1
	};

	RandomMoveBy();
	virtual ~RandomMoveBy();
	static RandomMoveBy* create(Range& range, int speed);
	virtual void update(float dt);
	void setRange(Range& range){ _range = range; }
	void setSpeed(int speed){ _speed = speed; }
protected:
	Range _range;
	MOVEDIR _xDir;
	MOVEDIR _yDir;
	int _speed;
};

class ENGIN_DLL RepeatRotateBy :public Action{
public:
	RepeatRotateBy();
	virtual ~RepeatRotateBy();
	static RepeatRotateBy* create(float angle);
	virtual void update(float dt);
protected:
	float _angleSpeed;
	float _totalAngle;
};

class ENGIN_DLL RepeatScaleBy :public Action{
public:
	RepeatScaleBy();
	virtual ~RepeatScaleBy();
	static RepeatScaleBy* create(float scale,float maxScale,float minScaleo);
	virtual void update(float dt);
protected:
	float _scaleSpeed;
	float _totalScale;
	float _maxScale;
	float _minScale;
	int   _scaleDir;
};

NS_ENGIN_END
#endif
