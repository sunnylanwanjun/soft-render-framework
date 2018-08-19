#ifndef __CAMERA_H
#define __CAMERA_H
#include "macros.h"
#include "Vec.h"
#include "mathUtil.h"
#include "Mat4.h"
#include "Ref.h"
#include "t3dlib5.h"
#define DEBUG_CAMERA
NS_ENGIN_BEGIN
class Action;
class ActionManager;
class ObjNode3D;
class RenderList;
class EventListenerKeyboard;
enum CameraType{
	CAM_UVN,
	CAM_EULER
};
enum CameraMove{
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT
};
class ENGIN_DLL Camera:public Ref{
public:
	int state;//相机状态
	int attr; //相机属性
	Vec4f pos;
	Vec4f u,v,n,target; //uvn相机属性
	Vec4f dir;      //euler相机属性
	float view_dis; //视距
	float fov_h;    //水平视野
	float fov_v;    //垂直视野

	float nc;		//近裁剪面near clip 
	float fc;		//远裁剪面far clip
	Plane3D rc;		//右裁剪面right clip
	Plane3D lc;		//左裁剪面left clip
	Plane3D tc;		//上裁剪面top clip
	Plane3D bc;		//下裁剪面bottom clip
	
	/*
	水平视角		fov_h
	视距			view_dis
	垂直视角		fov_v = 2*arctan[tan(fov_h/2)/ar]
	左上角(m,n分别是横，纵坐标)
	m = - view_dis * tan(fov_h/2)
	n = (view_dis/ar) * tan(fov_h/2)
	视平面宽高
	w = 2 * view_dis * tan(fov_h/2)
	h = 2 * (view_dis/ar) * tan(fov_h/2)
	视口 pw ph
	屏幕
	Xs = [(x-m)/-2m](pw-1)
	   = [(m-x)/m]*0.5*(pw-1)
	   = [1-x/m]*0.5*(pw-1)
	设0.5*(pw-1)为A
	Xs = A - A/m*x
	同理有
	Ys = B - B/n*y ( B = 0.5*(ph-1) )
	*/
	//视平面宽高
	float viewplane_w;
	float viewplane_h;
	float viewplane_top;
	float viewplane_left;
	//视口宽高
	float viewport_w;
	float viewport_h;
	float aspect_ratio;

	//屏幕坐标转化公式
	/*
	Xs=(x-viewplane_left)*(viewport_w-1)/viewplane_w
	Ys=(viewplane_top-y)*(viewport_h-1)/viewplane_h;
	设
	xScrCoef=(viewport_w-1)/viewplane_w;
	yScrCoef=(viewport_h-1)/viewplane_h;
	*/
	float xScrCoef;
	float yScrCoef;

	//从相机直接转屏幕的公式
	/*
	Xs=(x/(-2*tan(fov_h/2)*z)+0.5)(viewplane_w-1)
	Ys=(y/(-2*tan(fov_h/2)*(1/aspect_ratio)*z)+0.5)(viewplane_h-1)
	提取因子
	xCSCoef=1/(-2*tan(fov_h/2));
	yCSCoef=1/(-2*tan(fov_h/2)*(1/aspect_ratio));

	最终的显示与视矩没有关系，只与视野和屏幕宽高比有关系
	视矩只起到中间变量的作用
	*/
	//float xCSCoef;
	//float yCSCoef;

	Mat4 mcam; //相机变换矩阵
	Mat4 mper; //透视变换矩阵
	Mat4 mscr; //屏幕变换矩阵

	ActionManager* _actionManager;
public:
	Camera();
	virtual ~Camera();
	static Camera* create(int _attr,
		const Vec4f& _pos,
		float _nc,
		float _fc,
		float _fov_h,
		float _viewport_w,
		float _viewport_h);
	void setPos(const Vec3f& p);
	void setTarget(float dirAngle,float upAngle);
	void setDir(float x,float y,float z);
	//更新相机矩阵
	void updateEulerMatrix();
	void updateUVNMatrix();
	//更新透视矩阵
	void updatePerMatrix();
	//更新屏幕矩阵
	void updateScrMatrix();
	//播放相机动画
	void runAction(Action* action);
	//更新矩阵
	void updateCamMatrix();
	//移动相机
	void moveCamer(int dir,float speed);
#ifdef DEBUG_CAMERA
private:
	EventListenerKeyboard* ek;
	//测试相机
	void testCamera();
#endif
};
NS_ENGIN_END
#endif