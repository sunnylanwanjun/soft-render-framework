#include "ModelReader.h"
#include "ParseUtil.h"
#include <regex>
#include "Object3D.h"
#include "Material.h"
#include "MaterialNode3D.h"
#include "t3dlib1.h"
#include "FileUtils.h"
NS_ENGIN_BEGIN
bool LoadAsc( ObjNode3D* pObj,
						 const char* ascName,
						 const Vec3f& vs,
						 int mode )
{
	ParseUtil parse;
	std::regex pat;
	std::smatch matches;
	if(!parse.open(ascName))return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	pat="Named object:\\s*\"(\\w+)\"";
	if(parse.getLine(matches,pat)){
		strcpy(pObj->name,MATCH_STR(1));
	}
	
	pat="(\\d+)\\D*(\\d+)";
	if(!parse.getLine(matches,pat))
		return false;
	pObj->numVec=MATCH_INT(1);
	pObj->numPoly=MATCH_INT(2);

	int vecIdx=0;
	pat="X:([\\de\\.\\-\\+]+)\\s*Y:([\\de\\.\\-\\+]+)\\s*Z:([\\de\\.\\-\\+]+)";
	while(vecIdx<pObj->numVec){
		if(parse.getLine(matches,pat)){
			pObj->localVecList[vecIdx].x=MATCH_FLOAT(1);	
			pObj->localVecList[vecIdx].y=MATCH_FLOAT(2);	
			pObj->localVecList[vecIdx].z=MATCH_FLOAT(3);

			float temp;
			if(mode&LOAD_SWAP_XY){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].y);
			}
			if(mode&LOAD_SWAP_XZ){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].z);
			}
			if(mode&LOAD_SWAP_YZ){
				SWAP(temp,pObj->localVecList[vecIdx].y,pObj->localVecList[vecIdx].z);
			}
			Vec4f_Mul(pObj->localVecList[vecIdx],vs,pObj->localVecList[vecIdx]);
			pObj->localVecList[vecIdx].w = 1;
			vecIdx++;
		}
	}
	pObj->computeObjRadius();
	/////////////////////////////////////////////////////////////
	//�����
	int polyIdx=0;
	pat="A:(\\d+)\\s*B:(\\d+)\\s*C:(\\d+)";
	std::regex patColor("r(\\d+)g(\\d+)b(\\d+)a(\\d+)");
	while(polyIdx<pObj->numPoly){
		if(parse.getLine(matches,pat)){
			pObj->polyList[polyIdx].verIndex[2]=MATCH_INT(1);	
			pObj->polyList[polyIdx].verIndex[1]=MATCH_INT(2);	
			pObj->polyList[polyIdx].verIndex[0]=MATCH_INT(3);
			pObj->polyList[polyIdx].pVecList=pObj->worldVecList;
		}
		if(parse.getLine(matches,patColor)){
			int r=MATCH_INT(1);
			int g=MATCH_INT(2);
			int b=MATCH_INT(3);
			int a=MATCH_INT(4);
			pObj->polyList[polyIdx].color=RGB32BIT(a,r,g,b);
			SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_COLOR_32);
			SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_SHADE_FLAT);
			SETBIT(pObj->polyList[polyIdx].state,POLY_STATE_ACTIVE); 
			polyIdx++;
		}
	}
	return true;
}

bool LoadAsc( MaterialNode3D* pObj,
			 const char* ascName,
			 const Vec3f& vs,
			 int mode )
{
	ParseUtil parse;
	std::regex pat;
	std::smatch matches;
	if(!parse.open(ascName))return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	pat="Named object:\\s*\"(\\w+)\"";
	if(parse.getLine(matches,pat)){
		strcpy(pObj->name,MATCH_STR(1));
	}

	pat="(\\d+)\\D*(\\d+)";
	if(!parse.getLine(matches,pat))
		return false;
	pObj->numVec=MATCH_INT(1);
	pObj->numPoly=MATCH_INT(2);
	pObj->numFrame=1;
	pObj->curFrame=0;
	pObj->attr=OBJ_ATTR_SINGLE_FRAME;
	pObj->init(pObj->numVec,pObj->numPoly,1);
	
	int vecIdx=0;
	pat="X:([\\de\\.\\-\\+]+)\\s*Y:([\\de\\.\\-\\+]+)\\s*Z:([\\de\\.\\-\\+]+)";
	while(vecIdx<pObj->numVec){
		if(parse.getLine(matches,pat)){
			pObj->localVecList[vecIdx].x=MATCH_FLOAT(1);	
			pObj->localVecList[vecIdx].y=MATCH_FLOAT(2);	
			pObj->localVecList[vecIdx].z=MATCH_FLOAT(3);
			SETBIT(pObj->localVecList[vecIdx].attr,FVEC4F_ATTR_POINT);

			float temp;
			if(mode&LOAD_SWAP_XY){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].y);
			}
			if(mode&LOAD_SWAP_XZ){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].z);
			}
			if(mode&LOAD_SWAP_YZ){
				SWAP(temp,pObj->localVecList[vecIdx].y,pObj->localVecList[vecIdx].z);
			}
			Vec4f_Mul(pObj->localVecList[vecIdx].v4f(),vs,pObj->localVecList[vecIdx].v4f());
			pObj->localVecList[vecIdx].w = 1;
			vecIdx++;
		}
	}
	pObj->computeObjRadius();
	/////////////////////////////////////////////////////////////
	//�����
	int polyIdx=0;
	pat="A:(\\d+)\\s*B:(\\d+)\\s*C:(\\d+)";
	std::regex patColor("r(\\d+)g(\\d+)b(\\d+)a(\\d+)");
	while(polyIdx<pObj->numPoly){
		if(parse.getLine(matches,pat)){
			pObj->polyList[polyIdx].verIndex[2]=MATCH_INT(1);	
			pObj->polyList[polyIdx].verIndex[1]=MATCH_INT(2);	
			pObj->polyList[polyIdx].verIndex[0]=MATCH_INT(3);
			pObj->polyList[polyIdx].pVecList=pObj->worldVecList;
			pObj->polyList[polyIdx].pTexList=pObj->texList;
		}
		if(parse.getLine(matches,patColor)){
			int r=MATCH_INT(1);
			int g=MATCH_INT(2);
			int b=MATCH_INT(3);
			int a=MATCH_INT(4);
			pObj->polyList[polyIdx].color=RGB32BIT(a,r,g,b);
			SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_COLOR_32);
			SETBIT(pObj->polyList[polyIdx].state,POLY_STATE_ACTIVE);

			if(mode&LOAD_SHADE_CONSTANT){
				SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_SHADE_PURE);
			}
			else if(mode&LOAD_SHADE_FLAT){
				SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_SHADE_FLAT);
			}
			else if(mode&LOAD_SHADE_GOURAUD){
				SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_SHADE_GOURAUD);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[0]].attr,
					FVEC4F_ATTR_NORMAL);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[1]].attr,
					FVEC4F_ATTR_NORMAL);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[2]].attr,
					FVEC4F_ATTR_NORMAL);
			}
			else if(mode&LOAD_SHADE_PHONG){
				SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_SHADE_PHONG);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[0]].attr,
					FVEC4F_ATTR_NORMAL);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[1]].attr,
					FVEC4F_ATTR_NORMAL);
				SETBIT(pObj->polyList[polyIdx].pVecList[pObj->polyList[polyIdx].verIndex[2]].attr,
					FVEC4F_ATTR_NORMAL);
			}
			//plg��֧�ֲ��ʣ�����رղ�������
			SETBIT(pObj->polyList[polyIdx].attr,POLY_ATTR_DISABLE_MATERIAL);
			polyIdx++;
		}
	}
	return true;
}

bool LoadCob(ObjNode3D* pObj,const char* cobName,const Vec3f& vs,int mode){
	ParseUtil parse;
	std::regex pat;
	std::smatch matches;
	if(!parse.open(cobName))return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	pat="Name\\s*(\\w+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	strcpy(pObj->name,MATCH_STR(1));

	//ƽ�ƾ���
	Mat4 tm;
	//ƽ����
	pat="center\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._41=MATCH_FLOAT(1);
	tm._42=MATCH_FLOAT(2);
	tm._43=MATCH_FLOAT(3);
	//x��
	pat="x\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._11=MATCH_FLOAT(1);
	tm._21=MATCH_FLOAT(2);
	tm._31=MATCH_FLOAT(3);
	//y��
	pat="y\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._12=MATCH_FLOAT(1);
	tm._22=MATCH_FLOAT(2);
	tm._32=MATCH_FLOAT(3);
	//z��
	pat="z\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._13=MATCH_FLOAT(1);
	tm._23=MATCH_FLOAT(2);
	tm._33=MATCH_FLOAT(3);

	//////////////////////////////////////////////////////////////////////
	//��ת����
	Mat4 rm;
	pat="Transform";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	for(int i=0;i<4;i++){
		if(!parse.getLine(matches,pat)){
			return false;
		}
		rm.m[i<<2]=MATCH_FLOAT(1);
		rm.m[(i<<2)+1]=MATCH_FLOAT(2);
		rm.m[(i<<2)+2]=MATCH_FLOAT(3);
		rm.m[(i<<2)+3]=MATCH_FLOAT(4);
	}
	Mat4_Mul(rm,tm,rm);
	/////////////////////////////////
	//����
	pat="World Vertices\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pObj->numVec=MATCH_INT(1);

	int vecIdx=0;
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	while(vecIdx<pObj->numVec){
		if(parse.getLine(matches,pat)){
			pObj->localVecList[vecIdx].x=MATCH_FLOAT(1);	
			pObj->localVecList[vecIdx].y=MATCH_FLOAT(2);	
			pObj->localVecList[vecIdx].z=MATCH_FLOAT(3);
			Mat4_Mul(pObj->localVecList[vecIdx],rm,pObj->localVecList[vecIdx]);

			float temp;
			if(mode&LOAD_INVER_X){
				pObj->localVecList[vecIdx].x=-pObj->localVecList[vecIdx].x;
			}
			if(mode&LOAD_INVER_Y){
				pObj->localVecList[vecIdx].y=-pObj->localVecList[vecIdx].y;
			}
			if(mode&LOAD_INVER_Z){
				pObj->localVecList[vecIdx].z=-pObj->localVecList[vecIdx].z;
			}
			if(mode&LOAD_SWAP_XY){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].y);
			}
			if(mode&LOAD_SWAP_XZ){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].z);
			}
			if(mode&LOAD_SWAP_YZ){
				SWAP(temp,pObj->localVecList[vecIdx].y,pObj->localVecList[vecIdx].z);
			}
			Vec4f_Mul(pObj->localVecList[vecIdx],vs,pObj->localVecList[vecIdx]);
			pObj->localVecList[vecIdx].w = 1;
			vecIdx++;
		}
	}
	pObj->computeObjRadius();
	/////////////////////////////////////
	//��������  ��ʱû��
	pat="Texture Vertices\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	int numTex=MATCH_INT(1); //��������ĸ���

	int texIdx=0;
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	while(texIdx<numTex){
		if(parse.getLine(matches,pat)){
			//MATCH_FLOAT(1);	//x
			//MATCH_FLOAT(2);	//y
			texIdx++;
		}
	}

	/////////////////////////////////////
	//��
	pat="Faces\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pObj->numPoly=MATCH_INT(1);

	int polyIdx=0;
	int polyMatType[OBJ_MAX_POLY]={0};
	int materialMap[MAX_MATERIALS]={0};
	int useMaterialNum=0;
	pat="Face verts\\s(\\d)\\sflags\\s(\\d)";
	std::regex patVecIdx("<(\\d+),\\d+>\\s*<(\\d+),\\d+>\\s*<(\\d+),\\d+>");
	while(polyIdx<pObj->numPoly){
		if(parse.getLine(matches,pat)){
			if(MATCH_INT(1)!=3)
				return false;
			polyMatType[polyIdx]=MATCH_INT(2);// ���ʺ�
			//��¼һ��ʹ���˶������ز�
			if(materialMap[polyMatType[polyIdx]]==0){
				materialMap[polyMatType[polyIdx]]=1;
				++useMaterialNum;
			}
		}
		if(parse.getLine(matches,patVecIdx)){
			pObj->polyList[polyIdx].verIndex[2]=MATCH_INT(1);	
			pObj->polyList[polyIdx].verIndex[1]=MATCH_INT(2);	
			pObj->polyList[polyIdx].verIndex[0]=MATCH_INT(3);
			pObj->polyList[polyIdx].pVecList=pObj->worldVecList;
			pObj->polyList[polyIdx].state = POLY_STATE_ACTIVE;
			polyIdx++;
		}
	}

	/////////////////////////////////////
	//��ʼ���ز�
	int matIdx=0;
	int curMatType=0;
	Material* matList=MaterialMgr::getInstance()->materials;
	int matBase=MaterialMgr::getInstance()->num_materials;
	MaterialMgr::getInstance()->num_materials+=useMaterialNum;
	std::regex pat_matType("mat#\\s(\\d+)");
	std::regex pat_shaderClass("Shader\\sclass:\\s(\\w+)");
	std::regex pat_shaderName("Shader\\sname:\\s\"(\\w+\\s*\\w*)");
	std::regex pat_rgb("rgb\\s(\\d+\\.?\\d*),(\\d+\\.?\\d*),(\\d+\\.?\\d*)");
	std::regex pat_coef("alpha\\s(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)");
	std::regex pat_fileName("file\\sname:\\sstring\\s\"([^\"]*)");
	while(matIdx<useMaterialNum){
		//��ȡ���ʺ�
		if(parse.getLine(matches,pat_matType)){
			curMatType=MATCH_INT(1);
			matIdx++;
		}
		//��ȡ��ɫ
		int absIdx=curMatType+matBase;
		if(parse.getLine(matches,pat_rgb)){
			matList[absIdx].color.r=(int)(MATCH_FLOAT(1)*255+0.5);
			matList[absIdx].color.g=(int)(MATCH_FLOAT(2)*255+0.5);
			matList[absIdx].color.b=(int)(MATCH_FLOAT(3)*255+0.5);
		}
		//��ȡalpha
		if(parse.getLine(matches,pat_coef)){
			matList[absIdx].color.a=(int)(MATCH_FLOAT(1)*255+0.5);
			matList[absIdx].ka=MATCH_FLOAT(2);
			matList[absIdx].kd=1;
			matList[absIdx].ks=MATCH_FLOAT(3);
			matList[absIdx].power=MATCH_FLOAT(4);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].ka,matList[absIdx].ra);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].kd,matList[absIdx].rd);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].ks,matList[absIdx].rs);
		}
		
		//��ȡ��ɫ����
		static char shaderClass[256];
		static char shaderName[256];
		bool setShadeMode=false;
		while(true){
			if(parse.getLine(matches,pat_shaderClass)){
				strcpy(shaderClass,MATCH_STR(1));
			}
			//��ȡ��ɫ����
			if(parse.getLine(matches,pat_shaderName)){
				strcpy(shaderName,MATCH_STR(1));
			}
			if(strcmp(shaderClass,"color")==0){
				if(strcmp(shaderName,"texture map")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_TEXTURE);
					parse.getLine(matches,pat_fileName);
					strcpy(matList[absIdx].textureName,MATCH_STR(1));
				}
			}
			else if(strcmp(shaderClass,"reflectance")==0){
				setShadeMode=true;
				if(strcmp(shaderName,"constant")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_CONSTANT);
				}
				else if(strcmp(shaderName,"plastic")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_GOURAUD);
				}
				else if(strcmp(shaderName,"phong")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_PHONG);
				}
				else{
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_FLAT);
				}
				break;
			}
		}
		//����Ĭ����ɫ��ʽ
		if(!setShadeMode){
			SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_FLAT);
		}
	}

	//////////////////////////////////////////////////////
	//���ݶ����ʹ�ò��ʣ����ö���ε���ɫ����
	for(int i=0;i<pObj->numPoly;i++){
		Material& mat=matList[polyMatType[i]+matBase];
		pObj->polyList[i].color=RGBA2ARGB(mat.color.rgba);
		SETBIT(pObj->polyList[i].attr,POLY_ATTR_COLOR_32);
		SETBIT(pObj->polyList[i].state,POLY_STATE_ACTIVE);
		if(mat.attr&MATL_ATTR_SHADE_CONSTANT){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_CONSTANT);
		}
		else if(mat.attr&MATL_ATTR_SHADE_GOURAUD){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_GOURAUD);
		}
		else if(mat.attr&MATL_ATTR_SHADE_PHONG){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PHONG);
		}
		else if(mat.attr&MATL_ATTR_SHADE_FLAT){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_FLAT);
		}
	}
	return true;
}

bool LoadCob(MaterialNode3D* pObj,const char* cobName,const Vec3f& vs,int mode){
	ParseUtil parse;
	std::regex pat;
	std::smatch matches;
	if(!parse.open(cobName))return false;
	pObj->state = OBJ_STATE_ACTIVE|OBJ_STATE_VISIBLE;
	pat="Name\\s*(\\w+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	strcpy(pObj->name,MATCH_STR(1));

	//ƽ�ƾ���
	Mat4 tm;
	//ƽ����
	pat="center\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._41=MATCH_FLOAT(1);
	tm._42=MATCH_FLOAT(2);
	tm._43=MATCH_FLOAT(3);
	//x��
	pat="x\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._11=MATCH_FLOAT(1);
	tm._21=MATCH_FLOAT(2);
	tm._31=MATCH_FLOAT(3);
	//y��
	pat="y\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._12=MATCH_FLOAT(1);
	tm._22=MATCH_FLOAT(2);
	tm._32=MATCH_FLOAT(3);
	//z��
	pat="z\\s\\w{4}\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	tm._13=MATCH_FLOAT(1);
	tm._23=MATCH_FLOAT(2);
	tm._33=MATCH_FLOAT(3);

	//////////////////////////////////////////////////////////////////////
	//��ת����
	Mat4 rm;
	pat="Transform";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	for(int i=0;i<4;i++){
		if(!parse.getLine(matches,pat)){
			return false;
		}
		rm.m[i<<2]=MATCH_FLOAT(1);
		rm.m[(i<<2)+1]=MATCH_FLOAT(2);
		rm.m[(i<<2)+2]=MATCH_FLOAT(3);
		rm.m[(i<<2)+3]=MATCH_FLOAT(4);
	}
	Mat4_Mul(rm,tm,rm);
	/////////////////////////////////
	//����
	pat="World Vertices\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pObj->numVec=MATCH_INT(1);
	pObj->numPoly=pObj->numVec*3;//��Ȼ��������п��ܱ������ĿҪ��
	pObj->numFrame=1;
	pObj->curFrame=0;
	pObj->attr=OBJ_ATTR_SINGLE_FRAME;
	pObj->init(pObj->numVec,pObj->numPoly,1);

	int vecIdx=0;
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	while(vecIdx<pObj->numVec){
		if(parse.getLine(matches,pat)){
			pObj->localVecList[vecIdx].x=MATCH_FLOAT(1);	
			pObj->localVecList[vecIdx].y=MATCH_FLOAT(2);	
			pObj->localVecList[vecIdx].z=MATCH_FLOAT(3);
			Mat4_Mul(pObj->localVecList[vecIdx].v4f(),rm,pObj->localVecList[vecIdx].v4f());

			float temp;
			if(mode&LOAD_INVER_X){
				pObj->localVecList[vecIdx].x=-pObj->localVecList[vecIdx].x;
			}
			if(mode&LOAD_INVER_Y){
				pObj->localVecList[vecIdx].y=-pObj->localVecList[vecIdx].y;
			}
			if(mode&LOAD_INVER_Z){
				pObj->localVecList[vecIdx].z=-pObj->localVecList[vecIdx].z;
			}
			if(mode&LOAD_SWAP_XY){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].y);
			}
			if(mode&LOAD_SWAP_XZ){
				SWAP(temp,pObj->localVecList[vecIdx].x,pObj->localVecList[vecIdx].z);
			}
			if(mode&LOAD_SWAP_YZ){
				SWAP(temp,pObj->localVecList[vecIdx].y,pObj->localVecList[vecIdx].z);
			}
			Vec4f_Mul(pObj->localVecList[vecIdx].v4f(),vs,pObj->localVecList[vecIdx].v4f());
			pObj->localVecList[vecIdx].w = 1;
			vecIdx++;
		}
	}
	pObj->computeObjRadius();
	/////////////////////////////////////
	//��������  ��ʱû��
	pat="Texture Vertices\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	int numTex=MATCH_INT(1); //��������ĸ���

	int texIdx=0;
	pat="([\\de\\.\\-\\+]+)\\s([\\de\\.\\-\\+]+)";
	while(texIdx<numTex){
		if(parse.getLine(matches,pat)){
			pObj->texList[texIdx].x=MATCH_FLOAT(1);	//x
			pObj->texList[texIdx].y=MATCH_FLOAT(2);	//y
			texIdx++;
		}
	}

	/////////////////////////////////////
	//��
	pat="Faces\\s(\\d+)";
	if(!parse.getLine(matches,pat)){
		return false;
	}
	pObj->numPoly=MATCH_INT(1);

	int polyIdx=0;
	int polyMatType[OBJ_MAX_POLY]={0};
	int materialMap[MAX_MATERIALS]={0};
	int useMaterialNum=0;
	pat="Face verts\\s(\\d)\\sflags\\s\\d+\\smat\\s(\\d+)";
	std::regex patVecIdx("<(\\d+),(\\d+)>\\s*<(\\d+),(\\d+)>\\s*<(\\d+),(\\d+)>");
	while(polyIdx<pObj->numPoly){
		if(parse.getLine(matches,pat)){
			if(MATCH_INT(1)!=3)
				return false;
			polyMatType[polyIdx]=MATCH_INT(2);// ���ʺ�
			//��¼һ��ʹ���˶������ز�
			if(materialMap[polyMatType[polyIdx]]==0){
				materialMap[polyMatType[polyIdx]]=1;
				++useMaterialNum;
			}
		}
		if(parse.getLine(matches,patVecIdx)){
			pObj->polyList[polyIdx].pVecList=pObj->worldVecList;
			pObj->polyList[polyIdx].verIndex[2]=MATCH_INT(1);	
			pObj->polyList[polyIdx].verIndex[1]=MATCH_INT(3);	
			pObj->polyList[polyIdx].verIndex[0]=MATCH_INT(5);
			pObj->polyList[polyIdx].pTexList=pObj->texList;
			pObj->polyList[polyIdx].texIndex[2]=MATCH_INT(2);	
			pObj->polyList[polyIdx].texIndex[1]=MATCH_INT(4);	
			pObj->polyList[polyIdx].texIndex[0]=MATCH_INT(6);
			pObj->polyList[polyIdx].state = POLY_STATE_ACTIVE;
			polyIdx++;
		}
	}

	/////////////////////////////////////
	//��ʼ���ز�
	int matIdx=0;
	int curMatType=0;
	Material* matList=MaterialMgr::getInstance()->materials;
	int matBase=MaterialMgr::getInstance()->num_materials;
	MaterialMgr::getInstance()->num_materials+=useMaterialNum;
	std::regex pat_matType("mat#\\s(\\d+)");
	std::regex pat_shaderClass("Shader\\sclass:\\s(\\w+)");
	std::regex pat_shaderName("Shader\\sname:\\s\"(\\w+\\s*\\w*)");
	std::regex pat_rgb("rgb\\s(\\d+\\.?\\d*),(\\d+\\.?\\d*),(\\d+\\.?\\d*)");
	std::regex pat_coef("alpha\\s(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)\\D*(\\d+\\.?\\d*)");
	std::regex pat_fileName("file\\sname:\\sstring\\s\"([^\"]*)");
	while(matIdx<useMaterialNum){
		//��ȡ���ʺ�
		if(parse.getLine(matches,pat_matType)){
			curMatType=MATCH_INT(1);
			matIdx++;
		}
		//��ȡ��ɫ
		int absIdx=curMatType+matBase;
		if(parse.getLine(matches,pat_rgb)){
			matList[absIdx].color.r=(int)(MATCH_FLOAT(1)*255+0.5);
			matList[absIdx].color.g=(int)(MATCH_FLOAT(2)*255+0.5);
			matList[absIdx].color.b=(int)(MATCH_FLOAT(3)*255+0.5);
		}
		//��ȡalpha
		if(parse.getLine(matches,pat_coef)){
			matList[absIdx].color.a=(int)(MATCH_FLOAT(1)*255+0.5);
			matList[absIdx].ka=MATCH_FLOAT(2);
			matList[absIdx].kd=1;
			matList[absIdx].ks=MATCH_FLOAT(3);
			matList[absIdx].power=MATCH_FLOAT(4);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].ka,matList[absIdx].ra);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].kd,matList[absIdx].rd);
			RGBA_MUL(matList[absIdx].color,matList[absIdx].ks,matList[absIdx].rs);
		}

		//��ȡ��ɫ����
		static char shaderClass[256];
		static char shaderName[256];
		bool setShadeMode=false;
		while(true){
			if(parse.getLine(matches,pat_shaderClass)){
				strcpy(shaderClass,MATCH_STR(1));
			}
			//��ȡ��ɫ����
			if(parse.getLine(matches,pat_shaderName)){
				strcpy(shaderName,MATCH_STR(1));
			}
			if(strcmp(shaderClass,"color")==0){
				if(strcmp(shaderName,"texture map")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_TEXTURE);
					parse.getLine(matches,pat_fileName);
					FileUtils::getFileName(MATCH_STR(1),matList[absIdx].textureName);
					FileUtils::packTexPath(matList[absIdx].textureName);
					if(pObj->texture==nullptr){
						GetBmp(matList[absIdx].textureName,&pObj->texture);
						SETBIT(pObj->attr,OBJ_ATTR_TEXTURES);
					}
				}
			}
			else if(strcmp(shaderClass,"reflectance")==0){
				setShadeMode=true;
				if(strcmp(shaderName,"constant")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_CONSTANT);
				}
				else if(strcmp(shaderName,"plastic")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_GOURAUD);
				}
				else if(strcmp(shaderName,"phong")==0){
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_PHONG);
				}
				else{
					SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_FLAT);
				}
				break;
			}
		}
		//����Ĭ����ɫ��ʽ
		if(!setShadeMode){
			SETBIT(matList[absIdx].attr,MATL_ATTR_SHADE_FLAT);
		}
	}

	//////////////////////////////////////////////////////
	//���ݶ����ʹ�ò��ʣ����ö���ε���ɫ����
	for(int i=0;i<pObj->numPoly;i++){
		Material& mat=matList[polyMatType[i]+matBase];
		pObj->polyList[i].color=RGBA2ARGB(mat.color.rgba);
		SETBIT(pObj->polyList[i].attr,POLY_ATTR_COLOR_32);
		SETBIT(pObj->polyList[i].state,POLY_STATE_ACTIVE);
		if(mat.attr&MATL_ATTR_SHADE_CONSTANT){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_CONSTANT);
		}
		else if(mat.attr&MATL_ATTR_SHADE_GOURAUD){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_GOURAUD);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[0]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[1]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[2]].attr,
				FVEC4F_ATTR_NORMAL);
		}
		else if(mat.attr&MATL_ATTR_SHADE_PHONG){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_PHONG);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[0]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[1]].attr,
				FVEC4F_ATTR_NORMAL);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[2]].attr,
				FVEC4F_ATTR_NORMAL);
		}
		else if(mat.attr&MATL_ATTR_SHADE_FLAT){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_FLAT);
		}

		if(mat.attr&MATL_ATTR_SHADE_TEXTURE){
			SETBIT(pObj->polyList[i].attr,POLY_ATTR_SHADE_TEXTURE);
			pObj->polyList[i].pTexture=pObj->texture;
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[0]].attr,
				FVEC4F_ATTR_TEXTURE);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[1]].attr,
				FVEC4F_ATTR_TEXTURE);
			SETBIT(pObj->polyList[i].pVecList[pObj->polyList[i].verIndex[2]].attr,
				FVEC4F_ATTR_TEXTURE);
		}
		//�رղ�������???
		SETBIT(pObj->polyList[i].attr,POLY_ATTR_DISABLE_MATERIAL);
	}
	
	if(pObj->attr&OBJ_ATTR_TEXTURES){
		for(int i=0;i<numTex;i++){
			//���ڼ�¼�Ĳ�����0��1�ģ����ڸ��ݲ��ʵĿ�߽�������
			pObj->texList[i].x*=(pObj->texture->width-1);
			pObj->texList[i].y*=(pObj->texture->height-1);
			//��תX
			if(mode&LOAD_TEXTURE_INVER_X){
				pObj->texList[i].x=-pObj->texList[i].x;
			}
			//��תY
			if(mode&LOAD_TEXTURE_INVER_Y){
				pObj->texList[i].y=-pObj->texList[i].y;
			}
			//����XY
			if(mode&LOAD_TEXTURE_SWAP_XY){
				float temp;
				SWAP(temp,pObj->texList[i].x,pObj->texList[i].y);
			}
		}
	}
	MatPoly_VecIdx* newPolyList=new MatPoly_VecIdx[pObj->numPoly];
	memcpy(newPolyList,pObj->polyList,pObj->numPoly*sizeof MatPoly_VecIdx);
	delete[] pObj->polyList;
	pObj->polyList=newPolyList;
	return true;
}

NS_ENGIN_END
