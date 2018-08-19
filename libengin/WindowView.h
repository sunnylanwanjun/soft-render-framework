#ifndef __WINDOWVIEW_H
#define __WINDOWVIEW_H
#include "macros.h"
#include "Ref.h"
#include "Geometry.h"
NS_ENGIN_BEGIN
enum ResolutionPolicy{
	NO_BORDER,
};
class ENGIN_DLL WindowView:public Ref{
public:
	WindowView();
	virtual ~WindowView();
	static WindowView* create(const char* name,Rect& rect);
	bool windowShouldClose();
	bool handleEvents();
	bool initWithRect(const char* name,Rect& rect );
	Size getDesignResolutionSize();
	Size getFrameSize();
	void setFrameSize(Size& size);
	void setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy);
	void* getHwnd(){ return _hwnd; };
private:
	void* _hInstance;
	void* _hwnd;
	Size _screenSize;
	Size _resolutionSize;
	ResolutionPolicy _policy;
	bool _shouldClose;
};
NS_ENGIN_END
#endif