#ifndef __MOVIECLIP_H
#define __MOVIECLIP_H
#include "macros.h"
#include "Node.h"
#include <string>
#include <vector>
#include <string>
NS_ENGIN_BEGIN
class Texture;
class Renderer;
class ENGIN_DLL MovieClip :public Node{
public:
	MovieClip();
	virtual ~MovieClip();
	static MovieClip* createWithTextures(Texture* texList[],int numTex);
	static MovieClip* createWithFile(const std::string& filePreName, const std::string& fileSufName,int bIdx, int eIdx);
	bool initWithFile(const std::string& filePreName, const std::string& fileSufName, int bIdx, int eIdx);
	bool initWithTextures(Texture* texList[], int numTex);
	virtual void draw(Renderer* renderer, Mat4& transform);
	virtual void update(float dt);
	//movieclip api
	void gotoAndPlay(int index=0);
	void setInterval(float interval);
private:
	size_t _curFrame;
	float _intervalTime;
	std::vector<Texture*> _frames;
	Texture* _curTexture;
};
NS_ENGIN_END
#endif