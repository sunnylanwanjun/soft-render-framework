#include "raiders.h"
#include "raidersDef.h"
#include "Stars.h"
USING_ENGIN_NS
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index
#define LIGHT_NUM             4
float halfWidth;
float halfHeight;
Raiders::Raiders(){
	const Size& size=Director::getInstance()->getWindowSize();
	halfWidth=size.width*0.5f;
	halfHeight=size.height*0.5f;
}

Raiders::~Raiders(){
	delete aliens;
	planeTex->release();
	planeFlashTex->release();
}

Raiders* Raiders::create(){
	Raiders* raiders = new Raiders;
	raiders->autoRelease();
	raiders->init();
	raiders->scheduleUpdate();
	return raiders;
}

bool Raiders::init(){
	///////////////////////////////////////////////////////
	//初始化灯光
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
	const Size& size = Director::getInstance()->getWindowSize();
	////////////////////////////////////////////////////////
	//背景
	Sprite* bg = Sprite::create("nebblue01.bmp");
	addChild(bg,-3);
	////////////////////////////////////////////////////////
	//星星
	Stars* stars=new Stars;
	addChild(stars,-2);
	stars->release();//将引用降至1
	////////////////////////////////////////////////////////
	//外星人
	aliens=new Aliens;
	////////////////////////////////////////////////////////
	//初始化相机
	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D,-1);
	cam = Camera::create(CAM_EULER,
		Vec4f(0,0,0),
		NEAR_Z,FAR_Z,dPI4,size.width,size.height);
	scene3D->setCamera(cam);
	////////////////////////////////////////////////////////
	//渲染列表
	rend_list = MaterialRenderList::create();
	rend_list->setLight(false);
	rend_list->setSort(false);
	scene3D->addChild(rend_list);
	////////////////////////////////////////////////////////
	//武器
	Image* weaponImg = Image::create("crosshair01.bmp");
	Texture* weaponTex = Texture::createWithImage(weaponImg);
	weapon = Sprite::createWithTexture(weaponTex);
	addChild(weapon);
	//ShowCursor(false);//隐藏鼠标
	////////////////////////////////////////////////////////
	//飞行器
	planeTex = Texture::create("cockpit03.bmp");
	planeTex->retain();
	planeFlashTex = Texture::create("cockpit03b.bmp");
	planeFlashTex->retain();
	plane = Sprite::createWithTexture(planeTex);
	const Size& wndSize=Director::getInstance()->getWindowSize();
	Vec3f pos=Vec3f(wndSize.width*0.5f-400,wndSize.height*0.5f-300,0.0f);
	plane->setPosition(pos);
	bg->setPosition(pos);
	addChild(plane);
	return true;
}

Vec2f energy_bindings[6] = { Vec2f(342.0f, 527.0f), Vec2f(459.0f, 527.0f), 
	Vec2f(343.0f, 534.0f), Vec2f(458, 534.0f), 
	Vec2f(343.0f, 540.0f), Vec2f(458, 540.0f) };
	// these hold the positions of the weapon burst which use lighting too
	// the starting points are known, but the end points are computed on the fly
	// based on the cross hair
Vec2f weapon_bursts[3] = { Vec2f(78.0f, 500.0f), Vec2f(0.0f,0.0f),     // left energy weapon
	Vec2f(720.0f, 500.0f)}; 

void Raiders::update( float dt ){
	const Vec2f& wndPos=Director::getInstance()->getWindowOrigin();
	static POINT cursorPos;
	static Vec3f cursorVec;
	static Vec3f cursorCamVec;
	GetCursorPos(&cursorPos);
	
	if(IsLButtonDown()){
		plane->setTexture(planeFlashTex);
	}else{
		plane->setTexture(planeTex);
	}

	rend_list->reset();
	cursorVec.x=cursorPos.x-wndPos.x;
	cursorVec.y=cursorPos.y-wndPos.y;
	weapon_bursts[1]=cursorVec.v2f();
	cursorVec.x-=28;
	cursorVec.y-=28;
	weapon->setPosition(cursorVec);
	cursorCamVec.x=cursorPos.x-wndPos.x;
	cursorCamVec.y=cursorPos.y-wndPos.y;
	aliens->update(rend_list,cam,cursorCamVec.x,cursorCamVec.y);
}

void Raiders::draw(Renderer* renderer, Mat4& transform){

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}
	const RangeFIXP16& wndRange = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin = Director::getInstance()->getFIXP16WindowOrigin();

	if(IsLButtonDown()){
		DrawLightingLine((DWORD*)ddsd.lpSurface, 
			ddsd.lPitch >> 2, 
			wndOrigin, 
			wndRange, 
			0x0000ff00,
			weapon_bursts,
			3,
			5);
	}
	DrawLightingLine((DWORD*)ddsd.lpSurface, 
		ddsd.lPitch >> 2, 
		wndOrigin, 
		wndRange, 
		0x0000ff00,
		energy_bindings,
		6,
		3);
	lpdds->Unlock(nullptr);
}