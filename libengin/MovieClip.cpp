#include "MovieClip.h"
#include "Texture.h"
#include "Renderer.h"
#include "log.h"
NS_ENGIN_BEGIN
MovieClip::MovieClip() :_curFrame(0), _intervalTime(0){
	scheduleUpdate();
}

MovieClip::~MovieClip(){
	for (auto tex : _frames){
		tex->release();
	}
}

MovieClip* MovieClip::createWithTextures(Texture* texList[], int numTex){
	MovieClip* movieClip = new MovieClip;
	if (movieClip->initWithTextures(texList, numTex)){
		movieClip->autoRelease();
		return movieClip;
	}
	else{
		delete movieClip;
		return nullptr;
	}
}

bool MovieClip::initWithTextures(Texture* texList[], int numTex){
	for (int i = 0; i < numTex; i++){
		if (texList[i]){
			_frames.push_back(texList[i]);
			texList[i]->retain();
		}
		else{
			break;
		}
	}
	if (_frames.size()>0){
		_curTexture = _frames[0];
	}
	return true;
}

void MovieClip::draw(Renderer* renderer, Mat4& transform){
	renderer->render(_pos, _contentSize, _scaleX, _scaleY, _rotation, _curTexture);
}

void MovieClip::gotoAndPlay(int index/*=0*/){
	_curFrame = index;
}

void MovieClip::update(float dt){
	static float passTime = 0;
	passTime += dt;
	if (passTime < _intervalTime){
		return;
	}
	passTime = 0;
	if (++_curFrame >= _frames.size())
		_curFrame = 0;
	_curTexture = _frames[_curFrame];
	_contentSize = _curTexture->getContentSize();
}

void MovieClip::setInterval(float interval){
	_intervalTime = interval;
}

MovieClip* MovieClip::createWithFile(const std::string& filePreName, const std::string& fileSufName, int bIdx, int eIdx){
	MovieClip* movieClip = new MovieClip();
	if (movieClip->initWithFile(filePreName, fileSufName, bIdx, eIdx)){
		movieClip->autoRelease();
		return movieClip;
	}
	else{
		delete movieClip;
		return nullptr;
	}
}

bool MovieClip::initWithFile(const std::string& filePreName, const std::string& fileSufName, int bIdx, int eIdx){
	static char fileName[260];
	for (int i = bIdx; i <= eIdx; i++){
		sprintf(fileName, "%s%d%s", filePreName.c_str(), i, fileSufName.c_str());
		Texture* tex = Texture::create(fileName);
		if (tex == nullptr)
			break;
		tex->retain();
		_frames.push_back(tex);
	}
	if (_frames.size() > 0){
		_curTexture = _frames[0];
	}
	return _frames.size() > 0;
}

NS_ENGIN_END