#include "stdafx.h"
#include "SimpleMixerWin.h"
#include "../../../Include/SCreateWin.h"
#include "../../../Include/sstring.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/ShellCommon.h"
#include "../MediaServer/MediaServer.h"
#include "ContentProcessor.h"

SimpleMixerWin::SimpleMixerWin()
{
	m_hMSMainUIWnd = 0;
	m_pTreeCtrlUI = 0;
	m_pFileListCtrl = 0;
	m_pEffectListCtrl = 0;
	m_pNWin = 0;
	m_pFilePlayerObj = 0;
	m_pUIOutMsg = 0;
	m_hCurDlgWnd = 0;

	m_iSkinMode = SM_1080P;
	m_iEditMode = EM_NONE;

//	m_pVolumeCtrl = NULL;
//	m_pVolumeSlider = NULL;

	GetExecutionPath(NULL, m_szAPPath);
	sprintf(m_szFileName,"%s\\MOV\\glowing-disco-squares-vjloops-pack\\SQ-1.mov", m_szAPPath);

	m_pSIniFileW = new SIniFileW;

	char szAPPath[512];
	char szFileName[512];
	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);
	sprintf(szFileName, "%s\\effect_list.txt", szAPPath);
	wcscpy(wszIniFileName, ANSIToUnicode(szFileName));
}

SimpleMixerWin::~SimpleMixerWin()
{
	DestroyUI();

	if (m_pSIniFileW)
		delete m_pSIniFileW;
}

void SimpleMixerWin::DestroyUI()
{
	if (m_pTreeCtrlUI)
	{
		delete m_pTreeCtrlUI;
		m_pTreeCtrlUI = 0;
	}

	if (m_pFileListCtrl)
	{
		delete m_pFileListCtrl;
		m_pFileListCtrl = 0;
	}
	/*
	if (m_pEffectListCtrl)
	{
		delete m_pEffectListCtrl;
		m_pEffectListCtrl = 0;
	}
	
	if (m_pNWin)
		delete m_pNWin;

	if (m_pFilePlayerObj)
		delete m_pFilePlayerObj;
		*/
}

void SimpleMixerWin::SetMainUIHandle(HWND hWnd)
{
	m_hMSMainUIWnd = hWnd;
}

void SimpleMixerWin::SetUIOutMsg(MsgInfo* pObj)
{
	m_pUIOutMsg = pObj;
}

void SimpleMixerWin::SetIOSourceCtrlDll(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void SimpleMixerWin::SetFilePlayerObj(FilePlayerObj* pObj)
{
	m_pFilePlayerObj = pObj;
}

void SimpleMixerWin::SetNWin(CNWin* pWin)
{
	m_pNWin = pWin;
}

void SimpleMixerWin::SetEffectListCtrl(EffectListCtrl* pCtrl)
{
	m_pEffectListCtrl = pCtrl;
}

void SimpleMixerWin::SetSkinMode(int iMode)
{
	m_iSkinMode = iMode;
}

void SimpleMixerWin::Init()
{
	DestroyUI();

	m_pTreeCtrlUI = new TreeCtrlUI;
	m_pTreeCtrlUI->Init();
	m_pTreeCtrlUI->SetIOSourceCtrl(m_pIOSourceCtrlDll);
	m_pTreeCtrlUI->SetSimpleMode(true);
	m_pTreeCtrlUI->SetDialogMode(MFDM_SELECT);
	m_pTreeCtrlUI->EnableEnableSearchTag(false);

	m_pFileListCtrl = new FileListCtrl;
	m_pFileListCtrl->Create("FileList", IDC_SM_LIST_CTRL, 400, 200);
	m_pFileListCtrl->Init();
	m_pFileListCtrl->SetIOSourceCtrl(m_pIOSourceCtrlDll);
	m_pFileListCtrl->SetSimpleMode(true);
	m_pFileListCtrl->SetDisplayMode(FIDM_ICON);

	m_pFileListCtrl->SetEventCallback(this);
	m_pTreeCtrlUI->SetFileListCtrl(m_pFileListCtrl);

	/*
	std::string sVolumeCtrlBmp = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Vol.bmp";
	m_VolumeCtrlBmp.Load((char*)sVolumeCtrlBmp.c_str());

	std::string sVolumeCtrl = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Vol_Round.bmp";
	m_pVolumeCtrl = new CRoundSliderCtrl;
	m_pVolumeCtrl->SetBGFileName((char*)sVolumeCtrl.c_str());

	m_pVolumeSlider = new CBitSlider;
	*/
	m_pFilePlayerObj->SetEventCallback(this);

	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		//CWnd MainWin_Wnd;

		ShowWindow(hMainWnd, SW_HIDE);

		m_pTreeCtrlUI->SetParent(hMainWnd);

		if(m_iSkinMode == SM_1080P)
			m_pTreeCtrlUI->MoveWindow(570, 90, 690, 440);
		else if (m_iSkinMode == SM_1366)
			m_pTreeCtrlUI->MoveWindow(405, 64, 490, 312);
		else if (m_iSkinMode == SM_1280)
			m_pTreeCtrlUI->MoveWindow(376, 60, 455, 290);

		m_pTreeCtrlUI->ShowWindow(true);
		m_pTreeCtrlUI->Load();

		m_pFileListCtrl->SetParent(hMainWnd);

		if (m_iSkinMode == SM_1080P)
			m_pFileListCtrl->MoveWindow(570, 530, 690, 440);
		else if (m_iSkinMode == SM_1366)
			m_pFileListCtrl->MoveWindow(405, 376, 490, 312);
		else if (m_iSkinMode == SM_1280)
			m_pFileListCtrl->MoveWindow(376, 350, 455, 290);

		m_pFileListCtrl->ShowWindow(true);

		m_pEffectListCtrl->SetParent(hMainWnd);
		if (m_iSkinMode == SM_1080P)
			m_pEffectListCtrl->MoveWindow(1275, 90, 638, 890);
		else if (m_iSkinMode == SM_1366)
			m_pEffectListCtrl->MoveWindow(907, 64, 453, 632);
		else if (m_iSkinMode == SM_1280)
			m_pEffectListCtrl->MoveWindow(842 + 5, 60, 421, 587);
		m_pEffectListCtrl->ShowWindow(true);

		SetParent(m_pNWin->GetSafeHwnd(),hMainWnd);
		if (m_iSkinMode == SM_1080P)
			m_pNWin->MoveWindow(4, 90, 554, 353);
		else if (m_iSkinMode == SM_1366)
			m_pNWin->MoveWindow(4, 64, 394, 251);
		else if (m_iSkinMode == SM_1280)
			m_pNWin->MoveWindow(4, 60, 365, 232);

		/*
		MainWin_Wnd.Attach(hMainWnd);
		m_pVolumeCtrl->Create(150, 910, 109, 109, &MainWin_Wnd, 1002);
		m_pVolumeSlider->CreateV(191, 570, 25, 328, &MainWin_Wnd, 1003);

		std::string sVolumeSlider_layer = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_v.bmp";
		m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer.c_str(), 22, 9, 0);
		std::string sVolumeSlider_layer_t = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_t_v.bmp";
		m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer_t.c_str(), 22, 9, 1);
		std::string sVolumeSlider_layer_c = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_c_v.bmp";
		m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer_c.c_str(), 22, 9, 2);

		std::string sVolumeSlider = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider_v.bmp";
		std::string sVolumeSlider_Active = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider_active_v.bmp";
		m_pVolumeSlider->BuildBackItem((char*)sVolumeSlider.c_str(), (char*)sVolumeSlider_Active.c_str());
		m_pVolumeSlider->SetTopOffset(1);
		m_pVolumeSlider->SetPosV(50);

		MainWin_Wnd.Detach();
		*/
	}
//	Load();
}

void SimpleMixerWin::Show(bool bShow)
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		m_pTreeCtrlUI->ShowWindow(bShow);
		m_pFileListCtrl->ShowWindow(bShow);
		m_pEffectListCtrl->ShowWindow(bShow);

		m_pNWin->ShowWindow(bShow);

		if (bShow)
		{
			m_hCurDlgWnd = hMainWnd;

			m_pTreeCtrlUI->Load();
			m_pTreeCtrlUI->UpdateList();
//			SetTopWin(hMainWnd, true);

//			m_pVolumeSlider->ShowWindow(SW_SHOW);
//			m_pVolumeSlider->ShowWindow(SW_SHOW);

			CString strTemp = "";
			wchar_t wszFileName[512];
			ContentProcessor cp;

			CheckSourceFile();

			if (strcmp(m_szFileName, "") != 0)
			{
				wcscpy(wszFileName, ANSIToUnicode(m_szFileName));

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

	//			m_pFilePlayerObj->ResetD3D();
				m_pFilePlayerObj->Play();
			}

			EnableWindow(m_hMSMainUIWnd, false);
			ShowWindow(hMainWnd, SW_SHOW);
			SetFocus(hMainWnd);
			UpdateUIStatus();
//			ShowWindow(m_hMSMainUIWnd, SW_HIDE);
		}
		else
		{
			ShowWindow(m_hMSMainUIWnd, SW_SHOW);
			EnableWindow(m_hMSMainUIWnd, true);

			ShowWindow(hMainWnd, SW_HIDE);
//			m_pVolumeSlider->ShowWindow(SW_HIDE);
//			m_pVolumeSlider->ShowWindow(SW_HIDE);
			m_pFilePlayerObj->Close();
		}
	}
}

void SimpleMixerWin::BtnCancel()
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (m_hCurDlgWnd != hMainWnd)
		Config_BtnCancel();
	else
	{
		Show(false);

		if (m_hMSMainUIWnd)
			SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
	}
}

void SimpleMixerWin::BtnChange()
{
	CString strTemp = "";
	ContentProcessor cp;
	wchar_t wszFileName[512];
	wchar_t* wszSelFileName = m_pFileListCtrl->GetCurSelFileName();

	if (wcscmp(wszSelFileName, L"") == 0)
	{
		wszSelFileName = m_pTreeCtrlUI->GetCurSelFileName();
		if (wcscmp(wszSelFileName, L"") == 0)
			return;// wszFileName = NULL;
	}

	strcpy(m_szFileName, WCharToChar(wszSelFileName));
	wcscpy(wszFileName, wszSelFileName);

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

	m_pFilePlayerObj->Close();

	if (strTemp.IsEmpty())
		m_pFilePlayerObj->Open(wszFileName);
	else
	{
		wcscpy(wszFileName, ANSIToUnicode(strTemp.GetBuffer()));
		m_pFilePlayerObj->Open(wszFileName);
	}

	m_pFilePlayerObj->Play();
}

void SimpleMixerWin::BtnAdd()
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);

	SetForegroundWindow(hMainWnd);
	EnableWindow(hMainWnd,false);

	HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
	if (hWnd)
	{
		m_hCurDlgWnd = hWnd;
		CtrlItemInfo item_info;

		SetParent(m_pNWin->GetSafeHwnd(), hWnd);
		if (m_iSkinMode == SM_1080P)
			m_pNWin->MoveWindow(910, 90, 554, 353);
		else if (m_iSkinMode == SM_1366)
			m_pNWin->MoveWindow(845, 90, 514, 353);
		else if (m_iSkinMode == SM_1280)
			m_pNWin->MoveWindow(792, 90, 514, 353);

		//m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)EB_EDITBOX0, (void*)"Test");

		m_pUIOutMsg->SendMsg(UI_MSG_RESET_CONTENT, (void*)CBB_COMBOBOX0, 0);

#ifdef _ENABLE_HD_EFFECT
		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			sprintf(item_info.szText, "%02d. %s", g_hd_effect_param_items[i].iID, g_hd_effect_param_items[i].szEffectName);

			item_info.iIndex = -1;
			item_info.ulIconID = g_hd_effect_param_items[i].iID;
			m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);
		}
#else
		int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			sprintf(item_info.szText, "%s"/*"%02d. %s", g_effect_param_items[i].iID*/, g_effect_param_items[i].szEffectName);

			item_info.iIndex = -1;
			item_info.ulIconID = g_effect_param_items[i].iID;
			m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);
		}
#endif
		
		m_pUIOutMsg->SendMsg(UI_MSG_SET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);
		m_pUIOutMsg->SendMsg(UI_MSG_REDRAW_CTRL, (void*)CBB_COMBOBOX0, (void*)0);

		RangeInfo* pRange = new RangeInfo;

#ifdef _ENABLE_HD_EFFECT
		pRange->iMin = 1;
		pRange->iMax = 100;
#else
		pRange->iMin = 0;
		pRange->iMax = 100;
#endif
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER1, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER2, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER3, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER4, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER5, (void*)pRange);

		unsigned long ulPos = 50;
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER, (void*)ulPos);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER1, (void*)ulPos);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER2, (void*)ulPos);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER3, (void*)ulPos);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER4, (void*)ulPos);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER5, (void*)ulPos);

		Slider_Level(50);
		Slider_Param1(50);
		Slider_Param2(50);
		Slider_Param3(50);
		Slider_Param4(50);
		Slider_Param5(50);

		//show/hide ctrl
		//m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER1, (void*)0);

		for(int i = 0;i < 6;i++)
			ShowCtrl(i, false,"");

		//m_hCurDlgWnd = hWnd;
		ShowWindow(hWnd, SW_SHOW);
		SetTopWin(hWnd, true);

		m_iEditMode = EM_ADD;
	}
}

void SimpleMixerWin::BtnDelete()
{
	int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
	if (iCurSel != -1)
	{
		m_pEffectListCtrl->DeleteItem(iCurSel);
		Save();

		if (m_pEffectListCtrl->GetItemCount() == 0)
		{
			m_pFilePlayerObj->SetEffectIndex(0);
			m_pFilePlayerObj->SetEffectParam(0, 50, 50, 50, 50, 50);
		}
	}
}

void SimpleMixerWin::BtnEdit()
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);

	HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
	if (hWnd)
	{
		m_hCurDlgWnd = hWnd;

		CtrlItemInfo item_info;

		int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
		if (iCurSel != -1)
		{
			SMEffectItem* pSM_Item = m_pEffectListCtrl->GetItem(iCurSel);

			EnableWindow(hMainWnd, false);

			m_iEditMode = EM_EDIT;
			m_iSelIndex = iCurSel;

			strcpy(m_szCurItemName, pSM_Item->szName);

			SetParent(m_pNWin->GetSafeHwnd(), hWnd);
			if (m_iSkinMode == SM_1080P)
				m_pNWin->MoveWindow(910, 90, 554, 353);
			else if (m_iSkinMode == SM_1366)
				m_pNWin->MoveWindow(845, 90, 514, 353);
			else if (m_iSkinMode == SM_1280)
				m_pNWin->MoveWindow(600, 90, 365, 353);

			//pSM_Item->iEffectID;

			int iEffectIndex = 0;

#ifdef _ENABLE_HD_EFFECT
			int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
			for (int i = 0; i < iTotalOfEffectItems; i++)
			{
				if (pSM_Item->iEffectID == g_hd_effect_param_items[i].iID)
				{
					iEffectIndex = i;
					break;
				}
			}
#else
			int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
			for (int i = 0; i < iTotalOfEffectItems; i++)
			{
				if (pSM_Item->iEffectID == g_effect_param_items[i].iID)
				{
					iEffectIndex = i;
					break;
				}
			}
#endif

			m_pUIOutMsg->SendMsg(UI_MSG_RESET_CONTENT, (void*)CBB_COMBOBOX0, 0);

//			int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
			for (int i = 0; i < iTotalOfEffectItems; i++)
			{
#ifdef _ENABLE_HD_EFFECT
				sprintf(item_info.szText, "%02d. %s", g_hd_effect_param_items[i].iID, g_hd_effect_param_items[i].szEffectName);

				item_info.iIndex = -1;
				item_info.ulIconID = g_hd_effect_param_items[i].iID;
				m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);
#else
				sprintf(item_info.szText, "%s"/*"%02d. %s", g_effect_param_items[i].iID*/, g_effect_param_items[i].szEffectName);

				item_info.iIndex = -1;
				item_info.ulIconID = g_effect_param_items[i].iID;
				m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);
#endif
			}

			m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)EB_EDITBOX0, (void*)pSM_Item->szName);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)iEffectIndex);

			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER, (void*)pSM_Item->iLevel);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER1, (void*)pSM_Item->iParameter1);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER2, (void*)pSM_Item->iParameter2);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER3, (void*)pSM_Item->iParameter3);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER4, (void*)pSM_Item->iParameter4);
			m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER5, (void*)pSM_Item->iParameter5);

			Slider_Level(pSM_Item->iLevel);
			Slider_Param1(pSM_Item->iParameter1);
			Slider_Param2(pSM_Item->iParameter2);
			Slider_Param3(pSM_Item->iParameter3);
			Slider_Param4(pSM_Item->iParameter4);
			Slider_Param5(pSM_Item->iParameter5);

			SelectEffectItem(iEffectIndex);

#ifdef _ENABLE_HD_EFFECT
			m_pFilePlayerObj->SetEffectParam(pSM_Item->iLevel, pSM_Item->iParameter1, pSM_Item->iParameter2, pSM_Item->iParameter3, pSM_Item->iParameter4, pSM_Item->iParameter5);
#endif

			ShowWindow(hWnd, SW_SHOW);
			SetTopWin(hWnd, true);
		}
	}
}

void SimpleMixerWin::BtnStop()
{
	m_pFilePlayerObj->Stop();
	m_pFilePlayerObj->Seek(0);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER6, (void*)0);
}

void SimpleMixerWin::BtnPlay()
{
	m_pFilePlayerObj->Play();
}

void SimpleMixerWin::BtnPause()
{
	m_pFilePlayerObj->Pause();
}

void SimpleMixerWin::Config_BtnOK()
{
	HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
	if (hWnd)
	{
		//m_hCurDlgWnd = hWnd;

		char* pszName = (char*)m_pUIOutMsg->SendMsg(UI_MSG_GET_CTRL_TEXT, (void*)EB_EDITBOX0, 0);

		CString cstr(pszName);
		LVFINDINFO strToFind;
		strToFind.flags = LVFI_STRING;
		strToFind.psz = cstr.Trim();//CString(pszName).Trim().GetBuffer();

		int iRetItemIndex = m_pEffectListCtrl->FindItem(&strToFind);

		if (cstr.IsEmpty())
		{
			MessageBox(hWnd, "Name is needed!", "Alarm", MB_TASKMODAL| MB_TOPMOST);
			return;
		}
		else if (iRetItemIndex != -1)
		{
			if (m_iEditMode == EM_EDIT && strcmp(m_szCurItemName, cstr.GetBuffer()) == 0 && iRetItemIndex == m_iSelIndex)
				;
			else
			{
				MessageBox(hWnd, "Duplicate effect name!", "Alarm", MB_TASKMODAL| MB_TOPMOST);
				return;
			}
		}
			
		int iSel = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);
		
#ifdef _ENABLE_HD_EFFECT
		SMEffectItem sm_effect_item;
		strcpy(sm_effect_item.szName, cstr/*pszName*/);
		strcpy(sm_effect_item.szEffectName, g_hd_effect_param_items[iSel].szEffectName);
		sm_effect_item.iEffectID = g_hd_effect_param_items[iSel].iID;

		//if(strcmp(g_effect_param_items[iSel].szParam[0],"") != 0)
		if (iSel != 0)
			sm_effect_item.iLevel = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER, (void*)0);
		else
			sm_effect_item.iLevel = 50;

		if (strcmp(g_hd_effect_param_items[iSel].szParam[1], "") != 0)
			sm_effect_item.iParameter1 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER1, (void*)0);
		else
			sm_effect_item.iParameter1 = 50;

		if (strcmp(g_hd_effect_param_items[iSel].szParam[2], "") != 0)
			sm_effect_item.iParameter2 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER2, (void*)0);
		else
			sm_effect_item.iParameter2 = 50;

		if (strcmp(g_hd_effect_param_items[iSel].szParam[3], "") != 0)
			sm_effect_item.iParameter3 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER3, (void*)0);
		else
			sm_effect_item.iParameter3 = 50;

		if (strcmp(g_hd_effect_param_items[iSel].szParam[4], "") != 0)
			sm_effect_item.iParameter4 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER4, (void*)0);
		else
			sm_effect_item.iParameter4 = 50;

		if (strcmp(g_hd_effect_param_items[iSel].szParam[4], "") != 0)
			sm_effect_item.iParameter5 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER5, (void*)0);
		else
			sm_effect_item.iParameter5 = 50;
#else
		SMEffectItem sm_effect_item;
		strcpy(sm_effect_item.szName, cstr/*pszName*/);
		strcpy(sm_effect_item.szEffectName, g_effect_param_items[iSel].szEffectName);
		sm_effect_item.iEffectID = g_effect_param_items[iSel].iID;

		//if(strcmp(g_effect_param_items[iSel].szParam[0],"") != 0)
		if (iSel != 0)
			sm_effect_item.iLevel = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER, (void*)0);
		else
			sm_effect_item.iLevel = 50;

		if (strcmp(g_effect_param_items[iSel].szParam[1], "") != 0)
			sm_effect_item.iParameter1 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER1, (void*)0);
		else
			sm_effect_item.iParameter1 = 50;

		if (strcmp(g_effect_param_items[iSel].szParam[2], "") != 0)
			sm_effect_item.iParameter2 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER2, (void*)0);
		else
			sm_effect_item.iParameter2 = 50;

		if (strcmp(g_effect_param_items[iSel].szParam[3], "") != 0)
			sm_effect_item.iParameter3 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER3, (void*)0);
		else
			sm_effect_item.iParameter3 = 50;

		if (strcmp(g_effect_param_items[iSel].szParam[4], "") != 0)
			sm_effect_item.iParameter4 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER4, (void*)0);
		else
			sm_effect_item.iParameter4 = 50;

		if (strcmp(g_effect_param_items[iSel].szParam[4], "") != 0)
			sm_effect_item.iParameter5 = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)SD_SLIDER5, (void*)0);
		else
			sm_effect_item.iParameter5 = 50;
#endif

		if (m_iEditMode == EM_ADD)
			m_pEffectListCtrl->AddItem(&sm_effect_item);
		else if (m_iEditMode == EM_EDIT)
			m_pEffectListCtrl->UpdateItem(m_iSelIndex, &sm_effect_item);

		ShowWindow(hWnd, SW_HIDE);
	}

	Save();

	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	EnableWindow(hMainWnd, true);
//	SendMessage(hMainWnd, WM_PAINT,0,0);
	m_hCurDlgWnd = hMainWnd;
	SetParent(m_pNWin->GetSafeHwnd(), hMainWnd);
	if (m_iSkinMode == SM_1080P)
		m_pNWin->MoveWindow(4, 90, 554, 353);
	else if (m_iSkinMode == SM_1366)
		m_pNWin->MoveWindow(4, 64, 394, 251);
	else if (m_iSkinMode == SM_1280)
		m_pNWin->MoveWindow(4, 60, 365, 232);

	SetFocus(hMainWnd);
	SetActiveWindow(hMainWnd);
}

void SimpleMixerWin::Config_BtnCancel()
{
	HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
	if (hWnd)
	{
		//m_hCurDlgWnd = hWnd;
		ShowWindow(hWnd, SW_HIDE);
	}

	m_pUIOutMsg->SendMsg(UI_MSG_CANCLE, (void*)CBB_COMBOBOX0, (void*)0);

	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	EnableWindow(hMainWnd, true);
//	SendMessage(hMainWnd, WM_PAINT, 0, 0);
	m_hCurDlgWnd = hMainWnd;
	SetParent(m_pNWin->GetSafeHwnd(), hMainWnd);
	if (m_iSkinMode == SM_1080P)
		m_pNWin->MoveWindow(4, 90, 554, 353);
	else if (m_iSkinMode == SM_1366)
		m_pNWin->MoveWindow(4, 64, 394, 251);
	else if (m_iSkinMode == SM_1280)
		m_pNWin->MoveWindow(4, 60, 365, 232);

	SetFocus(hMainWnd);
	SetActiveWindow(hMainWnd);
}

bool SimpleMixerWin::IsConfigMode()
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (m_hCurDlgWnd != hMainWnd)
		return true;
	return false;
}

void SimpleMixerWin::SelectEffectItem(int iSel)
{
#ifdef _ENABLE_HD_EFFECT
	m_pFilePlayerObj->SetEffectIndex(g_hd_effect_param_items[iSel].iID);

	for (int i = 0; i < 6; i++)
	{
		if (strcmp(g_hd_effect_param_items[iSel].szParam[i], "") != 0)
			ShowCtrl(i, true, g_hd_effect_param_items[iSel].szParam[i]);
		else
			ShowCtrl(i, false, "");
	}
#else
	m_pFilePlayerObj->SetEffectIndex(g_effect_param_items[iSel].iID);

	for (int i = 0; i < 6; i++)
	{
		if(strcmp(g_effect_param_items[iSel].szParam[i],"") != 0)
			ShowCtrl(i, true, g_effect_param_items[iSel].szParam[i]);
		else
			ShowCtrl(i, false,"");
	}
#endif

	HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
	if (hWnd)
		m_pUIOutMsg->SendMsg(UI_MSG_REDRAW, (void*)0, (void*)hWnd);
}

void SimpleMixerWin::Slider_Level(int iSel)
{
	char szText[64];
	sprintf(szText,"%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT9, (void*)szText);

	m_pFilePlayerObj->SetEffectLevel(iSel);
}

void SimpleMixerWin::Slider_Param1(int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT10, (void*)szText);

	m_pFilePlayerObj->SetEffectParam1(iSel);
}

void SimpleMixerWin::Slider_Param2(int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT11, (void*)szText);

	m_pFilePlayerObj->SetEffectParam2(iSel);
}

void SimpleMixerWin::Slider_Param3(int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT12, (void*)szText);

	m_pFilePlayerObj->SetEffectParam3(iSel);
}

void SimpleMixerWin::Slider_Param4(int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT13, (void*)szText);

	m_pFilePlayerObj->SetEffectParam4(iSel);
}

void SimpleMixerWin::Slider_Param5(int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT14, (void*)szText);

	m_pFilePlayerObj->SetEffectParam5(iSel);
}

void SimpleMixerWin::Slider_Pos(int iEventType,int iPos)
{
	switch (iEventType)
	{
		case CEVT_PRESS_BAR:
			m_pFilePlayerObj->EnableUpdatePos(false);
			break;
		case CEVT_RELEASE_BAR:
			{
				double dPos = iPos / 1000.0;
				m_pFilePlayerObj->Seek(dPos);
				m_pFilePlayerObj->EnableUpdatePos(true);
			}
			break;
		case CEVT_UPDATE:
			break;
	}
}

/*
0 - level
1 ~ 5 - param1 ~ param5
*/
void SimpleMixerWin::ShowCtrl(int iIndex,bool bShow,char* szParamText)
{
	if (iIndex == 0) //level
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT3, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT3, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT9, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT3, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT9, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER, (void*)bShow);
		
	}
	else if (iIndex == 1) //param1
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT4, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT4, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT10, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER1, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT4, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT10, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER1, (void*)bShow);
	}
	else if (iIndex == 2) //param2
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT5, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT5, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT11, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER2, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT5, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT11, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER2, (void*)bShow);
	}
	else if (iIndex == 3) //param3
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT6, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT6, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT12, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER3, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT6, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT12, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER3, (void*)bShow);
	}
	else if (iIndex == 4) //param4
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT7, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT7, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT13, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER4, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT7, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT13, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER4, (void*)bShow);
	}
	else if (iIndex == 5) //param5
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT8, (void*)szParamText);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT8, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)TX_TEXT14, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)SD_SLIDER5, (void*)bShow);

		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT8, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)TX_TEXT14, (void*)bShow);
		m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)SD_SLIDER5, (void*)bShow);
	}
}

void SimpleMixerWin::Save()
{
	wchar_t wszKeyName[256];
	wchar_t wszData[512];

	m_pSIniFileW->Open(wszIniFileName, true, true);

	int iTotalOfEffects = m_pEffectListCtrl->GetItemCount();

	wsprintfW(wszData, L"%d", iTotalOfEffects);
	m_pSIniFileW->SetItemData(L"EffectList", L"TotalOfEffects", wszData);

	for (int j = 0; j < iTotalOfEffects; j++)
	{
		SMEffectItem* pCurItem = m_pEffectListCtrl->GetItem(j);
		if (pCurItem)
		{
			wsprintfW(wszKeyName, L"Effect%d", j);

			wcscpy(wszData, ANSIToUnicode(pCurItem->szName));
			m_pSIniFileW->SetItemData(wszKeyName, L"Name", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iEffectID);
			m_pSIniFileW->SetItemData(wszKeyName, L"EffectID", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iLevel);
			m_pSIniFileW->SetItemData(wszKeyName, L"LV", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iParameter1);
			m_pSIniFileW->SetItemData(wszKeyName, L"P1", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iParameter2);
			m_pSIniFileW->SetItemData(wszKeyName, L"P2", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iParameter3);
			m_pSIniFileW->SetItemData(wszKeyName, L"P3", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iParameter4);
			m_pSIniFileW->SetItemData(wszKeyName, L"P4", wszData);

			wsprintfW(wszData, L"%d", pCurItem->iParameter5);
			m_pSIniFileW->SetItemData(wszKeyName, L"P5", wszData);
		}
	}
	m_pSIniFileW->Write();
	m_pSIniFileW->Close();
}

void SimpleMixerWin::Load()
{
	wchar_t wszKeyName[256];
	wchar_t wszData[512];

	SMEffectItem sm_effect;

	m_pSIniFileW->Open(wszIniFileName, true, false);

	m_pEffectListCtrl->Reset();

	wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"EffectList", L"TotalOfEffects", L"0");
	int iTotalOfEffects = _wtoi(wszTotal);

	for (int j = 0; j < iTotalOfEffects; j++)
	{
		wsprintfW(wszKeyName, L"Effect%d", j);

		wchar_t* wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"Name", L"");
		strcpy(sm_effect.szName, WCharToChar(wszItemData));

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"EffectID", L"0");
		sm_effect.iEffectID = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"LV", L"0");
		sm_effect.iLevel = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"P1", L"0");
		sm_effect.iParameter1 = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"P2", L"0");
		sm_effect.iParameter2 = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"P3", L"0");
		sm_effect.iParameter3 = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"P4", L"0");
		sm_effect.iParameter4 = _wtoi(wszItemData);

		wszItemData = m_pSIniFileW->GetItemValue(wszKeyName, L"P5", L"0");
		sm_effect.iParameter5 = _wtoi(wszItemData);


		int iEffectIndex = 0;
#ifdef _ENABLE_HD_EFFECT
		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (sm_effect.iEffectID == g_hd_effect_param_items[i].iID)
			{
				iEffectIndex = i;
				break;
			}
		}

		strcpy(sm_effect.szEffectName, g_hd_effect_param_items[iEffectIndex].szEffectName);
#else
		int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (sm_effect.iEffectID == g_effect_param_items[i].iID)
			{
				iEffectIndex = i;
				break;
			}
		}

		strcpy(sm_effect.szEffectName, g_effect_param_items[iEffectIndex].szEffectName);
#endif

		m_pEffectListCtrl->AddItem(&sm_effect);
	}

	m_pSIniFileW->Close();
}

void SimpleMixerWin::SetDefault()
{
	int iCurSel = (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);
	if (iCurSel != -1)
	{
	//	int iEffectIndex = -1;

		//SMEffectItem* pSM_Item = m_pEffectListCtrl->GetItem(iCurSel);

		g_hd_effect_param_items[iCurSel].iID;

		RangeInfo* pRange = new RangeInfo;

#ifdef _ENABLE_HD_EFFECT
		pRange->iMin = 1;
		pRange->iMax = 100;
#else
		pRange->iMin = 0;
		pRange->iMax = 100;
#endif
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER1, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER2, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER3, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER4, (void*)pRange);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER5, (void*)pRange);

		switch (g_hd_effect_param_items[iCurSel].iID)
		{
			case HD_EID_BRIGHTNESS:
			case HD_EID_SATURATION:
				Slider_Level(50);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER, (void*)50);
				break;
			case HD_EID_CONTRAST:
			case HD_EID_HUE:
				Slider_Level(20);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER, (void*)20);
				break;
			case HD_EID_RGBA:
				Slider_Level(100);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER, (void*)100);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER1, (void*)50);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER2, (void*)50);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER3, (void*)50);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER4, (void*)50);
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER5, (void*)50);
				break;
		}

		HWND hWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_WIN, (void*)WIN_WIN0, 0);
		if (hWnd)
			m_pUIOutMsg->SendMsg(UI_MSG_REDRAW, (void*)0, (void*)hWnd);
	}
}

void SimpleMixerWin::ButtonEvent(unsigned long ulCtrlID)
{
	int iIndex = -1;
	switch (ulCtrlID)
	{
		case BN_BUTTON0:
			BtnChange();
			break;
		case BN_BUTTON1:
			BtnAdd();
			break;
		case BN_BUTTON2:
			BtnDelete();
			break;
		case BN_BUTTON3:
			BtnEdit();
			break;
		case BN_BUTTON4:
			Config_BtnOK();
			break;
		case BN_BUTTON5:
			Config_BtnCancel();
			break;
		case BN_BUTTON6:
			BtnStop();
			break;
		case BN_BUTTON7:
			BtnPlay();
			break;
		case BN_BUTTON8:
			BtnPause();
			break;
	}
}

void SimpleMixerWin::ComboBoxEvent(unsigned long ulCtrlID, int iSel)
{
	switch (ulCtrlID)
	{
		case CBB_COMBOBOX0:
			SelectEffectItem(iSel);
#ifdef _ENABLE_HD_EFFECT
			SetDefault();
#endif
			break;
	}
}

void SimpleMixerWin::SliderEvent(unsigned long ulCtrlID, int iEventType, int iSel)
{
	switch (ulCtrlID)
	{
		case SD_SLIDER:
			Slider_Level(iSel);
			break;
		case SD_SLIDER1:
			Slider_Param1(iSel);
			break;
		case SD_SLIDER2:
			Slider_Param2(iSel);
			break;
		case SD_SLIDER3:
			Slider_Param3(iSel);
			break;
		case SD_SLIDER4:
			Slider_Param4(iSel);
			break;
		case SD_SLIDER5:
			Slider_Param5(iSel);
			break;
		case SD_SLIDER6:
			{
				Slider_Pos(iEventType, iSel);
			}
			break;
	}
}

void* SimpleMixerWin::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case EF_EVT_SEL_CHANGED:
			{
				int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
				if (iCurSel != -1)
				{
					SMEffectItem* pSelItem = m_pEffectListCtrl->GetItem(iCurSel);

					EnableButtons(true);

					m_pFilePlayerObj->SetEffectIndex(pSelItem->iEffectID);
					m_pFilePlayerObj->SetEffectParam(pSelItem->iLevel, pSelItem->iParameter1, pSelItem->iParameter2, pSelItem->iParameter3, pSelItem->iParameter4, pSelItem->iParameter5);

#ifdef _ENABLE_HD_EFFECT
#endif
				}
				else
				{
					EnableButtons(false);
				}
			}
			break;
		case MF_EVT_SEL_CHANGED:
			{
				BtnChange();
			}
			break;
		case OBJ_MSG_SET_SLIDER_RANGE:
			{
				long lRange = (long)pParameter1;

				RangeInfo* pRange = new RangeInfo;
				pRange->iMin = 0;
				pRange->iMax = lRange;
				m_pUIOutMsg->SendMsg(UI_MSG_SET_RANGE, (void*)SD_SLIDER6, (void*)pRange);
			}
			break;
		case OBJ_MSG_SET_SLIDER_POS:
			{
				long lPos = (long)pParameter1;
				m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)SD_SLIDER6, (void*)lPos);
			}
			break;
		case OBJ_MSG_UPDATE_POS_INFO:
			m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)TX_TEXT15, (void*)pParameter1);
			break;
	}
	return 0;
}

void SimpleMixerWin::CheckSourceFile()
{
	//EQ_1!@#$%^.mov
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
			sprintf(szDecriptFileName,"%s!@#$%%^%s", szTmpStr, pch);

			if (PathFileExistsA(szDecriptFileName))
				return;
		}

		wchar_t* wszSelFileName = m_pFileListCtrl->GetFirstFileName();
		if (wszSelFileName)
		{
			strcpy(m_szFileName, WCharToChar(wszSelFileName));

			if (!PathFileExistsA(m_szFileName))
			{
				char* pch = strrchr(m_szFileName, '.');
				if (pch)
				{
					int iTmpLen = strlen(m_szFileName) - strlen(pch);
					memcpy(szTmpStr, m_szFileName, iTmpLen);
					szTmpStr[iTmpLen] = '\0';
					sprintf(szDecriptFileName, "%s!@#$%%^%s", szTmpStr, pch);

					//wcscpy(wszDecriptFileName, ANSIToUnicode(szDecriptFileName));

					if (PathFileExistsA(szDecriptFileName))
						return;
				}

				strcpy(m_szFileName, "");
			}
		}
	}
}

void SimpleMixerWin::EnableButtons(bool bEnable)
{
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)BN_BUTTON2, (void*)bEnable);
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)BN_BUTTON3, (void*)bEnable);

	m_pUIOutMsg->SendMsg(UI_MSG_REDRAW_CTRL, (void*)BN_BUTTON2, (void*)true);
	m_pUIOutMsg->SendMsg(UI_MSG_REDRAW_CTRL, (void*)BN_BUTTON3, (void*)true);
}

void SimpleMixerWin::UpdateUIStatus()
{
	int iCurSel = m_pEffectListCtrl->GetNextSelectItem(-1);
	if (iCurSel != -1)
		EnableButtons(true);
	else
		EnableButtons(false);
}



