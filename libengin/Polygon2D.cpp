#include "Polygon2D.h"
#include "Renderer.h"
#include "Director.h"
#include "mathUtil.h"
NS_ENGIN_BEGIN

Polygon2D::Polygon2D() :_isFill(true),numVer(0), _originVertexList(nullptr), color(0), globalVertexList(nullptr) /*_localVertexList(nullptr)*/{

}

Polygon2D::~Polygon2D(){
	delete[] _originVertexList;
	_originVertexList = nullptr;
	delete[] globalVertexList;
	globalVertexList = nullptr;
	//delete[] _localVertexList;
	//_localVertexList = nullptr;
	//delete[] _radiusList;
	//_radiusList = nullptr;
}

Polygon2D* Polygon2D::create(Vec3f& pos, int numVer, Vec3f* vList, DWORD color, bool isFill){
	Polygon2D* polygon2D = new Polygon2D;	
	polygon2D->_isFill = isFill;
	polygon2D->numVer = numVer;
	polygon2D->_originVertexList = new Vec3f[numVer];
	memcpy(polygon2D->_originVertexList,vList,numVer*sizeof Vec3f);
	//polygon2D->_localVertexList = new Vec3i[numVer];
	//memcpy(polygon2D->_localVertexList, vList, numVer*sizeof Vec3i);
	polygon2D->globalVertexList = new Vec3f[numVer];
	memset(polygon2D->globalVertexList,0,numVer*sizeof Vec3f);
	//polygon2D->_radiusList = new float[numVer];
	//for (int i = 0; i < numVer; i++){
	//	float r = vList[i].x * vList[i].x + vList[i].y * vList[i].y;
	//	polygon2D->_radiusList[i] = sqrt(r);
	//}

	polygon2D->color = color;
	polygon2D->setPosition(pos);
	polygon2D->autoRelease();
	return polygon2D;
}

void Polygon2D::draw(Renderer* renderer, Mat4& transform){

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const RangeFIXP16& wndRange = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin = Director::getInstance()->getFIXP16WindowOrigin();

	for (int i = 0; i < numVer; i++){
		Mat4_Mul(_originVertexList[i],transform,globalVertexList[i]);
	}

	if (_isFill){
		DrawPolygon2D_Fill((DWORD*)ddsd.lpSurface, 
			ddsd.lPitch >> 2, 
			wndOrigin, 
			wndRange, 
			color, 
			numVer, globalVertexList);
	}
	else{
		DrawPolygon2D_Line((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, numVer, globalVertexList);
	}

#ifdef SHOW_BOUNDING_BOX
	BoundingBox(globalVertexList,numVer,_bBox);
	_bList[0].x=_bBox.left;
	_bList[0].y=_bBox.top;
	_bList[1].x=_bBox.right;
	_bList[1].y=_bBox.top;
	_bList[2].x=_bBox.right;
	_bList[2].y=_bBox.bottom;
	_bList[3].x=_bBox.left;
	_bList[3].y=_bBox.bottom;
	DrawPolygon2D_Line((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, color, 4, _bList);
#endif
	lpdds->Unlock(nullptr);
}

/*
void Polygon2D::setPosition(const Vec2& pos){
	Node::setPosition(pos);
	for (int i = 0; i < _numVer; i++){
		_globalVertexList[i] = _localVertexList[i] + _pos;
	}
}

void Polygon2D::setRotation(float rotation){
	Node::setRotation(rotation);
	for (int i = 0; i < _numVer; i++){
		_localVertexList[i].x = (int)(_originVertexList[i].x*_scaleX*cos(_rotation) - _originVertexList[i].y*_scaleY*sin(_rotation));
		_localVertexList[i].y = (int)(_originVertexList[i].x*_scaleX*sin(_rotation) + _originVertexList[i].y*_scaleY*cos(_rotation));
	}
	setPosition(_pos);
}

void Polygon2D::setScale(float scale){
	Node::setScale(scale);
	for (int i = 0; i < _numVer; i++){
		_localVertexList[i].x = (int)(_originVertexList[i].x*_scaleX*cos(_rotation) - _originVertexList[i].y*_scaleY*sin(_rotation));
		_localVertexList[i].y = (int)(_originVertexList[i].x*_scaleX*sin(_rotation) + _originVertexList[i].y*_scaleY*cos(_rotation));
	}
	setPosition(_pos);
}

void Polygon2D::setScaleX(float scaleX){
	for (int i = 0; i < _numVer; i++){
		_localVertexList[i].x = (int)(_originVertexList[i].x*_scaleX*cos(_rotation) - _originVertexList[i].y*_scaleY*sin(_rotation));
		_localVertexList[i].y = (int)(_originVertexList[i].x*_scaleX*sin(_rotation) + _originVertexList[i].y*_scaleY*cos(_rotation));
	}
	setPosition(_pos);
}

void Polygon2D::setScaleY(float scaleY){
	for (int i = 0; i < _numVer; i++){
		_localVertexList[i].x = (int)(_originVertexList[i].x*_scaleX*cos(_rotation) - _originVertexList[i].y*_scaleY*sin(_rotation));
		_localVertexList[i].y = (int)(_originVertexList[i].x*_scaleX*sin(_rotation) + _originVertexList[i].y*_scaleY*cos(_rotation));
	}
	setPosition(_pos);
}*/

NS_ENGIN_END