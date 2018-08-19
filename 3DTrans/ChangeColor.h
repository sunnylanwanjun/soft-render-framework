#ifndef __CHANGE_COLOR_H
#define __CHANGE_COLOR_H
#include "engin.h"
#define TEXTURE_NUM 10
class ChangeColor:public engin::Node{
public:
	ChangeColor();
	~ChangeColor();
	static ChangeColor* create();
	virtual bool init();
	void onKeyPress(int keyCode,engin::EventKeyboard* e);
private:
	engin::Texture* text[TEXTURE_NUM];
	engin::Texture* curText;
	engin::Sprite*  sp;
	engin::EventListenerKeyboard *ek;
	int texIdx;
};
#endif