#ifndef __EVENT_H
#define __EVENT_H
#include "macros.h"
#include <string>
#include "Ref.h"
NS_ENGIN_BEGIN
class Node;
typedef std::string EventID;
class ENGIN_DLL Event:public Ref{
public:
	Event();
	virtual ~Event();
	static Event* create(EventID eid);
	void stop();
	bool isStop();
public:
	EventID id;
	Node*   curTarget;
private:
	bool _isStop;
};
NS_ENGIN_END
#endif