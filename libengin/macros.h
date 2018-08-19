#ifndef __MACROS_H
#define __MACROS_H
#pragma warning( disable: 4251 )
#define __DEBUG

#if defined(_USE_ENGIN_DLL)
	#define ENGIN_DLL     __declspec(dllimport)
#else         /* use a DLL library */
    #define ENGIN_DLL     __declspec(dllexport)
#endif

#define NS_ENGIN_BEGIN namespace engin {
#define NS_ENGIN_END   }
#define USING_ENGIN_NS using namespace engin;

#define PLATFORM_UNKNOWN            0
#define PLATFORM_IOS                1
#define PLATFORM_ANDROID            2
#define PLATFORM_WIN32              3
// Determine target platform by compile environment macro.
#ifdef _WIN32
#include <assert.h>
#include <stdarg.h>
#define TARGET_PLATFORM PLATFORM_WIN32
#else
#define TARGET_PLATFORM             PLATFORM_UNKNOWN
#endif
#define USE_D3D
#define INVALID_INDEX -1

typedef long LONG;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        size_t;
typedef unsigned int		uint;

//MACROS API
#define RGB16BIT555(r,g,b) ((((r)&0x1f)<<10)+(((g)&0x1f)<<5)+((b)&0x1f))
#define RGB16BIT565(r,g,b) (((((r)&0x1f))<<11)+(((g)&0x3f)<<5)+((b)&0x1f))
#define RGB32BIT(a,r,g,b) ((((a)&0xff)<<24)+((((r)&0xff))<<16)+(((g)&0xff)<<8)+((b)&0xff))
#define RGBA32BIT(r,g,b,a) ((((r)&0xff)<<24)+(((g)&0xff)<<16)+(((b)&0xff)<<8)+((a)&0xff))

#define WHITE(c)  (c.rgba=RGBA32BIT(255,255,255,0))
#define GRAY(c)   (c.rgba=RGBA32BIT(100,100,100,0))
#define BLACK(c)  (c.rgba=RGBA32BIT(0,0,0,0))
#define RED(c)    (c.rgba=RGBA32BIT(255,0,0,0))
#define GREEN(c)  (c.rgba=RGBA32BIT(0,255,0,0))
#define BLUE(c)   (c.rgba=RGBA32BIT(0,0,255,0))

#define CEIL(n) ((n)<0?int(n):(int(n)<n?int(n)+1:int(n)))
#define FLOOR(n) ((n)>0?int(n):(int(n)>n?int(n)-1:int(n)))

#define RGB32BIT_R(c) (((c)&0x00ff0000)>>16)
#define RGB32BIT_G(c) (((c)&0x0000ff00)>>8)
#define RGB32BIT_B(c) (((c)&0x000000ff))
#define LIMIT(c,up) ((c)>(up)?(up):(c))
#define RGB32BIT_ADD(c0,c1) \
	(\
     (LIMIT((RGB32BIT_R(c0)+RGB32BIT_R(c1)),0xff)<<16)+\
	 (LIMIT((RGB32BIT_G(c0)+RGB32BIT_G(c1)),0xff)<< 8)+\
	 (LIMIT((RGB32BIT_B(c0)+RGB32BIT_B(c1)),0xff))\
    )
#define RGB32BIT_ADD_COLOR(c,r,g,b) \
	(\
	(LIMIT(DWORD(RGB32BIT_R(c)+r),0xff)<<16)+\
	(LIMIT(DWORD(RGB32BIT_G(c)+g),0xff)<< 8)+\
	(LIMIT(DWORD(RGB32BIT_B(c)+b),0xff))\
	)
#define RGB32BIT_K(c,k)\
	(\
	(LIMIT(DWORD(RGB32BIT_R(c)*k),0xff)<<16)+\
	(LIMIT(DWORD(RGB32BIT_G(c)*k),0xff)<< 8)+\
	(LIMIT(DWORD(RGB32BIT_B(c)*k),0xff))\
	)
#define RGB32BIT_MUL(c0,c1)\
	(\
	(LIMIT((RGB32BIT_R(c0)*RGB32BIT_R(c1)),0xff)<<16)+\
	(LIMIT((RGB32BIT_G(c0)*RGB32BIT_G(c1)),0xff)<< 8)+\
	(LIMIT((RGB32BIT_B(c0)*RGB32BIT_B(c1)),0xff))\
	)
//RGB中的一种颜色是否达到最大值
#define ISRGB32BIT_FULL(c)\
	(\
	(RGB32BIT_R(c)>=0xff)||\
	(RGB32BIT_G(c)>=0xff)||\
	(RGB32BIT_B(c)>=0xff)\
    )
#define GDISPATCH Director::getInstance()->getEventDispatcher()
#define DDRAW_INIT_STRUCT(st) {ZeroMemory(&st,sizeof st);st.dwSize=sizeof st;}
#define KEYDOWN(vk_code) (GetAsyncKeyState(vk_code)&0x8000?1:0)
#define KEYUP(vk_code) (GetAsyncKeyState(vk_code)&0x8000?0:1)
#define RButton(buttonVal) (buttonVal&MK_RBUTTON?1:0)
#define LButton(buttonVal) (buttonVal&MK_LBUTTON?1:0)
#define IsLButtonDown()  (GetKeyState(VK_LBUTTON) < 0)
#define IsRButtonDown()  (GetKeyState(VK_RBUTTON) < 0)
#define IsMButtonDown()  (GetKeyState(VK_MBUTTON) < 0)
#define MAX(num0,num1) (num0>num1?num0:num1)
#define MIN(num0,num1) (num0<num1?num0:num1)
#define CMP(num0,num1) ((num0)>=(num1)?1:-1)
#define ABS(num) ((num)>0?(num):-(num))
#define SWAP(temp,t0,t1) temp=t0;t0=t1;t1=temp;
#define RGBA2ARGB(rgba) ((rgba>>8)|(rgba<<24))
#define ARGB2RGBA(argb) ((argb<<8)|(argb>>24))
#define WRAP_BYTE(num) ((num)<0?0:((num)>255?255:(num)))

#define MATCH_INT(i) (atoi(((std::string)matches[i]).c_str()))
#define MATCH_FLOAT(i) ((float)atof(((std::string)matches[i]).c_str()))
#define MATCH_STR(i) (((std::string)matches[i]).c_str())

#define E4 ((float)1e-4)
#define E5 ((float)1e-5)
#define E6 ((float)1e-6)
//常用常量
#define dPI 3.141592653f       //PI
#define d2PI 6.283185306f      //PI*2
#define dPI2 1.5707963265f     //PI/2
#define dPI4 0.78539816325f    //PI/4
#define d1DIVPI 0.3183098862f  //1/PI
#define d1DIV2PI 0.1591549431f //1/2PI
#define ZEROVEC Vec3f(0,0,0)
#define NORMVEC Vec3f(1,1,1)

//闭区间
#define Rand_Range(n,m) (n+rand()%(m-(n)+1))
#define Deg2Rad(d) (d*dPI/180)
#define Rad2Deg(r) (r*180*d1DIVPI)

typedef int FIXP16;

#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
//四舍五入
//(1-0.5)*65536
#define FIXP16_ROUND_UP  0x00008000 
#define FIXP16_1         0x00010000
#define FIXP16_F1        0xffff0000

#define INT_2_FIXP16(val) ((val)<<FIXP16_SHIFT)
#define FLOAT_2_FIXP16(val) ((FIXP16)((val)*FIXP16_MAG+0.5))
#define FIXP16_2_INT(val) ((val)>>FIXP16_SHIFT)
#define FIXP16_2_FLOAT(val) ((float)(val)/FIXP16_MAG)
#define FIXP16_2_WP(val) ((val)>>FIXP16_SHIFT)
#define FIXP16_2_DP(val) ((val)&FIXP16_DP_MASK)
#define FIXP16_FLOOR(val) ((val)&FIXP16_WP_MASK)
#define FIXP16_ROUND(val) (((val)+FIXP16_ROUND_UP)>>FIXP16_SHIFT)
#define FIXP16_OVER(val) (ABS(val)>32767.0f)
#define FIXP16_INVERSE_OVER(n) (ABS(n)<0x00000003)
#define FIXP16_XYOVER(vec) (FIXP16_OVER(vec.x)||FIXP16_OVER(vec.y))

#define ASSERT(cond,msg) assert(cond)

#define SETBIT(dst,bit) (dst=dst|bit)
#define RESETBIT(dst,bit) (dst=(dst&(~bit)))

NS_ENGIN_BEGIN

struct timeval
{
	long long tv_sec;		// seconds
	long long tv_usec;    // microSeconds
};

enum GAME_MODE{
	MODE_WND,
	MODE_FULLSCREEN,
};

// 除了 std::placeholders::_1 其它是在绑定期就遇先保存起来，到后来
// 真正调用callback时，再传递给函数的参数，而std::placeholders::_1是
// 在真正调用callback时，再显示传递的参数
#define CALL_BACK_0(callback,thisObj,...) \
	std::bind(&callback,thisObj,##__VA_ARGS__)
#define CALL_BACK_1(callback,thisObj,...) \
	std::bind(&callback,thisObj,std::placeholders::_1,##__VA_ARGS__)
#define CALL_BACK_2(callback,thisObj,...) \
	std::bind(&callback,thisObj,std::placeholders::_1,std::placeholders::_2,##__VA_ARGS__)
#define CALL_BACK_3(callback,thisObj,...) \
	std::bind(&callback,thisObj,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,##__VA_ARGS__)

NS_ENGIN_END

#endif
