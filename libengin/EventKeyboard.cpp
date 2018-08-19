#include "EventKeyboard.h"
NS_ENGIN_BEGIN
EventKeyboard::EventKeyboard():
isPress(false),
keyCode(KEY_NONE){
	id=EVENT_ID_KEYBOARD;
}

EventKeyboard::~EventKeyboard(){

}
NS_ENGIN_END