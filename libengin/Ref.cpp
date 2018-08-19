#include "Ref.h"
#include "PoolMgr.h"
NS_ENGIN_BEGIN
Ref::Ref() :_referenceCount(1)
{

}

Ref::~Ref()
{

}

void Ref::retain()
{
	++_referenceCount;
}

void Ref::release()
{
	--_referenceCount;
	if (_referenceCount <= 0)
		delete this;
}

void Ref::autoRelease()
{
	PoolMgr::getInstance()->getCurPool()->addObject(this);
}

NS_ENGIN_END
