#include "ParseUtil.h"
NS_ENGIN_BEGIN
ParseUtil::ParseUtil(){

}

ParseUtil::~ParseUtil(){
	close();
}

bool ParseUtil::open( const char* fileName ){
	fin.open(fileName);
	return fin.is_open();
}

int ParseUtil::getLine(){
	while(true){
		fin.getline(buffer,PARSER_BUFFER_SIZE);
		if(fin.gcount()==0)
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
		//trimLeft(buffer);
		//trimRight(buffer);
		return strlen(buffer);
	}
	return 0;
}

int ParseUtil::getLine(std::smatch& matches,std::regex& pat){
	while(true){
		fin.getline(buffer,PARSER_BUFFER_SIZE);
		fin.clear();
		if(fin.gcount()==0)
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
		str_buffer=buffer;
		if(std::regex_search(str_buffer,matches,pat))
			return strlen(buffer);
	}
	return 0;
}

void ParseUtil::close(){
	if(fin.is_open())
		fin.close();
}

void trimLeft( char* buffer ){
	int len=strlen(buffer)-1;
	int spaceNum=0;
	for(int i=0;i<len;i++){
		if(isspace(buffer[i]))
			spaceNum++;
		else
			break;
	}
	if(spaceNum>0)
		memcpy(buffer,buffer+spaceNum,len-spaceNum+1);
}

void trimRight( char* buffer ){
	for(int i=strlen(buffer)-1;i>=0;i++){
		if(isspace(buffer[i]))
			buffer[i]='\0';
		else
			break;
	}
}

NS_ENGIN_END