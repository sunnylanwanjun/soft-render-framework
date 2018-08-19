#include "Light.h"
NS_ENGIN_BEGIN
void LightMgr::initLight(   int           index,      // index of light to create (0..MAX_LIGHTS-1)
							int          _state,      // state of light
							int          _attr,       // type of light, and extra qualifiers
							RGBA         _c_ambient,  // ambient light intensity
							RGBA         _c_diffuse,  // diffuse light intensity
							RGBA         _c_specular, // specular light intensity
							Vec4f*		 _pos,        // position of light
							Vec4f*  	 _dir,        // direction of light
							float        _kc,         // 衰减系数 常数项
							float        _kl,		  // 衰减系数 一次项
							float        _kq,         // 衰减系数 二次项
							float        _spot_inner, // 聚光灯内角
							float        _spot_outer, // 聚光灯外角
							float        _pf)
{
	Light& pLight=lights[index];
	
	pLight.state=_state;
	pLight.attr=_attr;
	pLight.cAmbient=_c_ambient;
	pLight.cDiffuse=_c_diffuse;
	pLight.cSpecular=_c_specular;
	if(_pos)
		pLight.pos=*_pos;
	if(_dir)
		pLight.dir=*_dir;
	pLight.kc=_kc;
	pLight.kl=_kl;
	pLight.kq=_kq;
	pLight.inner=_spot_inner;   
	pLight.outer=_spot_outer;   
	pLight.pf=_pf;
}
LightMgr* LightMgr::_instance=nullptr;
NS_ENGIN_END