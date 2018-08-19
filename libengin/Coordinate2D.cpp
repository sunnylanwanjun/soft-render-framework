#include "Coordinate2D.h"
#include "Director.h"
#include "Renderer.h"
NS_ENGIN_BEGIN
Coordinate2D::Coordinate2D():color(0x00ffffff){
	
}

Coordinate2D::~Coordinate2D(){

}

void Coordinate2D::draw( Renderer* renderer, Mat4& transform ){
	
	
	
	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}
	
	const RangeFIXP16& wndRange = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin = Director::getInstance()->getFIXP16WindowOrigin();
	const Size& size = Director::getInstance()->getWindowSize();

	p0.x=0;p1.x=size.width;
	for(int i=0;i<row;i++){
		p0.y=size.height/2-i*_scaleY;
		p1.y=p0.y;
		DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, p0.v2f(), p1.v2f());	
	}
	for(int i=1;i<row;i++){
		p0.y=size.height/2+i*_scaleY;
		p1.y=p0.y;
		DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, p0.v2f(), p1.v2f());	
	}
	p0.y=0;p1.y=size.height;
	for(int i=0;i<col;i++){
		p0.x=size.width/2-i*_scaleX;
		p1.x=p0.x;
		DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, p0.v2f(), p1.v2f());	
	}
	for(int i=1;i<col;i++){
		p0.x=size.width/2+i*_scaleX;
		p1.x=p0.x;
		DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, p0.v2f(), p1.v2f());	
	}
	DrawPolygon2D_Fill((DWORD*)ddsd.lpSurface, 
		ddsd.lPitch >> 2, 
		wndOrigin, 
		wndRange, 
		color, 4, originvList);
	lpdds->Unlock(nullptr);
}

bool Coordinate2D::init(){
	Size size = Director::getInstance()->getWindowSize();
	row=(int)ceil(size.height/_scaleY/2);
	col=(int)ceil(size.width/_scaleX/2);
	originvList[0]=Vec3f(size.width/2-5,size.height/2-5);
	originvList[1]=Vec3f(originvList[0].x+10,originvList[0].y);
	originvList[2]=Vec3f(originvList[0].x+10,originvList[0].y+10);
	originvList[3]=Vec3f(originvList[0].x,originvList[0].y+10);
	return true;
}

Coordinate2D* Coordinate2D::create( float scale/*=1.0f*/ ){
	Coordinate2D* coor = new Coordinate2D;
	coor->autoRelease();
	coor->setScale(scale);
	return coor;
}

NS_ENGIN_END