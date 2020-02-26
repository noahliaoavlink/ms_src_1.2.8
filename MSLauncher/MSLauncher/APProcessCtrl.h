#pragma once

#ifndef _APProcessCtrl_H
#define _APProcessCtrl_H

#include "NWin.h"
#include "Schedule.h"

class APProcessCtrl : public WinMsgCallback
{
	PROCESS_INFORMATION pi;

	CNWin* m_pNWin;

	Schedule m_Schedule;
public:
	APProcessCtrl();
	~APProcessCtrl();

	void Init();
	void Launch();
	void Close();

	void ParseCopyData(COPYDATASTRUCT* pCopyDataStruct);

	void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);
};

#endif