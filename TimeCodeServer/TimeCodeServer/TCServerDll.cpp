#include "stdafx.h"
#include "TCServerDll.h"

TCServerDll::TCServerDll()
{
	m_hInst = 0;
	m_pTCSObj = 0;
}

TCServerDll::~TCServerDll()
{
	if (m_pTCSObj)
		_Net_TCS_Destroy(m_pTCSObj);

	FreeLib();
}

bool TCServerDll::LoadLib()
{
	char szFileName[] = "TCServer.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Net_TCS_Create = (_Net_TCS_Create_Proc)GetProcAddress(m_hInst, "_Net_TCS_Create");
		_Net_TCS_Destroy = (_Net_TCS_Destroy_Proc)GetProcAddress(m_hInst, "_Net_TCS_Destroy");
		_Net_TCS_StartServer = (_Net_TCS_StartServer_Proc)GetProcAddress(m_hInst, "_Net_TCS_StartServer");
		_Net_TCS_StopServer = (_Net_TCS_StopServer_Proc)GetProcAddress(m_hInst, "_Net_TCS_StopServer");
		_Net_TCS_UpdateTimeCode = (_Net_TCS_UpdateTimeCode_Proc)GetProcAddress(m_hInst, "_Net_TCS_UpdateTimeCode");
		_Net_TCS_SetTotalOfTimeCode = (_Net_TCS_SetTotalOfTimeCode_Proc)GetProcAddress(m_hInst, "_Net_TCS_SetTotalOfTimeCode");
		_Net_TCS_TC_Start = (_Net_TCS_TC_Start_Proc)GetProcAddress(m_hInst, "_Net_TCS_TC_Start");
		_Net_TCS_TC_Stop = (_Net_TCS_TC_Stop_Proc)GetProcAddress(m_hInst, "_Net_TCS_TC_Stop");
		_Net_TCS_TC_Pause = (_Net_TCS_TC_Pause_Proc)GetProcAddress(m_hInst, "_Net_TCS_TC_Pause");
		_Net_TCS_GetTCStatus = (_Net_TCS_GetTCStatus_Proc)GetProcAddress(m_hInst, "_Net_TCS_GetTCStatus");
		_Net_TCS_GetTotalOfTimeCode = (_Net_TCS_GetTotalOfTimeCode_Proc)GetProcAddress(m_hInst, "_Net_TCS_GetTotalOfTimeCode");

		m_pTCSObj = _Net_TCS_Create();
		return true;
	}
	return false;
}

void TCServerDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

int TCServerDll::StartServer(int iPort)
{
	if (m_pTCSObj)
		return _Net_TCS_StartServer(m_pTCSObj, iPort);
	return 0;
}

void TCServerDll::StopServer()
{
	if (m_pTCSObj)
		_Net_TCS_StopServer(m_pTCSObj);
}

void TCServerDll::UpdateTimeCode(unsigned long ulTimeCode)
{
	if (m_pTCSObj)
		_Net_TCS_UpdateTimeCode(m_pTCSObj, ulTimeCode);
}

void TCServerDll::SetTotalOfTimeCode(unsigned long ulTimeCode)
{
	if (m_pTCSObj)
		_Net_TCS_SetTotalOfTimeCode(m_pTCSObj, ulTimeCode);
}

void TCServerDll::TC_Start()
{
	if (m_pTCSObj)
		_Net_TCS_TC_Start(m_pTCSObj);
}

void TCServerDll::TC_Stop()
{
	if (m_pTCSObj)
		_Net_TCS_TC_Stop(m_pTCSObj);
}

void TCServerDll::TC_Pause()
{
	if (m_pTCSObj)
		_Net_TCS_TC_Pause(m_pTCSObj);
}

int TCServerDll::GetTCStatus()
{
	if (m_pTCSObj)
	return _Net_TCS_GetTCStatus(m_pTCSObj);
}

unsigned long TCServerDll::GetTotalOfTimeCode()
{
	if (m_pTCSObj)
		return _Net_TCS_GetTotalOfTimeCode(m_pTCSObj);
}