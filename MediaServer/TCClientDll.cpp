#include "stdafx.h"
#include "TCClientDll.h"

TCClientDll::TCClientDll()
{
	m_hInst = 0;
	m_pTCCObj = 0;
}

TCClientDll::~TCClientDll()
{
	if (m_pTCCObj)
		_Net_TCC_Destroy(m_pTCCObj);

	FreeLib();
}

bool TCClientDll::LoadLib()
{
	char szFileName[] = "TCClient.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Net_TCC_Create = (_Net_TCC_Create_Proc)GetProcAddress(m_hInst, "_Net_TCC_Create");
		_Net_TCC_Destroy = (_Net_TCC_Destroy_Proc)GetProcAddress(m_hInst, "_Net_TCC_Destroy");
		_Net_TCC_SearchServer = (_Net_TCC_SearchServer_Proc)GetProcAddress(m_hInst, "_Net_TCC_SearchServer");
		_Net_TCC_GetServerList = (_Net_TCC_GetServerList_Proc)GetProcAddress(m_hInst, "_Net_TCC_GetServerList");
		_Net_TCC_SetEventCallback = (_Net_TCC_SetEventCallback_Proc)GetProcAddress(m_hInst, "_Net_TCC_SetEventCallback");
		_Net_TCC_Connect = (_Net_TCC_Connect_Proc)GetProcAddress(m_hInst, "_Net_TCC_Connect");
		_Net_TCC_Close = (_Net_TCC_Close_Proc)GetProcAddress(m_hInst, "_Net_TCC_Close");
		_Net_TCC_Login = (_Net_TCC_Login_Proc)GetProcAddress(m_hInst, "_Net_TCC_Login");
		_Net_TCC_IsConnected = (_Net_TCC_IsConnected_Proc)GetProcAddress(m_hInst, "_Net_TCC_IsConnected");
		_Net_TCC_GetReceiveStatus = (_Net_TCC_GetReceiveStatus_Proc)GetProcAddress(m_hInst, "_Net_TCC_GetReceiveStatus");
		
		m_pTCCObj = _Net_TCC_Create();
		return true;
	}
	return false;
}

void TCClientDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void TCClientDll::SearchServer()
{
	if (m_pTCCObj)
		_Net_TCC_SearchServer(m_pTCCObj);
}

void* TCClientDll::GetServerList()
{
	if (m_pTCCObj)
		return _Net_TCC_GetServerList(m_pTCCObj);
}

void TCClientDll::SetEventCallback(EventCallback* pObj)
{
	if (m_pTCCObj)
		_Net_TCC_SetEventCallback(m_pTCCObj, pObj);
}

bool TCClientDll::Connect(char* szIP, int iPort)
{
	if (m_pTCCObj)
		return _Net_TCC_Connect(m_pTCCObj, szIP, iPort);
}

void TCClientDll::Close()
{
	if (m_pTCCObj)
		_Net_TCC_Close(m_pTCCObj);
}

void TCClientDll::Login(char* szID, char* szPW)
{
	if (m_pTCCObj)
		_Net_TCC_Login(m_pTCCObj, szID, szPW);
}

bool TCClientDll::IsConnected()
{
	if (m_pTCCObj)
		return _Net_TCC_IsConnected(m_pTCCObj);
	return false;
}

int TCClientDll::GetReceiveStatus()
{
	if (m_pTCCObj)
		return _Net_TCC_GetReceiveStatus(m_pTCCObj);
	return 0;
}
