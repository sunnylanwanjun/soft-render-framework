#include "Material.h"
NS_ENGIN_BEGIN
void Material::reset(){
	if(texture.data){
		delete texture.data;
	}
	memset(this,0,sizeof Material);
}

///////////////////////////≤ƒ÷ π‹¿Ì∆˜///////////////////////////////
MaterialMgr* MaterialMgr::_instance=nullptr;
MaterialMgr* MaterialMgr::getInstance(){
	if(_instance==nullptr){
		_instance=new MaterialMgr;
	}
	return _instance;
}

void MaterialMgr::destroyInstance(){
	if(_instance){
		delete _instance;
		_instance=nullptr;
	}
}

MaterialMgr::MaterialMgr():num_materials(0){
	memset(this,0,MAX_MATERIALS);
}

MaterialMgr::~MaterialMgr(){

}

void MaterialMgr::reset(){
	for(int i=0;i<num_materials;i++){
		materials[i].reset();
	}
	num_materials=0;
}

NS_ENGIN_END