#include "NewModelViewer.h"
USING_ENGIN_NS
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index
#define LIGHT_NUM             4
NewModelViewer::NewModelViewer(){

}

NewModelViewer::~NewModelViewer(){	
	if(obj0){
		obj0->release();
		obj0=nullptr;
	}
}

NewModelViewer* NewModelViewer::create(){
	NewModelViewer* viewer = new NewModelViewer;
	viewer->autoRelease();
	viewer->init();
	viewer->scheduleUpdate();
	return viewer;
}

bool NewModelViewer::init(){
	Size size = Director::getInstance()->getWindowSize();
	Vec3f vList[4] = { Vec3f(0, 0), Vec3f(size.width, 0), 
		Vec3f(size.width, size.height/2), Vec3f(0, size.height/2)};

	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D);
	cam = Camera::create(CAM_EULER,
		Vec4f(180,40,0),
		200.0f,12000.0f,dPI4,size.width,size.height);
	scene3D->setCamera(cam);
	
	obj0 = MaterialNode3D::createWithCobfile("tie04.cob");
	obj0->retain();
	obj0->setPosition(Vec3f(0,0,400));
	obj0->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj0->setScale(Vec3f(5.0f,5.00f,5.00f));
	obj0->setLight(false);
	//scene3D->addChild(obj0);
	
	rl=MaterialRenderList::create();
	rl->setLight(true);
	scene3D->addChild(rl);

	/*obj0 = MaterialNode3D::createWithCobfile("water_constant_01.cob");
	obj0->setPosition(Vec3f(-170,0,400));
	//obj0->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj0->setScale(Vec3f(6.0f,6.0f,6.0f));
	obj0->retain();
	//rl->insertObj(*obj0);
	//obj0->setLight(true);
	//scene3D->addChild(obj0);
	
	obj1 = MaterialNode3D::createWithCobfile("water_flat_01.cob");
	obj1->setPosition(Vec3f(0,0,400));
	//obj1->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj1->setScale(Vec3f(6.0f,6.0f,6.0f));
	obj1->retain();
	//obj1->setLight(true);
	//rl->insertObj(*obj1);
	//scene3D->addChild(obj1);
	
	obj2 = MaterialNode3D::createWithCobfile("water_gouraud_01.cob");
	obj2->setPosition(Vec3f(170,0,400));
	//obj2->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	obj2->setScale(Vec3f(6.0f,6.0f,6.0f));
	obj2->retain();
	//rl->insertObj(*obj2);
	//obj2->setLight(true);
	//scene3D->addChild(obj2);
	*/

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

	Vec4f dlight_dir(1,0,1);

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

void NewModelViewer::update( float dt ){

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
	lightMgr->lights[POINT_LIGHT_INDEX].pos.x = 1000*cos(plight_ang);
	lightMgr->lights[POINT_LIGHT_INDEX].pos.y = 100;
	lightMgr->lights[POINT_LIGHT_INDEX].pos.z = 1000*sin(plight_ang);

	if ((plight_ang+=0.03f) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.x = 1000*cos(slight_ang);
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.y = 200;
	lightMgr->lights[SPOT_LIGHT_INDEX].pos.z = 1000*sin(slight_ang);

	if ((slight_ang-=0.05f) < 0)
		slight_ang = 360;
	
	static Vec3f rotate;//=Vec3f(351.069855f,0.755997360f,0.0f);
	//if(KEYDOWN('T')){
		if ((rotate.x+=0.01f) > 360) rotate.x = 0;
		if ((rotate.y+=0.02f) > 360) rotate.y = 0;
		if ((rotate.z+=0.03f) > 360) rotate.z = 0;
	//}else if(KEYDOWN('G')){
	//	if ((rotate.x-=0.01f) < 0) rotate.x = 360;
	//	if ((rotate.y-=0.02f) < 0) rotate.y = 360;
	//	if ((rotate.z-=0.03f) < 0) rotate.z = 360;
	//}
	rl->reset();

	//obj0->setRotation(rotate);
	obj0->updateModelViewTransMat();
	obj0->updateLocalToWorldTrans(obj0->getModelViewMat());
	obj0->cullObj(cam);
	rl->insertObj(*obj0);
	obj0->resetState();

	/*obj1->setRotation(rotate);
	obj1->updateModelViewTransMat();
	obj1->updateLocalToWorldTrans(obj1->getModelViewMat());
	obj1->cullObj(cam);
	rl->insertObj(*obj1);
	obj1->resetState();

	obj2->setRotation(rotate);
	obj2->updateModelViewTransMat();
	obj2->updateLocalToWorldTrans(obj2->getModelViewMat());
	obj2->cullObj(cam);
	rl->insertObj(*obj2);
	obj2->resetState();*/
}