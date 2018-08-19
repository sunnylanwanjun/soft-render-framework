#include "Stars.h"
#include "raidersDef.h"
USING_ENGIN_NS
extern float halfWidth;
extern float halfHeight;
Stars::Stars(){
	const Size& size=Director::getInstance()->getWindowSize();
	//halfWidth=size.width*0.5f;
	//halfHeight=size.height*0.5f;
	for(int i=0;i<NUM_STARS;i++){
		stars[i].x=(rand()%(int)(size.width)-halfWidth)*60;
		stars[i].y=(rand()%(int)(size.height)-halfHeight)*60;
		stars[i].z=NEAR_Z+rand()%int(FAR_Z-NEAR_Z);
	}
}

Stars::~Stars(){

}

void Stars::draw( engin::Renderer* renderer, engin::Mat4& transform ){
	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const Range& wndRange = Director::getInstance()->getClipRange();
	const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
	static DWORD color=RGB32BIT(0,0xff,0xff,0xff);
	for(int i=0;i<NUM_STARS;i++){
		stars[i].z-=PLAYER_SPEED;
		if(stars[i].z<=NEAR_Z)
			stars[i].z=FAR_Z;
		int x=int(NEAR_Z*stars[i].x/stars[i].z+halfWidth+wndOrigin.x);
		int y=int(-NEAR_Z*stars[i].y/stars[i].z+halfHeight+wndOrigin.y);
		if(InRange(x,y,wndRange))
			Plot32(x,y,color,(DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2);
	}
	lpdds->Unlock(nullptr);
}
