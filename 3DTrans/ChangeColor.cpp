#include "ChangeColor.h"
USING_ENGIN_NS
ChangeColor::ChangeColor():ek(nullptr),texIdx(0){

}

ChangeColor::~ChangeColor(){
	if(ek){
		GDISPATCH->removeEventListener(ek);
		ek=nullptr;
	}
	for(int i=0;i<TEXTURE_NUM;i++){
		text[i]->release();
	}
	if(curText){
		curText->release();
		curText=nullptr;
	}
}

ChangeColor* ChangeColor::create(){
	ChangeColor* changeColor=new ChangeColor;
	if(changeColor->init()){
		changeColor->autoRelease();
		return changeColor;
	}
	else{
		delete changeColor;
		return nullptr;
	}
}

bool ChangeColor::init(){
	char fileName[20];
	for(int i=0;i<TEXTURE_NUM;i++){
		sprintf(fileName,"p%d.png",i);
		text[i]=Texture::create(fileName);
		text[i]->retain();
	}
	
	curText=Texture::create("p1.png");
	curText->retain();

	sp = Sprite::createWithTexture(curText);
	addChild(sp);
	ek = EventListenerKeyboard::create();
	ek->onKeyPress=CALL_BACK_2(ChangeColor::onKeyPress,this);
	GDISPATCH->addEventListener(ek);
	return true;
}

void ChangeColor::onKeyPress(int keyCode,EventKeyboard* e){
	Size size=curText->getContentSize();
	DWORD* data=(DWORD*)(curText->getData());
	if(keyCode==EventKeyboard::KEY_UP_ARROW){
		Color_Add(data,0x00010101,int(size.width*size.height),data);
		//Color_Mul(data,2.0,int(size.width*size.height),data);
		curText->updateDirectDrawSurface();
	}else if(keyCode==EventKeyboard::KEY_DOWN_ARROW){
		Color_Add(data,-0x00010101,int(size.width*size.height),data);
		//Color_Mul(data,0.5,int(size.width*size.height),data);
		curText->updateDirectDrawSurface();
	}else if(keyCode==EventKeyboard::KEY_LEFT_ARROW){
		texIdx-=1;
		if(texIdx<0)
			texIdx=0;
		curText->setTexture(*text[texIdx]);	
	}else if(keyCode==EventKeyboard::KEY_RIGHT_ARROW){
		texIdx+=1;
		if(texIdx>=TEXTURE_NUM)
			texIdx=TEXTURE_NUM-1;
		curText->setTexture(*text[texIdx]);	
	}
	sp->setTexture(curText);
}