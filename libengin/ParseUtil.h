#ifndef __PARSEUTIL_H
#define __PARSEUTIL_H
#include "macros.h"
#include <fstream>
#include <regex>
NS_ENGIN_BEGIN
#define PARSER_BUFFER_SIZE              256
extern ENGIN_DLL void trimLeft(char* buffer);
extern ENGIN_DLL void trimRight(char* buffer);
class ENGIN_DLL ParseUtil{
public:
	ParseUtil();
	~ParseUtil();
	bool open(const char* fileName);
	int getLine();
	int getLine(std::smatch& matches,std::regex& pat);
	void close();
	char buffer[PARSER_BUFFER_SIZE];
	std::string str_buffer;
	std::ifstream fin;
};
NS_ENGIN_END
#endif