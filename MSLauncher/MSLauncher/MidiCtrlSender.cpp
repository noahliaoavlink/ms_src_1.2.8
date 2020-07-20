#include "stdafx.h"
#include "MidiCtrlSender.h"
#include "Mmsystem.h"

//"Winmm.lib"
#pragma comment(lib, "winmm.lib")


//DWORD WINAPI MCSenderThread(LPVOID lpParam);

MidiCtrlSender::MidiCtrlSender()
{
	m_hMCReciever = 0;

	/*
	m_bIsAlive = false;
	m_hMCSenderThread = NULL;
	m_dwLastTime = 0;

	InitializeCriticalSection(&m_CriticalSection);
	*/
}

MidiCtrlSender::~MidiCtrlSender()
{
	//DeleteCriticalSection(&m_CriticalSection);
}

bool MidiCtrlSender::CheckMidiCtrlReciever()
{
	m_hMCReciever = FindWindowA("MidiCtrl_Reciever",NULL);
	if (m_hMCReciever)
		return true;
	return false;
}

void MidiCtrlSender::Stop()
{
	m_CopyData.dwData = MCC_STOP;
	m_CopyData.cbData = 0;
	m_CopyData.lpData = 0;

	if (m_hMCReciever)
		SendMessage(m_hMCReciever, WM_COPYDATA, 0, (LPARAM)&m_CopyData);

	
//	m_bIsAlive = false;
//	::WaitForSingleObject(m_hMCSenderThread, 10000);
	
}

void MidiCtrlSender::Play()
{
	m_CopyData.dwData = MCC_PLAY;
	m_CopyData.cbData = 0;
	m_CopyData.lpData = 0;

	if (m_hMCReciever)
		SendMessage(m_hMCReciever, WM_COPYDATA, 0, (LPARAM)&m_CopyData);
/*
	m_ulCurTimeCode = 0;
	m_dwLastTime = 0;

	m_bIsAlive = true;
	DWORD nThreadID;
	m_hMCSenderThread = CreateThread(0, 0, MCSenderThread, (void *)this, 0, &nThreadID);
	*/
}

void MidiCtrlSender::SwitchTo(int iItem) 
{
	m_lItem = iItem;

	char szMsg[512];
	sprintf(szMsg, "#SwitchTo# MidiCtrlSender::SwitchTo() - CID_SWITCH_TO %u\n", m_lItem);
	OutputDebugStringA(szMsg);

	m_CopyData.dwData = MCC_SWITCH_TO;
	m_CopyData.cbData = sizeof(long);
	m_CopyData.lpData = &m_lItem;

	if (m_hMCReciever)
		SendMessage(m_hMCReciever, WM_COPYDATA, 0, (LPARAM)&m_CopyData);
}

/*
void MidiCtrlSender::UpdateTimeCode(long lTimeCode)
{
	m_lCurTimeCode = lTimeCode;

	m_CopyData.dwData = MCC_UPDATE_TIME_CODE;
	m_CopyData.cbData = sizeof(long);
	m_CopyData.lpData = &m_lCurTimeCode;

	SendMessage(m_hMCReciever, WM_COPYDATA, 0, (LPARAM)&m_CopyData);
}

void MidiCtrlSender::TimeCodeLoop()
{
	EnterCriticalSection(&m_CriticalSection);

	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime != 0 && dwCurTime - m_dwLastTime >= 33)
	{
		m_ulCurTimeCode += (dwCurTime - m_dwLastTime);

		UpdateTimeCode(m_ulCurTimeCode);

		m_dwLastTime = dwCurTime;
	}
	else if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;

	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

bool MidiCtrlSender::IsAlive()
{
	return m_bIsAlive;
}

DWORD WINAPI MCSenderThread(LPVOID lpParam)
{
	MidiCtrlSender* pObj = (MidiCtrlSender*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->TimeCodeLoop();
	}
	return 0;
}
*/