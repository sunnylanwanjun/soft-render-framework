#include "PoolMgr.h"
NS_ENGIN_BEGIN
AutoReleasePool::AutoReleasePool(){
	PoolMgr::getInstance()->push(this);
}

AutoReleasePool::~AutoReleasePool(){
	this->clear();
	PoolMgr::getInstance()->pop();
}

void AutoReleasePool::clear(){
	std::vector<Ref*> arr;
	arr.swap(_objArr);
	for (auto obj : arr){
		obj->release();
	}
}

void AutoReleasePool::addObject(Ref* obj){
	_objArr.push_back(obj);
}

////////////////////////////////////内存池管理器//////////////////////////////////

PoolMgr* PoolMgr::getInstance(){
	if (_instance == nullptr){
		_instance = new PoolMgr();
		new AutoReleasePool;
	}
	return _instance;
}

void PoolMgr::destroyInstance(){
	PoolMgr* mgr = PoolMgr::getInstance();
	delete mgr;
}

AutoReleasePool* PoolMgr::getCurPool(){
	return _autoStack.back();
}

PoolMgr::PoolMgr(){

}

PoolMgr::~PoolMgr(){
	while (!_autoStack.empty()){
		auto autoPool = _autoStack.back();
		delete autoPool;
	}
}

void PoolMgr::push(AutoReleasePool* pool){
	_autoStack.push_back(pool);
}

void PoolMgr::pop(){
	_autoStack.pop_back();
}

PoolMgr* PoolMgr::_instance=nullptr;

NS_ENGIN_END