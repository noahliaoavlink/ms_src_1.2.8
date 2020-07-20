#include "stdafx.h"
#include "TCAdapter.h"
#include "ServiceProvider.h"
#include "MediaSerVer.h"

#define _TC_SERVER_PORT 2017

TCAdapter::TCAdapter()
{
	m_iMode = TCM_NORMAL;

	m_bEnableLog = false;
	m_pLogFileDll = 0;
	m_pLogFileObj = 0;

	m_pTCServerDll = NULL;
	m_pTCClientDll = NULL;

	m_bDoSynPB = false;
	m_iTimeCodeOffset = 0;// -100;
	m_dOldDisplayPos = 0.0;
	m_dOldServerDisplayTime = 0.0;

	m_AvgTimeCodeOffset.SetMax(10);

	strcpy(m_szServerIP,"127.0.0.1");
	m_iPort = _TC_SERVER_PORT;

	m_pTCClientDll = new TCClientDll;
	m_pTCClientDll->LoadLib();
	m_pTCClientDll->SetEventCallback(this);

	m_pTCServerDll = new TCServerDll;
	m_pTCServerDll->LoadLib();

	CheckLogStatus();
}

TCAdapter::~TCAdapter()
{
	if (m_pTCServerDll)
	{
		delete m_pTCServerDll;
		m_pTCServerDll = NULL;
	}

	if (m_pTCClientDll)
	{
		delete m_pTCClientDll;
		m_pTCClientDll = NULL;
	}
}

void TCAdapter::FreeObj()
{
	if (m_pLogFileObj)
	{
		if (m_pLogFileDll)
			m_pLogFileDll->_Out_LGD_Destroy(m_pLogFileObj);
		m_pLogFileObj = NULL;
	}
}

void TCAdapter::SetMode(int iMode)
{
	m_iMode = iMode;
}

int TCAdapter::GetMode()
{
	return m_iMode;
}

void TCAdapter::SetServerIP(char* szIP)
{
	strcpy(m_szServerIP, szIP);
}

void TCAdapter::SetPort(int iPort)
{
	m_iPort = iPort;
}

char* TCAdapter::GetServerIP()
{
	return m_szServerIP;
}

int TCAdapter::GetPort()
{
	return m_iPort;
}


void TCAdapter::UpdateTimeCodeFromTCEngine(double dTimeCode)
{
	double dCurDisplayPos = 0.0;

	char szCurItemName[128];
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	strcpy(szCurItemName,manager->GetSelectedItem("").c_str());

	if (m_iMode == TCM_NORMAL 
		|| m_iMode == TCM_SERVER
		|| (!IsWorking() && m_iMode == TCM_CLIENT) 
		|| (m_pTCClientDll->GetReceiveStatus() == CRS_HUNGER && m_iMode == TCM_CLIENT))
	{
		for (int i = ST_STREAM0; i <= ST_STREAM9; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				if (pOutDevice->IsOpened())
				{
					if (manager->CheckItemName(pOutDevice->GetGroupName()))
					{
						pOutDevice->UpdateGlobalTimeCode(dTimeCode);
						double dCurPos = pOutDevice->GetCurPos();
						if (dCurPos > 0)
							dCurDisplayPos = dCurPos;
					}
				}
			}
		}

		/*
		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsOpened())
			{
				g_MediaStreamAry.at(x)->UpdateGlobalTimeCode(dTimeCode);
				double dCurPos = g_MediaStreamAry.at(x)->GetCurPos();
				if (dCurPos > 0)
					dCurDisplayPos = dCurPos;
			}
		}
		*/
	}

	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
		{
			unsigned long ulTimeCode = dTimeCode * 1000.0;
			unsigned long ulFrameTime = dCurDisplayPos * 1000.0;
			m_pTCServerDll->UpdateCurFrameTime(ulFrameTime);
			m_pTCServerDll->UpdateTimeCode(ulTimeCode);
		}
	}
}

void TCAdapter::UpdateTimeCodeFromTCClient(double dTimeCode)
{
	auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
	if (m_iMode == TCM_CLIENT)
	{
		/*
		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsOpened())
				g_MediaStreamAry.at(x)->UpdateGlobalTimeCode(dTimeCode);
		}
		*/

		TimeCode total_time_code = time_code_engine->GetTotalTimecode();
		total_time_code.TotalMilliseconds();

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		unsigned long ulTimeCode = dTimeCode * 1000.0;
		for (int i = ST_STREAM0; i <= ST_STREAM9; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				if (pOutDevice->IsOpened())
				{
					if (manager->CheckItemName(pOutDevice->GetGroupName()))
					{
						if(ulTimeCode <= total_time_code.TotalMilliseconds())
							pOutDevice->UpdateGlobalTimeCode(dTimeCode);
					}
				}
			}
		}
	}

	
	time_code_engine->SetTimecode(dTimeCode * 1000);
	time_code_engine->DoUpdateTimeCode();
}

void TCAdapter::UpdateTimeCodeFromTCClient(double dTimeCode,double dServerDisplayTime)
{
	char szMsg[512];
	double dCurDisplayPos = 0.0;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	if (m_iMode == TCM_CLIENT)
	{
		/*
		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsOpened())
			{
				g_MediaStreamAry.at(x)->UpdateGlobalTimeCode(dTimeCode);
				double dCurPos = g_MediaStreamAry.at(x)->GetCurPos();
				if (dCurPos > 0)
					dCurDisplayPos = dCurPos;
			}
		}
		*/

		for (int i = ST_STREAM0; i <= ST_STREAM9; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				if (pOutDevice->IsOpened())
				{
					if (manager->CheckItemName(pOutDevice->GetGroupName()))
					{
						pOutDevice->UpdateGlobalTimeCode(dTimeCode);
						double dCurPos = pOutDevice->GetCurPos();
						if (dCurPos > 0)
							dCurDisplayPos = dCurPos;
					}
				}
			}
		}
	}

	if (dCurDisplayPos > 0 && dServerDisplayTime > 0)
	{
		bool bUpdate = false;
		double dTmpNum = dCurDisplayPos - dServerDisplayTime;

		if (dCurDisplayPos != m_dOldDisplayPos)
		{
			m_dOldDisplayPos = dCurDisplayPos;
			bUpdate = true;
		}

		if (m_dOldServerDisplayTime != dServerDisplayTime)
		{
			m_dOldServerDisplayTime = dServerDisplayTime;
			bUpdate = true;
		}

		if(dCurDisplayPos != dServerDisplayTime && bUpdate)
			m_AvgTimeCodeOffset.Calculate(dTmpNum * 1000.0);

		//sprintf(szMsg, "TCAdapter::UpdateTimeCodeFromTCClient offset_time:%f (%f , %f)\n", m_AvgTimeCodeOffset.GetAverage(), dCurDisplayPos , dServerDisplayTime);
		//OutputDebugStringA(szMsg);
	}

	auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
	time_code_engine->SetTimecode(dTimeCode * 1000);
	time_code_engine->DoUpdateTimeCode();
}

void TCAdapter::SetTotalOfTimeCode(unsigned long ulTimeCode)
{
	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			m_pTCServerDll->SetTotalOfTimeCode(ulTimeCode);
	}
}

void TCAdapter::Start()
{
	Stop();

	switch (m_iMode)
	{
		case TCM_NORMAL:
			break;
		case TCM_SERVER:
		{
			if(m_pTCServerDll)
				m_pTCServerDll->StartServer(m_iPort);  //_TC_SERVER_PORT
		}
		break;
		case TCM_CLIENT:
			{
				if (m_pTCClientDll)
				{
					m_pTCClientDll->Connect(m_szServerIP, m_iPort);
					//Sleep(50);
					//m_pTCClientDll->Login();
				}
			}
			break;
	}
}

void TCAdapter::Stop()
{
	TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pEditViewWnd = m_manager->GetEditViewWnd();

	switch (m_iMode)
	{
		case TCM_NORMAL:
			break;
		case TCM_SERVER:
			{
				if (m_pTCServerDll)
				{
					if (m_pTCServerDll->IsWorking())
						PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);

					m_pTCServerDll->StopServer();
				}
			}
			break;
		case TCM_CLIENT:
			{
				if (m_pTCClientDll)
				{
					if (m_pTCClientDll->IsConnected())
						PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);

					m_pTCClientDll->Close();
				}
			}
			break;
	}
}

void TCAdapter::TC_Start()
{
	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			m_pTCServerDll->TC_Start();
	}
}

void TCAdapter::TC_Stop()
{
	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			m_pTCServerDll->TC_Stop();
	}
}

void TCAdapter::TC_Pause()
{
	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			m_pTCServerDll->TC_Pause();
	}
}

void TCAdapter::TC_SwitchTo(int iIndex)
{
	char szMsg[512];
	sprintf(szMsg, "TCAdapter::TC_SwitchTo: %d\n", iIndex);
	OutputDebugStringA(szMsg);

	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			m_pTCServerDll->TC_SwitchTo(iIndex);
	}
}

bool TCAdapter::IsWorking()
{
	if (m_iMode == TCM_SERVER)
	{
		if (m_pTCServerDll)
			return m_pTCServerDll->IsWorking();
	}
	else if (TCM_CLIENT)
	{
		if (m_pTCClientDll)
			return m_pTCClientDll->IsConnected();
	}
	return false;
}

void TCAdapter::SearchServer()
{
	if (m_pTCClientDll)
		m_pTCClientDll->SearchServer();
}

void* TCAdapter::GetServerList()
{
	if (m_pTCClientDll)
		return m_pTCClientDll->GetServerList();
	return 0;
}

int TCAdapter::GetReceiveStatus()
{
	if (m_pTCClientDll && m_iMode == TCM_CLIENT)
		return m_pTCClientDll->GetReceiveStatus();
	return 0;
}

int TCAdapter::GetTotalOfClients()
{
	if (m_pTCServerDll)
		return m_pTCServerDll->GetTotalOfClients();
}

void TCAdapter::CheckLogStatus()
{
	m_bEnableLog = GetPrivateProfileInt("LogFile", "TCAdapter", 0, theApp.m_strCurPath + "Setting.ini");

	if (m_bEnableLog)
	{
		if (m_pLogFileDll)
		{
			m_pLogFileObj = m_pLogFileDll->_Out_LGD_Create();
			m_pLogFileDll->_Out_LGD_Init(m_pLogFileObj,"TCAdapter");
		}
	}
}

void TCAdapter::SetLogFileDll(LogFileDll* pObj)
{
	m_pLogFileDll = pObj;
}

void TCAdapter::Add2LogFile(int iLevel,char* szMsg)
{
	if (m_bEnableLog && m_pLogFileDll && m_pLogFileObj)
	{
		m_pLogFileDll->_Out_LGD_AddLog(m_pLogFileObj, iLevel,szMsg);
	}
}

void* TCAdapter::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	char szMsg[512];

	TimelineManager* manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	switch (iMsg)
	{
		case CID_LOGIN_RET:
			{
				int iRet = (int)pParameter1;
				if (iRet == LR_SUCESSED)
					;
				else if (iRet == LR_FAILED)
				{
					sprintf(szMsg,"Login Failed!!");
					Add2LogFile(LL_ERROR, szMsg);
				}
			}
			break;
		case CID_START:
			{
				SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);

				sprintf(szMsg, "CID_START");
				Add2LogFile(LL_INFO, szMsg);
			}
			break;
		case CID_STOP:
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
			manager->DoPlayback_AllOutDevices(PBC_CLOSE);
			manager->ClearAll();
			sprintf(szMsg, "CID_STOP");
			Add2LogFile(LL_INFO, szMsg);
			break;
		case CID_PAUSE:
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
			sprintf(szMsg, "CID_PAUSE");
			Add2LogFile(LL_INFO, szMsg);
			break;
		case CID_UPDATE_TIME_CODE:
			{
				long lTimeCode = (long)pParameter1;
				long lServerDisplayTimeCode = (long)pParameter2;

				//m_iTimeCodeOffset = m_AvgTimeCodeOffset.GetAverage();
				if (m_AvgTimeCodeOffset.GetAverage() < 0)
				{
					if (m_iTimeCodeOffset != 0)
					{
						if (m_iTimeCodeOffset < 0)
						{
							if (lTimeCode + m_iTimeCodeOffset > 0)
								lTimeCode += m_iTimeCodeOffset;
							else
								lTimeCode = 0;
						}
						else
							lTimeCode += m_iTimeCodeOffset;
					}
				}
			
				double dTimeCode = lTimeCode / 1000.0;
				double dFrameTime = lServerDisplayTimeCode / 1000.0;

				if (m_bDoSynPB)
				{
					int iCount = 0;
					
					/*
					for (int i = ST_STREAM0; i <= ST_STREAM9; i++)
					{
						OutDevice* pOutDevice = manager->GetOutDevice(i);
						if (pOutDevice)
						{
							if (pOutDevice->IsOpened())
							{
								if (manager->CheckItemName(pOutDevice->GetGroupName()))
								{
									pOutDevice->Seek(dTimeCode);
									//Sleep(50);
									iCount++;
								}
							}
						}
					}

					if (iCount > 0)  */
						m_bDoSynPB = false;
				}
				else
				{
					//sprintf(szMsg, "TCAdapter::Event\A1GCID_UPDATE_TIME_CODE %f(%d)\n", dTimeCode, lTimeCode);
					//OutputDebugStringA(szMsg);

					UpdateTimeCodeFromTCClient(dTimeCode);
					//UpdateTimeCodeFromTCClient(dTimeCode, dFrameTime);
				}
			}
			break;
		case CID_DO_SYN_PB:
			{
				long lTimeCode = (long)pParameter1;
				double dPos = lTimeCode / 1000.0;
				//Seek(0, dPos);

				//PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
				SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);

				sprintf(szMsg, "CID_DO_SYN_PB");
				Add2LogFile(LL_INFO, szMsg);

				/*
				for (int x = 0; x < g_MediaStreamAry.size(); x++)
				{
					if (g_MediaStreamAry.at(x)->IsOpened())
						g_MediaStreamAry.at(x)->Seek(dPos);
				}
				*/

				UpdateTimeCodeFromTCClient(dPos);

//				m_bDoSynPB = true;

				sprintf(szMsg, "TCAdapter::Event: CID_DO_SYN_PB %f(%d)\n", dPos,lTimeCode);
				OutputDebugStringA(szMsg);
			}
			break;
		case CID_CONTINUE:
			PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
			//m_bDoSynPB = true;
			sprintf(szMsg, "CID_CONTINUE");
			Add2LogFile(LL_INFO, szMsg);
			break;
		case CID_DISCONNECT:
			{
				int kk = 0;

				sprintf(szMsg, "TCAdapter::Event: CID_DISCONNECT !!!!!\n");
				OutputDebugStringA(szMsg);
			}
			break;
		case CID_CLIENT_EVENT:
			{
				int iLevel = (int)pParameter1;
				Add2LogFile(iLevel, (char*)pParameter2);
			}
			break;
		case CID_SERVER_EVENT:
			{
				int iLevel = (int)pParameter1;
				Add2LogFile(iLevel, (char*)pParameter2);
			}
			break;
		case CID_SWITCH_TO:
			{
				long lIndex = (long)pParameter1;
				manager->ChangeTo(lIndex);

				sprintf(szMsg, "#SwitchTo# TCAdapter::Event: CID_SWITCH_TO %d\n", lIndex);
				OutputDebugStringA(szMsg);
			}
			break;
	}
	return 0;
}
