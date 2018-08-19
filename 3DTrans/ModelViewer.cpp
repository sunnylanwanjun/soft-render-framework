#include "ModelViewer.h"
USING_ENGIN_NS
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index
#define LIGHT_NUM             4
ModelViewer::ModelViewer(){

}

ModelViewer::~ModelViewer(){	

}

ModelViewer* ModelViewer::create(){
	ModelViewer* viewer = new ModelViewer;
	viewer->autoRelease();
	viewer->init();
	viewer->scheduleUpdate();
	return viewer;
}

bool ModelViewer::init(){
	Size size = Director::getInstance()->getWindowSize();
	Vec3f vList[4] = { Vec3f(0, 0), Vec3f(size.width, 0), 
		Vec3f(size.width, size.height/2), Vec3f(0, size.height/2)};

	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D);
	cam = Camera::create(CAM_EULER,
		Vec4f(0,40,0),
		200.0f,12000.0f,dPI4,size.width,size.height);
	scene3D->setCamera(cam);

	ObjNode3D* obj0 = ObjNode3D::createWithCobfile("hammer03.cob");
	obj0->setPosition(Vec3f(0,0,300.0f));
	obj0->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj0->setScale(Vec3f(10.0f,10.0f,10.0f));
	obj0->setLight(true);
	scene3D->addChild(obj0);

	ObjNode3D* obj1 = ObjNode3D::createWithAscfile("hammer03.asc");
	obj1->setPosition(Vec3f(0,0,300.0f));
	obj1->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj1->setScale(Vec3f(10.0f,10.0f,10.0f));
	obj1->setLight(true);
	scene3D->addChild(obj1);

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

void ModelViewer::update( float dt ){

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
}