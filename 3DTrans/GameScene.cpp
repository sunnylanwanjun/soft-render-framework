#include "GameScene.h"
#include "Terrain3D.h"
#include "ChangeColor.h"
#include "Terrain3D_Obj.h"
#include "Terrain3D_Material.h"
#include "ModelViewer.h"
#include "NewModelViewer.h"
#include "raiders.h"
USING_ENGIN_NS
const Size& wndSize = Director::getInstance()->getWindowSize();
Vec3f GetRandomPos(){
	static Vec3f pos;
	pos.x = (float)(rand() % (int)wndSize.width);
	pos.y = (float)(rand() % (int)wndSize.height);
	return pos;
}

GameScene::GameScene(){
	
}

GameScene::~GameScene(){

}

GameScene* GameScene::create(){
	GameScene* scene = new GameScene;
	if (scene->init()){
		scene->autoRelease();
		return scene;
	}
	else{
		delete scene;
		return nullptr;
	}
}

bool GameScene::init(){
	return createRaiders();
}

bool GameScene::createDrawFunc(){
	float scaleX=60.0f;
	float scaleY=60.0f;
	auto coor = Coordinate2D::create();
	coor->setScaleX(scaleX);
	coor->setScaleY(scaleY);
	coor->init();
	addChild(coor);
	auto d0=DrawFunc::create([](float x,float& y)->bool{
		/*if(ABS(x)<=E5)
			return false;
		y=1/(x);*/
		y=pow((cos(Deg2Rad(x))-cos(Deg2Rad(20.0f)))/(cos(Deg2Rad(15.0f))-cos(Deg2Rad(20.0f))),0.2f);
		//y=sin(x);
		return true;
	});
	d0->setMinX(15);
	d0->setMaxX(90);
	d0->setScaleX(scaleX);
	d0->setScaleY(scaleY);
	d0->color=0x0000ffff;//黄
	d0->init();
	auto d1=DrawFunc::create([](float x,float& y)->bool{
		/*if(ABS(x)<=E5)
			return false;
		y=1/(x*x);*/
		y=pow((cos(Deg2Rad(x))-cos(Deg2Rad(20.0f)))/(cos(Deg2Rad(15.0f))-cos(Deg2Rad(20.0f))),1.0f);
		return true;
	});
	d1->setMinX(15);
	d1->setMaxX(20);
	d1->color=0x000000ff;//蓝
	d1->setScaleX(scaleX);
	d1->setScaleY(scaleY);
	d1->init();
	auto d2=DrawFunc::create([](float x,float& y)->bool{
		/*if(ABS(x)<=E5)
			return false;
		y=1/(x*x+x+1);*/
		y=pow((cos(Deg2Rad(x))-cos(Deg2Rad(20.0f)))/(cos(Deg2Rad(15.0f))-cos(Deg2Rad(20.0f))),5.0f);
		return true;
	});
	d2->setMinX(15);
	d2->setMaxX(20);
	d2->color=0x0000ff00;//绿
	d2->setScaleX(scaleX);
	d2->setScaleY(scaleY);
	d2->init();
	addChild(d0);
	addChild(d1);
	addChild(d2);
	return true;
}

bool GameScene::createChangeColor(){
	ChangeColor* changeColor=ChangeColor::create();
	addChild(changeColor);
	return true;
}

bool GameScene::createTriangle3D(){
	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D);
	Size size = Director::getInstance()->getWindowSize();
	Camera* cam = Camera::create(CAM_EULER,
		Vec4f(0,0,0),
		15,1000,dPI2,size.width,size.height);
	scene3D->setCamera(cam);
	Polygon3D_Vec poly0;
	poly0.attr=POLY_ATTR_SIDE_2;
	poly0.color=0x0000ff00;
	poly0.state=POLY_STATE_ACTIVE;
	poly0.lVecList[0]=Vec4f(0.0f,5.0f,0.0f);
	poly0.lVecList[1]=Vec4f(0.0f,0.0f,0.0f);
	poly0.lVecList[2]=Vec4f(50.0f,0.0f,0.0f);
	//poly0.lVecList[0]=Vec4f(10.0f,0.0f,0.0f);
	//poly0.lVecList[1]=Vec4f(0.0f,0.0f,10.0f);
	//poly0.lVecList[2]=Vec4f(-10.0f,0.0f,0.0f);
	Polygon3D_Vec poly1;
	poly1.attr=POLY_ATTR_SIDE_2;
	poly1.color=0x0000ff00;
	poly1.state=POLY_STATE_ACTIVE;
	poly1.lVecList[0]=Vec4f(0.0f,5.0f,0.0f);
	poly1.lVecList[1]=Vec4f(50.0f,5.0f,0.0f);
	poly1.lVecList[2]=Vec4f(50.0f,0.0f,0.0f);
	RenderList* rl = RenderList::create();
	//rl->runAction(RepeatRotate3DBy::create(Vec3f(0.0f,0.01f,0.0f)));
	rl->insertPoly(poly0);
	//rl->setRotation(Vec3f(0.0f,0.499999821f,0.0f));
	rl->insertPoly(poly1);
	rl->setPosition(Vec3f(0.0f,-30.0f,100.0f));
	scene3D->addChild(rl);

	EventListenerKeyboard* ek = EventListenerKeyboard::create();
	ek->onKeyPress = [rl](EventKeyboard::KeyCode keyCode,EventKeyboard* e){
		static Vec3f rotation;
		if(keyCode==EventKeyboard::KEY_T){
			rotation.y+=0.01f;
		}else if(keyCode==EventKeyboard::KEY_G){
			rotation.y-=0.01f;
		}
		rl->setRotation(rotation);
	};
	GDISPATCH->addEventListener(ek);
	return true;
}

bool GameScene::createCube2Side(){
	Layer3D* layer3D = Layer3D::create();
	Scene3D* scene3D = Scene3D::create();
	layer3D->setScene(scene3D);
	addChild(layer3D);
	Size size = Director::getInstance()->getWindowSize();
	Camera* cam = Camera::create(CAM_EULER,
		Vec4f(0,0,0),//Vec4f(0,-500,500),
		50.0f,12000.0f,dPI2,size.width,size.height);
	//cam->runAction(RepeatCameraMove::create(900,0.05f));
	scene3D->setCamera(cam);
	//for(int i=0;i<16;i++){
	//	for(int j=0;j<16;j++){
	//		ObjNode3D* obj = ObjNode3D::createWithPlgfile("tank1.plg");
	//		obj->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	//		obj->setPosition(Vec3f(j*250.0f-1500.0f,0.0f,i*250.0f+250/2));
	//		scene3D->addChild(obj);
	//	}
	//}

	//ObjNode3D* obj = ObjNode3D::createWithPlgfile("tower1.plg");
	ObjNode3D* obj = ObjNode3D::createWithCobfile("sphere01.cob");
	obj->setScale(Vec3f(20.0f,20.0f,20.0f));
	obj->runAction(RepeatRotate3DBy::create(Vec3f(0.00f,0.01f,0.0f)));
	//obj->setRotation(Vec3f(0.00f,30.0f,0.0f));
	obj->setPosition(Vec3f(0,0,400));
	scene3D->addChild(obj);

	return true;
}

bool GameScene::createModelView(){
	addChild(NewModelViewer::create());
	return true;
}

bool GameScene::createTerrain3D(){
	addChild(Terrain3D_Material::create());
	return true;
}

bool GameScene::createRaiders(){
	addChild(Raiders::create());
	return true;
}

bool GameScene::createMoveSprite(){

	static DWORD happy_bitmap[64] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00000000, 0x00000000,
		0x00000000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00000000,
		0x00000000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00000000,
		0x00000000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00ff0000, 0x00000000, 0x00ff0000, 0x00000000,
		0x00000000, 0x00ff0000, 0x00ff0000, 0x00000000, 0x00000000, 0x00ff0000, 0x00ff0000, 0x00000000,
		0x00000000, 0x00000000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	Texture* texture = Texture::create("test.bmp");
	if (texture == nullptr){
		return false;
	}
	//Image* image = Image::createWithRGBData((BYTE*)happy_bitmap,8,8);
	//image->retain();
	//Texture* texture = Texture::createWithImage(image);
	Size size = Director::getInstance()->getWindowSize();
	Range range(0, 0, size.width, size.height);
	texture->retain();
	for (int i = 0; i < 100; i++){
		auto sp = Sprite::createWithTexture(texture);
		if (sp){
			//sp->setScale(0.5);
			sp->setPosition(GetRandomPos());
			sp->runAction(RandomMoveBy::create(range,300+rand()%500));
			addChild(sp);
		}
	}
	texture->release();
	//image->release();

	return true;
}

bool GameScene::createMoveMC(){
	Size size = Director::getInstance()->getWindowSize();
	Range range(0, 0, size.width, size.height);
	for (int i = 0; i < 10; i++){
		MovieClip* mc = MovieClip::createWithFile("frame", ".png", 1, 27);
		if (mc == nullptr){
			return false;
		}
		mc->setInterval(0.05f);
		mc->setPosition(GetRandomPos());
		mc->runAction(RandomMoveBy::create(range, 300 + rand() % 500));
		addChild(mc);
	}

	/*Label* label = Label::create("hello world hello world hello \n world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world hello world ");
	if (label == nullptr){
	return false;
	}
	label->setPosition(Vec2(100, 100));
	addChild(label);*/

	return true;
}

bool GameScene::createLine(){
	Size size = Director::getInstance()->getWindowSize();
	Range range(0, 0, size.width, size.height);
	//Line* line0 = Line::create(Vec3f(100.0f, 100.0f), Vec3f(0.0f, 0.0f), Vec3f(50.0f, 50.0f), RGB32BIT(0, 255, 0, 0));
	//Line* line1 = Line::create(Vec3f(200.0f, 200.0f), Vec3f(50.0f, 0.0f), Vec3f(0.0f, 50.0f), RGB32BIT(0, 255, 0, 0));
	
	//line->setContentSize(Size(50,50));
	//line->runAction(RandomMoveBy::create(range, 300 + rand() % 500));
	//addChild(line0);
	//addChild(line1);

	//EventListener* listener = nullptr;
	//auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	//使用lamda表达式 对于局部变量，要注意，所有的函数的局部变量都会在函数结束时被回收
	//所以使用引用传递来保存局部变量，是不安全的，很有可能是CCCCC的值，还有，程序是按从上到
	//下的顺序编译的，所以如果在遇到lamda表达式时，要使用的参数还为空，要到后面才赋值，
	//那么lamda表达式所使用的也是空值
	/*auto drawCallback = [&,line0,line1,listener](Event* e){
		LineCrossType lct=LineCross(*line0,*line1,nullptr,nullptr,&p);
		Log("%d,%d",p.x,p.y);
		eventDispatcher->removeEventListener(listener);
	};
	listener = EventListener::create(drawCallback,Director::EVENT_AFTER_DRAW);*/
	//listener = EventListener::create(nullptr,Director::EVENT_AFTER_DRAW);
	//listener->onEvent = CALL_BACK_1(GameScene::onEventCallback,this,line0,line1,listener);
	//eventDispatcher->addEventListener(listener);

	//addChild(Line::create(Vec3f(10, 10), Vec3f(100, 400), RGB32BIT(0, 0, 255, 0)));
	//addChild(Line::create(Vec3f(10, 10), Vec3f(400, 50), RGB32BIT(0, 0, 255, 0)));
	//addChild(Line::create(Vec3f(10, 10), Vec3f(400, 10), RGB32BIT(0, 0, 0, 255)));
	//addChild(Line::create(Vec3f(10, 400), Vec3f(400, 10), RGB32BIT(0, 255, 255, 255)));
	int x0 = 800/2;
	int y0 = 600/2;
	int r = 100;
	int i=0;
	//for (float angle = 0.0f; angle < 2 * 3.14; angle += 0.1f,i++){
	float angle=0.1;
		float y1 = r*sin(angle);
		float x1 = r*cos(angle);
		addChild(Line::create(Vec3f(0,0),Vec3f(0, 0), Vec3f(x1, y1), RGB32BIT(0, 255, 255, 255)));
		addChild(Line::create(Vec3f(x1,y1),Vec3f(0, 0), Vec3f(x1, y1), RGB32BIT(0, 255, 255, 255)));
	//}
	return true;
}

void GameScene::onEventCallback(Event* event,Line* line0,Line* line1,EventListener* listener){
	Vec3f p;
	LineCrossType lct=LineCross2D(line0->gp0,line0->v,line1->gp0,line0->v,nullptr,nullptr,&p);
	Log("%f,%f,%d",p.x,p.y,lct);
	Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}

bool GameScene::createPolygon(){
	Size size = Director::getInstance()->getWindowSize();
	Range range(0, 0, size.width, size.height);
	Vec3f vList[6] = { Vec3f(25, 25), Vec3f(0, 50), Vec3f(-25, 25), Vec3f(-25, -25), Vec3f(0, -50), Vec3f(25, -25) };
	//Vec3i vList[3] = {Vec3i(0, 0),Vec3i(0, 25),Vec3i(-25, 25)};
	//Vec3i vList[6] = { Vec3i(50, 0), Vec3i(25, 25), Vec3i(-25, 25), Vec3i(-50, 0), Vec3i(-25, -25), Vec3i(25, -25) };
	//Vec3i vList[6] = { Vec3i(400, 400), Vec3i(0, 800), Vec3i(-400, 400), Vec3i(-400, -400), Vec3i(0, -800), Vec3i(400, -400) };
	for (int i = 0; i < 16; i++){
		//Polygon2D* polygon = Polygon2D::create(Vec3i(rand() % size.width, rand() % size.height), 6, vList,RGB32BIT(0,0,255,0),true);
		Polygon2D* polygon = Polygon2D::create(Vec3f(100, 100), 6, vList,RGB32BIT(0,0,255,0),true);
		//polygon->setRotation(3.14f/2);
		//polygon->setContentSize(Size(50,100));
		//polygon->setAnchorPoint(Vec2f(0,0));
		//polygon->runAction(RandomMoveBy::create(range, 300 + rand() % 500));
		polygon->runAction(RepeatRotateBy::create(0.5f));
		//polygon->runAction(RepeatScaleBy::create(0.4f,2.0f,0.5f));
		addChild(polygon);
	}
	return true;
}

bool GameScene::createTriangle(){
	Size size = Director::getInstance()->getWindowSize();
	Range range(0, 0, size.width, size.height);
	Vec3f vList[][3] = 
	{
		{Vec3f(0, 0),Vec3f(100, 0),Vec3f(100, 100)},
		//{Vec3i(0, 0),Vec3i(0, 25),Vec3i(-25, 25)},
		//{Vec3i(0, 0),Vec3i(-25, 25),Vec3i(-25, 0)},
	};
	int numV = sizeof(vList)/(3*sizeof(Vec3f));
	//for (int i = 0; i < numV; i++){
		Triangle* triangle = Triangle::create(Vec3f(300,300), vList[0][0], vList[0][1], vList[0][2], RGB32BIT(0,0,255,0));
		//Triangle* triangle = Triangle::create(Vec3i(100, 100), vList[i][0], vList[i][1], vList[i][2], RGB32BIT(0,0,255,0));
		//triangle->setRotation(dPI2-0.01);
		//triangle->setContentSize(Size(50,50));
		//triangle->setAnchorPoint(Vec2f(0,0));
		//polygon->runAction(RandomMoveBy::create(range, 300 + rand() % 500));
		//triangle->runAction(RepeatRotateBy::create(0.14f));
		//polygon->runAction(RepeatScaleBy::create(0.4f,1.5,0.5));
		addChild(triangle);
	//}

	EventListenerKeyboard* ek = EventListenerKeyboard::create();
	ek->onKeyPress = [triangle](EventKeyboard::KeyCode keyCode,EventKeyboard* e){
		static float rotation=0;
		if(keyCode==EventKeyboard::KEY_T){
			rotation+=0.01f;
		}else if(keyCode==EventKeyboard::KEY_G){
			rotation-=0.01f;
		}
		triangle->setRotation(rotation);
	};
	GDISPATCH->addEventListener(ek);
	return true;
}