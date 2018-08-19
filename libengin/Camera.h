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
	int state;//���״̬
	int attr; //�������
	Vec4f pos;
	Vec4f u,v,n,target; //uvn�������
	Vec4f dir;      //euler�������
	float view_dis; //�Ӿ�
	float fov_h;    //ˮƽ��Ұ
	float fov_v;    //��ֱ��Ұ

	float nc;		//���ü���near clip 
	float fc;		//Զ�ü���far clip
	Plane3D rc;		//�Ҳü���right clip
	Plane3D lc;		//��ü���left clip
	Plane3D tc;		//�ϲü���top clip
	Plane3D bc;		//�²ü���bottom clip
	
	/*
	ˮƽ�ӽ�		fov_h
	�Ӿ�			view_dis
	��ֱ�ӽ�		fov_v = 2*arctan[tan(fov_h/2)/ar]
	���Ͻ�(m,n�ֱ��Ǻᣬ������)
	m = - view_dis * tan(fov_h/2)
	n = (view_dis/ar) * tan(fov_h/2)
	��ƽ����
	w = 2 * view_dis * tan(fov_h/2)
	h = 2 * (view_dis/ar) * tan(fov_h/2)
	�ӿ� pw ph
	��Ļ
	Xs = [(x-m)/-2m](pw-1)
	   = [(m-x)/m]*0.5*(pw-1)
	   = [1-x/m]*0.5*(pw-1)
	��0.5*(pw-1)ΪA
	Xs = A - A/m*x
	ͬ����
	Ys = B - B/n*y ( B = 0.5*(ph-1) )
	*/
	//��ƽ����
	float viewplane_w;
	float viewplane_h;
	float viewplane_top;
	float viewplane_left;
	//�ӿڿ��
	float viewport_w;
	float viewport_h;
	float aspect_ratio;

	//��Ļ����ת����ʽ
	/*
	Xs=(x-viewplane_left)*(viewport_w-1)/viewplane_w
	Ys=(viewplane_top-y)*(viewport_h-1)/viewplane_h;
	��
	xScrCoef=(viewport_w-1)/viewplane_w;
	yScrCoef=(viewport_h-1)/viewplane_h;
	*/
	float xScrCoef;
	float yScrCoef;

	//�����ֱ��ת��Ļ�Ĺ�ʽ
	/*
	Xs=(x/(-2*tan(fov_h/2)*z)+0.5)(viewplane_w-1)
	Ys=(y/(-2*tan(fov_h/2)*(1/aspect_ratio)*z)+0.5)(viewplane_h-1)
	��ȡ����
	xCSCoef=1/(-2*tan(fov_h/2));
	yCSCoef=1/(-2*tan(fov_h/2)*(1/aspect_ratio));

	���յ���ʾ���Ӿ�û�й�ϵ��ֻ����Ұ����Ļ��߱��й�ϵ
	�Ӿ�ֻ���м����������
	*/
	//float xCSCoef;
	//float yCSCoef;

	Mat4 mcam; //����任����
	Mat4 mper; //͸�ӱ任����
	Mat4 mscr; //��Ļ�任����

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
	//�����������
	void updateEulerMatrix();
	void updateUVNMatrix();
	//����͸�Ӿ���
	void updatePerMatrix();
	//������Ļ����
	void updateScrMatrix();
	//�����������
	void runAction(Action* action);
	//���¾���
	void updateCamMatrix();
	//�ƶ����
	void moveCamer(int dir,float speed);
#ifdef DEBUG_CAMERA
private:
	EventListenerKeyboard* ek;
	//�������
	void testCamera();
#endif
};
NS_ENGIN_END
#endif