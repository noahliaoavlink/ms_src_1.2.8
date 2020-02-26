#pragma once
#ifndef _Log4MediaCtrl_H_
#define _Log4MediaCtrl_H_

#include "LogFile.h"
#include "..\\..\\Include\\LogCommon.h"

/*
enum MCTreeCtrlCmd
{
	MCTCC_ADD_FOLDER = 1,
	MCTCC_ADD_FILE, 
	MCTCC_DEL_FOLDER,
	MCTCC_DEL_FILE,
	MCTCC_RESET,
	MCTCC_COPY_FILE,
};
*/

class Log4MediaCtrl
{
	LogFile m_LogFile;

	bool m_bSuspend;
protected:
public:
	Log4MediaCtrl();
	~Log4MediaCtrl();

	void Init();
	void Suspend();
	void Resume();
	void AddLog(int iCmd, wchar_t* wszFolderName, wchar_t* wszFileName, unsigned long ulFileID);
	void DoBackup();
};

#endif
