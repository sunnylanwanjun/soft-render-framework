#ifndef __APPLICATION_H
#define __APPLICATION_H
#include "macros.h"
NS_ENGIN_BEGIN
class ENGIN_DLL ApplicationProtocal{
public:
	virtual bool applicationDidFinishLaunching() = 0;
};

class ENGIN_DLL Application :public ApplicationProtocal{
public:
	Application();
	virtual ~Application();
	int run();
};
NS_ENGIN_END
#endif