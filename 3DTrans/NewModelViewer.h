#ifndef __NEWMODELVIEWER_H
#define __NEWMODELVIEWER_H
#include "engin.h"
class NewModelViewer:public engin::Node{
public:
	NewModelViewer();
	virtual ~NewModelViewer();
	static NewModelViewer* create();
	bool init();
	void update(float dt);
private:
	engin::Camera       *cam;       // the single camera
	engin::LightMgr     *lightMgr;
	engin::MaterialNode3D *obj0;
	engin::MaterialNode3D *obj1;
	engin::MaterialNode3D *obj2;
	engin::MaterialRenderList *rl;
};
#endif