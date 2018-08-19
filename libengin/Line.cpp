#include "Line.h"
#include "t3dlib1.h"
#include "Renderer.h"
#include "Director.h"
NS_ENGIN_BEGIN
Line::Line():p0(0.0f,0.0f,0.0f),p1(0.0f,0.0f,0.0f){

}

Line::~Line(){

}

Line* Line::create(Vec3f& pos, Vec3f& p0, Vec3f& p1, DWORD color){
	Line* line = new Line;
	line->_pos = pos;
	line->p0 = p0;
	line->p1 = p1;
	line->color = color;
	line->autoRelease();
	return line;
}

void Line::draw(Renderer* renderer, Mat4& transform){

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	Mat4_Mul(p0,transform,gp0);
	Mat4_Mul(p1,transform,gp1);
	v.x = gp1.x-gp0.x;
	v.y = gp1.y-gp0.y;
	v.z = gp1.z-gp0.z;

	const RangeFIXP16& wndRange = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin = Director::getInstance()->getFIXP16WindowOrigin();

	DrawLine((DWORD*)ddsd.lpSurface, 
		      ddsd.lPitch >> 2, 
			  wndOrigin, 
			  wndRange, 
			  color, 
			  gp0.v2f(), 
			  gp1.v2f());
	/*DrawGouraudLine((DWORD*)ddsd.lpSurface, 
					ddsd.lPitch >> 2, 
					wndOrigin, 
					wndRange,
					gp0.v2f(), 
					gp1.v2f(),
					0x00ff0000,
					0x0000ff00);*/
	lpdds->Unlock(nullptr);
}

NS_ENGIN_END