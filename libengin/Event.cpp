#include "Event.h"
NS_ENGIN_BEGIN

Event::Event():_isStop(false),curTarget(nullptr){

}

Event::~Event(){

}

void Event::stop(){
	_isStop=true;
}

bool Event::isStop(){
	return _isStop;
}

Event* Event::create( EventID eid ){
	Event* event = new Event;
	event->id = eid;
	event->autoRelease();
	return event;
}

NS_ENGIN_END