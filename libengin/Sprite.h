#ifndef __SPRITE_H
#define __SPRITE_H
#include "macros.h"
#include "Node.h"
#include <string>
#include "Texture.h"
NS_ENGIN_BEGIN
class Renderer;
class ENGIN_DLL Sprite :public Node{
public:
	Sprite();
	virtual ~Sprite();
	static Sprite* create(const std::string& fileName);
	static Sprite* createWithTexture(Texture* texture);
	bool initWithFile(const std::string& fileName);
	bool initWithTexture(Texture* texture);
	void setTexture(Texture* texture);
	virtual void draw(Renderer* renderer, Mat4& transform);
private:
	Texture* _texture;
};
NS_ENGIN_END
#endif