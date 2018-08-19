#include "EventListenerTouch.h"
#include "EventTouch.h"
NS_ENGIN_BEGIN
EventListenerTouch::EventListenerTouch():
onTouchBegan(nullptr),
onTouchMoved(nullptr),
onTouchEnded(nullptr),
onTouchCancelled(nullptr){
	id = EVENT_ID_TOUCH;
}

EventListenerTouch::~EventListenerTouch(){

}
NS_ENGIN_END