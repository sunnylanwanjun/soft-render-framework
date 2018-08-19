#ifndef __RENDER_H
#define __RENDER_H
#include "macros.h"
#include "Geometry.h"
#include "Texture.h"
#include <ddraw.h>
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib, "dxguid.lib ")
NS_ENGIN_BEGIN
class ENGIN_DLL Renderer{
public:
	Renderer();
	virtual ~Renderer();
	void render(Vec3f& pos, Size& contentSize, float scaleX, float scaleY, float rotation, Texture* data);
	void render(Vec3f& pos, Size& contentSize, float scaleX, float scaleY, float rotation, LPDIRECTDRAWSURFACE7 lpdds);
	void flip();
	void clear();
	bool initRender();
	LPDIRECTDRAW7 getDirectDraw();
	LPDIRECTDRAWSURFACE7 getDirectDrawSurface();
	Size& getSurfaceSize(){ return _surfaceSize; }
private:
	LPDIRECTDRAW7 _lpdd7;
	LPDIRECTDRAWSURFACE7 _lpddsprimary;
	LPDIRECTDRAWSURFACE7 _lpddsback;
	LPDIRECTDRAWSURFACE7 _lpddsbuffer;
	LPDIRECTDRAWCLIPPER _lpddbufferclipper;
	LPDIRECTDRAWCLIPPER _lpddclipper;
	Size _surfaceSize;
};
NS_ENGIN_END
#endif