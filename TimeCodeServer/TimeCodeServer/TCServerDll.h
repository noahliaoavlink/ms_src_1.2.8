#pragma once

#ifndef _TCServerDll_H_
#define _TCServerDll_H_

typedef void* (*_Net_TCS_Create_Proc)();
typedef void (*_Net_TCS_Destroy_Proc)(void* pTCSObj);
typedef int (*_Net_TCS_StartServer_Proc)(void* pTCSObj, int iPort);
typedef void (*_Net_TCS_StopServer_Proc)(void* pTCSObj);
typedef void (*_Net_TCS_UpdateTimeCode_Proc)(void* pTCSObj, unsigned long ulTimeCode);
typedef void (*_Net_TCS_SetTotalOfTimeCode_Proc)(void* pTCSObj, unsigned long ulTimeCode);
typedef void (*_Net_TCS_TC_Start_Proc)(void* pTCSObj);
typedef void (*_Net_TCS_TC_Stop_Proc)(void* pTCSObj);
typedef void (*_Net_TCS_TC_Pause_Proc)(void* pTCSObj);
typedef int (*_Net_TCS_GetTCStatus_Proc)(void* pTCSObj);
typedef unsigned long (*_Net_TCS_GetTotalOfTimeCode_Proc)(void* pTCSObj);

class TCServerDll
{
	HINSTANCE m_hInst;
	void* m_pTCSObj;
protected:
public:
	TCServerDll();
	~TCServerDll();

	bool LoadLib();
	void FreeLib();

	int StartServer(int iPort);
	void StopServer();
	void UpdateTimeCode(unsigned long ulTimeCode);
	void SetTotalOfTimeCode(unsigned long ulTimeCode);
	void TC_Start();
	void TC_Stop();
	void TC_Pause();
	int GetTCStatus();
	unsigned long GetTotalOfTimeCode();

	_Net_TCS_Create_Proc _Net_TCS_Create;
	_Net_TCS_Destroy_Proc _Net_TCS_Destroy;
	_Net_TCS_StartServer_Proc _Net_TCS_StartServer;
	_Net_TCS_StopServer_Proc _Net_TCS_StopServer;
	_Net_TCS_UpdateTimeCode_Proc _Net_TCS_UpdateTimeCode;
	_Net_TCS_SetTotalOfTimeCode_Proc _Net_TCS_SetTotalOfTimeCode;
	_Net_TCS_TC_Start_Proc _Net_TCS_TC_Start;
	_Net_TCS_TC_Stop_Proc _Net_TCS_TC_Stop;
	_Net_TCS_TC_Pause_Proc _Net_TCS_TC_Pause;
	_Net_TCS_GetTCStatus_Proc _Net_TCS_GetTCStatus;
	_Net_TCS_GetTotalOfTimeCode_Proc _Net_TCS_GetTotalOfTimeCode;
	
};

#endif