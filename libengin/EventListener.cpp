#include "EventListener.h"
NS_ENGIN_BEGIN

EventListener::EventListener():fixedPriority(0),id(""),onEvent(nullptr),target(nullptr),isRegister(true){

}

EventListener::~EventListener(){

}

EventListener* EventListener::create( ON_EVENT oe, EventID eid, int prior ){
	EventListener* eventListener = new EventListener;
	eventListener->autoRelease();
	eventListener->onEvent = oe;
	eventListener->id = eid;
	eventListener->fixedPriority = prior;
	return eventListener;
}


NS_ENGIN_END