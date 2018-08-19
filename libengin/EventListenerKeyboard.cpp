#include "EventListenerKeyboard.h"
#include "EventKeyboard.h"
NS_ENGIN_BEGIN

EventListenerKeyboard* EventListenerKeyboard::create(int prior/*=0*/){
	EventListenerKeyboard* eventListenerKeyboard = new EventListenerKeyboard;
	eventListenerKeyboard->autoRelease();
	return eventListenerKeyboard;
}

EventListenerKeyboard::EventListenerKeyboard():
onKeyRelease(nullptr),
onKeyPress(nullptr){
	id = EVENT_ID_KEYBOARD;
}

EventListenerKeyboard::~EventListenerKeyboard(){

}
NS_ENGIN_END
