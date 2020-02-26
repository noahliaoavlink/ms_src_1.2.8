#pragma once

#ifndef _LogFile_H_
#define _LogFile_H_

#include "..\\..\\Include\\StrConv.h"
#include "FileTrimmer.h"

class LogFile
{
	wchar_t m_wszPath[512];
	wchar_t m_wszFileName[512];
	wchar_t m_wszModuleName[128];

	FileTrimmer m_FileTrimmer;
protected:
public:
	LogFile();
	~LogFile();

	void SetModuleName(wchar_t* wszName);
	void SetPath(wchar_t* wszPath);

	bool AppendFile(wchar_t* wszContents);
	bool AppendFile(wchar_t* wszContents, SYSTEMTIME st);
};

#endif
