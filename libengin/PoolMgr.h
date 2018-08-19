#ifndef __POOLMGR_H
#define __POOLMGR_H
#include "macros.h"
#include "Ref.h"
#include <vector>
NS_ENGIN_BEGIN
class ENGIN_DLL AutoReleasePool{
public:
	AutoReleasePool();
	virtual ~AutoReleasePool();
	void addObject(Ref* obj);
	void clear();
private:
	std::vector<Ref*> _objArr;
};

class ENGIN_DLL PoolMgr{
	friend class AutoReleasePool;
public:
	static PoolMgr* getInstance();
	static void destroyInstance();
	AutoReleasePool* getCurPool();
	void push(AutoReleasePool* pool);
	void pop();
private:
	PoolMgr();
	virtual ~PoolMgr();
	std::vector<AutoReleasePool*> _autoStack;
	static PoolMgr* _instance;
};
NS_ENGIN_END
#endif