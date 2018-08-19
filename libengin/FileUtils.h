#ifndef __FILEUTILS_H
#define __FILEUTILS_H
#include "macros.h"
#include <string>
#include "Data.h"
NS_ENGIN_BEGIN
class ENGIN_DLL FileUtils
{
public:
	static FileUtils* getInstance();
	static void destroyInstance();
	static void getFileName(const char* path,char* fileName);
	static void packTexPath(char* fileName){
		static char tempTex[256];
		sprintf(tempTex,"%s%s","./",fileName);
		memcpy(fileName,tempTex,strlen(tempTex)+1);
	}
	Data getDataFromFile(const std::string& fileName);
private:
	FileUtils();
	~FileUtils();
private:
	static FileUtils* _instance;
};
NS_ENGIN_END
#endif
