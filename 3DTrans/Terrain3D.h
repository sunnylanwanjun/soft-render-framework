#ifndef __TERRAIN3D_H
#define __TERRAIN3D_H
#include "engin.h"
#define NUM_TOWERS        96
#define NUM_TANKS         24
#define NUM_MARKERS		  400
#define TERRAIN_RADIUS    4000
class Terrain3D:public engin::Node{
public:
	Terrain3D();
	virtual ~Terrain3D();
	static Terrain3D* create();
	bool init();
	void update(float dt);
private:
	engin::Camera       *cam;       // the single camera

	engin::ObjNode3D    *obj_tower,    // used to hold the master tower
						*obj_tank,     // used to hold the master tank
						*obj_marker,   // the ground marker
						*obj_player;   // the player object             

	engin::Vec3f        towers[NUM_TOWERS],
						tanks[NUM_TANKS],
						markers[NUM_MARKERS];
	engin::RenderList   *rend_list; // the render list
	engin::LightMgr            *lightMgr;
};
#endif