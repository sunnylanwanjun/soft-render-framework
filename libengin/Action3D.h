#ifndef __ACTION3D_H
#define __ACTION3D_H
#include "macros.h"
#include "Action.h"
#include "Mat4.h"
NS_ENGIN_BEGIN
class ENGIN_DLL RepeatRotate3DBy :public Action{
public:
	RepeatRotate3DBy();
	virtual ~RepeatRotate3DBy();
	static RepeatRotate3DBy* create(const Vec3f& rotateV);
	virtual void update(float dt);
protected:
	Mat4 _rm;
	Vec3f _rotate;
	Vec3f _totalRotate;
};

class ENGIN_DLL RepeatCameraMove :public Action{
public:
	RepeatCameraMove();
	virtual ~RepeatCameraMove();
	static RepeatCameraMove* create(float dis,float angleSpeed);
	virtual void update(float dt);
protected:
	float _dis;
	float _angle;
	float _angleSpeed;
	Vec3f _pos;
};

NS_ENGIN_END
#endif