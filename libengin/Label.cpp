#include "Label.h"
#include "Director.h"
#include "Renderer.h"
#include "log.h"
NS_ENGIN_BEGIN

Label::Label():
_label(""),
_lpdds(nullptr),
_hAlignType(HLeft),
_vAlignType(VLeft),
_isSingleLine(false),
_fontSize(20),
_fontName("微软雅黑"),
_fontColor(RGB(255,255,255)),
_hFont(nullptr){
	setContentSize(Size(100,75));
	_bounds.top = 0;
	_bounds.left = 0;
	_bounds.right = (LONG)_contentSize.width;
	_bounds.bottom = (LONG)_contentSize.height;
}

Label::~Label(){
	if (_hFont){
		DeleteObject(_hFont);
		_hFont = nullptr;
	}
	if (_lpdds){
		_lpdds->Release();
		_lpdds = nullptr;
	}
}

Label* Label::create(const std::string& text, COLORREF color /*= RGB(255, 255, 255)*/, const std::string& fontName/*="微软雅黑"*/, int fontSize/*=20*/, HAlignType hat/*=HLeft*/, VAlignType vat/*=VLeft*/){
	Label* label = new Label;
	label->_hAlignType = hat;
	label->_vAlignType = vat;
	label->_fontSize = fontSize;
	label->_fontName = fontName;
	label->_fontColor = color;
	label->updateFont(fontName.c_str(), fontSize, false, true);

	if (label->setLabel(text)){
		label->autoRelease();
		return label;
	}
	else{
		delete label;
		return nullptr;
	}
}

void Label::updateFont(LPCTSTR pszFaceName, int nSize, bool bBold, bool bAntiAliasing)
{
	if (_hFont){
		DeleteObject(_hFont);
		_hFont = nullptr;
	}

	static LOGFONT LogFont;
	ZeroMemory(&LogFont, 0);
	LogFont.lfHeight = nSize;
	LogFont.lfCharSet = DEFAULT_CHARSET;
	strcpy(LogFont.lfFaceName, pszFaceName);

	if (bBold){
		LogFont.lfWeight = FW_BOLD;
	}
	else{
		LogFont.lfWeight = FW_NORMAL;
	}

	if (bAntiAliasing){
		LogFont.lfQuality = ANTIALIASED_QUALITY;
	}
	else{
		LogFont.lfQuality = 0;
	}

	_hFont = CreateFontIndirect(&LogFont);
}

bool Label::setLabel(const std::string& text){
	_label = text;
	return updateDirectDrawSurface();
}

void Label::draw(Renderer* renderer, Mat4& transform){
	renderer->render(_pos, _contentSize, _scaleX, _scaleY, _rotation, _lpdds);
}

bool Label::updateDirectDrawSurface(){
	if (_lpdds == nullptr){
		_lpdds = getDirectDrawSurface((int)_contentSize.width,(int)_contentSize.height);
	}

	if (_lpdds == nullptr){
		return false;
	}

	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = 0x00;
	_lpdds->Blt(nullptr, nullptr, nullptr, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);

	HDC hdc;
	if (FAILED(_lpdds->GetDC(&hdc))){
		Error("Label::updateDirectDrawSurface Get DC Failed 1 ");
		return false;
	}

	HGDIOBJ hOldf = SelectObject(hdc, _hFont);
	DrawText(hdc, _label.c_str(), -1, &_bounds, DT_CALCRECT|_hAlignType|_vAlignType);
	
	//当容纳不下字体时，需要重新校准表面的尺寸
	if (_bounds.right > _contentSize.width || _bounds.bottom > _contentSize.height){
		SelectObject(hdc, hOldf);
		_lpdds->ReleaseDC(hdc);
		setContentSize(Size(_bounds.right,_bounds.bottom));

		_lpdds = getDirectDrawSurface((int)_contentSize.width, (int)_contentSize.height);
		_lpdds->Blt(nullptr, nullptr, nullptr, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
		if (FAILED(_lpdds->GetDC(&hdc))){
			Error("Label::updateDirectDrawSurface Get DC Failed 2 ");
			return false;
		}
		hOldf = SelectObject(hdc, _hFont);
	}
	
	SetTextColor(hdc, _fontColor);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, _label.c_str(), -1, &_bounds, _hAlignType | _vAlignType);
	SelectObject(hdc, hOldf);
	_lpdds->ReleaseDC(hdc);

	return true;
}

LPDIRECTDRAWSURFACE7 Label::getDirectDrawSurface(int w, int h){
	if (_lpdds){
		_lpdds->Release();
		_lpdds = nullptr;
	}
	LPDIRECTDRAW7 lpdd7 = Director::getInstance()->getRenderer()->getDirectDraw();
	_lpdds = DirectDrawSurfaceInit_Common(lpdd7, w, h, DDSCAPS_VIDEOMEMORY);
	return _lpdds;
}

NS_ENGIN_END