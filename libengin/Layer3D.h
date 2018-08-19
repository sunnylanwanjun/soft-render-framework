#ifndef __LAYER3D_H
#define __LAYER3D_H
#include "macros.h"
#include "Node.h"
#include "Scene3D.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Layer3D:public Node{
public:
	Layer3D();
	virtual ~Layer3D();
	static Layer3D* create();
	virtual Scene3D*			getScene(){return _scene;}
	virtual void				setScene( Scene3D* scene );
	virtual void				draw(Renderer* renderer,Mat4& transform);
private:
	Scene3D* _scene;
};
NS_ENGIN_END
#endif