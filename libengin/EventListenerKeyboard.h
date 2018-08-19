#ifndef __EVENTLISTENERKEYBOARD_H
#define __EVENTLISTENERKEYBOARD_H
#include "macros.h"
#include "EventListener.h"
#include "EventKeyboard.h"
NS_ENGIN_BEGIN
typedef std::function<void(EventKeyboard::KeyCode,EventKeyboard*)> keyboardCallback;
class ENGIN_DLL EventListenerKeyboard:public EventListener{
public:
	keyboardCallback onKeyPress;
	keyboardCallback onKeyRelease;

	EventListenerKeyboard();
	virtual ~EventListenerKeyboard();
	static EventListenerKeyboard* create(int prior=0);
};
NS_ENGIN_END
#endif