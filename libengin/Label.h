#ifndef __LABEL_H
#define __LABEL_H
#include "macros.h"
#include "Node.h"
#include "t3dlib1.h"
#include <string>
NS_ENGIN_BEGIN

enum HAlignType{
	HLeft = 0,
	HCenter = DT_CENTER,
};

enum VAlignType{
	VLeft = 0,
	VCenter = DT_VCENTER,
};
 
class Renderer;
class ENGIN_DLL Label :public Node{
public:
	Label();
	~Label();
	static Label* create(const std::string& text,COLORREF color=RGB(255,255,255),const std::string& fontName="Î¢ÈíÑÅºÚ",int fontSize=20,HAlignType hat=HLeft,VAlignType vat=VLeft);
	bool setLabel(const std::string& text);
	void draw(Renderer* renderer, Mat4& transform);
	void setFontSize(int fontSize){ _fontSize = fontSize; updateFont(_fontName.c_str(), _fontSize, false, true); }
	void setFontName(const std::string& fontName){ _fontName = fontName;  updateFont(_fontName.c_str(), _fontSize, false, true); }
	void setHAlignType(HAlignType alignType){ _hAlignType = alignType; }
	void setVAlignType(VAlignType alignType){ _vAlignType = alignType; }
	void setSingleLine(bool isSingleLine){ _isSingleLine = isSingleLine; }
	void setFontColor(COLORREF color){ _fontColor = color; }
	bool updateDirectDrawSurface();
	void updateFont(LPCTSTR pszFaceName, int nSize, bool bBold, bool bAntiAliasing);
	std::string getLabel(){ return _label; }
private:
	LPDIRECTDRAWSURFACE7 getDirectDrawSurface(int w,int h);
private:
	COLORREF				_fontColor;
	HAlignType				_hAlignType;
	VAlignType				_vAlignType;
	bool					_isSingleLine;
	int						_fontSize;
	std::string				_fontName;
	RECT					_bounds;
	std::string				_label;
	LPDIRECTDRAWSURFACE7	_lpdds;
	HFONT				    _hFont;
};
NS_ENGIN_END
#endif