#include "DrawFunc.h"
#include "Director.h"
#include "Renderer.h"
NS_ENGIN_BEGIN
DrawFunc::DrawFunc():color(0x0000ff00),_setMinX(false),_setMaxX(false){
}

DrawFunc::~DrawFunc(){

}

DrawFunc* DrawFunc::create( DRAWFUNC func,float scale){
	DrawFunc* drawFunc = new DrawFunc;
	drawFunc->autoRelease();
	drawFunc->_func=func;
	drawFunc->setScale(scale);
	return drawFunc;
}

bool DrawFunc::init(){
	Size size=Director::getInstance()->getWindowSize();
	//放大最小与最大值
	if(_setMinX){
		_minX=(int)(_minX*_scaleX);
	}else{
		_minX=(int)-size.width/2;
	}
	if(_setMaxX){
		_maxX=(int)(_maxX*_scaleX);
	}else{
		_maxX=(int)(_minX+size.width);
	}
	
	if(_maxX-_minX>size.width){
		_maxX=(int)(_minX+size.width);
	}

	float halfH=size.height/2;
	float tx=(float)_minX;
	float ty;
	float nty,mty;
	bool needSlerp=false;//是否需要插值，因为x轴连续，但y不连续,插值用到了
	//中点法，就是在F(x+1)=Y1值与F(x)=Y0值进行比较，如果Y不连续，就求
	//F(x-0.5)=MiddleY，用Y0->MiddleY这个过程，取X值，MiddleY->Y1取X+1的值
	/*缩放原理，如果不进行放大，那么显示出来的图形，有可能很小，
	放大时，增加1个像素，相当于x轴移动1/_scaleX，而对应F(1/_scaleX)也应放大_scaleY
	倍，相当于把X轴和Y轴都进行了拉伸，也就是(x,y)对应(x*scaleX,y*scaleY)，但要注意
	放大不是等价，也就是F(x)=y并不一定F(kx)=ky，因为放大是对所以的二维函数都适应的
	而符合上式的必须得是线性函数
	*/
	Vec2i tp;
	while(tx<=_maxX){
		//在传参时，必须缩小到原始值
		bool isValid=_func(tx/_scaleX,nty);
		//有时对于函数X可能没有意义，如1/x，当x=0时，倒数不存在
		if(!isValid){
			//这个时候，不需要插值
			needSlerp=false;
			tx++;
			continue;
		}
		nty*=_scaleY;
		//过滤超出屏幕显示的Y值
		if(ABS(nty)>halfH){
			nty=nty>0?halfH+1:-halfH-1;
		}
		//插值
		if(needSlerp){
			_func((tx-0.6f)/_scaleX,mty);
			mty*=_scaleY;
			int dir=CMP(nty,ty);
			while(ABS(nty-ty)>1){
				if(dir*(ty-mty)<0){
					tp.x=(int)(tx-1-_minX);
				}else{
					tp.x=(int)(tx-_minX);
				}
				tp.y=(int)(-ty+halfH);
				_vecs.push_back(tp);
				ty=ty+dir;
			}
		}
		needSlerp=true;
		tp.x=(int)(tx-_minX);
		tp.y=(int)(-nty+halfH);		
		_vecs.push_back(tp);
		ty=nty;
		tx++;
	}
	return true;
}

void DrawFunc::draw( Renderer* renderer, Mat4& transform ){

	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const Range& wndRange = Director::getInstance()->getClipRange();
	const Vec2f& wndOrigin = Director::getInstance()->getWindowOrigin();
	int ox=(int)wndOrigin.x;
	int oy=(int)wndOrigin.y;
	for(size_t i=0;i<_vecs.size();i++){
		int x=_vecs[i].x+ox;
		int y=_vecs[i].y+oy;
		if(InRange((float)x,(float)y,wndRange))
			Plot32(x,y,color,(DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2);
	}

	lpdds->Unlock(nullptr);
}

//根据原始比率设置X的最小值
void DrawFunc::setMinX( int min ){
	_setMinX=true;
	_minX=min;
}

//根据原始比率设置X的最大值
void DrawFunc::setMaxX( int max ){
	_setMaxX=true;
	_maxX=max;
}



NS_ENGIN_END