#ifndef __LOG_H
#define __LOG_H
#pragma warning(disable:4996)
#include "macros.h"
#include <iostream>
#include <fstream>
#include <stdarg.h>
NS_ENGIN_BEGIN
#define LOG_MAX_ROW 5000
#define MAX_LOG_LENGTH 0x4000
inline void ClearLog(){
	std::ofstream fout("log.txt",std::ios::out|std::ios::trunc);
	fout << "";
	fout.close();
}

inline void Log(const char* format, ...){
	static DWORD logRow = 0;
	if (++logRow >= LOG_MAX_ROW){
		logRow = 0;
		ClearLog();
		return;
	}
	static char buff[MAX_LOG_LENGTH];
	//std::ofstream fout("log.txt", std::ios::app | std::ios::out);
	va_list argv;
	va_start(argv, format);
	vsnprintf(buff, MAX_LOG_LENGTH - 3, format, argv);
	//fout << buff;
#ifdef __DEBUG
	std::cout << buff;
#endif
	va_end(argv);
	//fout.close();
}

inline void Error(const char* err){
	Log("Error:%s\n", err);
}
NS_ENGIN_END
#endif