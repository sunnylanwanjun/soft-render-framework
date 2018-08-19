#include "Application.h"
#include "Director.h"
#include "WindowView.h"
#include <windows.h>
#include "InitEngin.h"
NS_ENGIN_BEGIN
Application::Application(){

}

Application::~Application(){

}

int Application::run(){
	if (!applicationDidFinishLaunching()){
		return 0;
	}

	//初始化引擎
	initLog2Table();

	auto director = Director::getInstance();
	auto wndView = director->getWindowView();
	if (wndView == nullptr){
		return 0; 
	}
	wndView->retain();
	auto gameMode = director->getGameMode();
	while (!wndView->windowShouldClose()){
		//在窗口模式下，由于blt速度差的原因，只能跑到30到40帧，太快，会导致directdraw的blt速度跟不上cpu的速度，出现画面残影的现象
		if (gameMode == MODE_WND){
			Sleep(15);
		}
		wndView->handleEvents();
		director->mainLoop();
	}
	wndView->release();
	return 0;
}
NS_ENGIN_END

