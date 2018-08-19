#include "EventDispatcher.h"
#include <algorithm>
#include "EventListenerTouch.h"
#include "EventListenerKeyboard.h"
NS_ENGIN_BEGIN

class DispatchGuard{
public:
	DispatchGuard(int& count):_count(count){_count++;}
	~DispatchGuard(){_count--;}
private:
	int& _count;
};

EventDispatcher::EventDispatcher():_dispatchingCount(0){

}

EventDispatcher::~EventDispatcher(){
	removeAllEventListener();
}

void EventDispatcher::addEventListener( EventListener* listener ){
	if(listener==nullptr){
		return;
	}

	listener->retain();
	if(_dispatchingCount!=0){
		_waitToAddListener.push_back(listener);
		return;
	}
	auto it = _listenerMap.find(listener->id);
	ListenerVector* plv = nullptr;
	if(it==_listenerMap.end()){
		plv = new ListenerVector;
		_listenerMap[listener->id] = plv;
	}
	else{
		plv = it->second;
	}
	plv->push_back(listener);
	_dirtyMap[listener->id]=true;
}

void EventDispatcher::removeEventListener( EventListener* listener ){
	if(listener==nullptr){
		return;
	}
	//还没来得及加入派发列表的监听器
	auto wit = std::find(_waitToAddListener.begin(),_waitToAddListener.end(),listener);
	if(wit!=_waitToAddListener.end()){
		listener->release();
		_waitToAddListener.erase(wit);
		return;
	}

	auto it0 = _listenerMap.find(listener->id);
	if(it0==_listenerMap.end()){
		return;
	}
	ListenerVector* plv = it0->second;
	auto it1 = std::find(plv->begin(),plv->end(),listener);
	if(it1==plv->end()){
		return;
	}
	if(_dispatchingCount==0){
		plv->erase(it1);
		listener->release();
		if(plv->empty()){
			_listenerMap.erase(it0);
			_dirtyMap.erase(_dirtyMap.find(listener->id));
			delete plv;
		}
		else{
			_dirtyMap[listener->id]=true;
		}
	}
	else{
		listener->isRegister=false;
	}
}

void EventDispatcher::removeAllEventListener(){
	if(_dispatchingCount==0){
		for(auto it0:_listenerMap){
			ListenerVector* plv = it0.second;
			for(auto it1:*plv){
				it1->release();
			}
			delete plv;
		}
		_listenerMap.clear();
	}
	else{
		for(auto it0:_listenerMap){
			ListenerVector* plv = it0.second;
			for(auto it1:*plv){
				it1->isRegister=false;
			}
		}
	}
	if(!_waitToAddListener.empty()){
		for(auto l:_waitToAddListener){
			l->release();
		}
		_waitToAddListener.clear();
	}
}

void EventDispatcher::dispatchEvent( Event* e ){
	if(e==nullptr){
		return;
	}
	DispatchGuard dg(_dispatchingCount);
	auto it0 = _listenerMap.find(e->id);
	if(it0==_listenerMap.end()){
		return;
	}
	ListenerVector* plv = it0->second;
	sortEventListener(plv,e->id);
	for(auto it1:*plv){
		e->curTarget = it1->target;
		it1->onEvent(e);
		if(e->isStop())
			break;
	}
	updateListener();
}

void EventDispatcher::dispatchTouchEvent(EventTouch* e){
	if(e==nullptr)return;
	DispatchGuard dg(_dispatchingCount);
	auto it0 = _listenerMap.find(e->id);
	if(it0==_listenerMap.end()){
		return;
	}
	ListenerVector* plv = it0->second;
	EventTouch* eventTouch=(EventTouch*)e;
	sortEventListener(plv,e->id);
	for(auto it1:*plv){
		e->curTarget = it1->target;
		EventListenerTouch* touchListener=(EventListenerTouch*)(it1);
		if(e->touchCode==EventTouch::TouchCode::BEGAN){
			if(touchListener->onTouchBegan)
				touchListener->onTouchBegan(eventTouch->touches[0],eventTouch);
		}else if(e->touchCode==EventTouch::TouchCode::MOVED){
			if(touchListener->onTouchMoved)
				touchListener->onTouchMoved(eventTouch->touches[0],eventTouch);
		}else if(e->touchCode==EventTouch::TouchCode::ENDED){
			if(touchListener->onTouchEnded)
				touchListener->onTouchEnded(eventTouch->touches[0],eventTouch);
		}else if(e->touchCode==EventTouch::TouchCode::CANCELLED){
			if(touchListener->onTouchCancelled)
				touchListener->onTouchCancelled(eventTouch->touches[0],eventTouch);
		}
		if(e->isStop())
			break;
	}
	updateListener();
}

void EventDispatcher::dispatchKeyboardEvent(EventKeyboard* e){
	if(e==nullptr)return;
	DispatchGuard dg(_dispatchingCount);
	auto it0 = _listenerMap.find(e->id);
	if(it0==_listenerMap.end()){
		return;
	}
	ListenerVector* plv = it0->second;
	sortEventListener(plv,e->id);
	EventKeyboard* keyEvent=(EventKeyboard*)e;
	for(auto it1:*plv){
		e->curTarget = it1->target;
		EventListenerKeyboard* keyListener=(EventListenerKeyboard*)(it1);
		if(e->isPress){
			if(keyListener->onKeyPress)
				keyListener->onKeyPress(keyEvent->keyCode,keyEvent);
		}else{
			if(keyListener->onKeyRelease)
				keyListener->onKeyRelease(keyEvent->keyCode,keyEvent);
		}
		if(e->isStop())
			break;
	}
	updateListener();
}

void EventDispatcher::sortEventListener(ListenerVector* lv,EventID id){
	if(!_dirtyMap[id]){
		return;
	}
	std::sort(lv->begin(),lv->end(),[&lv](EventListener* L0,EventListener* L1){
		return L0->fixedPriority>L1->fixedPriority;
	});
}

EventDispatcher* EventDispatcher::create(){
	EventDispatcher* eventDispatcher = new EventDispatcher;
	eventDispatcher->autoRelease();
	return eventDispatcher;
}

void EventDispatcher::updateListener(){
	if(_dispatchingCount>1){
		return;
	}
	if(!_waitToAddListener.empty()){
		for(auto l:_waitToAddListener){
			auto it = _listenerMap.find(l->id);
			ListenerVector* plv = nullptr;
			if(it==_listenerMap.end()){
				plv = new ListenerVector;
				_listenerMap[l->id] = plv;
			}
			else{
				plv = it->second;
			}
			plv->push_back(l);
			_dirtyMap[l->id]=true;
		}
	}
	for(auto it=_listenerMap.begin();it!=_listenerMap.end();){
		ListenerVector* lv=it->second;
		for(auto it0=lv->begin();it0!=lv->end();){
			if(!(*it0)->isRegister){
				it0=lv->erase(it0);
			}
			else{
				it0++;
			}
		}
		if(lv->empty()){
			it=_listenerMap.erase(it);
		}
		else{
			it++;
		}
	}
}

NS_ENGIN_END