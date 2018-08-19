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
	//�Ŵ���С�����ֵ
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
	bool needSlerp=false;//�Ƿ���Ҫ��ֵ����Ϊx����������y������,��ֵ�õ���
	//�е㷨��������F(x+1)=Y1ֵ��F(x)=Y0ֵ���бȽϣ����Y������������
	//F(x-0.5)=MiddleY����Y0->MiddleY������̣�ȡXֵ��MiddleY->Y1ȡX+1��ֵ
	/*����ԭ����������зŴ���ô��ʾ������ͼ�Σ��п��ܺ�С��
	�Ŵ�ʱ������1�����أ��൱��x���ƶ�1/_scaleX������ӦF(1/_scaleX)ҲӦ�Ŵ�_scaleY
	�����൱�ڰ�X���Y�ᶼ���������죬Ҳ����(x,y)��Ӧ(x*scaleX,y*scaleY)����Ҫע��
	�Ŵ��ǵȼۣ�Ҳ����F(x)=y����һ��F(kx)=ky����Ϊ�Ŵ��Ƕ����ԵĶ�ά��������Ӧ��
	��������ʽ�ı���������Ժ���
	*/
	Vec2i tp;
	while(tx<=_maxX){
		//�ڴ���ʱ��������С��ԭʼֵ
		bool isValid=_func(tx/_scaleX,nty);
		//��ʱ���ں���X����û�����壬��1/x����x=0ʱ������������
		if(!isValid){
			//���ʱ�򣬲���Ҫ��ֵ
			needSlerp=false;
			tx++;
			continue;
		}
		nty*=_scaleY;
		//���˳�����Ļ��ʾ��Yֵ
		if(ABS(nty)>halfH){
			nty=nty>0?halfH+1:-halfH-1;
		}
		//��ֵ
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

//����ԭʼ��������X����Сֵ
void DrawFunc::setMinX( int min ){
	_setMinX=true;
	_minX=min;
}

//����ԭʼ��������X�����ֵ
void DrawFunc::setMaxX( int max ){
	_setMaxX=true;
	_maxX=max;
}



NS_ENGIN_END