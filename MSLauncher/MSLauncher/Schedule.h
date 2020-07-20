#pragma once

#ifndef _Schedule_H
#define _Schedule_H

#include "ThreadBase.h"
#include "PBSchedule.h"

class Schedule : public ThreadBase
{
	bool m_bEnable;

	int m_iHour;
	int m_iMinute;
	int m_iSecond;

	DWORD m_dwLastTime;
	HANDLE m_hProcess;

	bool m_bShutdownIsFinished;

	PBSchedule* m_pPBSchedule;

public:
	Schedule();
	~Schedule();

	void Start();
	void Stop();

	void SetProcessHandle(HANDLE hProcess);
	void LoadConfig();
	bool CheckShutdownTime();
	void DoShutdown();

	void ThreadEvent();
};

#endif