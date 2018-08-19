#ifndef __RAIDERS_H
#define __RAIDERS_H
#include "engin.h"
#include "Aliens.h"
class Raiders:public engin::Node{
public:
	Raiders();
	virtual ~Raiders();
	static Raiders* create();
	bool init();
	void update(float dt);
	void draw(engin::Renderer* renderer, engin::Mat4& transform);
private:
	Aliens*                     aliens;
	engin::Camera				*cam;       
	engin::MaterialRenderList   *rend_list; 
	engin::LightMgr             *lightMgr;
	engin::Sprite				*weapon;
	engin::Texture              *planeTex;
	engin::Texture				*planeFlashTex;
	engin::Sprite				*plane;
};
#endif