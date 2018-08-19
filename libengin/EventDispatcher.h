#ifndef __EVENTDISPATCHER_H
#define __EVENTDISPATCHER_H
#include "macros.h"
#include <vector>
#include <map>
#include "Event.h"
#include "EventListener.h"
#include "EventTouch.h"
#include "EventKeyboard.h"
NS_ENGIN_BEGIN
class ENGIN_DLL EventDispatcher:public Ref{
	typedef std::vector<EventListener*> ListenerVector;
public:
	EventDispatcher();
	virtual ~EventDispatcher();
	static EventDispatcher* create();
	void addEventListener(EventListener* listener);
	void removeEventListener(EventListener* listener);
	void removeAllEventListener();
	void dispatchEvent(Event* e);
	void dispatchTouchEvent(EventTouch* e);
	void dispatchKeyboardEvent(EventKeyboard* e);
	void sortEventListener(ListenerVector* lv,EventID id);
	void updateListener();
private:
	std::map<std::string,ListenerVector*> _listenerMap;
	std::map<std::string,bool> _dirtyMap;
	std::vector<EventListener*> _waitToAddListener;
	int _dispatchingCount;
};
NS_ENGIN_END
#endif