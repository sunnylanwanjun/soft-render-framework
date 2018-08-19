#include "ActionManager.h"
NS_ENGIN_BEGIN

ActionManager::ActionManager(){

}

ActionManager::~ActionManager(){
	for (auto it = _vActionList.begin(); it != _vActionList.end(); it++){
		(*it)->stop();
		(*it)->release();
	}
}

void ActionManager::update(float dt){
	for (int i = _vActionList.size() - 1; i >= 0;i--){
		Action* action = _vActionList[i];
		if (action->isDone()){
			removeAction(action);
		}
		else{
			action->update(dt);
		}
	}
}

void ActionManager::addAction(Action* action, Ref* target){
	action->retain();
	action->start(target);
	_vActionList.push_back(action);
}

void ActionManager::removeAction(Action* action){
	for (auto it=_vActionList.begin(); it != _vActionList.end();it++){
		if (*it == action){
			(*it)->stop();
			(*it)->release();
			_vActionList.erase(it);
			break;
		}
	}
}

NS_ENGIN_END