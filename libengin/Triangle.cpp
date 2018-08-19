#include "Triangle.h"
#include "Renderer.h"
#include "Director.h"
#include "Material.h"
NS_ENGIN_BEGIN

Triangle::Triangle():p0(0,0),p1(0,0),p2(0,0),
gp0(0,0),gp1(0,0),gp2(0,0){

}

Triangle::~Triangle(){

}

Triangle* Triangle::create( Vec3f pos, Vec3f p0, Vec3f p1, Vec3f p2, DWORD color ){
	Triangle* triangle = new Triangle;
	triangle->p0 = p0;
	triangle->p1 = p1;
	triangle->p2 = p2;
	triangle->color = color;
	triangle->_pos = pos;
	triangle->autoRelease();
	return triangle;
}

void Triangle::draw( Renderer* renderer, Mat4& transform ){

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const Range& wndRange = Director::getInstance()->getClipRange();
	const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
	const RangeFIXP16& wndRange_FIX = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin_FIX = Director::getInstance()->getFIXP16WindowOrigin();

	Mat4_Mul(p0,transform,gp0);
	Mat4_Mul(p1,transform,gp1);
	Mat4_Mul(p2,transform,gp2);

	DrawTriangle((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin_FIX, wndRange_FIX, color, gp0.v2f(), gp1.v2f(), gp2.v2f());
	lpdds->Unlock(nullptr);
}

NS_ENGIN_END