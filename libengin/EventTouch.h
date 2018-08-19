#ifndef __EVENTTOUCH_H
#define __EVENTTOUCH_H
#include "macros.h"
#include "Event.h"
#include <vector>
#include "Vec.h"
NS_ENGIN_BEGIN
const EventID EVENT_ID_TOUCH = "event_id_touch";
class Touch;
typedef std::vector<Touch*> TOUCH_VECTOR;
class ENGIN_DLL Touch:public Ref{
public:
	Touch(){}
	~Touch(){}
	int id;
	Vec3f point;
};
class ENGIN_DLL EventTouch:public Event{
public:

	enum TouchCode
	{
		UNKNOW,
		BEGAN,
		MOVED,
		ENDED,
		CANCELLED
	};

	EventTouch();
	EventTouch(TOUCH_VECTOR _touches){
		touches=_touches;
	}
	virtual ~EventTouch();
	TouchCode touchCode;
	TOUCH_VECTOR touches;
};
NS_ENGIN_END
#endif