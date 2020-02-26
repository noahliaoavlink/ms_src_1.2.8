#include "stdafx.h"
#include "SimpleMediaCtrl.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/SCreateWin.h"

#define IDC_LIST_CTRL 1002

extern void GetExecutionPath(HMODULE hModule, char* szPath);
void* _cdecl _UIMsgProc(int iMsg, void* pParameter1, void* pParameter2);

SimpleMediaCtrl* g_pSimpleMediaCtrl;

SimpleMediaCtrl::SimpleMediaCtrl()
{
	g_pSimpleMediaCtrl = this;
	m_pIOSourceCtrlDll = 0;
	m_pCSVReader = 0;
	m_pMediaFileManagerCallback = 0;
	m_pTreeCtrlUI = 0;
	m_pFileListCtrl = 0;
	m_hUIInst = 0;
}

SimpleMediaCtrl::~SimpleMediaCtrl()
{
	if (m_pMediaFileManagerCallback)
		m_pMediaFileManagerCallback = NULL;

	DestroyUI();

	//if (m_pFileInfoCtrl)
		//delete m_pFileInfoCtrl;

//	if (m_pIOSourceCtrlDll)
	//{
		//m_pIOSourceCtrlDll->SaveMediaFile();
		//m_pIOSourceCtrlDll->SaveTask();
		//delete m_pIOSourceCtrlDll;
	//}

#ifdef ENABLE_UI_CTRL
#else
	FreeLib();
#endif

}

#ifdef ENABLE_UI_CTRL
#else

bool SimpleMediaCtrl::LoadLib()
{
	//Load UI
	m_hUIInst = LoadLibraryA("MiracleUI.dll");
	if (m_hUIInst)
	{
		UI_SetMsgProc = (MsgOutlet_Proc)GetProcAddress(m_hUIInst, "MsgOutlet");

		m_pUIOutMsg = new MsgInfo;
		m_pUILocalMsg = new MsgInfo;
		m_pUILocalMsg->SendMsg = _UIMsgProc;
		(*UI_SetMsgProc)(m_pUIOutMsg, m_pUILocalMsg);
		return true;
	}

	return false;
}

void SimpleMediaCtrl::FreeLib()
{
	if (m_hUIInst)
	{
		m_pUIOutMsg->SendMsg(UI_MSG_DESTROY, 0, 0);
		delete m_pUIOutMsg;
		delete m_pUILocalMsg;
		FreeLibrary(m_hUIInst);
	}
}
#endif

void SimpleMediaCtrl::Init()
{
#ifdef ENABLE_UI_CTRL
	DestroyUI();
#else
	LoadLib();

	char szAPPath[256];
	char szSkinDir[256];
	SkinInfo skininfo;

	skininfo.bHide = false;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szSkinDir, "%s\\UI Folder\\Media", szAPPath);
	strcpy(skininfo.szFolder, szSkinDir);
	strcpy(skininfo.szSettingFile, "SimpleMediaCtrl_UIInfo.txt");

	m_pUIOutMsg->SendMsg(UI_MSG_INIT, &skininfo, 0);
#endif

//	m_pIOSourceCtrlDll = new IOSourceCtrlDll;
//	m_pIOSourceCtrlDll->LoadLib();

	//m_pNWin = new CNWin("STRDisplayWnd", WT_NORMAL);
	m_pTreeCtrlUI = new TreeCtrlUI;
	m_pTreeCtrlUI->Init();
	m_pTreeCtrlUI->SetIOSourceCtrl(m_pIOSourceCtrlDll);
	m_pTreeCtrlUI->SetSimpleMode(true);

	m_pFileListCtrl = new FileListCtrl;
	m_pFileListCtrl->Create("FileList", IDC_LIST_CTRL, 400, 200);
	m_pFileListCtrl->Init();
	m_pFileListCtrl->SetIOSourceCtrl(m_pIOSourceCtrlDll);
	m_pFileListCtrl->SetSimpleMode(true);

	m_pTreeCtrlUI->SetFileListCtrl(m_pFileListCtrl);

	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		ShowWindow(hMainWnd, SW_HIDE);

		m_pTreeCtrlUI->SetParent(hMainWnd);

		m_pTreeCtrlUI->MoveWindow(5, 5, 315, 630);
		m_pTreeCtrlUI->ShowWindow(true);
		m_pTreeCtrlUI->Load();

		m_pFileListCtrl->SetParent(hMainWnd);
		m_pFileListCtrl->MoveWindow(325, 53, 629, 520);
		m_pFileListCtrl->ShowWindow(true);

		CtrlItemInfo item_info;
		m_pUIOutMsg->SendMsg(UI_MSG_RESET_CONTENT, (void*)CBB_COMBOBOX0, 0);

		strcpy(item_info.szText, "None");
		m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);

		for (int i = 0; i < m_pCSVReader->GetTotalOfTags(); i++)
		{
			char* szCurTag = m_pCSVReader->GetTagItemString(i);
			if (szCurTag)
			{
				//m_TagList.AddString(szCurTag);

				item_info.iIndex = -1;
				item_info.ulIconID = 0;
				strcpy(item_info.szText, szCurTag);
				m_pUIOutMsg->SendMsg(UI_MSG_ADD_STRING, (void*)CBB_COMBOBOX0, (void*)&item_info);
			}
		}

	}

	m_pUIOutMsg->SendMsg(UI_MSG_SET_CUR_SEL, (void*)CBB_COMBOBOX0, (void*)0);

	m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)BN_BUTTON2, (void*)false);
	m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)BN_BUTTON3, (void*)false);

	m_pTreeCtrlUI->SetDialogMode(MFDM_SELECT);

	//m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)PIC_PICTURE1, (void*)false);
}

#ifdef ENABLE_UI_CTRL
void SimpleMediaCtrl::SetUIOutMsg(MsgInfo* pObj)
{
	m_pUIOutMsg = pObj;
}
#endif

void SimpleMediaCtrl::DestroyUI()
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
}

void SimpleMediaCtrl::SetMediaFileManagerCallback(MediaFileManagerCallback* pObj)
{
	m_pMediaFileManagerCallback = pObj;
}

void SimpleMediaCtrl::SetIOSourceCtrlDll(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void SimpleMediaCtrl::SetCSVReader(CSVReader* pObj)
{
	m_pCSVReader = pObj;
}

void SimpleMediaCtrl::Show(bool bShow)
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		if(m_pTreeCtrlUI)
			m_pTreeCtrlUI->ShowWindow(bShow);

		if (bShow)
		{
			ShowWindow(hMainWnd, SW_SHOW);
			m_pTreeCtrlUI->Load();
			m_pTreeCtrlUI->UpdateList();
			SetTopWin(hMainWnd,true);
		}
		else
		{
#ifdef ENABLE_UI_CTRL
			if(m_pFileListCtrl)
				m_pFileListCtrl->ShowWindow(false);
#endif
			ShowWindow(hMainWnd, SW_HIDE);
		}
	}
}

void SimpleMediaCtrl::BtnOK()
{
	Show(false);

	wchar_t* wszFileName = m_pFileListCtrl->GetCurSelFileName();

	if (wcscmp(wszFileName, L"") == 0)
	{
		wszFileName = m_pTreeCtrlUI->GetCurSelFileName();
		if (wcscmp(wszFileName, L"") == 0)
			wszFileName = NULL;
		else
			;
		//return;
	}

	if (m_pMediaFileManagerCallback)
		m_pMediaFileManagerCallback->ReturnFileName(wszFileName);
}

void SimpleMediaCtrl::BtnCancel()
{
	if (m_pMediaFileManagerCallback)
		m_pMediaFileManagerCallback->ReturnFileName(NULL);
	Show(false);
}

void SimpleMediaCtrl::IconMode()
{
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)PIC_PICTURE2, (void*)false);
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)PIC_PICTURE1, (void*)true);
	m_pFileListCtrl->SetDisplayMode(FIDM_ICON);
}

void SimpleMediaCtrl::ListMode()
{
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)PIC_PICTURE1, (void*)false);
	m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)PIC_PICTURE2, (void*)true);
	m_pFileListCtrl->SetDisplayMode(FIDM_LIST);
}

void SimpleMediaCtrl::SelchangeTagItem(int iIndex)
{
	wchar_t wszKeyword[256];
	if (iIndex > 0)
	{
		char* szCurTag = m_pCSVReader->GetTagItemString(iIndex - 1);
		if (szCurTag)
		{
			wcscpy(wszKeyword, ANSIToUnicode(szCurTag));

			m_pIOSourceCtrlDll->DoTagSearch(wszKeyword);
			int iTotal = m_pIOSourceCtrlDll->GetTotalOfFileList();

			m_pFileListCtrl->Clean();

			int iIndex = 0;

			for (int i = 0; i < iTotal; i++)
			{
				MediaFileInfo* pCurInfo = m_pIOSourceCtrlDll->GetMediaFileInfo(i);
				if (pCurInfo)
				{
					if (wcsstr(pCurInfo->wszTag, wszKeyword))
					{
						m_pFileListCtrl->InsertItem(iIndex, pCurInfo->wszFileName);
						iIndex++;
					}
				}
			}
		}
	}
	else
		m_pTreeCtrlUI->UpdateList();
}

void SimpleMediaCtrl::ButtonEvent(unsigned long ulCtrlID)
{
	int iIndex = -1;
	switch (ulCtrlID)
	{
		/*
	case CB_CHECK_BOX0:
		ListMode();
		break;
	case CB_CHECK_BOX1:
		IconMode();
		break;
		*/
	case BN_BUTTON0:
		BtnOK();
		break;
	case BN_BUTTON1:
		BtnCancel();
		break;
	case BN_BUTTON2:
		IconMode();
		break;
	case BN_BUTTON3:
		ListMode();
		break;
		
	}
}

void SimpleMediaCtrl::ComboBoxEvent(unsigned long ulCtrlID, int iSel)
{
	switch (ulCtrlID)
	{
		case CBB_COMBOBOX0:
			SelchangeTagItem(iSel);
			break;
	}
}

#ifdef ENABLE_UI_CTRL
#else
void* _cdecl _UIMsgProc(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case UI_MSG_CLICKED:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;

				if (g_pSimpleMediaCtrl)
					g_pSimpleMediaCtrl->ButtonEvent(ulCtrlID);
			}
			break;
		case UI_MSG_CTRL_SELECTED:
			{
				unsigned long ulCtrlID = (unsigned long)pParameter1;
				int iCurSel = (int)pParameter2;

				if (g_pSimpleMediaCtrl)
					g_pSimpleMediaCtrl->ComboBoxEvent(ulCtrlID, iCurSel);
			}
			break;
		case UI_MSG_ESCAPE:
			{
				if (g_pSimpleMediaCtrl)
					g_pSimpleMediaCtrl->BtnCancel();
			}
			break;
	}
	return 0;
}
#endif