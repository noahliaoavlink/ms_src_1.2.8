#include "stdafx.h"
#include "Log4Debug.h"
#include "..\\..\\Include\\ShellCommon.h"
#include "..\\..\\Include\\sstring.h"

Log4Debug::Log4Debug()
{
}

Log4Debug::~Log4Debug()
{
}

void Log4Debug::Init(char* szModulName)
{
	char szProgramDataPath[128];
	char szMediaServerPath[256];
	char szLogPath[256];
	//char szLogBackupPath[256];
	int iRet = GetFolderPath(CSIDL_COMMON_APPDATA, szProgramDataPath);
	sprintf(szMediaServerPath, "%s\\MediaServer", szProgramDataPath);
	sprintf(szLogPath, "%s\\%s", szMediaServerPath, szModulName);

	if (!FolderIsExist(szMediaServerPath))
		MakeDir(szMediaServerPath);

	if (!FolderIsExist(szLogPath))
		MakeDir(szLogPath);

	wchar_t* wszLogPath = ANSIToUnicode(szLogPath);
	m_LogFile.SetPath(wszLogPath);

	wchar_t* wszModuleName = ANSIToUnicode(szModulName);
	m_LogFile.SetModuleName(wszModuleName);
}

void Log4Debug::AddLogW(int iLevel, wchar_t* wszLog)
{
	wchar_t wszContents[512];
	wchar_t wszLevel[128];
	SYSTEMTIME st;
	GetLocalTime(&st);

	switch (iLevel)
	{
		case LL_INFO:
			wcscpy(wszLevel, L"INFO");
			break;
		case LL_DEBUG:
			wcscpy(wszLevel, L"DEBUG");
			break;
		case LL_ERROR:
			wcscpy(wszLevel, L"ERROR");
			break;
	}

	wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [%s] <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, wszLevel,wszLog);
	wcscat(wszContents, L"\r\n");
	m_LogFile.AppendFile(wszContents);
}

void Log4Debug::AddLogW_debug(int iLevel, wchar_t* wszLog, SYSTEMTIME st_filename)
{
	wchar_t wszContents[512];
	wchar_t wszLevel[128];
	SYSTEMTIME st;
	GetLocalTime(&st);

	switch (iLevel)
	{
		case LL_INFO:
			wcscpy(wszLevel, L"INFO");
			break;
		case LL_DEBUG:
			wcscpy(wszLevel, L"DEBUG");
			break;
		case LL_ERROR:
			wcscpy(wszLevel, L"ERROR");
			break;
	}

	wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [%s] <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, wszLevel, wszLog);
	wcscat(wszContents, L"\r\n");
	m_LogFile.AppendFile(wszContents, st_filename);
}

void Log4Debug::AddLog(int iLevel, char* szLog)
{
	wchar_t wszLog[512];
	wcscpy(wszLog, ANSIToUnicode(szLog));
	AddLogW(iLevel,wszLog);
}

void Log4Debug::AddLog_debug(int iLevel, char* szLog, SYSTEMTIME st_filename)
{
	wchar_t wszLog[512];
	wcscpy(wszLog, ANSIToUnicode(szLog));
	//wchar_t* wszLog = ANSIToUnicode(szLog);
	AddLogW_debug(iLevel, wszLog, st_filename);
}