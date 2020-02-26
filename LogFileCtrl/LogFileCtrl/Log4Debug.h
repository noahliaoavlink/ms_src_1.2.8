#pragma once

#ifndef _Log4Debug_H_
#define _Log4Debug_H_

#include "LogFile.h"
#include "..\\..\\Include\\LogCommon.h"
/*
enum LogLevel
{
	LL_INFO = 0,
	LL_DEBUG,
	LL_ERROR,
};
*/

class Log4Debug
{
	LogFile m_LogFile;
protected:
public:
	Log4Debug();
	~Log4Debug();

	void Init(char* szModulName);
	void AddLogW(int iLevel, wchar_t* wszLog);
	void AddLog(int iLevel, char* szLog);

	void AddLogW_debug(int iLevel, wchar_t* wszLog, SYSTEMTIME st_filename);
	void AddLog_debug(int iLevel, char* szLog, SYSTEMTIME st_filename);
};

#endif