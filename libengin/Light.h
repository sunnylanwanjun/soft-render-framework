#ifndef __LIGHT_H
#define __LIGHT_H
#include "macros.h"
#include "Material.h"
#include "Vec.h"
#include "Ref.h"
NS_ENGIN_BEGIN
#define LIGHT_ATTR_AMBIENT      0x0001
#define LIGHT_ATTR_DIRECTIONAL	0x0002
#define LIGHT_ATTR_POINT        0x0004
#define LIGHT_ATTR_SPOT1		0x0008
#define LIGHT_ATTR_SPOT2		0x0010
#define LIGHT_STATE_ON			1
#define LIGHT_STATE_OFF			0
#define MAX_LIGHT				8

struct ENGIN_DLL Light{
public:
	int state;
	int attr;
	int id;
	RGBA cAmbient;
	RGBA cDiffuse;
	RGBA cSpecular;
	Vec4f pos;

	//���Դ��۹������
	Vec4f dir;
	float kc,kl,kq;//˥������
	//�۹������
	float inner;   //�ڽ�
	float outer;   //���
	float pf;      //ָ��
};

class ENGIN_DLL LightMgr{
public:
	Light lights[MAX_LIGHT];
	int   num_lights;
	LightMgr(){reset();}
	static LightMgr* _instance;
	static LightMgr* getInstance(){
		if(_instance==nullptr){
			_instance=new LightMgr;
		}
		return _instance;
	}
	static void destroyInstance(){
		if(_instance){
			delete _instance;
		}
	}
	void reset(){memset(lights,0,sizeof MAX_LIGHT);num_lights=0;}
	void initLight(int           index,       // index of light to create (0..MAX_LIGHTS-1)
					int          _state,      // state of light
					int          _attr,       // type of light, and extra qualifiers
					RGBA         _c_ambient,  // ambient light intensity
					RGBA         _c_diffuse,  // diffuse light intensity
					RGBA         _c_specular, // specular light intensity
					Vec4f*		 _pos,        // position of light
					Vec4f*		 _dir,        // direction of light
					float        _kc,         // ˥��ϵ�� ������
					float        _kl,		  // ˥��ϵ�� һ����
					float        _kq,         // ˥��ϵ�� ������
					float        _spot_inner, // �۹���ڽ�
					float        _spot_outer, // �۹�����
					float        _pf);
};
NS_ENGIN_END
#endif