#include "Aliens.h"
#include "raidersDef.h"
USING_ENGIN_NS
extern float halfWidth;
extern float halfHeight;
Aliens::Aliens(){
	init();
}

Aliens::~Aliens(){
	obj_aliens->release();
}

bool Aliens::init(){
	obj_aliens = MaterialNode3D::createWithCobfile("tie04.cob");//tie04.cob
	//obj_aliens->setScale(Vec3f(80.0f,80.0f,80.0f));
	obj_aliens->setScale(Vec3f(30.0f,30.0f,30.0f));
	obj_aliens->retain();
	return true;
}

//#define DRAW_BOUNDING
void Aliens::update( MaterialRenderList* rl,Camera* cam,float mouseX,float mouseY ){
#ifdef DRAW_BOUNDING
	Renderer* renderer = Director::getInstance()->getRenderer();
	auto lpdds = renderer->getDirectDrawSurface();
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr))){
		return;
	}

	const RangeFIXP16& wndRange = Director::getInstance()->getFIXP16ClipRange();
	const Vec2FIXP16& wndOrigin = Director::getInstance()->getFIXP16WindowOrigin();
#endif
	 
	for(int i=0;i<NUM_ALIENS;i++){
		if(aliens[i].state==DEAD){
			aliens[i].pos.x=(float)Rand_Range(-1000,1000);
			aliens[i].pos.y=(float)Rand_Range(-1000,1000);
			aliens[i].pos.z=40000.0f;
			aliens[i].speed=(float)Rand_Range(50,100); 
			aliens[i].angSpeed=0.02f*(1+rand()%5); 
			aliens[i].rotate.z=0;
			aliens[i].rotate.x=0;
			aliens[i].rotate.y=0;
			aliens[i].state=ALIVE;
		}
		aliens[i].pos.z-=aliens[i].speed;
		aliens[i].rotate.z+=aliens[i].angSpeed;
		aliens[i].rotate.x+=aliens[i].angSpeed;
		aliens[i].rotate.y+=aliens[i].angSpeed;
		if(aliens[i].pos.z<=NEAR_Z){
			aliens[i].state=DEAD;
		}
		if(aliens[i].state==ALIVE){
			obj_aliens->setPosition(aliens[i].pos);
			obj_aliens->setRotation(aliens[i].rotate);
			obj_aliens->updateModelViewTransMat();
			obj_aliens->updateLocalToWorldTrans(obj_aliens->getModelViewMat());
			obj_aliens->cullObj(cam);

			if(aliens[i].pos.z<15000.0f&&IsLButtonDown()){
				static Vec4f screenPos;
				Mat4_Mul(aliens[i].pos,cam->mcam,screenPos.v3f());
				Mat4_Mul(screenPos,cam->mper,screenPos);
				screenPos.homogeneous();
				Mat4_Mul(screenPos,cam->mscr,screenPos);
				static Vec4f radius;
				radius.x=aliens[i].pos.x;
				radius.y=aliens[i].pos.y+obj_aliens->avgRadiuses[0];
				radius.z=aliens[i].pos.z;
				radius.w=1.0f;
				Mat4_Mul(radius,cam->mcam,radius);
				Mat4_Mul(radius,cam->mper,radius);
				radius.homogeneous();
				Mat4_Mul(radius,cam->mscr,radius);
				int dis=Fast_Distance_2D(int(screenPos.x-radius.x),int(screenPos.y-radius.y));
				if(mouseX>screenPos.x-dis&&mouseX<screenPos.x+dis&&
					mouseY>screenPos.y-dis&&mouseY<screenPos.y+dis){
						aliens[i].state=DEAD;
						insertExplosion(obj_aliens);
						continue;
#ifdef DRAW_BOUNDING
						std::cout<<"MOUSE IN"<<std::endl;
						static Vec2f leftTop;
						static Vec2f rightTop;
						static Vec2f leftBottom;
						static Vec2f rightBottom;
						leftTop.x=screenPos.x-dis;
						leftTop.y=screenPos.y-dis;
						rightTop.x=screenPos.x+dis;
						rightTop.y=leftTop.y;
						leftBottom.x=leftTop.x;
						leftBottom.y=screenPos.y+dis;
						rightBottom.x=rightTop.x;
						rightBottom.y=leftBottom.y;
						DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, 0x00ff0000,leftTop,rightTop);
						DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, 0x00ff0000,rightTop,rightBottom);
						DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, 0x00ff0000,rightBottom,leftBottom);
						DrawLine((DWORD*)ddsd.lpSurface, ddsd.lPitch >> 2, wndOrigin, wndRange, 0x00ff0000,leftBottom,leftTop);
#endif
				}
			}
			rl->insertObj(*obj_aliens);
			obj_aliens->resetState();
		}
	}
	drawExplosion(rl);
	#ifdef DRAW_BOUNDING
	lpdds->Unlock(nullptr);
	#endif
}

void Aliens::insertExplosion(const MaterialNode3D* obj){
	bool hasInsert=false;
	for(int i=0;i<NUM_EXPLO;i++){
		if(explosions[i].state==OBJ_STATE_NULL){
			hasInsert=true;
			MaterialNode3D::copyObj_World(explosions+i,obj);
			explosions[i].ivar1=(DWORD)(new Vec3f[explosions[i].numPoly]);
			for(int j=0;j<explosions[i].numPoly;j++){
				((Vec3f*)explosions[i].ivar1)[j].x=(float)Rand_Range(-100,100);
				((Vec3f*)explosions[i].ivar1)[j].y=(float)Rand_Range(-100,100);
				((Vec3f*)explosions[i].ivar1)[j].z=(float)Rand_Range(-100,100);
			}
			explosions[i].ivar2=100;
			break;
		}
	}

	if(!hasInsert){
		MaterialNode3D::copyObj_World(explosions,obj);
		explosions[0].ivar1=(DWORD)(new Vec3f[explosions[0].numPoly]);
		for(int j=0;j<explosions[0].numPoly;j++){
			((Vec3f*)explosions[0].ivar1)[j].x=(float)Rand_Range(-50,50);
			((Vec3f*)explosions[0].ivar1)[j].y=(float)Rand_Range(-50,50);
			((Vec3f*)explosions[0].ivar1)[j].z=(float)Rand_Range(-50,50);
		}
		explosions[0].ivar2=100;
	}
}

void Aliens::drawExplosion(MaterialRenderList* rl){
	for(int i=0;i<NUM_EXPLO;i++){
		MaterialNode3D& obj=explosions[i];
		if(obj.state&OBJ_STATE_ACTIVE){
			if(--obj.ivar2<=0){
				obj.state=OBJ_STATE_NULL;
				delete[] (Vec3f*)obj.ivar1;
				MaterialNode3D::destroyObj(&obj);
				continue;
			}
			for(int j=0;j<obj.numPoly;j++){
				MatPoly_VecIdx& poly=obj.polyList[j];
				Vec3f_Add(poly.pVecList[poly.verIndex[0]].v3f(),
					((Vec3f*)obj.ivar1)[j],poly.pVecList[poly.verIndex[0]].v3f());
				Vec3f_Add(poly.pVecList[poly.verIndex[1]].v3f(),
					((Vec3f*)obj.ivar1)[j],poly.pVecList[poly.verIndex[1]].v3f());
				Vec3f_Add(poly.pVecList[poly.verIndex[2]].v3f(),
					((Vec3f*)obj.ivar1)[j],poly.pVecList[poly.verIndex[2]].v3f());
				poly.updateNormVec();
			}
			rl->insertObj(obj);
		}
	}
}