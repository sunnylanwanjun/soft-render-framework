#include "mathUtil.h"
#include "log.h"
NS_ENGIN_BEGIN
int Fast_Distance_2D(int x, int y)
{
	// this function computes the distance from 0,0 to x,y with 3.5% error

	// first compute the absolute value of x,y
	x = abs(x);
	y = abs(y);

	// compute the minimum of x,y
	int mn = MIN(x,y);

	// return the distance
	return(x+y-(mn>>1)-(mn>>2)+(mn>>4));

} // end Fast_Distance_2D

///////////////////////////////////////////////////////////////////////////////

float Fast_Distance_3D(float fx, float fy, float fz)
{
	// this function computes the distance from the origin to x,y,z

	int temp=0;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = (int)(fabs(fx) * 1024);
	y = (int)(fabs(fy) * 1024);
	z = (int)(fabs(fz) * 1024);

	// sort values
	if (y < x) SWAP(temp,x,y);
	if (z < y) SWAP(temp,y,z);
	if (y < x) SWAP(temp,x,y);
	int dist = (z + 11 * (y >> 5) + (x >> 2) );

	// compute distance with 8% error
	return((float)(dist >> 10));

} // end Fast_Distance_3D

int Distance2D2(int x0,int y0,int x1,int y1){
	int x=x0-x1;
	int y=y0-y1;
	return x*x+y*y;
}

void BoundingBox(const Vec3f veclist[],int numVec,Range& range){
	range.left=veclist[0].x;
	range.right=range.left;
	range.top=veclist[0].y;
	range.bottom=range.top;
	for(int i=0;i<numVec;i++){
		if(range.left>veclist[i].x)
			range.left=veclist[i].x;
		if(range.right<veclist[i].x)
			range.right=veclist[i].x;
		if(range.top>veclist[i].y)
			range.top=veclist[i].y;
		if(range.bottom<veclist[i].y)
			range.bottom=veclist[i].y;
	}
}

bool PolygonCollistion(const Vec3f veclist0[],int numVec0,const Vec3f veclist1[],int numVec1){
	Range r0;
	BoundingBox(veclist0,numVec0,r0);
	Range r1;
	BoundingBox(veclist1,numVec1,r1);
	//0左上 1右下
	if(r0.right  >   r1.left && 
	   r0.left   <=  r1.left && 
	   r0.bottom >   r1.top &&  
	   r0.top    <=  r1.top){
	   return true;
	}
	if( r1.right  >   r0.left && 
		r1.left   <=  r0.left && 
		r1.bottom >   r0.top && 
		r1.top    <=  r0.top){
		return true;
	}
	return false;
}

bool PointInPolygon(const Vec3f veclist[],int numVec,const Vec3f& p){
	bool oddNum=false;
	for(int i=0;i<numVec;i++){
		int next=i==numVec-1?0:i+1;
		if((veclist[i].y<p.y&&veclist[next].y>=p.y)||
		   (veclist[next].y<p.y&&veclist[i].y>=p.y)){
			float x=(float)veclist[i].x+(p.y-veclist[i].y)*(veclist[i].x-veclist[next].x)
				/(veclist[i].y-veclist[next].y);
			if(x<p.x){
				oddNum=!oddNum;
			}
		}
	}
	return oddNum;
}

/*
公式推导
(x,y)=(x11,y11)+(x1v,y1v)*t1
(x,y)=(x21,y21)+(x2v,y2v)*t2
联立方程组可得
x=x11+x1v*t1=x21+x2v*t2
y=y11+y1v*t1=y21+y2v*t2
x11*y1v+x1v*y1v*t1=x21*y1v+x2v*y1v*t2
y11*x1v+y1v*x1v*t1=y21*x1v+y2v*x1v*t2
x11*y1v-y11*x1v=x21*y1v+x2v*y1v*t2-(y21*x1v+y2v*x1v*t2)
x11*y1v-y11*x1v=x21*y1v-y21*x1v+(x2v*y1v-y2v*x1v)*t2
t2 = (x11*y1v-y11*x1v-x21*y1v+y21*x1v)/(x2v*y1v-y2v*x1v)
t2 = (y1v*(x11-x21)-x1v*(y11-y21))/(x2v*y1v-y2v*x1v)
   = (x1v*(y11-y21)-y1v*(x11-x21))/(y2v*x1v-x2v*y1v)
x11*y2v+x1v*y2v*t1=x21*y2v+x2v*y2v*t2
x2v*y11+x2v*y1v*t1=x2v*y21+x2v*y2v*t2
x11*y2v+x1v*y2v*t1-x21*y2v=x2v*y11+x2v*y1v*t1-x2v*y21
x11*y2v+x2v*y21-x21*y2v-x2v*y11=x2v*y1v*t1-x1v*y2v*t1
y2v*(x11-x21)-x2v*(y11-y21)=(x2v*y1v-x1v*y2v)*t1
t1=(x2v*(y11-y21)-y2v*(x11-x21))/(y2v*x1v-x2v*y1v)
*/
//LineCrossType LineCross2D(const Line& L1, const Line& L2, float *t1, float *t2, Vec3f* p){
/*
L1p:L1的起点
L1v:L1的向量
L2p:L2的起点
L2v:L2的向量
t1:交点在L1的参数
t2:交点在L2的参数
p:交点P
返回交点是否在线段上
*/
LineCrossType LineCross2D(const Vec3f& L1p, const Vec3f& L1v, const Vec3f& L2p, const Vec3f& L2v, float *t1, float *t2, Vec3f* p){
	float delta=L2v.y*L1v.x-L2v.x*L1v.y;
	if(ABS(delta)<E5){
		return LCT_NOT;
	}
	
	float tt2=(L1v.x*(L1p.y-L2p.y)-L1v.y*(L1p.x-L2p.x))/delta;
	float tt1=(L2v.x*(L1p.y-L2p.y)-L2v.y*(L1p.x-L2p.x))/delta;
	//x11+x1v*t1=x21+x2v*t2
	//y11+y1v*t1=y21+y2v*t2
	//x1v*t1-x2v*t2=x21-x11
	//y1v*t1-y2v*t2=y21-y11
	//Mat2X2 coef(L1v.x,-L2v.x,L1v.y,-L2v.y);
	//Mat2X1 res(L2p.x-L1p.x,L2p.y-L1p.y);
	//float tt1,tt2;
	//Equation2(coef,res,&tt1,&tt2);

	if(t1)
		*t1=tt1;
	if(t2)
		*t2=tt2;
	if(p){
		p->x=L1p.x+L1v.x*tt1;
		p->y=L1p.y+L1v.y*tt1;
	}
	if(tt1>=0&&tt1<=1&&tt2>=0&&tt2<=1)
		return LCT_IN;
	return LCT_OUT;
}

/*
n:法向量
p:平面上一点
p0:检测点
*/
PointInPlane GetPointInPlane(const Vec3f& n,const Vec3f& p,const Vec3f& p0){
	static Vec3f v;
	Vec3f_Sub(p0,p,v);
	float pointMul=Vec3f_Mul(n,v);
	if(pointMul==0){
		return PIP_ON;
	}else if(pointMul>0){
		return PIP_IN;
	}else{
		return PIP_OUT;
	}
}

/*
p0:平面三个点之一
p1:平面三个点之一
p2:平面三个点之一
p3:检测点
*/
PointInPlane GetPointInPlane(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,const Vec3f& p3){
	static Vec3f n;
	GetNormalVec(p0,p1,p2,n);
	return GetPointInPlane(n,p0,p3);
}

/*
plane:平面
p:检测点
*/
PointInPlane GetPointInPlane(const Plane3D& plane,const Vec3f& p){
	return GetPointInPlane(plane.v,plane.p,p);
}

/*
Lp:L的起点
Lv:L的向量
p:检测点
*/
PointInLine GetPointInLine(const Vec3f& Lp,const Vec3f& Lv,const Vec3f& p){
	if(ABS(Lv.x)>E5){
		float t=(p.x-Lp.x)/Lv.x;
		if(t<0||t>1){
			return PIL_OUT;
		}else{
			return PIL_IN;
		}
	}else if(ABS(Lv.y)>E5){
		float t=(p.y-Lp.y)/Lv.y;
		if(t<0||t>1){
			return PIL_OUT;
		}else{
			return PIL_IN;
		}
	}
	return PIL_OUT;
}

/*
Lp:线的起点
Lv:线的终点
n:平面法线
p:平面上一点
cross:线与面的交点
*/
LinePlaneCrossType LinePlaneCross(const Vec3f& Lp,const Vec3f& Lv,const Vec3f& n,const Vec3f& p,Vec3f& cross){
	//线与面平行
	if(Vec3f_Mul(Lv,n)==0)
		return LPCT_NOT;
#ifdef USE_MATRIX_SOLVE
	//A*x+B*y+C*z=D
	Mat3X3 coef;
	Mat3X1 res;
	coef.m11=n.x;
	coef.m12=n.y;
	coef.m13=n.z;
	res.m11=n.x*p.x+n.y*p.y+n.z*p.z;
	float a_b = L.v.x/L.v.y;
	float b_c = L.v.y/L.v.z;
	res.m21 = L.gp0.x-a_b*L.gp0.y;
	res.m31 = L.gp0.y-b_c*L.gp0.z;
	coef.m21=1;
	coef.m22=-a_b;
	coef.m23=0;
	coef.m31=0;
	coef.m32=1;
	coef.m33=-b_c;
	if(!Equation3(coef,res,&cross.x,&cross.y,&cross.z))
		return LPCT_NOT;//方程组无解
	if(GetPointInLine(L,cross)==PIL_IN)
		return LPCT_ON;
	PointInPlane pip = GetPointInPlane(n,p,L.gp0);
	if(pip==PIP_ON)
		return LPCT_ON;
	if(pip==PIP_IN)
		return LPCT_IN;
	return LPCT_OUT;
#else
	// ax+by+cz+d=0
	//n.x*(L.gp0.x+L.v.x*t)+
	//n.y*(L.gp0.y+L.v.y*t)+
	//n.z*(L.gp0.z+L.v.z*t)=n.x*p.x+n.y*p.y+n.z*p.z;
	//(n.x*L.v.x+n.y*L.v.y+n.z*L.v.z)*t=
	//n.x*(p.x-L.gp0.x)+n.y*(p.y-L.gp0.y)+n.z*(p.z-L.gp0.z);
	float t=n.x*Lv.x+n.y*Lv.y+n.z*Lv.z;
	if(ABS(t)<=E5)
		return LPCT_OUT;
	t=(n.x*(p.x-Lp.x)+n.y*(p.y-Lp.y)+n.z*(p.z-Lp.z))/t;
	if(t>=0&&t<=1){
		cross.x=Lp.x+Lv.x*t;
		cross.y=Lp.y+Lv.y*t;
		cross.z=Lp.z+Lv.z*t;
		return LPCT_ON;
	}
	PointInPlane pip = GetPointInPlane(n,p,Lp);
	if(pip==PIP_ON)
		return LPCT_ON;
	if(pip==PIP_IN)
		return LPCT_IN;
	return LPCT_OUT;
#endif
}

void VecToPolar2D(const Vec3f& v,Polar2D& p){
	p.angle=v.x!=0?atan(v.y/v.x):dPI2;
	p.r=v.getModel();
}

void PolarToVec2D(Vec3f& v,Polar2D& p){
	v.x = p.r*cos(p.angle);
	v.y = p.r*sin(p.angle);
}

void VecToPolar3D(Vec3f& v,Polar3D& p){
	p.r = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	p.xAngle = v.x!=0?atan(v.y/v.x):dPI2;
	p.zAngle = p.r!=0?acos(v.z/p.r):0;
}

void PolarToVec3D(Vec3f& v,Polar3D& p){
	v.z = p.r*cos(p.zAngle);
	float r = p.r*sin(p.zAngle);
	v.x = r*cos(p.xAngle);
	v.y = r*sin(p.xAngle);
}

void Vec3DToColumn(Vec3f& v,Column& c){
	VecToPolar2D(v,c.polar);
	c.z = v.z;
}

void ColumnToVec3D(Vec3f& v,Column& c){
	PolarToVec2D(v,c.polar);
	v.z = c.z;
}

//限制-180到180度
float WrapAngle(float angle,float minAngle=-dPI,float maxAngle=dPI){
	if(angle<minAngle){
		angle+=ceil((minAngle-angle)*d1DIV2PI);
	}
	else if(angle>maxAngle){
		angle-=ceil((angle-maxAngle)*d1DIV2PI);
	}
	return angle;
}

void GetPlane3D(const Vec3f& p0,const Vec3f& p1,const Vec3f& p2,Plane3D& plane){
	GetNormalVec(p0,p1,p2,plane.v);
	plane.p=p0;
}

NS_ENGIN_END