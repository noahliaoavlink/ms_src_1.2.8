#include "stdafx.h"
#include "Log4MediaCtrl.h"

#include "..\\..\\Include\\ShellCommon.h"
#include "..\\..\\Include\\sstring.h"

Log4MediaCtrl::Log4MediaCtrl()
{
	m_bSuspend = false;
}

Log4MediaCtrl::~Log4MediaCtrl()
{
}

void Log4MediaCtrl::Init()
{
	char szProgramDataPath[128];
	char szMediaServerPath[256];
	char szLogPath[256];
	//char szLogBackupPath[256];
	int iRet = GetFolderPath(CSIDL_COMMON_APPDATA, szProgramDataPath);

	sprintf(szMediaServerPath,"%s\\MediaServer", szProgramDataPath);
	sprintf(szLogPath, "%s\\MediaCtrl", szMediaServerPath);
	//sprintf(szLogBackupPath, "%s\\MediaCtrl_backup", szMediaServerPath);

	if(!FolderIsExist(szMediaServerPath))
		MakeDir(szMediaServerPath);

	if (!FolderIsExist(szLogPath))
		MakeDir(szLogPath);

	//if (!FolderIsExist(szLogBackupPath))
		//MakeDir(szLogBackupPath);

	wchar_t* wszLogPath = ANSIToUnicode(szLogPath);
	m_LogFile.SetPath(wszLogPath);

	wchar_t* wszModuleName = ANSIToUnicode("MediaCtrl");
	m_LogFile.SetModuleName(wszModuleName);
}

void Log4MediaCtrl::Suspend()
{
	m_bSuspend = true;
}

void Log4MediaCtrl::Resume()
{
	m_bSuspend = false;
}

void Log4MediaCtrl::AddLog(int iCmd, wchar_t* wszFolderName, wchar_t* wszFileName,unsigned long ulFileID)
{
	wchar_t wszContents[512];
	SYSTEMTIME st;
	GetLocalTime(&st);

	if (m_bSuspend)
		return;

	switch (iCmd)
	{
		case MCTCC_ADD_FOLDER:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Add_Folder] <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,st.wMilliseconds,
				wszFolderName);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_ADD_FILE:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Add_File] <%s> <%s> %u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				wszFolderName, wszFileName, ulFileID);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_DEL_FOLDER:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Del_Folder] <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				wszFolderName);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_DEL_FILE:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Del_File] <%s> <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				wszFolderName, wszFileName);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_RESET:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Reset] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,st.wMilliseconds);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_COPY_FILE:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Copy_File] <%s> <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				wszFolderName, wszFileName);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_LOAD_CSV:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Load_CSV] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
		case MCTCC_DUMP:
			wsprintf(wszContents, L"%d/%02d/%02d_%02d:%02d:%02d_%03d; [Dump] <%s>", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				wszFolderName);
			wcscat(wszContents, L"\r\n");
			m_LogFile.AppendFile(wszContents);
			break;
	}
}

void Log4MediaCtrl::DoBackup()
{
	char szAPPath[256];
	char szFileName[256];
	char szNewFileName[512];
	GetExecutionPath(NULL, szAPPath);

	sprintf(szFileName, "%s\\tree_ctrl.txt", szAPPath);

	//BOOL bRet = CopyFile();
}
