#include "AppDelegate.h"
#include "GameScene.h"
AppDelegate::AppDelegate(){

}

AppDelegate::~AppDelegate(){

}

bool AppDelegate::applicationDidFinishLaunching(){
	ClearLog();
	Director* director = Director::getInstance();
	director->setGameMode(MODE_WND);
	//director->setAnimationInterval(1.0f/30.f);
	WindowView* wndView=director->getWindowView();
	if (!wndView){
		wndView = WindowView::create("T3DLIB", Rect(100,100,800,600));
		director->setWindowView(wndView);
	}
	if (!wndView){
		return false;
	}
	auto gameScene = GameScene::create();
	if (gameScene == nullptr){
		return false;
	}
	director->runWithScene(gameScene);
	return true;
}
