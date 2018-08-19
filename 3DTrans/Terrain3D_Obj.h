#ifndef __TERRAIN3D_OBJ_H
#define __TERRAIN3D_OBJ_H
#include "engin.h"
#define NUM_TOWERS        96
#define NUM_TANKS         24
#define NUM_MARKERS		  400
#define TERRAIN_RADIUS    4000
class Terrain3D_Obj:public engin::Node{
public:
	Terrain3D_Obj();
	virtual ~Terrain3D_Obj(); 
	static Terrain3D_Obj* create();
	bool init();
	void update(float dt);
private:
	engin::Camera       *cam;       // the single camera

	engin::ObjNode3D    *towers[NUM_TOWERS],    // used to hold the master tower
		*tanks[NUM_TANKS],     // used to hold the master tank
		*markers[NUM_MARKERS],   // the ground marker
		*obj_player;   // the player object   
	engin::LightMgr            *lightMgr;
};
#endif