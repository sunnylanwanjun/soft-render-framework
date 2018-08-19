#ifndef __SCENE3D_H
#define __SCENE3D_H
#include "macros.h"
#include "Node3D.h"
#include "Camera.h"
NS_ENGIN_BEGIN
class Camera;
class Renderer;
class ENGIN_DLL Scene3D:public Node3D{
public:
	Scene3D();
	virtual ~Scene3D();
	static Scene3D* create();
	void drawScene(Renderer* renderer);
	void setCamera(Camera* cam);
private:
	Camera* _cam;
};
NS_ENGIN_END
#endif