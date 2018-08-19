#ifndef __SCENE_H
#define __SCENE_H
#include "macros.h"
#include "Renderer.h"
#include "Node.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Scene:public Node{
public:
	Scene();
	virtual ~Scene();
private:
	
};
NS_ENGIN_END
#endif