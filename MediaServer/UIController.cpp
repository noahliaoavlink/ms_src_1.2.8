#include "stdafx.h"
#include "UIController.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/Sstring.h"
#include "MediaServer.h"


UIController* g_pUIController;

void* _cdecl _UICtrlMsgProc(int iMsg, void* pParameter1, void* pParameter2);

UIController::UIController()
{
	g_pUIController = this;
	m_hMSMainUIWnd = 0;

	m_pNWin = 0;
	m_pFilePlayerObj = 0;
	m_pEffectListCtrl = 0;

	m_pAudioConfigWin = 0;
	m_pUIOutMsg = 0;

	m_iUIMode = UIM_NONE;
	m_iSkinMode = SM_1080P;
}

UIController::~UIController()
{
	FreeLib();

	if (m_pSimpleMediaCtrl)
		delete m_pSimpleMediaCtrl;

	if (m_pSimpleMixerWin)
		delete m_pSimpleMixerWin;

	if (m_pEffectSelectorWin)
		delete m_pEffectSelectorWin;

	if (m_pAudioConfigWin)
		delete m_pAudioConfigWin;

	if (m_pNWin)
		delete m_pNWin;

	if (m_pFilePlayerObj)
		delete m_pFilePlayerObj;

	if (m_pEffectListCtrl)
		delete m_pEffectListCtrl;
}

bool UIController::LoadLib()
{
	//Load UI
	m_hUIInst = LoadLibraryA("MiracleUI.dll");
	if (m_hUIInst)
	{
		UI_SetMsgProc = (MsgOutlet_Proc)GetProcAddress(m_hUIInst, "MsgOutlet");

		m_pUIOutMsg = new MsgInfo;
		m_pUILocalMsg = new MsgInfo;
		m_pUILocalMsg->SendMsg = _UICtrlMsgProc;
		(*UI_SetMsgProc)(m_pUIOutMsg, m_pUILocalMsg);
		return true;
	}
	else
	{
		::MessageBoxA(NULL, "Load 'MiracleUI.dll' Failed!!", "", MB_OK| MB_TOPMOST);
	}

	return false;
}

void UIController::FreeLib()
{
	if (m_hUIInst)
	{
		m_pUIOutMsg->SendMsg(UI_MSG_DESTROY, 0, 0);
		delete m_pUIOutMsg;
		delete m_pUILocalMsg;
		FreeLibrary(m_hUIInst);
	}
}

void UIController::Init()
{
	LoadLib();

	int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (iScreenWidth >= 1920)
		m_iSkinMode = SM_1080P;
	else if (iScreenWidth < 1920 && iScreenWidth >= 1366)
		m_iSkinMode = SM_1366;
	else if (iScreenWidth < 1366)
		m_iSkinMode = SM_1280;

	m_pNWin = new CNWin("MixerDisplayWin");
	m_pNWin->SetBKColor(0, 0, 0);
	m_pFilePlayerObj = new FilePlayerObj;

	if (m_iSkinMode == SM_1080P)
		m_pNWin->SetWinSize(554, 353,false);
	else if (m_iSkinMode == SM_1366)
		m_pNWin->SetWinSize(394, 251, false);
	else if (m_iSkinMode == SM_1280)
		m_pNWin->SetWinSize(370, 232, false);

	m_pFilePlayerObj->Init(m_pNWin->GetSafeHwnd());

	m_pEffectListCtrl = new EffectListCtrl;
	m_pEffectListCtrl->Create("EffectList", IDC_SM_EFFECT_LIST_CTRL, 400, 200);
	m_pEffectListCtrl->Init();
	m_pEffectListCtrl->SetSimpleMode(true);

	m_pSimpleMediaCtrl = new SimpleMediaCtrl;
#ifdef ENABLE_UI_CTRL
	m_pSimpleMediaCtrl->SetUIOutMsg(m_pUIOutMsg);
#endif

	m_pSimpleMixerWin = new SimpleMixerWin;
	m_pSimpleMixerWin->SetUIOutMsg(m_pUIOutMsg);
	m_pSimpleMixerWin->SetMainUIHandle(m_hMSMainUIWnd);
	m_pSimpleMixerWin->SetFilePlayerObj(m_pFilePlayerObj);
	m_pSimpleMixerWin->SetNWin(m_pNWin);
	m_pSimpleMixerWin->SetEffectListCtrl(m_pEffectListCtrl);
	m_pSimpleMixerWin->Load();

	m_pEffectSelectorWin = new EffectSelectorWin;
	m_pEffectSelectorWin->SetUIOutMsg(m_pUIOutMsg);
	m_pEffectSelectorWin->SetMainUIHandle(m_hMSMainUIWnd);
	m_pEffectSelectorWin->SetFilePlayerObj(m_pFilePlayerObj);
	m_pEffectSelectorWin->SetNWin(m_pNWin);
	m_pEffectSelectorWin->SetEffectListCtrl(m_pEffectListCtrl);

	m_pAudioConfigWin = new AudioConfigWin;
	m_pAudioConfigWin->SetUIOutMsg(m_pUIOutMsg);
	m_pAudioConfigWin->SetMainUIHandle(m_hMSMainUIWnd);

	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	
#if 1
	sprintf(szSkinDir, "%s\\UI Folder\\Media", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
	strcpy(skininfo.szSettingFile, "SimpleMediaCtrl_UIInfo.txt");
#else
	sprintf(szSkinDir, "%s\\UI Folder\\SimpleMixer", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
	strcpy(skininfo.szSettingFile, "SimpleMixer_UIInfo.txt");
#endif

	if (m_pUIOutMsg)
	{
		m_pUIOutMsg->SendMsg(UI_MSG_INIT, &skininfo, 0);

		HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
		if (hMainWnd)
		{
			ShowWindow(hMainWnd, SW_HIDE);
		}
	}
}

void UIController::SetMediaFileManagerCallback(MediaFileManagerCallback* pObj)
{
	m_pMediaFileManagerCallback = pObj;
}

void UIController::SetIOSourceCtrlDll(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void UIController::SetCSVReader(CSVReader* pObj)
{
	m_pCSVReader = pObj;
}

void UIController::SetMainUIHandle(HWND hWnd)
{
	m_hMSMainUIWnd = hWnd;
}

AudioConfigWin* UIController::GetAudioConfigWin()
{
	return m_pAudioConfigWin;
}

void UIController::OpenSimpleMediaCtrl()
{
	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szSkinDir, "%s\\UI Folder\\Media", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
	strcpy(skininfo.szSettingFile, "SimpleMediaCtrl_UIInfo.txt");

	m_pUIOutMsg->SendMsg(UI_MSG_CHANGE_SKIN, szSkinDir, skininfo.szSettingFile);

	m_pSimpleMediaCtrl->SetIOSourceCtrlDll(m_pIOSourceCtrlDll);
	m_pSimpleMediaCtrl->SetCSVReader(m_pCSVReader);
	m_pSimpleMediaCtrl->SetMediaFileManagerCallback(m_pMediaFileManagerCallback);

	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_MAIN_WIN_MOVABLE, (void*)WIN_MAIN, (void*)true);

	m_pSimpleMediaCtrl->Init();
	m_pSimpleMediaCtrl->Show(true);
	m_iUIMode = UIM_SP_MEDIA;
}

void UIController::OpenSimpleMixer()
{
	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szSkinDir, "%s\\UI Folder\\SimpleMixer", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);

	if(m_iSkinMode == SM_1080P)
		strcpy(skininfo.szSettingFile, "SimpleMixer_UIInfo.txt");
	else if(m_iSkinMode == SM_1366)
		strcpy(skininfo.szSettingFile, "SimpleMixer_1366_UIInfo.txt");
	else if(m_iSkinMode == SM_1280)
		strcpy(skininfo.szSettingFile, "SimpleMixer_1280_UIInfo.txt");

	m_pUIOutMsg->SendMsg(UI_MSG_CHANGE_SKIN, szSkinDir, skininfo.szSettingFile);

	m_pSimpleMixerWin->SetIOSourceCtrlDll(m_pIOSourceCtrlDll);
	m_pSimpleMixerWin->SetSkinMode(m_iSkinMode);

	m_pSimpleMixerWin->Init();
	m_pSimpleMixerWin->Show(true);

	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_MAIN_WIN_MOVABLE, (void*)WIN_MAIN, (void*)false);

	m_pEffectListCtrl->SetEventCallback(m_pSimpleMixerWin);
	m_iUIMode = UIM_SP_MIXER;
}

void UIController::OpenEffectSelector()
{
	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szSkinDir, "%s\\UI Folder\\SimpleMixer", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
	if (m_iSkinMode == SM_1080P)
		strcpy(skininfo.szSettingFile, "EffectSelector_UIInfo.txt");
	else if (m_iSkinMode == SM_1366)
		strcpy(skininfo.szSettingFile, "EffectSelector_1366_UIInfo.txt");
	else if (m_iSkinMode == SM_1280)
		strcpy(skininfo.szSettingFile, "EffectSelector_1280_UIInfo.txt");

	m_pUIOutMsg->SendMsg(UI_MSG_CHANGE_SKIN, szSkinDir, skininfo.szSettingFile);

	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_MAIN_WIN_MOVABLE, (void*)WIN_MAIN, (void*)true);

	m_pEffectSelectorWin->SetSkinMode(m_iSkinMode);
	m_pEffectSelectorWin->SetIOSourceCtrlDll(m_pIOSourceCtrlDll);
	m_pEffectSelectorWin->Init();
	m_pEffectSelectorWin->Show(true);
	m_pEffectListCtrl->SetEventCallback(m_pEffectSelectorWin);
	m_iUIMode = UIM_SP_EFFECT_SELECTOR;
}

void UIController::OpenAudioConfig()
{
	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szSkinDir, "%s\\UI Folder\\TimeLine", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
//	if (m_iSkinMode == SM_1080P)
		strcpy(skininfo.szSettingFile, "AudioConfig_UIInfo.txt");
//	else if (m_iSkinMode == SM_1366)
//		strcpy(skininfo.szSettingFile, "EffectSelector_1366_UIInfo.txt");

	m_pUIOutMsg->SendMsg(UI_MSG_CHANGE_SKIN, szSkinDir, skininfo.szSettingFile);

	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_MAIN_WIN_MOVABLE, (void*)WIN_MAIN, (void*)true);

	m_pAudioConfigWin->Init();
	m_pAudioConfigWin->Show(true);

	m_iUIMode = UIM_SP_AUDIO_CONFIG;
}

void UIController::ButtonEvent(unsigned long ulCtrlID)
{
	switch (m_iUIMode)
	{
		case UIM_SP_MEDIA:
			{
				if (m_pSimpleMediaCtrl)
					m_pSimpleMediaCtrl->ButtonEvent(ulCtrlID);
			}
			break;
		case UIM_SP_MIXER:
			{
				if (m_pSimpleMixerWin)
					m_pSimpleMixerWin->ButtonEvent(ulCtrlID);
			}
			break;
		case UIM_SP_EFFECT_SELECTOR:
			{
				if (m_pEffectSelectorWin)
					m_pEffectSelectorWin->ButtonEvent(ulCtrlID);
			}
			break;
		case UIM_SP_AUDIO_CONFIG:
			{
				if (m_pAudioConfigWin)
					m_pAudioConfigWin->ButtonEvent(ulCtrlID);
			}
			break;
	}

}

void UIController::ComboBoxEvent(unsigned long ulCtrlID, int iSel)
{
	switch (m_iUIMode)
	{
		case UIM_SP_MEDIA:
			{
				if (m_pSimpleMediaCtrl)
					m_pSimpleMediaCtrl->ComboBoxEvent(ulCtrlID, iSel);
			}
		break;
		case UIM_SP_MIXER:
			{
				if (m_pSimpleMixerWin)
					m_pSimpleMixerWin->ComboBoxEvent(ulCtrlID, iSel);
			}
			break;
	}
}

void UIController::SelChangeEvent(unsigned long ulCtrlID, int iSel)
{
	//if (m_pSimpleMediaCtrl)
		//m_pSimpleMediaCtrl->SelChangeEvent(ulCtrlID, iSel);

	switch (m_iUIMode)
	{
		case UIM_SP_MEDIA:
			{
				//if (m_pSimpleMediaCtrl)
				//	m_pSimpleMediaCtrl->ComboBoxEvent(ulCtrlID, iSel);
			}
			break;
		case UIM_SP_MIXER:
			{
				//if (m_pSimpleMixerWin)
					//m_pSimpleMixerWin->SelChangeEvent(ulCtrlID, iSel);
			}
			break;
	}
}

void UIController::SliderEvent(unsigned long ulCtrlID, int iValue, int iEventType)
{
	switch (m_iUIMode)
	{
		case UIM_SP_MEDIA:
		{
		}
		break;
		case UIM_SP_MIXER:
			{
				if (m_pSimpleMixerWin)
					m_pSimpleMixerWin->SliderEvent(ulCtrlID, iEventType, iValue);
			}
		break;
		case UIM_SP_AUDIO_CONFIG:
			{
			if (m_pAudioConfigWin)
				m_pAudioConfigWin->SliderEvent(ulCtrlID, iEventType, iValue);
			}
			break;
	}

	/*
	switch (iEventType)
	{
		case CEVT_PRESS_BAR:
			{
				//m_bIsPressed = true;
			}
			break;
		case CEVT_RELEASE_BAR:
			{
				//Seek(iValue);
				//m_bIsPressed = false;
			}
			break;
		case CEVT_UPDATE:
			{
			}
			break;
		default:
			;
	}
	*/
}

void UIController::CancelEvent()
{
	switch (m_iUIMode)
	{
		case UIM_SP_MEDIA:
			{
				if (m_pSimpleMediaCtrl)
					m_pSimpleMediaCtrl->BtnCancel();
				m_iUIMode = UIM_NONE;
			}
			break;
		case UIM_SP_MIXER:
			{
				if (m_pSimpleMixerWin)
				{
					if (!m_pSimpleMixerWin->IsConfigMode())
						m_iUIMode = UIM_NONE;
					m_pSimpleMixerWin->BtnCancel();
				}
			}
			break;
		case UIM_SP_EFFECT_SELECTOR:
			{
				if (m_pEffectSelectorWin)
					m_pEffectSelectorWin->BtnCancel();
				m_iUIMode = UIM_NONE;
			}
			break;
	}

	//m_iUIMode = UIM_NONE;
}

void* _cdecl _UICtrlMsgProc(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case UI_MSG_CLICKED:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;

				if (g_pUIController)
					g_pUIController->ButtonEvent(ulCtrlID);
			}
			break;
		case UI_MSG_CTRL_SELECTED:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				unsigned long ulSel = (unsigned long)pParameter2;

				if (g_pUIController)
					g_pUIController->ComboBoxEvent(ulCtrlID, ulSel);
			}
			break;
		case UI_MSG_SELCHANGE:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				unsigned long ulSel = (unsigned long)pParameter2;

				if (g_pUIController)
					g_pUIController->SelChangeEvent(ulCtrlID, ulSel);
			}
			break;
		case UI_MSG_CTRL_UPDATE:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				int iValue = (int)pParameter2;

				if (g_pUIController)
					g_pUIController->SliderEvent(ulCtrlID, iValue, CEVT_UPDATE);
			}
			break;
		case UI_MSG_PRESS_BAR:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				int iValue = (int)pParameter2;

				if (g_pUIController)
					g_pUIController->SliderEvent(ulCtrlID, iValue, CEVT_PRESS_BAR);
			}
			break;
		case UI_MSG_RELEASE_BAR:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				int iValue = (int)pParameter2;

				if (g_pUIController)
					g_pUIController->SliderEvent(ulCtrlID, iValue, CEVT_RELEASE_BAR);
			}
			break;
		case UI_MSG_ESCAPE:
		{
			if (g_pUIController)
				g_pUIController->CancelEvent();

			//if (g_pSimpleMediaCtrl)
			//	g_pSimpleMediaCtrl->BtnCancel();
		}
		break;
		
		case UI_MSG_GET_TIP_LANGUAGE:
		{
			wchar_t wszStr[512];
			wchar_t wszAppName[128];
			char szIniFile[512];
			wchar_t wszIniFile[512];
			sprintf(szIniFile, "%s\\Language\\mixer_%s.ini", theApp.m_strCurPath, theApp.m_strLanguage);
			wcscpy(wszIniFile, ANSIToUnicode(szIniFile));

			unsigned long ulCtrlID = (unsigned long)pParameter2;
			
			int item = ulCtrlID - TIP_BN_BUTTON0;
			if (item < 1 || item >8) swprintf(wszStr, L"");
			else {
				swprintf(wszAppName, L"Item%d", item - 1);
				GetPrivateProfileStringW(wszAppName, L"Str", L"", wszStr, 512, wszIniFile);
			}
			return wszStr;
		}
		break;
	}
	return 0;
}
