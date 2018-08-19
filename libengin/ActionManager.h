#ifndef __ACTIONMANAGER_H
#define __ACTIONMANAGER_H
#include "macros.h"
#include "Ref.h"
#include <vector>
#include "Action.h"
NS_ENGIN_BEGIN
class ENGIN_DLL ActionManager:public Ref
{
public:
	ActionManager();
	virtual ~ActionManager();
	void addAction(Action* action,Ref* target);
	void removeAction(Action* action);
	void update(float dt);
private:
	std::vector<Action*> _vActionList;
};
NS_ENGIN_END
#endif
