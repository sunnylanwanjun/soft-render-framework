#include "PlgReader.h"
#include "Object3D.h"
#include "Vec.h"
#include "ParseUtil.h"
#include <regex>
#include "ModelReader.h"
#include "MaterialNode3D.h"
NS_ENGIN_BEGIN
int ReadLine(char* buffer,int bufLen,std::ifstream& in){
	while(true){
		in.getline(buffer,bufLen);
		if(in.gcount()==0)
			break;
		int readCount=strlen(buffer);
		if(readCount==0||buffer[0]=='#'){
			continue;
		}
		int idx=0;
		for(idx=0;idx<readCount&&isspace(buffer[idx]);idx++);
		if(idx>=readCount){
			continue;
		}
		return readCount;
	}
	return 0;
}

bool LoadPlg(ObjNode3D* pObj,const char* plgName,const Vec3f& vs){
	if(pObj==nullptr||plgName==nullptr)return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	std::ifstream in(plgName);
	if(!in.is_open()){
		Log("Error:open plg failed,name is %s",plgName);
		return false;
	}
	char buffer[LOAD_BUFFER_LEN+1];
	int readCount=ReadLine(buffer,LOAD_BUFFER_LEN,in);
	if(readCount==0){
		Log("Error:read plg header failed,name is %s",plgName);
		in.close();
		return false;
	}
	sscanf(buffer,"%s %d %d",&pObj->name,&pObj->numVec,&pObj->numPoly);
	int radiusSum=0;
	int maxRadius=0;
	for(int i=0;i<pObj->numVec;i++){
		readCount=ReadLine(buffer,LOAD_BUFFER_LEN,in);
		if(readCount==0){
			Log("Error:read plg vec failed,name is %s,vec index is %d",plgName,i);
			in.close();
			return false;
		}
		sscanf(buffer,"%f %f %f",
			&pObj->localVecList[i].x,
			&pObj->localVecList[i].y,
			&pObj->localVecList[i].z);
		Vec4f_Mul(pObj->localVecList[i],vs,pObj->localVecList[i]);
		pObj->localVecList[i].w = 1;
	}
	pObj->computeObjRadius();
	//////////////////////////////////////////////////////////////
	char surfaceDesc[8];
	int iSurfaceDesc;
	int poly_num_verts=0;//总是为3
	for(int i=0;i<pObj->numPoly;i++){
		readCount=ReadLine(buffer,LOAD_BUFFER_LEN,in);
		if(readCount==0){
			Log("Error:read plg poly failed,name is %s,poly index is %d",plgName,i);
			in.close();
			return false;
		}
		sscanf(buffer,"%s %d %d %d %d",
			surfaceDesc,
			&poly_num_verts,
			&pObj->polyList[i].verIndex[2],
			&pObj->polyList[i].verIndex[1],
			&pObj->polyList[i].verIndex[0]);
		pObj->polyList[i].pVecList=pObj->worldVecList;
		//面描述符
		if(surfaceDesc[0]=='0'&&surfaceDesc[1]=='x'){
			sscanf(surfaceDesc,"%x",&iSurfaceDesc);
		}else{
			iSurfaceDesc=atoi(surfaceDesc);
		}
		//是否双面
		if((iSurfaceDesc&PLG_SIDE_MASK)==PLG_SIDE_2){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SIDE_2);
		}
		//颜色值
		if((iSurfaceDesc&PLG_COLOR_MASK)==PLG_COLOR_16){
			int r=((iSurfaceDesc&0x0f00)>>4);//>>8<<4
			int g=(iSurfaceDesc&0x00f0);     //>>4<<4
			int b=((iSurfaceDesc&0x000f)<<4);//<<4
			pObj->polyList[i].color=RGB32BIT(0,r,g,b);
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_COLOR_32);
		}else{
			Log("Error:engin can not support 8 bit Object,name is %s",plgName);
			in.close();
			return false;
		}
		//处理着色模式
		int shade=(iSurfaceDesc&PLG_SHADE_MASK);
		switch(shade){
		case PLG_SHADE_PURE: 
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PURE);
			break;
		case PLG_SHADE_FLAT:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_FLAT);
			break;
		case PLG_SHADE_GOURAUD:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_GOURAUD);
			break;
		case PLG_SHADE_PHONG:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PHONG);
			break;
		default:
			Log("Error:engin can not support shade mode %d,plg name is %s",shade,plgName);
			in.close();
			return false;
		}
		SETBIT(pObj->polyList[i].state,POLY_STATE_ACTIVE); 
	}//end for poly
	in.close();
	return true;
}

bool LoadPlg(MaterialNode3D* pObj,const char* plgName,const Vec3f& v){
	ParseUtil parse;
	std::regex pat;
	std::smatch matches;
	if(!parse.open(plgName))return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	//读取名称，顶点个数，多边形个数
	pat="(\\w+)\\s(\\d+)\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	strcpy(pObj->name,MATCH_STR(1));
	pObj->numVec=MATCH_INT(2);
	pObj->numPoly=MATCH_INT(3);
	pObj->numFrame=1;
	pObj->curFrame=0;
	pObj->attr=OBJ_ATTR_SINGLE_FRAME;
	pObj->init(pObj->numVec,pObj->numPoly,1);
	pat="(\\s*[\\de\\.\\-\\+]+)\\s*([\\de\\.\\-\\+]+)\\s*([\\de\\.\\-\\+]+)";
	for(int i=0;i<pObj->numVec;i++){
		if(!parse.getLine(matches,pat)){
			return false;
		}
		pObj->localVecList[i].x=MATCH_FLOAT(1);
		pObj->localVecList[i].y=MATCH_FLOAT(2);
		pObj->localVecList[i].z=MATCH_FLOAT(3);
		SETBIT(pObj->localVecList[i].attr,FVEC4F_ATTR_POINT);
		Vec4f_Mul(pObj->localVecList[i].v4f(),v,pObj->localVecList[i].v4f());
		pObj->localVecList[i].w = 1;
	}
	pObj->computeObjRadius();
	//////////////////////////////////////////////////////////////
	char surfaceDesc[8];
	int iSurfaceDesc;
	int poly_num_verts=0;//总是为3
	pat="(\\w+)\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)";
	for(int i=0;i<pObj->numPoly;i++){
		if(!parse.getLine(matches,pat)){
			return false;
		}
		strcpy(surfaceDesc,MATCH_STR(1));
		poly_num_verts=MATCH_INT(2);
		pObj->polyList[i].verIndex[2]=MATCH_INT(3);
		pObj->polyList[i].verIndex[1]=MATCH_INT(4);
		pObj->polyList[i].verIndex[0]=MATCH_INT(5);
		pObj->polyList[i].pVecList=pObj->worldVecList;
		pObj->polyList[i].pTexList=pObj->texList;
		//面描述符
		if(surfaceDesc[0]=='0'&&surfaceDesc[1]=='x'){
			sscanf(surfaceDesc,"%x",&iSurfaceDesc);
		}else{
			iSurfaceDesc=atoi(surfaceDesc);
		}
		//是否双面
		if((iSurfaceDesc&PLG_SIDE_MASK)==PLG_SIDE_2){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SIDE_2);
		}
		//颜色值
		if((iSurfaceDesc&PLG_COLOR_MASK)==PLG_COLOR_16){
			int r=((iSurfaceDesc&0x0f00)>>4);//>>8<<4
			int g=(iSurfaceDesc&0x00f0);     //>>4<<4
			int b=((iSurfaceDesc&0x000f)<<4);//<<4
			pObj->polyList[i].color=RGB32BIT(0,r,g,b);
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_COLOR_32);
		}else{
			return false;
		}
		//处理着色模式
		int shade=(iSurfaceDesc&PLG_SHADE_MASK);
		switch(shade){
		case PLG_SHADE_PURE: 
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PURE);
			break;
		case PLG_SHADE_FLAT:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_FLAT);
			break;
		case PLG_SHADE_GOURAUD:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_GOURAUD);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[0]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[1]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[2]].attr,
				FVEC4F_ATTR_NORMAL);
			break;
		case PLG_SHADE_PHONG:
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PHONG);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[0]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[1]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[2]].attr,
				FVEC4F_ATTR_NORMAL);
			break;
		default:
			return false;
		}
		SETBIT(pObj->polyList[i].state,POLY_STATE_ACTIVE); 
		//plg不支持材质，这里关闭材质属性
		SETBIT(pObj->polyList[i].attr,POLY_ATTR_DISABLE_MATERIAL);
	}//end for poly
	return true;
}

NS_ENGIN_END