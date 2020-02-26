#pragma once

#ifndef _TCClientDll_H
#define _TCClientDll_H

//#include "../../../Include/PluginObjMsg.h"
#include "../../../Include/MediaFileCommon.h"

typedef void* (*_Net_TCC_Create_Proc)();
typedef void (*_Net_TCC_Destroy_Proc)(void* pTCCObj);
typedef void (*_Net_TCC_SearchServer_Proc)(void* pTCCObj);
typedef void* (*_Net_TCC_GetServerList_Proc)(void* pTCCObj);
typedef void (*_Net_TCC_SetEventCallback_Proc)(void* pTCCObj, EventCallback* pObj);
typedef bool (*_Net_TCC_Connect_Proc)(void* pTCCObj, char* szIP, int iPort);
typedef void (*_Net_TCC_Close_Proc)(void* pTCCObj);
typedef void (*_Net_TCC_Login_Proc)(void* pTCCObj, char* szID, char* szPW);
typedef bool (*_Net_TCC_IsConnected_Proc)(void* pTCCObj);
typedef int (*_Net_TCC_GetReceiveStatus_Proc)(void* pTCCObj);

class TCClientDll
{	
	HINSTANCE m_hInst;
	void* m_pTCCObj;
public:
	TCClientDll();
	~TCClientDll();

	bool LoadLib();
	void FreeLib();

	void SearchServer();
	void* GetServerList();
	void SetEventCallback(EventCallback* pObj);
	bool Connect(char* szIP, int iPort);
	void Close();
	void Login(char* szID = "", char* szPW = "");
	bool IsConnected();
	int GetReceiveStatus();

	_Net_TCC_Create_Proc _Net_TCC_Create;
	_Net_TCC_Destroy_Proc _Net_TCC_Destroy;
	_Net_TCC_SearchServer_Proc _Net_TCC_SearchServer;
	_Net_TCC_GetServerList_Proc _Net_TCC_GetServerList;
	_Net_TCC_SetEventCallback_Proc _Net_TCC_SetEventCallback;
	_Net_TCC_Connect_Proc _Net_TCC_Connect;
	_Net_TCC_Close_Proc _Net_TCC_Close;
	_Net_TCC_Login_Proc _Net_TCC_Login;
	_Net_TCC_IsConnected_Proc _Net_TCC_IsConnected;
	_Net_TCC_GetReceiveStatus_Proc _Net_TCC_GetReceiveStatus;
};

#endif
