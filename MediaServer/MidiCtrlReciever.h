#pragma once

#ifndef _MidiCtrlReciever_H
#define _MidiCtrlReciever_H

//#include "../../../Include/ShellCommon.h"
#include "../../../Include/MidiCtrlCommon.h"
#include "../../../Include/SQQueue.h"
#include "NWin.h"

class MidiCtrlReciever : public WinMsgCallback
{
	CNWin* m_pNWin;

	SQQueue* m_pTimeCodeQueue;
	double* m_pTmpTimeCode;

	bool m_bIsAlive;
	HANDLE m_hMCRecieverThread;
	CRITICAL_SECTION m_CriticalSection;
public:
	MidiCtrlReciever();
	~MidiCtrlReciever();

	void Init();
	void ParseCopyData(COPYDATASTRUCT* pCopyDataStruct);
	void UpdateTimeCodeFromMidiCtrl(double dTimeCode);
	void AddTimeCodeToQueue(double dTimecode);
	double* GetTimeCodeFromeQueue();
	void ProcessTimeCode();

	void Play();
	void Stop();
	void Pause();
	bool IsAlive();
	void TimeCodeLoop();

	void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);
};

#endif