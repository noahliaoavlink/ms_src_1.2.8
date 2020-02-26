#include "stdafx.h"
#include "LogFile.h"

LogFile::LogFile()
{
	wcscpy(m_wszModuleName, L"timeline");
	wcscpy(m_wszFileName, L"");
	wcscpy(m_wszPath, L"");
}

LogFile::~LogFile()
{
}

void LogFile::SetModuleName(wchar_t* wszName)
{
	wcscpy(m_wszModuleName, wszName);
}

void LogFile::SetPath(wchar_t* wszPath)
{
	wcscpy(m_wszPath, wszPath);
}

bool LogFile::AppendFile(wchar_t* wszContents)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	return AppendFile(wszContents,st);
}

bool LogFile::AppendFile(wchar_t* wszContents, SYSTEMTIME st)
{
	FILE* m_pFile;
	bool bDoRecycle = false;

	//SYSTEMTIME st;
	//GetLocalTime(&st);
	wsprintf(m_wszFileName, L"%s\\%s_%d_%.2d_%.2d.txt", m_wszPath, m_wszModuleName, st.wYear, st.wMonth, st.wDay);

	if (!PathFileExists(m_wszFileName))
		bDoRecycle = true;

	if ((m_pFile = _wfopen(m_wszFileName, L"a+t")) == 0)
	{
		return false;
	}
	else
	{
		fseek(m_pFile, 0, SEEK_END);
		std::string sData = UnicodeToUTF8(wszContents);
		fwrite(sData.c_str(), 1, sData.size(), m_pFile);
		fclose(m_pFile);
	}

	if (bDoRecycle)
	{
		char* szPath = WCharToChar(m_wszPath);
		m_FileTrimmer.SetPath(szPath);
		m_FileTrimmer.DoRemoveFiles();
	}

	return true;
}