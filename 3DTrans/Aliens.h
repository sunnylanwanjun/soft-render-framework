#ifndef __ALIENS_H
#define __ALIENS_H
#include "engin.h"
#define NUM_ALIENS         16
#define NUM_EXPLO          4
#define ALIVE			   1
#define DEAD               2
struct Alien{
	engin::Vec3f pos;
	engin::Vec3f rotate;
	float speed;
	int state;
	float angSpeed;
	Alien():state(DEAD){
		
	}
};

class Aliens{
public:
	Aliens();
	virtual ~Aliens();
	bool init();
	void insertExplosion(const engin::MaterialNode3D* obj);
	void drawExplosion(engin::MaterialRenderList* rl);
	void update(engin::MaterialRenderList* rl,engin::Camera* cam,float mouseX,float mouseY);
private:
	engin::MaterialNode3D    *obj_aliens;           
	engin::MaterialNode3D    explosions[NUM_EXPLO];
	Alien					 aliens[NUM_ALIENS];
};
#endif