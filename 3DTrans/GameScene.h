#ifndef __GAMESCENE_H
#define __GAMESCENE_H
#include "engin.h"
class GameScene :public engin::Scene{
public:
	GameScene();
	virtual ~GameScene();
	bool init();
	bool createMoveSprite();
	bool createMoveMC();
	bool createLine();
	bool createPolygon();
	bool createTriangle();
	bool createTriangle3D();
	bool createCube2Side();
	bool createTerrain3D();
	bool createModelView();
	bool createChangeColor();
	bool createDrawFunc();
	bool createRaiders();
	void onEventCallback(engin::Event* event,engin::Line* line0,engin::Line* line1,engin::EventListener* eventListener);
	static GameScene* create();
};
#endif