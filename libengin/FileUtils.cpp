#include "FileUtils.h"
#include <fstream>
#include <regex>
NS_ENGIN_BEGIN
FileUtils* FileUtils::getInstance(){
	if (_instance == nullptr){
		_instance = new FileUtils;
	}
	return _instance;
}

void FileUtils::destroyInstance(){
	delete FileUtils::getInstance();
}

void FileUtils::getFileName(const char* path,char* fileName){
	std::regex pat("([^\\\\/]+)$");
	std::smatch matches;
	std::string str_path(path);
	if(std::regex_search(str_path,matches,pat)){
		strcpy(fileName,MATCH_STR(1));
	}
}

Data FileUtils::getDataFromFile(const std::string& fileName){
	std::ifstream fin;
	Data data;
	fin.open(fileName,std::ios::in|std::ios::binary);
	if (!fin.is_open()){
		return data;
	}
	fin.seekg(0, std::ios::end);
	auto size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	BYTE* bytes = new BYTE[(int)size];
	data.fastSet(bytes, (int)size);
	fin.read((char*)bytes, size);
	fin.close();
	return data;
}

FileUtils::FileUtils(){

}

FileUtils::~FileUtils(){

}

FileUtils* FileUtils::_instance = nullptr;

NS_ENGIN_END