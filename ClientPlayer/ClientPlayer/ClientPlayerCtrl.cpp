#include "stdafx.h"
#include "ClientPlayerCtrl.h"

ClientPlayerCtrl::ClientPlayerCtrl()
{
	m_pFilePlayerObj = new FilePlayerObj;
	m_pTCClientDll = new TCClientDll;

	strcpy(m_szIP, "127.0.0.1");
	m_iPort = 2007;
}

ClientPlayerCtrl::~ClientPlayerCtrl()
{
	if (m_pFilePlayerObj)
		delete m_pFilePlayerObj;

	if (m_pTCClientDll)
		delete m_pTCClientDll;
}

void ClientPlayerCtrl::Init(HWND hWnd)
{
	m_pFilePlayerObj->Init(hWnd);

	m_pTCClientDll->LoadLib();
	m_pTCClientDll->SetEventCallback(this);
	m_pTCClientDll->SearchServer();
}

void ClientPlayerCtrl::SetFileName(wchar_t* wszFileName)
{
	wcscpy(m_wszFileName, wszFileName);
}

void ClientPlayerCtrl::SetDisplayMode(char* szMode)
{
	if (strcmp(szMode, "Normal") == 0)
		m_pFilePlayerObj->SetDisplayMode(DM_NORMAL);
	else if (strcmp(szMode, "TVWall") == 0)
		m_pFilePlayerObj->SetDisplayMode(DM_TVWALL);
}

void ClientPlayerCtrl::SetLayoutInfo(int iID, int iIndex)
{
	m_pFilePlayerObj->SetLayoutInfo(iID, iIndex);
}

void ClientPlayerCtrl::Connect()
{
	m_pFilePlayerObj->Close();
	m_pFilePlayerObj->Open(m_wszFileName);
	/*
	SQList* pServerList = (SQList*)m_pTCClientDll->GetServerList();
	if (pServerList->GetTotal() > 0)
	{
		BCUDPServerInfo* pCurItem = (BCUDPServerInfo*)pServerList->Get(0);
		if (pCurItem)
		{
			m_pTCClientDll->Connect(pCurItem->szIP, pCurItem->iPort);
//			Sleep(50);
//			m_pTCClientDll->Login();
		}
	}
	*/

	m_pTCClientDll->Connect(m_szIP, m_iPort);

	m_pFilePlayerObj->Play();
}

void ClientPlayerCtrl::SetServerInfo(char* szIP, int iPort)
{
	strcpy(m_szIP, szIP);
	m_iPort = iPort;
}

void ClientPlayerCtrl::Stop()
{
	m_pTCClientDll->Close();

	m_pFilePlayerObj->Stop();
	m_pFilePlayerObj->Close();
}

void* ClientPlayerCtrl::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	char szMsg[512];
	switch (iMsg)
	{
		case CID_LOGIN_RET:
		{
			long lRet = (long)pParameter1;
			sprintf(szMsg, "LOGIN_RET - (%d)", lRet);
			//gUpdateStatusStr(szMsg);
		}
		break;
		case CID_START:
		{
			sprintf(szMsg, "CID_START");
			//gUpdateStatusStr(szMsg);

			//Open(0, "D:\\test_video\\planet2\\02.mp4");
			//Play(0);
			m_pFilePlayerObj->Play();
		}
		break;
		case CID_STOP:
			sprintf(szMsg, "CID_STOP");
			//gUpdateStatusStr(szMsg);
			//Stop(0);
			//Close(0);
			m_pFilePlayerObj->Stop();
			break;
		case CID_PAUSE:
			sprintf(szMsg, "CID_PAUSE");
			//gUpdateStatusStr(szMsg);
			//Pause(0);
			m_pFilePlayerObj->Pause();
			break;
		case CID_UPDATE_TIME_CODE:
		{
			long lTimeCode = (long)pParameter1;
			//extern void gUpdateTimeCodeStr(long lTimeCode);
			//gUpdateTimeCodeStr(lTimeCode);
			double dCurTimeCode = lTimeCode / 1000.0;
			m_pFilePlayerObj->UpdateGlobalTimeCode(dCurTimeCode);
		}
		break;
		case CID_DO_SYN_PB:
		{
			long lTimeCode = (long)pParameter1;

			sprintf(szMsg, "CID_DO_SYN_PB - (%d)", lTimeCode);
			//gUpdateStatusStr(szMsg);

			//Open(0, "D:\\test_video\\planet2\\02.mp4");
			//Play(0);
			double dPos = lTimeCode / 1000.0;
			//Seek(0, dPos);
			m_pFilePlayerObj->Play();
			m_pFilePlayerObj->Seek(dPos);
		}
		break;
		case CID_CONTINUE:
			long lTimeCode = (long)pParameter1;
			double dPos = lTimeCode / 1000.0;
			//Seek(0, dPos);
			//Play(0);
			m_pFilePlayerObj->Seek(dPos);
			m_pFilePlayerObj->Play();
			break;
	}
	return 0;
}