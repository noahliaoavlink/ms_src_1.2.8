#pragma once

#ifndef _KeyProChecker_H
#define _KeyProChecker_H

#include "..\\..\\include\\MidiCtrlCommon.h"

//#include "MidiCtrlCommon.h"
//#include "NWin.h"

class KeyProChecker
{
	/*
	HWND m_hMCReciever;

	bool m_bIsAlive;
	HANDLE m_hMCSenderThread;
	CRITICAL_SECTION m_CriticalSection;

	long m_lCurTimeCode;
	DWORD m_dwLastTime;
	unsigned long m_ulCurTimeCode;

	COPYDATASTRUCT m_CopyData;
	*/
public:
	KeyProChecker();
	~KeyProChecker();

	void Check();

	/*
	bool CheckMidiCtrlReciever();

	void Play();
	void Stop();
	void UpdateTimeCode(long lTimeCode);

	bool IsAlive();
	void TimeCodeLoop();
	*/
};

#endif