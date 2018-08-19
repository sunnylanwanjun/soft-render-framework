#ifndef __DIRECTOR_H
#define __DIRECTOR_H
#include "macros.h"
#include "Ref.h"
#include <vector>
#include "Geometry.h"
#include "Scheduler.h"
#include "Mat4.h"
NS_ENGIN_BEGIN 

class Scene;
class WindowView;
class Renderer;
class PoolMgr;
class Label;
class ActionManager;
class Event;
class EventDispatcher;

class ENGIN_DLL Director:public Ref{
public:

	static char * EVENT_AFTER_DRAW;
	
	Director();
	virtual ~Director();
	void mainLoop();
	void end();
	void reset();
	static Director* getInstance();
	void purgeDirector();
	void setNextScene();
	void replaceScene(Scene* scene);
	void runWithScene(Scene* scene);
	void setWindowView(WindowView* view);
	void drawScene();
	void setDisplayStats(bool displayStats) { _displayStats = displayStats; }
	void updateStats();
	GAME_MODE getGameMode(){ return _gameMode; }
	void setGameMode(GAME_MODE gameMode){ _gameMode = gameMode; }
	Renderer* getRenderer(){ return _renderer; }
	Scheduler* getScheduler() { return _scheduler; }
	WindowView* getWindowView();
	const Size& getWindowSize();
	void updateWindowPos();
	const Vec2f& getWindowOrigin(){
		return wndOrigin;
	}
	const Range& getClipRange(){
		if (_gameMode == MODE_FULLSCREEN){
			return fullScreenRange;
		}
		else{
			return wndClipRange;
		}
	}
	const Vec2FIXP16& getFIXP16WindowOrigin(){
		return fixp16WndOrigin;
	}
	const RangeFIXP16& getFIXP16ClipRange(){
		if (_gameMode == MODE_FULLSCREEN){
			return fixp16_fullScreenRange;
		}
		else{
			return fixp16_wndClipRange;
		}
	}
	ActionManager* getActionManager(){ return _actionManager; }
	void setAnimationInterval(float interval){_animationInterval=interval;}
	EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }
private:
	void calculateDeltaTime();
	float GetAverageFPS();
	float GetFilterFPS();
	float GetCountFPS();
private:
	static Director* _instance;
	bool _purgeDirectorInNextLoop;
	bool _displayStats;
	Renderer* _renderer;
	Scene*  _runingScene;
	Scene*  _nextScene;
	std::vector<Scene*> _vScene;
	WindowView* _wndView;
	PoolMgr* _poolMgr;
	Scheduler* _scheduler;
	ActionManager* _actionManager;
	GAME_MODE _gameMode;
	struct timeval* _preTime;
	float   _deltaTime;
	Label*  _statusLabel;
	float   _edgeHeight;
	float   _edgeWidth;
	Size  _wndSize;
	
	Mat4 _modelViewMatrix;
	float _animationInterval;
	Event* _eventAfterDraw;
	EventDispatcher* _eventDispatcher;
public:
	Range wndClipRange;
	RangeFIXP16 fixp16_wndClipRange;
	Range  fullScreenRange;
	RangeFIXP16  fixp16_fullScreenRange;
	RECT wndRect;
	Vec2f wndOrigin;
	Vec2FIXP16 fixp16WndOrigin;
};

NS_ENGIN_END
#endif