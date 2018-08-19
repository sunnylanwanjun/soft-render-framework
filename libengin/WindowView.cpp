#include "WindowView.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <winuser.h>
#include "log.h"
#include "Geometry.h"
#include "Director.h"
#include "EventTouch.h"
#include "EventKeyboard.h"
#include "EventDispatcher.h"
NS_ENGIN_BEGIN
#define WIN_CLASS_NAME "T3DLIB"
#define STYLE_FULLSCREEN WS_POPUP
#define STYLE_WINDOW WS_OVERLAPPED | WS_VISIBLE //WS_OVERLAPPEDWINDOW 不用这个是因为不想用户改变窗口的大小
#define DISPATCHER \
	Director::getInstance()->getEventDispatcher()
static Touch touch;
static TOUCH_VECTOR touchVector;
static EventTouch eventTouch(touchVector);
static EventKeyboard eventKeyboard;
static std::map<DWORD,EventKeyboard::KeyCode> keyCodeMap;
void initEvent(){
	touchVector.push_back(&touch);
	
	//keyCodeMap[VK_NONE]=EventKeyboard::KeyCode::KEY_NONE;
	keyCodeMap[VK_PAUSE]=EventKeyboard::KeyCode::KEY_PAUSE;
	//keyCodeMap[VK_SCROLL_LOCK]=EventKeyboard::KeyCode::KEY_SCROLL_LOCK;
	keyCodeMap[VK_PRINT]=EventKeyboard::KeyCode::KEY_PRINT;
	//keyCodeMap[VK_SYSREQ]=EventKeyboard::KeyCode::KEY_SYSREQ;
	//keyCodeMap[VK_BREAK]=EventKeyboard::KeyCode::KEY_BREAK;
	keyCodeMap[VK_ESCAPE]=EventKeyboard::KeyCode::KEY_ESCAPE;
	keyCodeMap[VK_BACK]=EventKeyboard::KeyCode::KEY_BACK;
	//keyCodeMap[VK_BACKSPACE]=EventKeyboard::KeyCode::KEY_BACKSPACE;
	keyCodeMap[VK_TAB]=EventKeyboard::KeyCode::KEY_TAB;
	//keyCodeMap[VK_BACK_TAB]=EventKeyboard::KeyCode::KEY_BACK_TAB;
	keyCodeMap[VK_RETURN]=EventKeyboard::KeyCode::KEY_RETURN;
	//keyCodeMap[VK_CAPS_LOCK]=EventKeyboard::KeyCode::KEY_CAPS_LOCK;
	keyCodeMap[VK_SHIFT]=EventKeyboard::KeyCode::KEY_SHIFT;
	keyCodeMap[VK_LSHIFT]=EventKeyboard::KeyCode::KEY_LEFT_SHIFT;
	keyCodeMap[VK_RSHIFT]=EventKeyboard::KeyCode::KEY_RIGHT_SHIFT;
	//keyCodeMap[VK_CTRL]=EventKeyboard::KeyCode::KEY_CTRL;
	keyCodeMap[VK_LCONTROL]=EventKeyboard::KeyCode::KEY_LEFT_CTRL;
	keyCodeMap[VK_RCONTROL]=EventKeyboard::KeyCode::KEY_RIGHT_CTRL;
	//keyCodeMap[VK_ALT]=EventKeyboard::KeyCode::KEY_ALT;
	//keyCodeMap[VK_LEFT_ALT]=EventKeyboard::KeyCode::KEY_LEFT_ALT;
	//keyCodeMap[VK_RIGHT_ALT]=EventKeyboard::KeyCode::KEY_RIGHT_ALT;
	keyCodeMap[VK_MENU]=EventKeyboard::KeyCode::KEY_MENU;
	//keyCodeMap[VK_HYPER]=EventKeyboard::KeyCode::KEY_HYPER;
	keyCodeMap[VK_INSERT]=EventKeyboard::KeyCode::KEY_INSERT;
	keyCodeMap[VK_HOME]=EventKeyboard::KeyCode::KEY_HOME;
	//keyCodeMap[VK_PG_UP]=EventKeyboard::KeyCode::KEY_PG_UP;
	keyCodeMap[VK_DELETE]=EventKeyboard::KeyCode::KEY_DELETE;
	keyCodeMap[VK_END]=EventKeyboard::KeyCode::KEY_END;
	//keyCodeMap[VK_PG_DOWN]=EventKeyboard::KeyCode::KEY_PG_DOWN;
	keyCodeMap[VK_LEFT]=EventKeyboard::KeyCode::KEY_LEFT_ARROW;
	keyCodeMap[VK_RIGHT]=EventKeyboard::KeyCode::KEY_RIGHT_ARROW;
	keyCodeMap[VK_UP]=EventKeyboard::KeyCode::KEY_UP_ARROW;
	keyCodeMap[VK_DOWN]=EventKeyboard::KeyCode::KEY_DOWN_ARROW;
	//keyCodeMap[VK_NUM_LOCK]=EventKeyboard::KeyCode::KEY_NUM_LOCK;
	//keyCodeMap[VK_KP_PLUS]=EventKeyboard::KeyCode::KEY_KP_PLUS;
	//keyCodeMap[VK_KP_MINUS]=EventKeyboard::KeyCode::KEY_KP_MINUS;
	//keyCodeMap[VK_KP_MULTIPLY]=EventKeyboard::KeyCode::KEY_KP_MULTIPLY;
	//keyCodeMap[VK_KP_DIVIDE]=EventKeyboard::KeyCode::KEY_KP_DIVIDE;
	//keyCodeMap[VK_KP_ENTER]=EventKeyboard::KeyCode::KEY_KP_ENTER;
	//keyCodeMap[VK_KP_HOME]=EventKeyboard::KeyCode::KEY_KP_HOME;
	//keyCodeMap[VK_KP_UP]=EventKeyboard::KeyCode::KEY_KP_UP;
	//keyCodeMap[VK_KP_PG_UP]=EventKeyboard::KeyCode::KEY_KP_PG_UP;
	//keyCodeMap[VK_KP_LEFT]=EventKeyboard::KeyCode::KEY_KP_LEFT;
	//keyCodeMap[VK_KP_FIVE]=EventKeyboard::KeyCode::KEY_KP_FIVE;
	//keyCodeMap[VK_KP_RIGHT]=EventKeyboard::KeyCode::KEY_KP_RIGHT;
	//keyCodeMap[VK_KP_END]=EventKeyboard::KeyCode::KEY_KP_END;
	//keyCodeMap[VK_KP_DOWN]=EventKeyboard::KeyCode::KEY_KP_DOWN;
	//keyCodeMap[VK_KP_PG_DOWN]=EventKeyboard::KeyCode::KEY_KP_PG_DOWN;
	//keyCodeMap[VK_KP_INSERT]=EventKeyboard::KeyCode::KEY_KP_INSERT;
	//keyCodeMap[VK_KP_DELETE]=EventKeyboard::KeyCode::KEY_KP_DELETE;
	keyCodeMap[VK_F1]=EventKeyboard::KeyCode::KEY_F1;
	keyCodeMap[VK_F2]=EventKeyboard::KeyCode::KEY_F2;
	keyCodeMap[VK_F3]=EventKeyboard::KeyCode::KEY_F3;
	keyCodeMap[VK_F4]=EventKeyboard::KeyCode::KEY_F4;
	keyCodeMap[VK_F5]=EventKeyboard::KeyCode::KEY_F5;
	keyCodeMap[VK_F6]=EventKeyboard::KeyCode::KEY_F6;
	keyCodeMap[VK_F7]=EventKeyboard::KeyCode::KEY_F7;
	keyCodeMap[VK_F8]=EventKeyboard::KeyCode::KEY_F8;
	keyCodeMap[VK_F9]=EventKeyboard::KeyCode::KEY_F9;
	keyCodeMap[VK_F10]=EventKeyboard::KeyCode::KEY_F10;
	keyCodeMap[VK_F11]=EventKeyboard::KeyCode::KEY_F11;
	keyCodeMap[VK_F12]=EventKeyboard::KeyCode::KEY_F12;
	keyCodeMap[VK_SPACE]=EventKeyboard::KeyCode::KEY_SPACE;
	//keyCodeMap[VK_EXCLAM]=EventKeyboard::KeyCode::KEY_EXCLAM;
	//keyCodeMap[VK_QUOTE]=EventKeyboard::KeyCode::KEY_QUOTE;
	//keyCodeMap[VK_NUMBER]=EventKeyboard::KeyCode::KEY_NUMBER;
	//keyCodeMap[VK_DOLLAR]=EventKeyboard::KeyCode::KEY_DOLLAR;
	//keyCodeMap[VK_PERCENT]=EventKeyboard::KeyCode::KEY_PERCENT;
	//keyCodeMap[VK_CIRCUMFLEX]=EventKeyboard::KeyCode::KEY_CIRCUMFLEX;
	//keyCodeMap[VK_AMPERSAND]=EventKeyboard::KeyCode::KEY_AMPERSAND;
	//keyCodeMap[VK_APOSTROPHE]=EventKeyboard::KeyCode::KEY_APOSTROPHE;
	//keyCodeMap[VK_LEFT_PARENTHESIS]=EventKeyboard::KeyCode::KEY_LEFT_PARENTHESIS;
	//keyCodeMap[VK_RIGHT_PARENTHESIS]=EventKeyboard::KeyCode::KEY_RIGHT_PARENTHESIS;
	//keyCodeMap[VK_ASTERISK]=EventKeyboard::KeyCode::KEY_ASTERISK;
	//keyCodeMap[VK_PLUS]=EventKeyboard::KeyCode::KEY_PLUS;
	//keyCodeMap[VK_COMMA]=EventKeyboard::KeyCode::KEY_COMMA;
	//keyCodeMap[VK_MINUS]=EventKeyboard::KeyCode::KEY_MINUS;
	//keyCodeMap[VK_PERIOD]=EventKeyboard::KeyCode::KEY_PERIOD;
	//keyCodeMap[VK_SLASH]=EventKeyboard::KeyCode::KEY_SLASH;
	keyCodeMap['0']=EventKeyboard::KeyCode::KEY_0;
	keyCodeMap['1']=EventKeyboard::KeyCode::KEY_1;
	keyCodeMap['2']=EventKeyboard::KeyCode::KEY_2;
	keyCodeMap['3']=EventKeyboard::KeyCode::KEY_3;
	keyCodeMap['4']=EventKeyboard::KeyCode::KEY_4;
	keyCodeMap['5']=EventKeyboard::KeyCode::KEY_5;
	keyCodeMap['6']=EventKeyboard::KeyCode::KEY_6;
	keyCodeMap['7']=EventKeyboard::KeyCode::KEY_7;
	keyCodeMap['8']=EventKeyboard::KeyCode::KEY_8;
	keyCodeMap['9']=EventKeyboard::KeyCode::KEY_9;
	//keyCodeMap[VK_COLON]=EventKeyboard::KeyCode::KEY_COLON;
	//keyCodeMap[VK_SEMICOLON]=EventKeyboard::KeyCode::KEY_SEMICOLON;
	//keyCodeMap[VK_LESS_THAN]=EventKeyboard::KeyCode::KEY_LESS_THAN;
	//keyCodeMap[VK_EQUAL]=EventKeyboard::KeyCode::KEY_EQUAL;
	//keyCodeMap[VK_GREATER_THAN]=EventKeyboard::KeyCode::KEY_GREATER_THAN;
	//keyCodeMap[VK_QUESTION]=EventKeyboard::KeyCode::KEY_QUESTION;
	//keyCodeMap[VK_AT]=EventKeyboard::KeyCode::KEY_AT;
	keyCodeMap[VK_SHIFT&'A']=EventKeyboard::KeyCode::KEY_CAPITAL_A;
	keyCodeMap[VK_SHIFT&'B']=EventKeyboard::KeyCode::KEY_CAPITAL_B;
	keyCodeMap[VK_SHIFT&'C']=EventKeyboard::KeyCode::KEY_CAPITAL_C;
	keyCodeMap[VK_SHIFT&'D']=EventKeyboard::KeyCode::KEY_CAPITAL_D;
	keyCodeMap[VK_SHIFT&'E']=EventKeyboard::KeyCode::KEY_CAPITAL_E;
	keyCodeMap[VK_SHIFT&'F']=EventKeyboard::KeyCode::KEY_CAPITAL_F;
	keyCodeMap[VK_SHIFT&'G']=EventKeyboard::KeyCode::KEY_CAPITAL_G;
	keyCodeMap[VK_SHIFT&'H']=EventKeyboard::KeyCode::KEY_CAPITAL_H;
	keyCodeMap[VK_SHIFT&'I']=EventKeyboard::KeyCode::KEY_CAPITAL_I;
	keyCodeMap[VK_SHIFT&'J']=EventKeyboard::KeyCode::KEY_CAPITAL_J;
	keyCodeMap[VK_SHIFT&'K']=EventKeyboard::KeyCode::KEY_CAPITAL_K;
	keyCodeMap[VK_SHIFT&'L']=EventKeyboard::KeyCode::KEY_CAPITAL_L;
	keyCodeMap[VK_SHIFT&'M']=EventKeyboard::KeyCode::KEY_CAPITAL_M;
	keyCodeMap[VK_SHIFT&'N']=EventKeyboard::KeyCode::KEY_CAPITAL_N;
	keyCodeMap[VK_SHIFT&'O']=EventKeyboard::KeyCode::KEY_CAPITAL_O;
	keyCodeMap[VK_SHIFT&'P']=EventKeyboard::KeyCode::KEY_CAPITAL_P;
	keyCodeMap[VK_SHIFT&'Q']=EventKeyboard::KeyCode::KEY_CAPITAL_Q;
	keyCodeMap[VK_SHIFT&'R']=EventKeyboard::KeyCode::KEY_CAPITAL_R;
	keyCodeMap[VK_SHIFT&'S']=EventKeyboard::KeyCode::KEY_CAPITAL_S;
	keyCodeMap[VK_SHIFT&'T']=EventKeyboard::KeyCode::KEY_CAPITAL_T;
	keyCodeMap[VK_SHIFT&'U']=EventKeyboard::KeyCode::KEY_CAPITAL_U;
	keyCodeMap[VK_SHIFT&'V']=EventKeyboard::KeyCode::KEY_CAPITAL_V;
	keyCodeMap[VK_SHIFT&'W']=EventKeyboard::KeyCode::KEY_CAPITAL_W;
	keyCodeMap[VK_SHIFT&'X']=EventKeyboard::KeyCode::KEY_CAPITAL_X;
	keyCodeMap[VK_SHIFT&'Y']=EventKeyboard::KeyCode::KEY_CAPITAL_Y;
	keyCodeMap[VK_SHIFT&'Z']=EventKeyboard::KeyCode::KEY_CAPITAL_Z;
	//keyCodeMap[VK_LEFT_BRACKET]=EventKeyboard::KeyCode::KEY_LEFT_BRACKET;
	//keyCodeMap[VK_BACK_SLASH]=EventKeyboard::KeyCode::KEY_BACK_SLASH;
	//keyCodeMap[VK_RIGHT_BRACKET]=EventKeyboard::KeyCode::KEY_RIGHT_BRACKET;
	//keyCodeMap[VK_UNDERSCORE]=EventKeyboard::KeyCode::KEY_UNDERSCORE;
	//keyCodeMap[VK_GRAVE]=EventKeyboard::KeyCode::KEY_GRAVE;
	keyCodeMap['A']=EventKeyboard::KeyCode::KEY_A;
	keyCodeMap['B']=EventKeyboard::KeyCode::KEY_B;
	keyCodeMap['C']=EventKeyboard::KeyCode::KEY_C;
	keyCodeMap['D']=EventKeyboard::KeyCode::KEY_D;
	keyCodeMap['E']=EventKeyboard::KeyCode::KEY_E;
	keyCodeMap['F']=EventKeyboard::KeyCode::KEY_F;
	keyCodeMap['G']=EventKeyboard::KeyCode::KEY_G;
	keyCodeMap['H']=EventKeyboard::KeyCode::KEY_H;
	keyCodeMap['I']=EventKeyboard::KeyCode::KEY_I;
	keyCodeMap['J']=EventKeyboard::KeyCode::KEY_J;
	keyCodeMap['K']=EventKeyboard::KeyCode::KEY_K;
	keyCodeMap['L']=EventKeyboard::KeyCode::KEY_L;
	keyCodeMap['M']=EventKeyboard::KeyCode::KEY_M;
	keyCodeMap['N']=EventKeyboard::KeyCode::KEY_N;
	keyCodeMap['O']=EventKeyboard::KeyCode::KEY_O;
	keyCodeMap['P']=EventKeyboard::KeyCode::KEY_P;
	keyCodeMap['Q']=EventKeyboard::KeyCode::KEY_Q;
	keyCodeMap['R']=EventKeyboard::KeyCode::KEY_R;
	keyCodeMap['S']=EventKeyboard::KeyCode::KEY_S;
	keyCodeMap['T']=EventKeyboard::KeyCode::KEY_T;
	keyCodeMap['U']=EventKeyboard::KeyCode::KEY_U;
	keyCodeMap['V']=EventKeyboard::KeyCode::KEY_V;
	keyCodeMap['W']=EventKeyboard::KeyCode::KEY_W;
	keyCodeMap['X']=EventKeyboard::KeyCode::KEY_X;
	keyCodeMap['Y']=EventKeyboard::KeyCode::KEY_Y;
	keyCodeMap['Z']=EventKeyboard::KeyCode::KEY_Z;
	//keyCodeMap[VK_LEFT_BRACE]=EventKeyboard::KeyCode::KEY_LEFT_BRACE;
	//keyCodeMap[VK_BAR]=EventKeyboard::KeyCode::KEY_BAR;
	//keyCodeMap[VK_RIGHT_BRACE]=EventKeyboard::KeyCode::KEY_RIGHT_BRACE;
	//keyCodeMap[VK_TILDE]=EventKeyboard::KeyCode::KEY_TILDE;
	//keyCodeMap[VK_EURO]=EventKeyboard::KeyCode::KEY_EURO;
	//keyCodeMap[VK_POUND]=EventKeyboard::KeyCode::KEY_POUND;
	//keyCodeMap[VK_YEN]=EventKeyboard::KeyCode::KEY_YEN;
	//keyCodeMap[VK_MIDDLE_DOT]=EventKeyboard::KeyCode::KEY_MIDDLE_DOT;
	//keyCodeMap[VK_SEARCH]=EventKeyboard::KeyCode::KEY_SEARCH;
	//keyCodeMap[VK_DPAD_LEFT]=EventKeyboard::KeyCode::KEY_DPAD_LEFT;
	//keyCodeMap[VK_DPAD_RIGHT]=EventKeyboard::KeyCode::KEY_DPAD_RIGHT;
	//keyCodeMap[VK_DPAD_UP]=EventKeyboard::KeyCode::KEY_DPAD_UP;
	//keyCodeMap[VK_DPAD_DOWN]=EventKeyboard::KeyCode::KEY_DPAD_DOWN;
	//keyCodeMap[VK_DPAD_CENTER]=EventKeyboard::KeyCode::KEY_DPAD_CENTER;
	//keyCodeMap[VK_ENTER]=EventKeyboard::KeyCode::KEY_ENTER;
	keyCodeMap[VK_PLAY]=EventKeyboard::KeyCode::KEY_PLAY;
}

///////////////////////////////消息处理///////////////////////////////////////////
BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct){
	//根据用户区域调整窗口的大小
	RECT adjustRect;
	GetWindowRect(hwnd, &adjustRect);//{ rect.origin.x, rect.origin.y, rect.size.width, rect.size.height };
	AdjustWindowRectEx(&adjustRect, GetWindowStyle(hwnd), GetMenu(hwnd) != nullptr, GetWindowExStyle(hwnd));
	// x, y不能使用CW_USERDEFAULT，否则窗口会无法显示，因为CW_USERDEFAULT的值是0x800000，估计窗口被移动到很远的地方去了
	MoveWindow(hwnd, 100, 100, adjustRect.right - adjustRect.left, adjustRect.bottom - adjustRect.top, true);
	return true;
}

void Cls_OnClose(HWND hwnd){
	//int returnType = MessageBox(hwnd, TEXT("Do you want to close the window"), TEXT("Warning"), MB_YESNO);
	//if (returnType == IDYES){
	SendMessage(hwnd, WM_DESTROY, 0, 0);
	//}
}

void Cls_OnDestroy(HWND hwnd){
	PostQuitMessage(0);
}

void Cls_OnPaint(HWND hwnd){
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	EndPaint(hwnd, &ps);
}

void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags){
	eventTouch.touchCode=EventTouch::TouchCode::MOVED;
	touch.point=Vec3f((float)x,(float)y,0);
	DISPATCHER->dispatchTouchEvent(&eventTouch);
}

void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags){
	eventTouch.touchCode=EventTouch::TouchCode::BEGAN;
	touch.point=Vec3f((float)x,(float)y,0);
	DISPATCHER->dispatchTouchEvent(&eventTouch);
}

void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags){
	eventTouch.touchCode=EventTouch::TouchCode::ENDED;
	touch.point=Vec3f((float)x,(float)y,0);
	DISPATCHER->dispatchTouchEvent(&eventTouch);
}

void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags){
	eventKeyboard.keyCode=keyCodeMap[vk];
	eventKeyboard.isPress=fDown==0?false:true;
	DISPATCHER->dispatchKeyboardEvent(&eventKeyboard);
}

void Cls_OnMove(HWND hwnd, int x, int y){
	Director::getInstance()->updateWindowPos();
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//return DefWindowProc(hwnd, message, wParam, lParam);
	
	switch (message){
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_CLOSE, Cls_OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, Cls_OnMouseMove);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, Cls_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, Cls_OnLButtonUp);
		HANDLE_MSG(hwnd, WM_KEYDOWN, Cls_OnKey);
		HANDLE_MSG(hwnd, WM_KEYUP, Cls_OnKey);
		HANDLE_MSG(hwnd, WM_MOVE, Cls_OnMove);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////

WindowView::WindowView():
_hwnd(nullptr),
_hInstance(nullptr),
_screenSize(0,0),
_resolutionSize(0,0),
_policy(ResolutionPolicy::NO_BORDER),
_shouldClose(false)
{

}

WindowView::~WindowView(){

}

WindowView* WindowView::create(const char* name, Rect& rect)
{
	auto view = new WindowView;
	if (view->initWithRect(name, rect)){
		view->setFrameSize(Size(rect.size));
		view->autoRelease();
		return view;
	}
	else{
		delete view;
		return nullptr;
	}
}

bool WindowView::windowShouldClose(){
	return _shouldClose;
}

bool WindowView::handleEvents(){
	if (KEYDOWN(VK_ESCAPE)){
		SendMessage((HWND)_hwnd, WM_CLOSE, 0, 0);
	}
	static MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
		if (msg.message == WM_QUIT){
			_shouldClose = true;
			return true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}
	return false;
}

bool WindowView::initWithRect(const char* name, Rect& rect)
{
	//初始化触摸与按键事件
	initEvent();

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	_hInstance = GetModuleHandle(nullptr);
	HINSTANCE hInstance = (HINSTANCE)_hInstance;
	wc.hInstance = hInstance;
	wc.style = CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;
	//从WNDCLASS换成WNDCLASSEX时，要记得填充这个字段，不然会造成注册窗口失败的问题
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = WIN_CLASS_NAME;
	//如果没有这句，整个窗口的显示效果是源来残留在显存中的像素数据，看上去像截图一样
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//光标，如果没有这句，当鼠标从边界移动到窗口区域时，鼠标的状态不会刷新
	//the first param must be not nullptr,
	//so LoadCursor function can find the resource
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(Common_Cur));
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(Common_Icon));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(Common_Icon));
	wc.lpszMenuName = 0;

	if (!RegisterClassEx(&wc)){
		Error("RegisterClass failed");
		return false;
	}

	DWORD style = 0;
	if (Director::getInstance()->getGameMode() == MODE_FULLSCREEN){
		style = STYLE_FULLSCREEN;
	}
	else{
		style = STYLE_WINDOW;
	}
	_hwnd = CreateWindowEx(0, WIN_CLASS_NAME, name, style, (int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height, 0, 0, hInstance, 0);
	if (!_hwnd){
		Error("hwnd is nullptr");
		return false;
	}

	//ShowWindow((HWND)_hwnd, 0xa);
	//UpdateWindow((HWND)_hwnd);

	return true;
}

Size WindowView::getDesignResolutionSize(){
	return _resolutionSize;
}

Size WindowView::getFrameSize(){
	return _screenSize;
}

void WindowView::setFrameSize(Size& size){
	_resolutionSize = _screenSize = size;
}

void WindowView::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy){
	_resolutionSize.width = width;
	_resolutionSize.height = height;
	_policy = resolutionPolicy;
}

NS_ENGIN_END