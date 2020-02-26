#include "stdafx.h"
#include "EffectSelectorWin.h"
#include "../../../Include/SCreateWin.h"
#include "../../../Include/sstring.h"
#include "../../../Include/StrConv.h"
#include "../MediaServer/MediaServer.h"
#include "ContentProcessor.h"
#include "TimelineEditFacade.h"
#include "ServiceProvider.h"

EffectSelectorWin::EffectSelectorWin()
{
	m_hMSMainUIWnd = 0;
	m_pUIOutMsg = 0;
	m_pEffectListCtrl = 0;
	m_pNWin = 0;
	m_pFilePlayerObj = 0;
	m_pIOSourceCtrlDll = 0;
	m_iSkinMode = SM_1080P;

	GetExecutionPath(NULL, m_szAPPath);
	sprintf(m_szFileName, "%s\\MOV\\glowing-disco-squares-vjloops-pack\\SQ-1.mov", m_szAPPath);
}

EffectSelectorWin::~EffectSelectorWin()
{
	DestroyUI();
}

void EffectSelectorWin::DestroyUI()
{
}

void EffectSelectorWin::SetMainUIHandle(HWND hWnd)
{
	m_hMSMainUIWnd = hWnd;
}

void EffectSelectorWin::SetUIOutMsg(MsgInfo* pObj)
{
	m_pUIOutMsg = pObj;
}

void EffectSelectorWin::SetFilePlayerObj(FilePlayerObj* pObj)
{
	m_pFilePlayerObj = pObj;
}

void EffectSelectorWin::SetNWin(CNWin* pWin)
{
	m_pNWin = pWin;
}

void EffectSelectorWin::SetEffectListCtrl(EffectListCtrl* pCtrl)
{
	m_pEffectListCtrl = pCtrl;
}

void EffectSelectorWin::SetIOSourceCtrlDll(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void EffectSelectorWin::SetSkinMode(int iMode)
{
	m_iSkinMode = iMode;
}

void EffectSelectorWin::SetFileName(char* szFileName)
{
	strcpy(m_szFileName, szFileName);
}

void EffectSelectorWin::Init()
{
	DestroyUI();
	
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		ShowWindow(hMainWnd, SW_HIDE);

		m_pEffectListCtrl->SetParent(hMainWnd);
		if (m_iSkinMode == SM_1080P)
			m_pEffectListCtrl->MoveWindow(8, 80, 890, 540);
		else if (m_iSkinMode == SM_1366)
			m_pEffectListCtrl->MoveWindow(7, 80, 827, 540);
		else if (m_iSkinMode == SM_1280)
			m_pEffectListCtrl->MoveWindow(7, 80, 774, 540);
		m_pEffectListCtrl->ShowWindow(true);

		SetParent(m_pNWin->GetSafeHwnd(), hMainWnd);
		if (m_iSkinMode == SM_1080P)
			m_pNWin->MoveWindow(910, 90, 554, 353);
		else if (m_iSkinMode == SM_1366)
			m_pNWin->MoveWindow(845, 90, 514, 353);
		else if (m_iSkinMode == SM_1280)
			m_pNWin->MoveWindow(792, 90, 514, 353);
	}
}

void EffectSelectorWin::Show(bool bShow)
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		m_pEffectListCtrl->ShowWindow(bShow);
		m_pNWin->ShowWindow(bShow);

		if (bShow)
		{
			ShowWindow(hMainWnd, SW_SHOW);
			//SetTopWin(hMainWnd, true);

			SetParent(m_pNWin->GetSafeHwnd(), hMainWnd);

			SetForegroundWindow(hMainWnd);

			m_pUIOutMsg->SendMsg(UI_MSG_SET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);

			CString strTemp = "";
			wchar_t wszFileName[512];
			ContentProcessor cp;

			CheckSourceFile();

			wcscpy(wszFileName, ANSIToUnicode(m_szFileName));

			if (strcmp(m_szFileName, "") != 0)
			{
				if (g_PannelSetting.iEncodeMedia == 2)
				{
#ifdef PLAY_3D_MODE
					if (!IsRTSPUrl(m_szFileName))
					{
						if (cp.CheckFileEncryptStatus(wszFileName))
						{
							strTemp = CPlaneScene3D::FileDecoder(m_szFileName, "", FALSE, TRUE);
							cp.Write2DecryptFileList((char*)strTemp.GetString());
						}
					}
#endif
				}

				if (strTemp.IsEmpty())
					m_pFilePlayerObj->Open(wszFileName);
				else
				{
					wcscpy(wszFileName, ANSIToUnicode(strTemp.GetBuffer()));
					m_pFilePlayerObj->Open(wszFileName);
				}

				m_pFilePlayerObj->Play();
			}
		}
		else
		{
			ShowWindow(hMainWnd, SW_HIDE);
			m_pFilePlayerObj->Close();
		}
	}
}

void EffectSelectorWin::BtnOK()
{
	Show(false);

	int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
	if (iCurSel != -1)
	{
		float fDuration = 0.0;
		SMEffectItem* pSelItem = m_pEffectListCtrl->GetItem(iCurSel);

		int iSel = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);
		if (iSel != -1)
		{
			char* pszItemText = (char*)m_pUIOutMsg->SendMsg(UI_MSG_GET_ITEM_TEXT, (void*)CBB_COMBOBOX0, (void*)iSel);
			fDuration = atof(pszItemText);
		}

		m_EffectInfo.fDuration = fDuration;
		m_EffectInfo.iEffectID = pSelItem->iEffectID;
		m_EffectInfo.iLevel = pSelItem->iLevel;
		m_EffectInfo.iParameter1 = pSelItem->iParameter1;
		m_EffectInfo.iParameter2 = pSelItem->iParameter2;
		m_EffectInfo.iParameter3 = pSelItem->iParameter3;
		m_EffectInfo.iParameter4 = pSelItem->iParameter4;
		m_EffectInfo.iParameter5 = pSelItem->iParameter5;
		strcpy(m_EffectInfo.szName, pSelItem->szName);

		COPYDATASTRUCT cpd;

		TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		CWnd* pEditViewWnd = m_manager->GetEditViewWnd();

		cpd.dwData = EFE_INSERT_EFFECT;
		cpd.cbData = sizeof(EffectInfo);
		cpd.lpData = &m_EffectInfo;

		SendMessage(pEditViewWnd->GetSafeHwnd(),WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);

		//SMEffectItem
	}

	if (m_hMSMainUIWnd)
		SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
}

void EffectSelectorWin::BtnCancel()
{
	Show(false);

	if (m_hMSMainUIWnd)
		SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
}

void EffectSelectorWin::ButtonEvent(unsigned long ulCtrlID)
{
	int iIndex = -1;
	switch (ulCtrlID)
	{
		case BN_BUTTON4:
			BtnOK();
			break;
		case BN_BUTTON5:
			BtnCancel();
			break;
	}
}

void* EffectSelectorWin::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
	case EF_EVT_SEL_CHANGED:
	{
		int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
		if (iCurSel != -1)
		{
			SMEffectItem* pSelItem = m_pEffectListCtrl->GetItem(iCurSel);

			m_pFilePlayerObj->SetEffectIndex(pSelItem->iEffectID);
			m_pFilePlayerObj->SetEffectParam(pSelItem->iLevel, pSelItem->iParameter1, pSelItem->iParameter2, pSelItem->iParameter3, pSelItem->iParameter4, pSelItem->iParameter5);
		}
	}
	break;
	}
	return 0;
}

void EffectSelectorWin::CheckSourceFile()
{
	char szTmpStr[512];
	char szDecriptFileName[512];
	wchar_t wszDecriptFileName[512];

	if (!PathFileExistsA(m_szFileName))
	{
		char* pch = strrchr(m_szFileName, '.');
		if (pch)
		{
			int iTmpLen = strlen(m_szFileName) - strlen(pch);
			memcpy(szTmpStr, m_szFileName, iTmpLen);
			szTmpStr[iTmpLen] = '\0';
			sprintf(szDecriptFileName, "%s!@#$%%^%s", szTmpStr, pch);

			if (PathFileExistsA(szDecriptFileName))
				return;
		}

		if (m_pIOSourceCtrlDll->GetTotalOfFileList())
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(0);
			if (pCurItem)
			{
				strcpy(m_szFileName, WCharToChar(pCurItem->wszFileName));
				if (!PathFileExistsA(m_szFileName))
				{
					char* pch = strrchr(m_szFileName, '.');
					if (pch)
					{
						int iTmpLen = strlen(m_szFileName) - strlen(pch);
						memcpy(szTmpStr, m_szFileName, iTmpLen);
						szTmpStr[iTmpLen] = '\0';
						sprintf(szDecriptFileName, "%s!@#$%%^%s", szTmpStr, pch);

						//wcscpy(wszDecriptFileName,ANSIToUnicode(szDecriptFileName));

						if (PathFileExistsA(szDecriptFileName))
							return;
					}
					strcpy(m_szFileName, "");
				}
			}
		}
		else
			strcpy(m_szFileName, "");
	}
}

