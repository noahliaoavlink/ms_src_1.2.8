#pragma once

#ifndef _MidiCtrlSender_H
#define _MidiCtrlSender_H

#include "../../Include/MidiCtrlCommon.h"
//#include "NWin.h"

class MidiCtrlSender
{
	HWND m_hMCReciever;
/*
	bool m_bIsAlive;
	HANDLE m_hMCSenderThread;
	CRITICAL_SECTION m_CriticalSection;

	long m_lCurTimeCode;
	DWORD m_dwLastTime;
	unsigned long m_ulCurTimeCode;
	*/
	COPYDATASTRUCT m_CopyData;
	long m_lItem;
public:
	MidiCtrlSender();
	~MidiCtrlSender();

	bool CheckMidiCtrlReciever();

	void Play();
	void Stop();
	void SwitchTo(int iItem);

	/*
	void UpdateTimeCode(long lTimeCode);

	bool IsAlive();
	void TimeCodeLoop();
	*/
};

#endif