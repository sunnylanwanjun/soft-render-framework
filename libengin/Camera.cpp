#include "Camera.h"
#include "mathUtil.h"
#include "RenderList.h"
#include "Object3D.h"
#include "ActionManager.h"
#include "Director.h"
#ifdef DEBUG_CAMERA
#include "EventListenerKeyboard.h"
#include "EventDispatcher.h"
#endif
NS_ENGIN_BEGIN

Camera::Camera():state(0){
	_actionManager = Director::getInstance()->getActionManager();
#ifdef DEBUG_CAMERA
	ek=nullptr;
#endif
}

Camera::~Camera(){
#ifdef DEBUG_CAMERA
	if(ek){
		GDISPATCH->removeEventListener(ek);
	}
#endif
}

Camera* Camera::create( int _attr,
					   const Vec4f& _pos,
					   float _nc,
					   float _fc,
					   float _fov_h,
					   float _viewport_w,
					   float _viewport_h){
	Camera* camera = new Camera;
	camera->autoRelease();
	camera->attr=_attr;

	camera->pos=_pos;
	camera->nc=_nc;
	camera->fc=_fc;
	camera->fov_h=_fov_h;
	camera->viewport_w=_viewport_w;
	camera->viewport_h=_viewport_h;

	camera->u.x=1;
	camera->v.y=1;
	camera->n.z=1;
	camera->aspect_ratio=camera->viewport_w/camera->viewport_h;
	float _1_aspect_ratio=1/camera->aspect_ratio;
	float _tan_fov_h_2 = tan(camera->fov_h/2);
	float _tan_fov_v_2 = _tan_fov_h_2;//*_1_aspect_ratio;
	camera->fov_v = 2*atan(_tan_fov_v_2);
	
	camera->viewplane_w = 2.0f;
	camera->viewplane_h = 2.0f;//*_1_aspect_ratio;
	camera->viewplane_left = -1.0f;
	camera->viewplane_top = 1.0f;//*_1_aspect_ratio;//1/camera->aspect_ratio

	camera->xScrCoef=(camera->viewport_w-1)/camera->viewplane_w;
	camera->yScrCoef=(camera->viewport_h-1)/camera->viewplane_h;

	//��ƽ��߽��ϵ��ĵ�
	Vec3f lt(camera->viewplane_left,camera->viewplane_top);   //����
	Vec3f lb(camera->viewplane_left,-camera->viewplane_top);  //����
	Vec3f rt(-camera->viewplane_left,camera->viewplane_top);  //����
	Vec3f rb(-camera->viewplane_left,-camera->viewplane_top); //����

	//Զ�ü������Ͻǣ����½�
	Vec3f flt(-camera->fc*_tan_fov_h_2,camera->fc*_tan_fov_v_2);
	Vec3f frb(-flt.x,-flt.y);

	GetPlane3D(flt,lt,lb,camera->lc); //��ü���
	GetPlane3D(frb,rb,rt,camera->rc); //�Ҳü���
	GetPlane3D(rt,lt,flt,camera->tc); //�ϲü���
	GetPlane3D(lb,rb,frb,camera->bc); //�²ü���

	if(_attr==CAM_EULER){
		camera->setDir(0,0,0);
		camera->updateEulerMatrix();
	}else{
		camera->setTarget(0,0);
		camera->updateUVNMatrix();
	}
	camera->updatePerMatrix();
	camera->updateScrMatrix();
#ifdef DEBUG_CAMERA
	camera->testCamera();
#endif
	return camera;
}

void Camera::updateCamMatrix(){
	if(attr==CAM_EULER){
		updateEulerMatrix();
	}else{
		updateUVNMatrix();
	}
}

//dirAngle��������xzƽ���ͶӰ��z��ļн�,����ͼ˳ʱ��Ϊ��
//upAngle��������xzƽ��ļнǣ�Y��������Ϊ����������Ϊ��
void Camera::setTarget(float dirAngle,float upAngle){
	float cosUp=cos(upAngle);
	target.x=cosUp*sin(dirAngle);
	target.z=cosUp*cos(dirAngle);
	target.y=sin(upAngle);
	setDir(upAngle,-dirAngle,0);
}

void Camera::setPos(const Vec3f& p){
	pos.x=p.x;
	pos.y=p.y;
	pos.z=p.z;
}

//���������ж�������ת���򣬸���ƽ�����ת���Ƶ���
void Camera::setDir(float x,float y,float z){
	dir.x=x;
	dir.y=y;
	dir.z=z;
}

void Camera::updateEulerMatrix(){
	Mat4 tm;
	Mat4::createTranslation(-pos.x,-pos.y,-pos.z,tm);
	Mat4 rm;
	Mat4::createRotation(dir.v3f(),rm);
	rm.transpose();//�����������������ת�þ���
	Mat4_Mul(tm,rm,mcam);
}

/*
�赱ǰ�������ΪU
��U*Q=U'*R
R��QΪ����ϵ�еľ������磬
		|100|		  |001| 
|1 2 6|*|010|=|2 6 1|*|100|
		|001|		  |010|
����ÿһ�г�Ϊ������
  |Qx|=   |Rx|
U*|Qy|=U'*|Ry|
  |Qz|=   |Rz|
U'=U*Q*R^-1
��QΪ��λ����
U'=U*R^-1
��RΪ����������
U'=U*R^T
�������ת���Ĺ���Ϊ
Cam*R*T=Cam'������RΪ��ת��TΪƽ��
��Ϊ��Ҫ����ת�أ���Ϊ���ƽ�ƣ���ô����ķŴ����ת�����᲻��ȷ��
����Ҫ��ƽ�Ʒ�������棬����Ҫ����������ת��������꣬����
COORc=COORw*(R*T)^-1=COORw*T^-1*R^T��������Ҫע�������ת������
ת��ǰ��ÿ������������ת�ú�ÿ�г����������������Ǿ���Ĵ���
*/
//uvn�����Ϊ�˸��õ�ƽ����ֵ����ŷ�������ֵ�������Ծ��Ҳ���ǲ�ƽ��
//˵���ף�������ά�ռ�������������ʾ�϶��ǲ����ģ����Բ�ֵ�Ľ����Ψһ
//���ǾͲ�ƽ���ˣ���uvn�����������������������˲�ֵ�Ľ����Ψһ�ˣ�����
//������ƽ���Ĳ�ֵ���
void Camera::updateUVNMatrix(){
	//����target��ʼ��uvn
	n=target;n.w=1;
	u.x=0;u.y=1;u.z=0;u.w=1;
	//��Ҫ�������β�ˣ�����ȷ��������ֱ
	crossMultiply(u.v3f(),n.v3f(),v.v3f());
	v.w=1;
	crossMultiply(n.v3f(),v.v3f(),u.v3f());
	n.v3f().identify();
	u.v3f().identify();
	v.v3f().identify();
	Mat4 rm;//������ת�ú����ת����
	rm._11=v.x;      rm._12=u.x;      rm._13=n.x;  rm._14=0;
	rm._21=v.y;      rm._22=u.y;      rm._23=n.y;  rm._24=0;
	rm._31=v.z;      rm._32=u.z;      rm._33=n.z;  rm._34=0;
	rm._41=0;		 rm._42=0;		  rm._43=0;	   rm._44=1;
	Mat4 tm;
	Mat4::createTranslation(-pos.x,-pos.y,-pos.z,tm);
	Mat4_Mul(tm,rm,mcam);
}

void Camera::updatePerMatrix(){
	//���ﲻ֪Ϊ�ε�Ϊtan()��������ͶӰ���������ֱ���
	//�����Ƶ���Ϊcot����,Ŀ�궼��Ϊ�˵õ��Ӿ�d
	/*
	d=0.5w/tan(fov_h/2)
	����w����2��������
	d=1/tan(fov_h/2)�����������Ļ���չ�ֳ��Ļ��棬���Ǻ�С����Ұһ����
	������d=tan(fov_h/2)���Ե�һ�ж���������Ϊʲô������������
	͸�ӱ任�������ƽ���������Σ���ô
	��Ҫ��x����aspect_ratio�������ʧ�棬��Ϊ��Ļ�任ʱ������еȱ����죬��
	��Ļ�Ŀ�߱�Ϊaspect_ratio���������ν��������ͱ����ˡ�
	*/
	mper._11=1/tan(fov_h/2)/aspect_ratio;    mper._12=0;			mper._13=0;    mper._14=0;
	mper._21=0;			  mper._22=1/tan(fov_h/2);    mper._23=0;    mper._24=0;
	mper._31=0;			  mper._32=0;			mper._33=1;    mper._34=1;
	mper._41=0;			  mper._42=0;			mper._43=0;	   mper._44=0;
}

void Camera::updateScrMatrix(){
	//mscr.m11=xScrCoef;    mscr.m12=0;			mscr.m13=0;    mscr.m14=0;
	//mscr.m21=0;			  mscr.m22=-yScrCoef;   mscr.m23=0;    mscr.m24=0;
	//mscr.m31=0;			  mscr.m32=0;			mscr.m33=1;    mscr.m34=1;

	//mscr.m41=-viewplane_left*xScrCoef;			
	//mscr.m42=viewplane_top*yScrCoef;	
	//mscr.m43=0;			  mscr.m44=1;

	Mat4 scale;
	Mat4::createScale(viewport_w/viewplane_w,-viewport_h/viewplane_h,1,scale);
	Mat4 tm;
	Mat4::createTranslation(viewport_w/2,viewport_h/2,0,tm);
	Mat4_Mul(scale,tm,mscr);

	//float alpha = (0.5f*viewport_w-0.5f);
	//float beta  = (0.5f*viewport_h-0.5f);

	//mscr.m11=alpha;  mscr.m12= 0;     mscr.m13=0;    mscr.m14=0; 
	//mscr.m21=0;      mscr.m22=-beta;  mscr.m23=0;    mscr.m24=0; 
	//mscr.m31=0;      mscr.m32= 0;     mscr.m33=1;    mscr.m34=0;
	//mscr.m41=alpha;      mscr.m42= beta;     mscr.m43=0;    mscr.m44=1;

}

void Camera::moveCamer(int moveDir,float speed){
	switch(moveDir){
	case MOVE_FORWARD:
		pos.x-=speed*sin(dir.y);
		pos.z+=speed*cos(dir.y);
		break;
	case MOVE_BACKWARD:
		pos.x+=speed*sin(dir.y);
		pos.z-=speed*cos(dir.y);
		break;
	case MOVE_LEFT:
		pos.x-=speed*cos(dir.y);
		pos.z-=speed*sin(dir.y);
		break;
	case MOVE_RIGHT:
		pos.x+=speed*cos(dir.y);
		pos.z+=speed*sin(dir.y);
		break;
	}
}

void Camera::runAction(Action* action){
	_actionManager->addAction(action, this);
}

#ifdef DEBUG_CAMERA
void Camera::testCamera(){
	ek = EventListenerKeyboard::create();
	static float POS_CHANGE=30.0f;
	static float ANGLE_CHANGE=0.05f;

	if(attr==CAM_UVN){
		ek->onKeyPress = [this](EventKeyboard::KeyCode keyCode,EventKeyboard* e){
			static float dirAngle=0;
			static float upAngle=0;
			if(keyCode==EventKeyboard::KEY_A){
				pos.x-=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_S){
				pos.y-=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_D){
				pos.x+=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_W){
				pos.y+=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_Q){
				pos.z+=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_E){
				pos.z-=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_UP_ARROW){
				upAngle+=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_DOWN_ARROW){
				upAngle-=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_LEFT_ARROW){
				dirAngle-=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_RIGHT_ARROW){
				dirAngle+=ANGLE_CHANGE;
			}
			setTarget(dirAngle,upAngle);
			updateUVNMatrix();
		};
	}else{
		ek->onKeyPress = [this](EventKeyboard::KeyCode keyCode,EventKeyboard* e){
			static float x=0;
			static float y=0;
			static float z=0;
			if(keyCode==EventKeyboard::KEY_A){
				moveCamer(MOVE_LEFT,POS_CHANGE);
			}else if(keyCode==EventKeyboard::KEY_S){
				pos.y-=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_D){
				moveCamer(MOVE_RIGHT,POS_CHANGE);
			}else if(keyCode==EventKeyboard::KEY_W){
				pos.y+=POS_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_Q){
				moveCamer(MOVE_FORWARD,POS_CHANGE);
			}else if(keyCode==EventKeyboard::KEY_E){
				moveCamer(MOVE_BACKWARD,POS_CHANGE);
			}else if(keyCode==EventKeyboard::KEY_UP_ARROW){
				x+=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_DOWN_ARROW){
				x-=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_LEFT_ARROW){
				y+=ANGLE_CHANGE;
			}else if(keyCode==EventKeyboard::KEY_RIGHT_ARROW){
				y-=ANGLE_CHANGE;
			}
			setDir(x,y,z);
			updateEulerMatrix();
		};
	}
	GDISPATCH->addEventListener(ek);
}
#endif
NS_ENGIN_END