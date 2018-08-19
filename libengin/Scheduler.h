#ifndef __SCHEDULER_H
#define __SCHEDULER_H
#include "macros.h"
#include "Ref.h"
#include <vector>
#include <functional>
NS_ENGIN_BEGIN

typedef std::function<void(float)> SchedulerCallback;
struct ENGIN_DLL CallbackInfo{
public:
	SchedulerCallback callback;
	void* target;
};

class ENGIN_DLL Scheduler:public Ref{
public:
	Scheduler();
	virtual ~Scheduler();
	//��ģ���ԭ������չ�Ը�ǿЩ��ֻҪ������update������������ӵ�Scheduler��
	template<class T>
	void scheduleUpdate(T* target);
	template<class T>
	void unScheduleUpdate(T* target);
	void update(float dt);
private:
	std::vector<CallbackInfo> _vCallback;
};

//����������ʱ��һ��Ҫ�Ѷ���Ӷ�ʱ���б���ɾ����������Schedulerÿ֡���б�����ʱ�򣬻����������
template<class T>
void engin::Scheduler::unScheduleUpdate(T* target){
	for (size_t i = 0; i < _vCallback.size(); i++){
		if (_vCallback[i].target == target){
			_vCallback.erase(std::next(_vCallback.begin(),i));
			break;
		}
	}
}

template<class T>
void engin::Scheduler::scheduleUpdate(T* target){
	unScheduleUpdate(target);
	auto callback = [target](float dt){target->update(dt); };
	CallbackInfo callbackInfo;
	callbackInfo.callback = callback;
	callbackInfo.target = target;
	_vCallback.push_back(callbackInfo);
}

NS_ENGIN_END
#endif