#include "Renderer.h"
#include "t3dlib1.h"
#include "Director.h"
#include "WindowView.h"
#include "log.h"
NS_ENGIN_BEGIN
#define DIRECTDRAW_BPP 32
#define NEED_RENDER

static GAME_MODE _renderMode;
static HWND _wndHwnd;
static bool _supportRotation = true;

Renderer::Renderer()
:_lpdd7(nullptr),
_lpddsprimary(nullptr),
_lpddsback(nullptr),
_lpddsbuffer(nullptr),
_lpddbufferclipper(nullptr),
_surfaceSize(0,0){
	
}

Renderer::~Renderer(){
	if (_lpddsbuffer){
		_lpddsbuffer->Release();
		_lpddsbuffer = nullptr;
	}
	if (_lpddsback){
		_lpddsback->Release();
		_lpddsback = nullptr;
	}
	if (_lpddsprimary){
		_lpddsprimary->Release();
		_lpddsprimary = nullptr;
	}
	if (_lpdd7){
		_lpdd7->Release();
		_lpdd7 = nullptr;
	}
	if (_lpddclipper){
		_lpddclipper->Release();
		_lpddclipper = nullptr;
	}
	if (_lpddbufferclipper){
		_lpddbufferclipper->Release();
		_lpddbufferclipper = nullptr;
	}
}

bool Renderer::initRender(){
#ifdef NEED_RENDER
	auto director = Director::getInstance();
	auto wndView = director->getWindowView();
	_renderMode = director->getGameMode();
	_wndHwnd = (HWND)wndView->getHwnd();

	_surfaceSize.width = (float)GetSystemMetrics(SM_CXSCREEN);
	_surfaceSize.height = (float)GetSystemMetrics(SM_CYSCREEN);

	_lpdd7 = DirectDraw7Init(_wndHwnd, _renderMode == MODE_FULLSCREEN, (DWORD)_surfaceSize.width, (DWORD)_surfaceSize.height, DIRECTDRAW_BPP);
	if (_lpdd7 == nullptr){
		return false;
	}

	if (_renderMode == MODE_FULLSCREEN){
		DirectDrawSurfaceInit_Primary(_lpdd7, &_lpddsprimary, &_lpddsback);
		if (_lpddsprimary == nullptr || _lpddsback == nullptr){
			return false;
		}
		/*谁调用blt谁使用clipper,设置后，只有cliper区域的blt起作用,没有设置裁剪程序也不会挂，不过图像在靠近边缘的地方就消失了。*/
		RECT clipRect;
		clipRect.top = 0;
		clipRect.left = 0;
		clipRect.bottom = (LONG)_surfaceSize.height;
		clipRect.right = (LONG)_surfaceSize.width;
		_lpddclipper = DirectDrawClipperInit(_lpdd7, _lpddsback, 1, &clipRect);
		if (_lpddclipper == nullptr){
			return false;
		}
	}
	//非全屏模式下不能使用页面切换技术
	else{
		_lpddsprimary = DirectDrawSurfaceInit_Primary(_lpdd7);
		if (_lpddsprimary == nullptr){
			return false;
		}
		_lpddsbuffer = DirectDrawSurfaceInit_Common(_lpdd7, (int)_surfaceSize.width, (int)_surfaceSize.height);
		if (_lpddsbuffer == nullptr){
			return false;
		}
		_lpddclipper = DirectDrawClipperInit(_lpdd7, _lpddsprimary, _wndHwnd);
		if (_lpddclipper == nullptr){
			return false;
		}
		_lpddbufferclipper = DirectDrawClipperInit(_lpdd7, _lpddsbuffer, _wndHwnd);
		if (_lpddbufferclipper == nullptr){
			return false;
		}
	}
	
	DDPIXELFORMAT ddpf;
	DDRAW_INIT_STRUCT(ddpf);
	_lpddsprimary->GetPixelFormat(&ddpf);
	if (ddpf.dwRGBBitCount != DIRECTDRAW_BPP){
		Error("Renderer::initRender sorry,game only run in 32 bit mode");
		return false;
	}

	DDCAPS hel_caps;
	DDCAPS hal_caps;
	DDRAW_INIT_STRUCT(hel_caps);
	DDRAW_INIT_STRUCT(hal_caps);
	_lpdd7->GetCaps(&hal_caps, &hel_caps);
	if (!(hel_caps.dwFXCaps&DDFXCAPS_BLTROTATION) && !(hal_caps.dwFXCaps&DDFXCAPS_BLTROTATION)){
		Error("Renderer::initRender do not support rotation");
		_supportRotation = false;
	}

#endif
	return true;
}

void Renderer::render(Vec3f& pos, Size& contentSize, float scaleX, float scaleY, float rotation, Texture* data){
	LPDIRECTDRAWSURFACE7 lpdds = data->getDirectDrawSurface();
	render(pos, contentSize, scaleX, scaleY, rotation, lpdds);
}

void Renderer::render(Vec3f& pos, Size& contentSize, float scaleX, float scaleY, float rotation, LPDIRECTDRAWSURFACE7 lpdds){
#ifdef NEED_RENDER
	static RECT destRect;
	static RECT srcRect;

	srcRect.top = 0;
	srcRect.bottom = (LONG)(srcRect.top + contentSize.height);
	srcRect.left = 0;
	srcRect.right = (LONG)(srcRect.left + contentSize.width);

	if (_renderMode == MODE_FULLSCREEN){
		//全屏模式窗口左上角的位置是固定的
		destRect.top = (LONG)(pos.y);
		destRect.bottom = (LONG)(destRect.top + contentSize.height*scaleY);
		destRect.left = (LONG)(pos.x);
		destRect.right = (LONG)(destRect.left + contentSize.width*scaleX);

		//硬件光栅，不要锁定表面，否则会blt失败
		if (FAILED(_lpddsback->Blt(&destRect, lpdds, &srcRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr))){
			Error("Renderer::render Blt back failed");
		}
	}
	//窗口模式不能使用后备缓冲，所以只能直接blt到主表面
	else{
		//硬件光栅
		//主表面不支持目标色彩键，所以不能使用DDBLT_KEYSRC，否则会blt失败，但是缓冲表面可以使用
		//只要有blt的表面，都需要设置裁剪，否则会因为blt超出表面，而发生失败
		const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
		destRect.top = (LONG)(wndOrigin.y + pos.y);
		destRect.bottom = (LONG)(destRect.top + contentSize.height*scaleY);
		destRect.left = (LONG)(wndOrigin.x + pos.x);
		destRect.right = (LONG)(destRect.left + contentSize.width*scaleX);

		if (_supportRotation){
			static DDBLTFX ddbltfx;
			DDRAW_INIT_STRUCT(ddbltfx);
			ddbltfx.dwRotationAngle = (DWORD)rotation;
			if (FAILED(_lpddsbuffer->Blt(&destRect, lpdds, &srcRect, DDBLT_WAIT | DDBLT_KEYSRC | DDBLT_ROTATIONANGLE, &ddbltfx))){
				Error("Renderer::render Blt buffer failed 1 ");
			}
		}
		else{
			if (FAILED(_lpddsbuffer->Blt(&destRect, lpdds, &srcRect, DDBLT_WAIT | DDBLT_KEYSRC, nullptr))){
				Error("Renderer::render Blt buffer failed 2 ");
			}
		}
	}
#endif
}

void Renderer::clear(){
#ifdef NEED_RENDER
	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = 0x00;
	if (_renderMode == MODE_FULLSCREEN){
		_lpddsback->Blt(nullptr, nullptr, nullptr, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
	}
	else{
		_lpddsbuffer->Blt(nullptr, nullptr, nullptr, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
	}
#endif
}

void Renderer::flip(){
#ifdef NEED_RENDER
	if (_renderMode == MODE_FULLSCREEN){
		_lpddsprimary->Flip(nullptr, DDFLIP_WAIT);
		// 在窗口模式下，会出现画面残影的情况，而全屏模式下不会，在不使用flip，而使用blt把后备缓冲切换到
		// 主表面的情况下，也会出现画面残影的情况，所以主要问题在于blt，可能这个接口的效率不高，有待解决？？？！！！
		//_lpddsprimary->Blt(nullptr, _lpddsback, nullptr, DDBLT_WAIT, nullptr);
	}
	else{
		_lpddsprimary->Blt(nullptr, _lpddsbuffer, nullptr, DDBLT_WAIT, nullptr);
	}
#endif
}

LPDIRECTDRAW7 Renderer::getDirectDraw(){
	return _lpdd7;
}

LPDIRECTDRAWSURFACE7 Renderer::getDirectDrawSurface(){
	return _renderMode == MODE_FULLSCREEN ? _lpddsback : _lpddsbuffer;
}

NS_ENGIN_END