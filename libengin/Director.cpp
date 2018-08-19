#include "Director.h"
#include "Scene.h"
#include "WindowView.h"
#include "Renderer.h"
#include "PoolMgr.h"
#include "log.h"
#include "Label.h"
#include "ActionManager.h"
#include "Event.h"
#include "EventDispatcher.h"

NS_ENGIN_BEGIN

void getNowTime(timeval& time){
#if TARGET_PLATFORM==PLATFORM_WIN32
	LARGE_INTEGER freq, count;
	//Ƶ�� һ���ӽ��ж���Σ����Ե�������һ����Ҫ�೤ʱ��
	QueryPerformanceFrequency(&freq);
	//��ĿǰΪֹcpu�����˶��ٴ�
	QueryPerformanceCounter(&count);
	//��������ʱ�䣬���ǵ�ĿǰΪֹ�Ŀ���ʱ��
	time.tv_sec = (long long)count.QuadPart / freq.QuadPart;
	//����Ŵ�΢�룬����΢�뼶���ʱ�侫�ȣ�Ȼ���ټ�ȥ��Ĳ���
	//Ϊʲô���������أ���Ϊǰ��������ʱ�򣬾���û����ô�ߣ�����
	//0.123456789*0.001 ���� 0.000123456789 �����long���棬��Լ����0
	//�Ŵ�1000000��Ϊ123456.789*0.001 ��long���棬��Լ����123�����Ⱦ����ֳ�����
	time.tv_usec = (long long)((count.QuadPart * 1000000.0 / freq.QuadPart) - time.tv_sec * 1000000.0);
#endif
}

char * Director::EVENT_AFTER_DRAW = "director_after_draw";

Director::Director() 
:_purgeDirectorInNextLoop(false), 
_runingScene(nullptr), 
_nextScene(nullptr),
_wndView(nullptr),
_scheduler(nullptr),
_deltaTime(0),
_gameMode(MODE_WND),
_displayStats(true),
_renderer(nullptr),
_statusLabel(nullptr),
_edgeWidth(0),
_edgeHeight(0),
_wndSize(0,0),
fullScreenRange(0, 0, 0, 0),
_animationInterval(1.0f/60.0f),
wndClipRange(0,0,0,0){
	_scheduler = new Scheduler;
	_actionManager = new ActionManager;
	_scheduler->scheduleUpdate(_actionManager);
	_poolMgr = PoolMgr::getInstance();
	_preTime = new timeval;
	getNowTime(*_preTime);
	_modelViewMatrix.identify();
	_eventAfterDraw = Event::create(EVENT_AFTER_DRAW);
	_eventAfterDraw->retain();
	_eventDispatcher = EventDispatcher::create();
	_eventDispatcher->retain();
}

Director::~Director(){
	if(_eventAfterDraw){
		_eventAfterDraw->release();
		_eventAfterDraw = nullptr;
	}

	if(_eventDispatcher){
		_eventDispatcher->release();
		_eventDispatcher = nullptr;
	}

	delete _renderer;
	_renderer = nullptr;
	_poolMgr = nullptr;
	
	//scheduler һ��Ҫ��scheduler ��update�б�Ԫ���ͷ�ǰ�������������п�����scheduler��update����ʱ��Ԫ���Ѿ��ͷţ��Ӷ���ɱ���
	delete _scheduler;
	_scheduler = nullptr;

	//һ��Ҫ��scheduler��������
	if (_actionManager){
		_actionManager->release();
		_actionManager = nullptr;
	}

	delete _preTime;
	_preTime = nullptr;

	if (_statusLabel){
		_statusLabel->release();
		_statusLabel = nullptr;
	}
}

void Director::setWindowView(WindowView* view){
	if (view == nullptr){
		return;
	}
	_wndView = view;
	_renderer = new Renderer;

	RECT wndRect;
	RECT clientRect;
	GetWindowRect((HWND)_wndView->getHwnd(), &wndRect);
	GetClientRect((HWND)_wndView->getHwnd(), &clientRect);
	_edgeWidth = (float)(wndRect.right - wndRect.left - clientRect.right)/2.0f;
	_edgeHeight = (float)(wndRect.bottom - wndRect.top - clientRect.bottom)-_edgeWidth;

	if (_gameMode == MODE_FULLSCREEN){
		_wndSize.width = (float)GetSystemMetrics(SM_CXSCREEN);
		_wndSize.height = (float)GetSystemMetrics(SM_CYSCREEN);
	}
	else{
		_wndSize = _wndView->getDesignResolutionSize();
	}

	if (!_renderer->initRender()){
		_wndView->release();
		_wndView = nullptr;
	}

	Size surfaceSize = _renderer->getSurfaceSize();
	fullScreenRange.right = surfaceSize.width-1;
	fullScreenRange.bottom = surfaceSize.height-1;
	fixp16_fullScreenRange.right = FLOAT_2_FIXP16(surfaceSize.width-1);
	fixp16_fullScreenRange.bottom = FLOAT_2_FIXP16(surfaceSize.height-1);
	updateWindowPos();
}

const Size& Director::getWindowSize(){
	return _wndSize;
}

void Director::updateWindowPos(){
	if(_wndView==nullptr)
		return;
	if (_gameMode == MODE_FULLSCREEN){
		wndOrigin.x = 0;
		wndOrigin.y = 0;
	}
	else{
		GetWindowRect((HWND)_wndView->getHwnd(), &wndRect);
		wndOrigin.x = wndRect.left + _edgeWidth;
		wndOrigin.y = wndRect.top + _edgeHeight;
	}

	fixp16WndOrigin=wndOrigin;

	if (_gameMode == MODE_FULLSCREEN)
		return;

	const Vec2f& wndOrigin = getWindowOrigin();
	if (wndOrigin.x >= fullScreenRange.right || wndOrigin.y >= fullScreenRange.bottom){
		memset(&wndClipRange, 0, sizeof Range);
		memset(&fixp16_wndClipRange, 0, sizeof RangeFIXP16);
		return;
	}

	wndClipRange.left = wndOrigin.x<0 ? 0 : wndOrigin.x;
	wndClipRange.top = wndOrigin.y<0 ? 0 : wndOrigin.y;
	wndClipRange.right = wndOrigin.x + _wndSize.width;
	wndClipRange.bottom = wndOrigin.y + _wndSize.height;
	if (wndClipRange.right > fullScreenRange.right){
		wndClipRange.right = fullScreenRange.right;
	}
	if (wndClipRange.bottom > fullScreenRange.bottom){
		wndClipRange.bottom = fullScreenRange.bottom;
	}

	fixp16_wndClipRange.left = FLOAT_2_FIXP16(wndClipRange.left);
	fixp16_wndClipRange.right = FLOAT_2_FIXP16(wndClipRange.right);
	fixp16_wndClipRange.top = FLOAT_2_FIXP16(wndClipRange.top);
	fixp16_wndClipRange.bottom = FLOAT_2_FIXP16(wndClipRange.bottom);
}

void Director::purgeDirector(){
	reset();
	release();
}

void Director::mainLoop(){
	if (_purgeDirectorInNextLoop){
		purgeDirector();
		_purgeDirectorInNextLoop = false;
	}
	else{
		drawScene();
		_poolMgr->getCurPool()->clear();
	}
#ifdef CHANGE_FRAME
	float deltaFrame=(_animationInterval-_deltaTime)*1000;
	if(deltaFrame>0){
		DWORD start_count = GetTickCount();
		while(GetTickCount()-start_count<deltaFrame);
	}
#endif
}

void Director::drawScene(){
	calculateDeltaTime();
	//����clear���棬��ʹupdate���йػ�ͼ���߼���Ч
	_scheduler->update(_deltaTime);
	_renderer->clear();
	if (_nextScene)
		setNextScene();
	if (_runingScene)
		_runingScene->visit(_renderer,_modelViewMatrix);
	updateStats();
	_renderer->flip();
	_eventDispatcher->dispatchEvent(_eventAfterDraw);
}

void Director::end(){
	_purgeDirectorInNextLoop = true;
}

void Director::reset(){
	std::vector<Scene*> temp;
	temp.swap(_vScene);
	for (auto scene : temp){
		scene->release();
	}
	if (_runingScene)
		_runingScene->release();
	_runingScene = nullptr;
}

void Director::setNextScene(){
	if (_runingScene){
		_runingScene->release();
	}
	_nextScene->retain();
	_runingScene = _nextScene;
	_nextScene = nullptr;
}

void Director::replaceScene(Scene* scene){

}

void Director::runWithScene(Scene* scene){
	if (scene == nullptr)
		return;
	if(_vScene.size()==0)
		_vScene.resize(1);
	Scene* oldScene = _vScene[0];
	if (oldScene)
		oldScene->release();
	_vScene[0] = scene;
	scene->retain();
	_nextScene = scene;
}

Director* Director::getInstance(){
	if (_instance == nullptr){
		_instance = new Director;
	}
	return _instance;
}

WindowView* Director::getWindowView(){
	return _wndView;
}

void Director::calculateDeltaTime(){
#if TARGET_PLATFORM==PLATFORM_WIN32
	static timeval nowTime;
	getNowTime(nowTime);
	//΢�벿���Ѿ�����ļ�ȥ��
	_deltaTime = nowTime.tv_sec - _preTime->tv_sec + (nowTime.tv_usec - _preTime->tv_usec)/1000000.0f;
	_deltaTime = _deltaTime > 0 ? _deltaTime : 0;
	*_preTime = nowTime;
#endif
}

float Director::GetAverageFPS(){
	static float totalFrame = 0;
	static float totalTime = 0;
	totalFrame++;
	totalTime += _deltaTime;
	if (totalTime == 0){
		return 60;
	}
	return totalFrame / totalTime;
}

float Director::GetFilterFPS(){
	//cocos2dx�㷨���д��о�
	return 0;
}

float Director::GetCountFPS(){
	static float preFPS = 0;
	static float nowFPS = 0;
	static float passTime = 0;
	passTime += _deltaTime;
	nowFPS++;
	if (passTime >= 1){
		passTime = 0;
		preFPS = nowFPS;
		nowFPS = 0;
	}
	return preFPS;
}

void Director::updateStats(){
	if (!_displayStats){
		return;
	}
	if (_statusLabel == nullptr){
		_statusLabel = Label::create("FPS:60");
		_statusLabel->setFontSize(30);
		Size wndSize = this->getWindowSize();
		_statusLabel->setPosition(Vec3f(20.0f, wndSize.height - 30));
		_statusLabel->retain();
	}
	static char* fpsWord = "FPS:%.2f";
	static char fps[100];
	//˲ʱ �仯̫��
	//sprintf(fps, fpsWord, 1 / _deltaTime);
	//ƽ�� ����ʱ����������ϵ��������ӣ�ֱ��һ���ȶ�ֵ
	//sprintf(fps, fpsWord, GetAverageFPS());
	//ÿ��֡��
	sprintf(fps, fpsWord, GetCountFPS());
	static unsigned int preFPSCOUNT=0;
	static unsigned int nowFPSCOUNT=0;
	nowFPSCOUNT++;
	if(nowFPSCOUNT-preFPSCOUNT>360){
		preFPSCOUNT=nowFPSCOUNT;
		std::cout<<"fps:"<<fps<<std::endl;
	}
	//_statusLabel->setLabel(fps);
	//_statusLabel->draw(_renderer,_modelViewMatrix);
}

Director* Director::_instance=nullptr;

NS_ENGIN_END


