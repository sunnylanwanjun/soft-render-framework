#include "Scheduler.h"
NS_ENGIN_BEGIN
Scheduler::Scheduler(){

}

Scheduler::~Scheduler(){

}

void Scheduler::update(float dt){
	for (auto& info : _vCallback){
		info.callback(dt);
	}
}
NS_ENGIN_END