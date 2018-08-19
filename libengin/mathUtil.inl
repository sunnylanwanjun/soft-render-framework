#include "macros.h"
#include "mathUtil.h"
NS_ENGIN_BEGIN

struct ENGIN_DLL Mat2X2{
	union{
		float m[4];
		struct{
			float m11;float m12;
			float m21;float m22;
		};
	};
	Mat2X2(){memset(m,0,sizeof m);}
	Mat2X2(float n11,float n12,float n21,float n22):
		m11(n11),m12(n12),m21(n21),m22(n22){
	}
	Mat2X2(Mat2X2& o){memcpy(m,o.m,sizeof m);}
	void multiply(float k){
		m11*=k;m12*=k;
		m21*=k;m22*=k;
	} 
	///////////////////////////////////////////////////////
	//const 接口
	//矩阵的行列式
	float getDet()const{
		return m22*m11-m21*m12;
	}
	//矩阵的行列式的倒数
	float get1DivDet()const{
		float det=getDet();
		if(ABS(det)<=E5)
			return 0;
		return 1/det;
	}
	//伴随矩阵
	Mat2X2 getAdjMat()const{
		return Mat2X2(m22,-m12,-m21,m11);
	}
	//逆矩阵
	Mat2X2 getInverse()const{
		Mat2X2 inverse(m22,-m12,-m21,m11);
		inverse.multiply(get1DivDet());
		return inverse;
	}
};

struct ENGIN_DLL Mat3X1{
	union{
		float m[3];
		struct{
			float m11;
			float m21;
			float m31;
		};
	};
	Mat3X1(){memset(m,0,sizeof m);}
	Mat3X1(float n11,float n21,float n31):
		m11(n11),m21(n21),m31(n31){
	}
};

struct ENGIN_DLL Mat3X3{
	union{
		float m[9];
		struct{
			float m11;float m12;float m13;
			float m21;float m22;float m23;
			float m31;float m32;float m33;
		};
	};
	void multiply(float k){
		for(int i=0;i<9;i++){
			m[i]*=k;
		}
	} 
	//伴随矩阵
	Mat3X3 getAdjMat(){
		Mat3X3 ret;
		ret.m11 = m33*m22-m32*m23;
		ret.m12 = -(m12*m33-m32*m13);
		ret.m13 = m12*m23-m22*m13;
		ret.m21 = m21*m33-m31*m23;
		ret.m22 = -(m11*m33-m31*m13);
		ret.m23 = m11*m23-m21*m13;
		ret.m31 = m21*m32-m31*m22;
		ret.m32 = -(m11*m32-m31*m12);
		ret.m33 = m11*m22-m21*m12;
		return ret;
	}
	Mat3X3 getInverse(){
		Mat3X3 inverse = getAdjMat();
		inverse.multiply(get1DivDet());
		return inverse;
	}
	Mat3X3(){memset(m,0,sizeof m);}
	Mat3X3(	float n11,float n12,float n13,
		float n21,float n22,float n23,
		float n31,float n32,float n33):
	m11(n11),m12(n12),m13(n13),
		m21(n21),m22(n22),m23(n23),
		m31(n31),m32(n32),m33(n33){
	}
	Mat3X3(Mat3X3& o){memcpy(m,o.m,sizeof m);}
	//拷贝一列
	void setCol(Mat3X1& o,int col){
		m[col-1]=o.m11;
		m[col+2]=o.m21;
		m[col+5]=o.m31;
	}
	float getDet()const{
		return m11*(m22*m33-m32*m23)-m12*(m21*m33-m31*m23)+m13*(m21*m32-m31*m22);
	}
	float get1DivDet()const{
		float det=getDet();
		if(ABS(det)<=E5)
			return 0;
		return 1/det;
	}
};

struct ENGIN_DLL Mat2X1{
	union{
		float m[2];
		struct{
			float m11;
			float m21;
		};
	};
	Mat2X1(){memset(m,0,sizeof m);}
	Mat2X1(float n11,float n21):
		m11(n11),m21(n21){
	}
};

inline Mat2X1 ENGIN_DLL operator*(Mat2X2& m0,Mat2X1 m1){
		Mat2X1 m2;
		m2.m11 = m0.m11*m1.m11+m0.m12*m1.m21;
		m2.m21 = m0.m21*m1.m11+m0.m22*m1.m21;
		return m2;
} 	

struct ENGIN_DLL Plane3D{
	Vec3f p;
	Vec3f v;
};

/*
求解二元一次方程组
A*x+B*y=C
D*x+E*y=F
|A B||x| = |C|
|C D||y|   |F|
*/
inline bool ENGIN_DLL Equation2(Mat2X2& coef,Mat2X1& res,float* x,float* y){
	//矩阵行列式
	float _1divdet=coef.get1DivDet();
	if(ABS(_1divdet)<=E5)
		return false;
	Mat2X2 inverse=coef.getInverse();
	Mat2X1 ret=inverse*res;
	*x=ret.m11;
	*y=ret.m21;
	return true;
}

/*
求解三元一次方程组
A*x+B*y+C*z=D
D*x+E*y+G*z=H
I*x+J*y+K*z=L
|x| = |A B C||D|
|y|   |D E G||H|
|z|   |I J K||L|
*/
inline bool ENGIN_DLL Equation3(Mat3X3& coef,Mat3X1& res,float* x,float* y,float *z){
	float _1divdet=coef.get1DivDet();
	if(_1divdet==0)
		return false;
	Mat3X3 numerator=coef;
	numerator.setCol(res,1);
	*x=numerator.getDet()*_1divdet;
	numerator=coef;
	numerator.setCol(res,2);
	*y=numerator.getDet()*_1divdet;
	numerator=coef;
	numerator.setCol(res,3);
	*z=numerator.getDet()*_1divdet;
	return true;
}

inline void ENGIN_DLL FIXP16_Mul(FIXP16 f0,FIXP16 f1,FIXP16& f2){
	__asm{
		mov eax,f0;
		imul f1;
		shrd eax,edx,16;
		mov edx,f2;
		mov dword ptr[edx],eax;
	};
}

inline void ENGIN_DLL FIXP16_Div(FIXP16 f0,FIXP16 f1,FIXP16& f2){
	//if(f0>f1){
		__asm{
			mov eax,f0;
			cdq;
			shld edx,eax,16;
			sal eax,16;
			idiv f1;
			mov edx,f2;
			mov dword ptr[edx],eax;
		};
	/*}else{
		static float fixp16_mag=65536.0f;
		__asm{
			fild f0;
			fild f1;
			fdiv;
			fmul fixp16_mag;
			mov edx,f2;
			fistp dword ptr[edx];
		};
	}*/
}

inline void FAdd(float f0,float f1,float& f2){
	__asm{
		fld f0;
		fadd f1;
		fstp f2;
	};
}

inline void FAdd(float f0,int i1,float& f2){
	__asm{
		fild i1;
		fadd f0;
		fstp f2;
	};
}

inline void FAdd(float f0,int i1,int& i2){
	__asm{
		fild i1;
		fadd f0;
		fistp i2;
	};
}

inline void FSub(float f0,float f1,float& f2){
	__asm{
		fld f0;
		fsub f1;
		fstp f2;
	};
}

inline void FSub(float f0,int i1,float& f2){
	__asm{
		fld f0;
		fild i1;
		fsub;
		fstp f2;
	};
}

inline void FSub(float f0,int i1,int& i2){
	__asm{
		fld f0;
		fild i1;
		fsub;
		fistp i2;
	};
}

inline void FMul(float f0,float f1,float& f2){
	__asm{
		fld f0;
		fmul f1;
		fstp f2;
	}
}

inline void FMul(float f0,int i1,float& f2){
	__asm{
		fild i1;
		fmul f0;
		fstp f2;
	}
}

inline void FMul(float f0,int i1,int& i2){
	__asm{
		fild i1;
		fmul f0;
		Fistp i2;
	}
}

inline void FDiv(float f0,float f1,float& f2){
	__asm{
		fld f0;
		fdiv f1;
		fstp f2;
	};
}

inline void FDiv(float f0,int i1,float& f2){
	__asm{
		fld f0;
		fild i1;
		fdiv;
		fstp f2;
	};
}


inline void FDiv(float f0,int i1,int& i2){
	float f9;
	__asm{
		fld f9;
		fild i1;
		fdiv;
		fistp i2;
	};
}

NS_ENGIN_END