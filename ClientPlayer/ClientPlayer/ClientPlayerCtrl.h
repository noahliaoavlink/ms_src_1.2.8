#pragma once

#ifndef _ClientPlayerCtrl_H
#define _ClientPlayerCtrl_H

#include "FilePlayerObj.h"
#include "TCClientDll.h"

//#include "../../../Include/PluginObjMsg.h"
#include "../../Include/NetCommon.h"
#include "../../Include/SQList.h"

class ClientPlayerCtrl : public EventCallback
{
	FilePlayerObj* m_pFilePlayerObj;
	TCClientDll* m_pTCClientDll;

	char m_szIP[64];
	int m_iPort;

	wchar_t m_wszFileName[512];
public:
	ClientPlayerCtrl();
	~ClientPlayerCtrl();

	void Init(HWND hWnd);
	void SetFileName(wchar_t* wszFileName);
	void SetDisplayMode(char* szMode);
	void SetLayoutInfo(int iID, int iIndex);
	void SetServerInfo(char* szIP, int iPort);
	void Connect();
	void Stop();

	virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);
};

#endif