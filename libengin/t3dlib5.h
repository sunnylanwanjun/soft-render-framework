#ifndef __T3DLIB5_H
#define __T3DLIB5_H
#include "macros.h"
NS_ENGIN_BEGIN
class ObjNode3D;
class Camera;
class RenderList;

#define OBJ_MAX_VEC 1024
#define OBJ_MAX_POLY 1024
#define RENDER_MAX_POLY 32768 //2^15

// states of polygons and faces
#define POLY_STATE_ACTIVE             0x0001
#define POLY_STATE_CLIPPED            0x0002
#define POLY_STATE_BACKFACE           0x0004

//�߿�ڵ�״̬
#define OBJ_STATE_NULL                0x0000
#define OBJ_STATE_ACTIVE			  0x0001
#define OBJ_STATE_VISIBLE			  0x0002 
#define OBJ_STATE_CULLED			  0x0004

//���ʽڵ�����
#define OBJ_ATTR_SINGLE_FRAME      0x0001 
#define OBJ_ATTR_MULTI_FRAME       0x0002 
#define OBJ_ATTR_TEXTURES          0x0004 

//���������										//8 4 2 1 8 4 2 1 8 4 2 1 8 4 2 1
#define POLY_ATTR_SIDE_2				0x0001	//							  1
#define POLY_ATTR_TRANSPARENT			0x0002	//							1
#define POLY_ATTR_COLOR_8				0x0004	//						  1
#define POLY_ATTR_COLOR_16				0x0008	//						1
#define POLY_ATTR_COLOR_32				0x0010	//					  1
#define POLY_ATTR_SHADE_PURE            0x0020	//					1
#define POLY_ATTR_SHADE_CONSTANT        0x0020 	//					1
#define POLY_ATTR_SHADE_FLAT            0x0040	//				  1
#define POLY_ATTR_SHADE_GOURAUD         0x0080	//				1
#define POLY_ATTR_SHADE_PHONG           0x0100	//			  1
#define POLY_ATTR_SHADE_FASTPHONG       0x0100 	//			  1
#define POLY_ATTR_SHADE_TEXTURE         0x0200 	//			1
#define POLY_ATTR_ENABLE_MATERIAL       0x0800  //         1
#define POLY_ATTR_DISABLE_MATERIAL      0x1000  //        1

//��������
#define FVEC4F_ATTR_NULL             0x0000
#define FVEC4F_ATTR_POINT            0x0001     //3D�����
#define FVEC4F_ATTR_NORMAL           0x0002     //ӵ�з��ߵĶ���
#define FVEC4F_ATTR_TEXTURE          0x0004     //ӵ����������Ķ���

enum TransType{
	TRANS_LOCAL,			//ֻת������
	TRANS_GLOBAL,			//ֻת��ȫ��
	TRANS_LOCAL_TO_GLOBAL,	//�Ӿֲ�ת��ȫ��
};

enum RenderSortType{
	BY_AVG_Z,
	BY_MAX_Z,
	BY_MIN_Z,
};

#define CULL_X 0x0001  //����ƽ��
#define CULL_Y 0x0002  //����ƽ��
#define CULL_Z 0x0004  //Զ��ƽ��
#define CULL_ALL CULL_X|CULL_Y|CULL_Z //���вü���

#define POLY_NEED_TRANS(poly) \
	(!(poly==nullptr||\
	!(poly->state&POLY_STATE_ACTIVE)||\
	(poly->state&POLY_STATE_CLIPPED)||\
	(poly->state&POLY_STATE_BACKFACE)))
#define POLY_NEED_DRAW(poly) POLY_NEED_TRANS(poly)

NS_ENGIN_END
#endif