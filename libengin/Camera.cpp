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

	//视平面边角上的四点
	Vec3f lt(camera->viewplane_left,camera->viewplane_top);   //左上
	Vec3f lb(camera->viewplane_left,-camera->viewplane_top);  //左下
	Vec3f rt(-camera->viewplane_left,camera->viewplane_top);  //右上
	Vec3f rb(-camera->viewplane_left,-camera->viewplane_top); //右下

	//远裁剪面左上角，右下角
	Vec3f flt(-camera->fc*_tan_fov_h_2,camera->fc*_tan_fov_v_2);
	Vec3f frb(-flt.x,-flt.y);

	GetPlane3D(flt,lt,lb,camera->lc); //左裁剪面
	GetPlane3D(frb,rb,rt,camera->rc); //右裁剪面
	GetPlane3D(rt,lt,flt,camera->tc); //上裁剪面
	GetPlane3D(lb,rb,frb,camera->bc); //下裁剪面

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

//dirAngle，向量在xz平面的投影与z轴的夹角,俯视图顺时针为正
//upAngle，向量与xz平面的夹角，Y轴正方向为正，负方向为负
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

//用右手来判断正的旋转方向，根据平面的旋转来推导的
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
	rm.transpose();//正交矩阵的逆矩阵就是转置矩阵
	Mat4_Mul(tm,rm,mcam);
}

/*
设当前相机向量为U
有U*Q=U'*R
R和Q为坐标系中的矩阵，例如，
		|100|		  |001| 
|1 2 6|*|010|=|2 6 1|*|100|
		|001|		  |010|
其中每一行称为向量基
  |Qx|=   |Rx|
U*|Qy|=U'*|Ry|
  |Qz|=   |Rz|
U'=U*Q*R^-1
如Q为单位阵，则
U'=U*R^-1
如R为正交矩阵，则
U'=U*R^T
于是相机转化的过程为
Cam*R*T=Cam'，其中R为旋转，T为平移
那为何要先旋转呢，因为如果平移，那么后面的放大和旋转，都会不正确，
所以要把平移放在最后面，于是要把世界坐标转到相机坐标，则有
COORc=COORw*(R*T)^-1=COORw*T^-1*R^T，这里需要注意的是旋转矩阵，在
转置前，每列是向量基，转置后每行成了向量基，下面是具体的代码
*/
//uvn相机是为了更好的平滑插值，而欧拉相机插值会出现跳跃，也就是不平滑
//说到底，就是三维空间用两个量来表示肯定是不够的，所以插值的结果不唯一
//于是就不平滑了，而uvn用了三个量来描述相机，如此插值的结果就唯一了，所以
//就有了平滑的插值结果
void Camera::updateUVNMatrix(){
	//根据target初始化uvn
	n=target;n.w=1;
	u.x=0;u.y=1;u.z=0;u.w=1;
	//需要进行两次叉乘，才能确定两两垂直
	crossMultiply(u.v3f(),n.v3f(),v.v3f());
	v.w=1;
	crossMultiply(n.v3f(),v.v3f(),u.v3f());
	n.v3f().identify();
	u.v3f().identify();
	v.v3f().identify();
	Mat4 rm;//以下是转置后的旋转矩阵
	rm._11=v.x;      rm._12=u.x;      rm._13=n.x;  rm._14=0;
	rm._21=v.y;      rm._22=u.y;      rm._23=n.y;  rm._24=0;
	rm._31=v.z;      rm._32=u.z;      rm._33=n.z;  rm._34=0;
	rm._41=0;		 rm._42=0;		  rm._43=0;	   rm._44=1;
	Mat4 tm;
	Mat4::createTranslation(-pos.x,-pos.y,-pos.z,tm);
	Mat4_Mul(tm,rm,mcam);
}

void Camera::updatePerMatrix(){
	//这里不知为何得为tan()才能正常投影，否则会出现变形
	//理论推导则为cot（）,目标都是为了得到视距d
	/*
	d=0.5w/tan(fov_h/2)
	由于w等于2，所以有
	d=1/tan(fov_h/2)，可是这样的话，展现出的画面，像是很小的视野一样。
	而换成d=tan(fov_h/2)，显得一切都很正常，为什么？？？！！！
	透视变换后，如果视平面是正方形，那么
	最要把x除以aspect_ratio，否则会失真，因为屏幕变换时，会进行等比拉伸，而
	屏幕的宽高比为aspect_ratio，对正方形进行拉伸后就变形了。
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