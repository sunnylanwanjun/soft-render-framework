#ifndef __REF_H
#define __REF_H
#include "macros.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Ref{
public:
	Ref();
	virtual ~Ref();
	void retain();
	void release();
	void autoRelease();
	int getReference(){ return _referenceCount; }
private:
	int _referenceCount;
};
NS_ENGIN_END
#endif