#include "stdafx.h"
#include "PBSchedule.h"
#include "../../Include/sstring.h"
#include "../../Include/MidiCtrlCommon.h"

PBSchedule::PBSchedule()
{
	m_bEnable = false;
	m_bDoCheckReciever = true;

	m_pPlayList = new SQList;
	m_pPlayList->EnableRemoveData(false);
}

PBSchedule::~PBSchedule()
{
	if (m_pPlayList)
	{
		for (int i = 0; i < m_pPlayList->GetTotal(); i++)
		{
			PBItem* pCurItem = (PBItem*)m_pPlayList->Get(i);
			if (pCurItem)
				delete pCurItem;
		}

		delete m_pPlayList;
	}
}

void PBSchedule::LoadConfig()
{
	char szAPPath[512];
	char szIniPath[512];
	
	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\Setting.ini", szAPPath);

	int iEnable = GetPrivateProfileIntA("PBSchedule", "Enable", 0, szIniPath);
	if (iEnable == 1)
	{
		m_bEnable = true;
		LoadPlaylist();
	}
	else
		m_bEnable = false;
}

void PBSchedule::LoadPlaylist()
{
	char szAPPath[512];
	char szIniPath[512];
	char szAppName[128];
	char szKeyName[128];
	char szData[256];
	char szTmp[256];
	int iParam;
	
	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\MS_PB_Schedule.ini", szAPPath);

	m_pPlayList->Reset();

	int iTotal = GetPrivateProfileIntA("PBSchedule", "Total", 0, szIniPath);
	for (int i = 0; i < iTotal; i++)
	{
		sprintf(szAppName, "Item%d", i);

		PBItem* pNewItem = new PBItem;

		GetPrivateProfileStringA(szAppName, "Time", "", szData, 100, szIniPath);
		strcpy(szTmp, szData);

		sscanf(szTmp, "%02d:%02d:%02d", &pNewItem->iHour, &pNewItem->iMinute, &pNewItem->iSecond);

		GetPrivateProfileStringA(szAppName, "Cmd", "", szData, 100, szIniPath);

		if(strcmp(szData,"Play") == 0)
			pNewItem->iCmdID = MCC_PLAY;
		else if(strcmp(szData, "Stop") == 0)
			pNewItem->iCmdID = MCC_STOP;
		if (strcmp(szData, "SwitchTo") == 0)
			pNewItem->iCmdID = MCC_SWITCH_TO;

		pNewItem->iParameter = GetPrivateProfileIntA(szAppName, "Param",0, szIniPath);
		
		m_pPlayList->Add(pNewItem);
	}
}

void PBSchedule::CheckPlaylist()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	if (m_bEnable)
	{
		if (m_bDoCheckReciever)
		{
			bool bRet = m_MidiCtrlSender.CheckMidiCtrlReciever();
			if (bRet)
				m_bDoCheckReciever = false;
		}

		for (int i = 0; i < m_pPlayList->GetTotal(); i++)
		{
			PBItem* pCurItem = (PBItem*)m_pPlayList->Get(i);
			if (pCurItem)
			{
				if (st.wHour == pCurItem->iHour && st.wMinute == pCurItem->iMinute && st.wSecond == pCurItem->iSecond)
				{
					if (pCurItem->iCmdID == MCC_PLAY)
						m_MidiCtrlSender.Play();
					else if (pCurItem->iCmdID == MCC_STOP)
						m_MidiCtrlSender.Stop();
					else if (pCurItem->iCmdID == MCC_SWITCH_TO)
						m_MidiCtrlSender.SwitchTo(pCurItem->iParameter);
					break;
				}
			}
		}
	}
}