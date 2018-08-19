#ifndef __EVENTLISTENER_H
#define __EVENTLISTENER_H
#include "macros.h"
#include "Event.h"
#include <string>
#include <functional>
#include "Ref.h"
NS_ENGIN_BEGIN
class Node;
typedef std::function<void(Event*)> ON_EVENT;
class ENGIN_DLL EventListener:public Ref{
public:
	EventListener();
	virtual ~EventListener();
	static EventListener* create(ON_EVENT oe, EventID eid, int prior=0);
	EventID id;
	int fixedPriority;
	ON_EVENT onEvent;
	Node* target;
	bool isRegister;
};
NS_ENGIN_END
#endif