#ifndef __MODELVIEWER_H
#define __MODELVIEWER_H
#include "engin.h"
class ModelViewer:public engin::Node{
public:
	ModelViewer();
	virtual ~ModelViewer();
	static ModelViewer* create();
	bool init();
	void update(float dt);
private:
	engin::Camera       *cam;       // the single camera
	engin::LightMgr     *lightMgr;
};
#endif