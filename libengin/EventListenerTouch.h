#ifndef __EVENTLISTENERTOUCH_H
#define __EVENTLISTENERTOUCH_H
#include "macros.h"
#include "EventListener.h"
NS_ENGIN_BEGIN
class EventTouch;
class Touch;
class ENGIN_DLL EventListenerTouch:public EventListener{
public:
	typedef std::function<void(Touch*,EventTouch*)> touchCallback;
	touchCallback onTouchBegan;
	touchCallback onTouchMoved;
	touchCallback onTouchEnded;
	touchCallback onTouchCancelled;
	
	EventListenerTouch();
	virtual ~EventListenerTouch();
};
NS_ENGIN_END
#endif