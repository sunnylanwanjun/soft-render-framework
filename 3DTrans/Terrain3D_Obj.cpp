#include "Terrain3D_Obj.h"
#include "engin.h"
#include "Mat4.h"
USING_ENGIN_NS
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index
#define LIGHT_NUM             4
Terrain3D_Obj::Terrain3D_Obj(){

}

Terrain3D_Obj::~Terrain3D_Obj(){	
	
}

Terrain3D_Obj* Terrain3D_Obj::create(){
	Terrain3D_Obj* terrain = new Terrain3D_Obj;
	terrain->autoRelease();
	terrain->init();
	terrain->scheduleUpdate();
	return terrain;
}

bool Terrain3D_Obj::init(){
	Size size = Director::getInstance()->getWindowSize();
	Vec3f vList[4] = { Vec3f(0, 0), Vec3f(size.width, 0), 
		Vec3f(size.width, size.height/2), Vec3f(0, size.height/2)};
	//Polygon2D* sky = Polygon2D::create(Vec3f(0, size.height/2), 4, vList,RGB32BIT(0,0,140,192),true);
	//addChild(sky);
	//Polygon2D* terrain = Polygon2D::create(Vec3f(0, size.height/2), 4, vList,RGB32BIT(0,103,62,3),true);
	//addChild(terrain);

	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D);
	cam = Camera::create(CAM_EULER,
		Vec4f(0,40,0),
		200.0f,12000.0f,dPI4,size.width,size.height);
	scene3D->setCamera(cam);

	for(int i=0;i<NUM_TANKS;i++){
		ObjNode3D* obj_tank = ObjNode3D::createWithPlgfile("tank2.plg");
		obj_tank->setScale(Vec3f(0.75f,0.75f,0.75f));
		obj_tank->setLight(true);
		obj_tank->setPosition(Vec3f((float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS),
			0,(float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS)));
		scene3D->addChild(obj_tank);
	}

	obj_player = ObjNode3D::createWithPlgfile("tank3.plg");
	obj_player->setLight(true);
	obj_player->setScale(Vec3f(0.75f,0.75f,0.75f));
	scene3D->addChild(obj_player);

	for(int i=0;i<NUM_MARKERS;i++){
		ObjNode3D* obj_marker = ObjNode3D::createWithPlgfile("marker1.plg");
		obj_marker->setScale(Vec3f(1.0f,2.0f,1.0f));
		obj_marker->setLight(true);
		obj_marker->setPosition(Vec3f((float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS),
			0,(float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS)));
		scene3D->addChild(obj_marker);
	}

	for(int i=0;i<NUM_TOWERS;i++){
		ObjNode3D* obj_tower = ObjNode3D::createWithPlgfile("tower1.plg");
		obj_tower->setScale(Vec3f(1.0f,1.0f,1.0f));
		obj_tower->setLight(true);
		obj_tower->setPosition(Vec3f((float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS),
			0,(float)Rand_Range(-TERRAIN_RADIUS,TERRAIN_RADIUS)));
		scene3D->addChild(obj_tower);
	}

	RGBA gray;GRAY(gray);
	RGBA black;BLACK(black);
	RGBA green;GREEN(green);
	RGBA red;RED(red);
	lightMgr=LightMgr::getInstance();
	lightMgr->num_lights=LIGHT_NUM;
	lightMgr->initLight(
		AMBIENT_LIGHT_INDEX,
		LIGHT_STATE_ON,
		LIGHT_ATTR_AMBIENT,
		gray,black,black,0,0,0,0,0,0,0,0
		);

	Vec4f dlight_dir(-1,0,-1);

	// directional light
	lightMgr->initLight(INFINITE_LIGHT_INDEX,  
		LIGHT_STATE_ON,      // turn the light on
		LIGHT_ATTR_DIRECTIONAL, // infinite light type
		black, gray, black,    // color for diffuse term only
		NULL, &dlight_dir,     // need direction only
		0,0,0,                 // no need for attenuation
		0,0,0);                // spotlight info NA

	Vec4f plight_pos(0,200,0);

	// point light
	lightMgr->initLight(POINT_LIGHT_INDEX,
		LIGHT_STATE_ON,      // turn the light on
		LIGHT_ATTR_POINT,    // pointlight type
		black, green, black,   // color for diffuse term only
		&plight_pos, NULL,     // need pos only
		0,.001f,0,              // linear attenuation only
		0,0,1);                // spotlight info NA

	Vec4f slight_pos(0,200,0);
	Vec4f slight_dir(1,0,1);

	// spot light
	lightMgr->initLight(SPOT_LIGHT_INDEX,
		LIGHT_STATE_ON,         // turn the light on
		LIGHT_ATTR_SPOT2,  // spot light type 2
		black, red, black,      // color for diffuse term only
		&slight_pos, &slight_dir, // need pos only
		0,.001f,0,                 // linear attenuation only
		0,0,1);                   // spotlight powerfactor only
	return true;
}

void Terrain3D_Obj::update( float dt ){

	if(KEYDOWN(VK_NUMPAD1)){
		if(lightMgr->lights[AMBIENT_LIGHT_INDEX].state==LIGHT_STATE_ON){
			lightMgr->lights[AMBIENT_LIGHT_INDEX].state=LIGHT_STATE_OFF;
		}else{
			lightMgr->lights[AMBIENT_LIGHT_INDEX].state=LIGHT_STATE_ON;
		}
	}
	if(KEYDOWN(VK_NUMPAD2)){
		if(lightMgr->lights[INFINITE_LIGHT_INDEX].state==LIGHT_STATE_ON){
			lightMgr->lights[INFINITE_LIGHT_INDEX].state=LIGHT_STATE_OFF;
		}else{
			lightMgr->lights[INFINITE_LIGHT_INDEX].state=LIGHT_STATE_ON;
		}
	}
	if(KEYDOWN(VK_NUMPAD3)){
		if(lightMgr->lights[POINT_LIGHT_INDEX].state==LIGHT_STATE_ON){
			lightMgr->lights[POINT_LIGHT_INDEX].state=LIGHT_STATE_OFF;
		}else{
			lightMgr->lights[POINT_LIGHT_INDEX].state=LIGHT_STATE_ON;
		}
	}
	if(KEYDOWN(VK_NUMPAD4)){
		if(lightMgr->lights[SPOT_LIGHT_INDEX].state==LIGHT_STATE_ON){
			lightMgr->lights[SPOT_LIGHT_INDEX].state=LIGHT_STATE_OFF;
		}else{
			lightMgr->lights[SPOT_LIGHT_INDEX].state=LIGHT_STATE_ON;
		}
	}

	static float plight_ang = 0, slight_ang = 0; // angles for light motion

	// move point light source in ellipse around game world
	lightMgr->lights[POINT_LIGHT_INDEX].pos.x = 4000*cos(plight_ang);
	lightMgr->lights[POINT_LIGHT_INDEX].pos.y = 200;
	lightMgr->lights[POINT_LIGHT_INDEX].pos.z = 4000*sin(plight_ang);

	if ((plight_ang+=0.03f) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.x = 2000*cos(slight_ang);
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.y = 200;
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.z = 2000*sin(slight_ang);

	if ((slight_ang-=0.05f) < 0)
		slight_ang = 360;

	obj_player->setPosition(Vec3f(
		cam->pos.x-300*sin(cam->dir.y),
		cam->pos.y-70,
		cam->pos.z+300*cos(cam->dir.y)
		));
	obj_player->setRotation(cam->dir.v3f());
}