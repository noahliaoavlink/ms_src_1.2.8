// MediaCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "MediaCtrlDlg.h"
#include "afxdialogex.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/sstring.h"

#define _ESCAPE_KEY_ID 101
#define IDC_LIST_CTRL 1002
#define IDC_TAG_LIST  1003
// CMediaCtrlDlg dialog

unsigned int __stdcall _MC_Reset_ThreadProc(void* lpvThreadParm);
unsigned int __stdcall _MC_ReCreate_ThreadProc(void* lpvThreadParm);

extern CWaitingDlg g_WaitingDlg;

IMPLEMENT_DYNAMIC(CMediaCtrlDlg, CDialogEx)

CMediaCtrlDlg::CMediaCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEMP_DLG, pParent)
{
	m_FramePic = NULL;
	m_pIOSourceCtrlDll = 0;
	m_pTreeCtrlUI = 0;
	m_pFileListCtrl = 0;
	m_pFileInfoCtrl = 0;
	m_pReCreateBtn = 0;
	m_pResetBtn = 0;
	m_pUTipDll = 0;
}

CMediaCtrlDlg::~CMediaCtrlDlg()
{
}

void CMediaCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMediaCtrlDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_HOTKEY()
	ON_CBN_SELCHANGE(IDC_TAG_LIST, &CMediaCtrlDlg::OnCbnSelchangeCombo1)
	ON_MESSAGE(WM_BUTTON_CLICK, OnButtonClick)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMediaCtrlDlg message handlers
BOOL CMediaCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	m_iMode = LIST_MODE;

	m_pUTipDll = new UTipDll;
	m_pUTipDll->LoadLib();
	m_pUTipDll->_UT_Init(m_pUTipDll->GetUTObj());

	m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + "\\UI Folder\\Media\\MF_BG3.bmp");
/*	
	CItem_Class* pItem;
	pItem = new CItem_Class(this, m_hWnd, LIST_MODE, theApp.m_strCurPath + "\\UI Folder\\Media\\\\MFB-01.bmp", true, true, true);
	pItem->OffsetObject(CPoint(695, 10));
	m_FramePic->AddItem(pItem);
	
	pItem = new CItem_Class(this, m_hWnd, ICON_MODE, theApp.m_strCurPath + "\\UI Folder\\Media\\\\MFB-02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(780, 10));
	m_FramePic->AddItem(pItem);
	*/
	m_FramePic->Initialize();

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	if (iWidth != 1920 || iHeight != 1080)
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

	float fHScale = float(iWidth) / 1920;
	float fVScale = float(iHeight) / 1080;

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic->SetWindowRgn(NullRgn, true);

	m_pTreeCtrlUI = new TreeCtrlUI;
	m_pTreeCtrlUI->SetLogFileDll(m_pLogFileDll);

	m_pIOSourceCtrlDll = new IOSourceCtrlDll;
	m_pIOSourceCtrlDll->LoadLib();
	void* pLogFileObj = m_pTreeCtrlUI->GetLogFileObj();
	m_pIOSourceCtrlDll->SetLogFileDll(m_pLogFileDll, pLogFileObj);
	m_pIOSourceCtrlDll->Init();

	
	m_pTreeCtrlUI->Init();
	m_pTreeCtrlUI->SetIOSourceCtrl(m_pIOSourceCtrlDll);
	m_pTreeCtrlUI->SetParent(this->GetSafeHwnd());

	m_pFileListCtrl = new FileListCtrl;
	m_pFileListCtrl->Create("FileList", IDC_LIST_CTRL, 400, 200);
	m_pFileListCtrl->Init();
	m_pFileListCtrl->SetIOSourceCtrl(m_pIOSourceCtrlDll);

	m_pTreeCtrlUI->SetFileListCtrl(m_pFileListCtrl);

	m_pFileInfoCtrl = new FileInfoCtrl;
	m_pFileInfoCtrl->Init();
	m_pFileInfoCtrl->SetIOSourceCtrl(m_pIOSourceCtrlDll);

	m_pFileListCtrl->SetFileInfoCtrl(m_pFileInfoCtrl);

	m_pTreeCtrlUI->MoveWindow(2 * fHScale, 48 * fVScale, 537 * fHScale, 1029 * fVScale);
	m_pTreeCtrlUI->ShowWindow(true);
	m_pTreeCtrlUI->Load();

	m_pFileListCtrl->SetParent(this->GetSafeHwnd());
	m_pFileListCtrl->MoveWindow(545 * fHScale, 55 * fVScale, 1372 * fHScale, 590 * fVScale);
	m_pFileListCtrl->ShowWindow(true);

	m_pFileInfoCtrl->SetParent(this->GetSafeHwnd());
	m_pFileInfoCtrl->MoveWindow(546 * fHScale, 670 * fVScale, 1370 * fHScale, 400 * fVScale);
	m_pFileInfoCtrl->ShowWindow(true);

	int iTotal = m_pIOSourceCtrlDll->GetTotalOfFileList();
	if (iTotal == 0)
	{
		if (m_pLogFileDll)
		{
			if (pLogFileObj)
			{
				m_pLogFileDll->_Out_LMC_AddLog(pLogFileObj, MCTCC_LOAD_CSV, L"", L"", 0);
				m_pLogFileDll->_Out_LMC_Suspend(pLogFileObj);
			}
		}
	}

	LoadCSV();

	if (iTotal == 0)
	{
		if (m_pLogFileDll)
		{
			if (pLogFileObj)
				m_pLogFileDll->_Out_LMC_Resume(pLogFileObj);
		}
	}

	m_TagList.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,  //CBS_SORT | 
		CRect(1730 * fHScale, 15 * fVScale, 1890 * fHScale, 275 * fVScale), this, IDC_TAG_LIST);

	m_TagList.AddString("None");

	for (int i = 0; i < m_CSVReader.GetTotalOfTags(); i++)
	{
		char* szCurTag = m_CSVReader.GetTagItemString(i);
		if(szCurTag)
			m_TagList.AddString(szCurTag);
	}
	//m_TagList.AddString();
	m_TagList.SetCurSel(0);

	std::string sIconMode = theApp.m_strCurPath + "\\UI Folder\\Media\\Icon_mode2.bmp";
	std::string sListMode = theApp.m_strCurPath + "\\UI Folder\\Media\\List_mode2.bmp";

	m_IconMode.Load((char*)sIconMode.c_str());
	m_ListMode.Load((char*)sListMode.c_str());

	//531, 2, 198, 64
	m_rTab.left = 545 * fHScale;
	m_rTab.top = 1 * fVScale;
	m_rTab.right = m_rTab.left + 90 * fHScale;
	m_rTab.bottom = m_rTab.top + 47 * fVScale;

	m_rIcon.left = 545 * fHScale;
	m_rIcon.top = 1 * fVScale;
	//m_rIcon.right = m_rIcon.left + ((90 / 2) * fHScale);
	m_rIcon.right = m_rIcon.left + ((90 / 2));
	m_rIcon.bottom = m_rIcon.top + 47 * fVScale;

	//m_rList.left = (545 + (90 / 2)) * fHScale;
	m_rList.left = 545 * fHScale + (90 / 2);
	m_rList.top = 1 * fVScale;
	//m_rList.right = m_rList.left + ((90 / 2) * fHScale);
	m_rList.right = m_rList.left + ((90 / 2));
	m_rList.bottom = m_rList.top + 47 * fVScale;

	m_pReCreateBtn = new TxtButtonCtrl;
	m_pReCreateBtn->Create("ReCreate", 1600 * fHScale - 100, 20 * fVScale, 80, 20, this->GetSafeHwnd(), IDC_RECREATE);

	m_pResetBtn = new TxtButtonCtrl;
	m_pResetBtn->Create("Reset", 1600 * fHScale, 20 * fVScale, 80, 20, this->GetSafeHwnd(), IDC_RESET);

	SetTip();

	//RECT m_rList;

//	SetFocus();
//	GetActiveWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CMediaCtrlDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別	
	GetParent()->SendMessage(MSG_MAINUI);
	ShowWindow(SW_HIDE);
	return;

	CDialog::OnCancel();
}

void CMediaCtrlDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	ShowWindow(SW_SHOW);
	//	CDialog::OnOK();
}

void CMediaCtrlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼

	if (m_FramePic)
		delete m_FramePic;

	if (m_pResetBtn)
		delete m_pResetBtn;

	if (m_pReCreateBtn)
		delete m_pReCreateBtn;

	FreeUIObj();

	if (m_pUTipDll)
		delete m_pUTipDll;
}

void CMediaCtrlDlg::OnPaint()
{
	//m_bLBClicked = FALSE;

//	Invalidate();
	//UpdateWindow();
	m_FramePic->OnPaint();

	DrawDisplayModeIcon();

	m_pTreeCtrlUI->ReDraw();
	m_pFileListCtrl->ReDraw();
	m_pFileInfoCtrl->ReDraw();

	if (m_pResetBtn)
		m_pResetBtn->Draw();

	if(m_pReCreateBtn)
		m_pReCreateBtn->Draw();

	//RefreshRgbText();
	//RefreshEffectText();

	CDialog::OnPaint();
}

void CMediaCtrlDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
//		bool bRet = RegisterHotKey(this->GetSafeHwnd(), _ESCAPE_KEY_ID, 0, VK_ESCAPE);
		m_pTreeCtrlUI->UpdateList();
	}
	
	CDialog::OnShowWindow(bShow, nStatus);
}

void CMediaCtrlDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default

	if (nHotKeyId == _ESCAPE_KEY_ID)
	{
		UnregisterHotKey(this->GetSafeHwnd(), _ESCAPE_KEY_ID);
		OnCancel();
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

BOOL CMediaCtrlDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	switch (message)
	{/*
		case LIST_MODE:
			{
				m_pFileInfoCtrl->SetDisplayMode(FIDM_ICON);
				m_pFileListCtrl->SetDisplayMode(FIDM_LIST);
			}
			break;
		case ICON_MODE:
			{
				m_pFileInfoCtrl->SetDisplayMode(FIDM_LIST);
				m_pFileListCtrl->SetDisplayMode(FIDM_ICON);
			}
			break;
			*/
		case WM_LBUTTONDOWN:
			{
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);

				if (InRect(&m_rIcon, xPos, yPos))
				{
					m_iMode = ICON_MODE;
					DrawDisplayModeIcon();
					m_pFileInfoCtrl->SetDisplayMode(FIDM_LIST);
					m_pFileListCtrl->SetDisplayMode(FIDM_ICON);
				}
				else if (InRect(&m_rList, xPos, yPos))
				{
					m_iMode = LIST_MODE;
					DrawDisplayModeIcon();
					m_pFileInfoCtrl->SetDisplayMode(FIDM_ICON);
					m_pFileListCtrl->SetDisplayMode(FIDM_LIST);
				}
			}
			break;
		case WM_SYSCOMMAND:
		{
			if (SC_CLOSE == wParam)
				return true;
			else if (SC_KEYMENU == wParam)
				return true;
		}
		break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CMediaCtrlDlg::FreeUIObj()
{
	if (m_pTreeCtrlUI)
	{
//		m_pTreeCtrlUI->Save();
		m_pTreeCtrlUI->SetParent(NULL);
		delete m_pTreeCtrlUI;
	}

	if (m_pFileListCtrl)
	{
		m_pFileListCtrl->SetParent(NULL);
		delete m_pFileListCtrl;
	}

	if (m_pFileInfoCtrl)
	{
		m_pFileInfoCtrl->SetParent(NULL);
		delete m_pFileInfoCtrl;
	}

	if (m_pIOSourceCtrlDll)
	{
//		m_pIOSourceCtrlDll->SaveMediaFile();
		//m_pIOSourceCtrlDll->SaveTask();
		delete m_pIOSourceCtrlDll;
	}

}

void CMediaCtrlDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	CString str1;
	CComboBox* pTagList = (CComboBox*)GetDlgItem(IDC_TAG_LIST);
	wchar_t wszKeyword[256];

	if (pTagList->GetCurSel() > 0)
	{
		int n = pTagList->GetLBTextLen(pTagList->GetCurSel());
		pTagList->GetLBText(pTagList->GetCurSel(), str1.GetBuffer(n));

		wcscpy(wszKeyword, ANSIToUnicode(str1.GetBuffer()));

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
	else
		m_pTreeCtrlUI->UpdateList();
}

void CMediaCtrlDlg::DrawDisplayModeIcon()
{
	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	float fHScale = float(iWidth) / 1920;
	float fVScale = float(iHeight) / 1080;

	if(LIST_MODE == m_iMode)
		DrawBMP(&m_ListMode, 545 * fHScale, 1, 90, 47);
	else if (ICON_MODE == m_iMode)
		DrawBMP(&m_IconMode, 545 * fHScale, 1, 90, 47);
}

bool CMediaCtrlDlg::InRect(RECT* pRect,int x,int y)
{
	if (x >= pRect->left && x <= pRect->right && y >= pRect->top && y <= pRect->bottom)
		return true;
	return false;
}

void CMediaCtrlDlg::DrawBMP(ImageFile* pImgFile, int x, int y,int w,int h)
{
	BITMAPINFOHEADER bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.biWidth = pImgFile->GetWidth();
	bmpInfo.biHeight = pImgFile->GetHeight();
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;

	bmpInfo.biCompression = BI_RGB;
	bmpInfo.biSizeImage = pImgFile->GetWidth() * pImgFile->GetHeight() * 3;
	bmpInfo.biSizeImage = 0;

	HDC hdc;
	RECT rect;
	hdc = ::GetDC(this->GetSafeHwnd());

	if (x >= 0 && y >= 0)
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);
		StretchDIBits(hdc,
			x,
			y,
			w,
			h,
			0,
			0,
			pImgFile->GetWidth(),
			pImgFile->GetHeight() - 1,
			pImgFile->GetBuffer(),//pBuffer,
			(BITMAPINFO*)&bmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}

	::ReleaseDC(this->GetSafeHwnd(), hdc);
}

void CMediaCtrlDlg::LoadCSV(bool bReset)
{
	std::string sCSVFileName = theApp.m_strCurPath + "MOV\\Media-Tag-LUT.csv";
	std::string sMovPath = theApp.m_strCurPath + "MOV";
       wchar_t wszFileName[512];

	m_CSVReader.Open((char*)sCSVFileName.c_str());
	m_CSVReader.ParseData();
	m_CSVReader.Close();

	int iTotal = m_CSVReader.GetTotal();
	for (int i = 0; i < m_CSVReader.GetTotal(); i++)
	{
		TagInfo* pInfo = m_CSVReader.GetTagInfo(i);
		if (pInfo)
		{
			std::string sFullFilePath = sMovPath + "\\" + pInfo->szFileFolder + "\\" + pInfo->szFileName;
			char* szTag = m_CSVReader.GetTagString(i);

			wcscpy(wszFileName, ANSIToUnicode(sFullFilePath.c_str()));

			if (PathFileExists(sFullFilePath.c_str()))
			{
				if (g_PannelSetting.iEncodeMedia != 0)
					m_pIOSourceCtrlDll->BeginOpenFile(wszFileName);

				m_pTreeCtrlUI->AddCSVNode((char*)sFullFilePath.c_str(), pInfo->szFileFolder, pInfo->szFileName, szTag);

				if (g_PannelSetting.iEncodeMedia != 0)
					m_pIOSourceCtrlDll->EndOpenFile(wszFileName);
			}
			else
			{
				if (bReset)
				{
					char szTmpStr[512];
					char szDecriptFileName[512];
					char* pch = strrchr((char*)sFullFilePath.c_str(), '.');
					if (pch)
					{
						int iTmpLen = strlen(sFullFilePath.c_str()) - strlen(pch);
						memcpy(szTmpStr, sFullFilePath.c_str(), iTmpLen);
						szTmpStr[iTmpLen] = '\0';
						sprintf(szDecriptFileName, "%s!@#$%%^%s", szTmpStr, pch);

						if (PathFileExistsA(szDecriptFileName) && !m_pTreeCtrlUI->IsOpened(szDecriptFileName))
						{
							do
							{
								CPlaneScene3D::FileEncoder(szDecriptFileName, FALSE, TRUE);
							} while (0);

							if (PathFileExists(sFullFilePath.c_str()))
							{
								if (g_PannelSetting.iEncodeMedia != 0)
									m_pIOSourceCtrlDll->BeginOpenFile(wszFileName);

								m_pTreeCtrlUI->AddCSVNode((char*)sFullFilePath.c_str(), pInfo->szFileFolder, pInfo->szFileName, szTag);

								if (g_PannelSetting.iEncodeMedia != 0)
									m_pIOSourceCtrlDll->EndOpenFile(wszFileName);
							}
						}
					}
				}
			}
		}
	}
	m_pTreeCtrlUI->Save();

	if (m_pIOSourceCtrlDll)
		m_pIOSourceCtrlDll->SaveMediaFile();
}

IOSourceCtrlDll* CMediaCtrlDlg::GetIOSourceCtrlDll()
{
	return m_pIOSourceCtrlDll;
}

CSVReader* CMediaCtrlDlg::GetCSVReader()
{
	return &m_CSVReader;
}

void CMediaCtrlDlg::Reset()
{
	RECT rect;
	RECT rDlgRect;
	this->GetClientRect(&rDlgRect);
	g_WaitingDlg.GetClientRect(&rect);

	int x = (rDlgRect.right - rect.right) / 2;
	int y = (rDlgRect.bottom - rect.bottom) / 2;

	this->EnableWindow(false);

	g_WaitingDlg.ShowWindow(SW_SHOW);
	g_WaitingDlg.MoveWindow(x,y, rect.right, rect.bottom,false);
	m_pIOSourceCtrlDll->Reset();

	void* pLogFileObj = m_pTreeCtrlUI->GetLogFileObj();

	int iRemoveAllRet;
	try
	{
		iRemoveAllRet = m_pTreeCtrlUI->RemoveAll();
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in AddFile2 !! \n");
		::MessageBoxA(NULL, "m_pTreeCtrlUI->RemoveAll() Failed!!", "", MB_OK| MB_TOPMOST);
	}

	if (m_pLogFileDll)
	{
		if (pLogFileObj)
		{
			m_pLogFileDll->_Out_LMC_AddLog(pLogFileObj, MCTCC_RESET, L"", L"", 0);
			m_pLogFileDll->_Out_LMC_AddLog(pLogFileObj, MCTCC_LOAD_CSV, L"", L"", 0);
			m_pLogFileDll->_Out_LMC_Suspend(pLogFileObj);
		}
	}

	try
	{
		LoadCSV(true);
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in AddFile2 !! \n");
		::MessageBoxA(NULL, "LoadCSV() Failed!!", "", MB_OK| MB_TOPMOST);
	}

	if (m_pLogFileDll)
	{
		if (pLogFileObj)
			m_pLogFileDll->_Out_LMC_Resume(pLogFileObj);
	}

	if (iRemoveAllRet == -1)
	{
		ReCreate2();
	}

	g_WaitingDlg.ShowWindow(SW_HIDE);

	if (iRemoveAllRet == -1)
	{
		int i;
		char szMsg[512];
		char szFileName[515];
		char szFolderName[512];
		CString strFolderName = theApp.m_strCurPath + "MOV\\User";
		strcpy(szFolderName, strFolderName.GetString());

		SQList* pFileList = new SQList;
		SQList* pFolderList = new SQList;

		m_pTreeCtrlUI->SearchAllFiles(szFolderName, pFileList, pFolderList);

		for (i = 0; i < pFileList->GetTotal(); i++)
		{
			strcpy(szFileName, (char*)pFileList->Get(i));

			if (m_pTreeCtrlUI->IsOpened(szFileName))
			{
				sprintf(szMsg, "The action can't be completed because the '%s' is open in another module.", szFileName);
				::MessageBoxA(NULL, szMsg, "Warning", MB_OK| MB_TOPMOST);
			}

		}

		for (i = 0; i < pFileList->GetTotal(); i++)
		{
			char* pCurItem = (char*)pFileList->Get(i);
			delete pCurItem;
		}
		pFileList->EnableRemoveData(false);
		pFileList->Reset();
		delete pFileList;

		for (i = 0; i < pFolderList->GetTotal(); i++)
		{
			char* pCurItem = (char*)pFolderList->Get(i);
			delete pCurItem;
		}
		pFolderList->EnableRemoveData(false);
		pFolderList->Reset();
		delete pFolderList;
	}

	this->EnableWindow(true);
}

void CMediaCtrlDlg::ReCreate()
{
#if 0
	int i;
	char szFolderName[512];
	char szCurFolder[512];
	char szCurFile[512];
	char szFilePath[512];
	int iFolderLen = 0;
	SQList* pFileList = new SQList;
	SQList* pFolderList = new SQList;

	RECT rect;
	RECT rDlgRect;
	this->GetClientRect(&rDlgRect);
	g_WaitingDlg.GetClientRect(&rect);

	int x = (rDlgRect.right - rect.right) / 2;
	int y = (rDlgRect.bottom - rect.bottom) / 2;

	this->EnableWindow(false);

	g_WaitingDlg.ShowWindow(SW_SHOW);
	g_WaitingDlg.MoveWindow(x, y, rect.right, rect.bottom, false);

	CString strFolderName = theApp.m_strCurPath + "MOV\\User";
	strcpy(szFolderName, strFolderName.GetString());

	iFolderLen = strlen(szFolderName) + 1;

	m_pTreeCtrlUI->SearchAllFiles(szFolderName, pFileList, pFolderList);

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		strcpy(szCurFolder, pCurItem + iFolderLen);

		InsertFolderNode(szCurFolder);
	}

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFileList->Get(i);
		strcpy(szCurFile, pCurItem);
		char* pch = strrchr(pCurItem,'\\');
		if (pch)
		{
			int iPathLen = strlen(pCurItem) - strlen(pch);
			memcpy(szFilePath, pCurItem, iPathLen);
			szFilePath[iPathLen] = '\0';
		}

		strcpy(szCurFolder, szFilePath + iFolderLen);

		InsertFileNode(szCurFolder, szCurFile);
	}

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFileList->Get(i);
		delete pCurItem;
	}
	pFileList->EnableRemoveData(false);
	pFileList->Reset();
	delete pFileList;

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		delete pCurItem;
	}
	pFolderList->EnableRemoveData(false);
	pFolderList->Reset();
	delete pFolderList;

	m_pTreeCtrlUI->Save();
	m_pIOSourceCtrlDll->SaveMediaFile();

	g_WaitingDlg.ShowWindow(SW_HIDE);
	this->EnableWindow(true);
#else
	RECT rect;
	RECT rDlgRect;
	this->GetClientRect(&rDlgRect);
	g_WaitingDlg.GetClientRect(&rect);

	int x = (rDlgRect.right - rect.right) / 2;
	int y = (rDlgRect.bottom - rect.bottom) / 2;

	this->EnableWindow(false);

	g_WaitingDlg.ShowWindow(SW_SHOW);
	g_WaitingDlg.MoveWindow(x, y, rect.right, rect.bottom, false);

	ReCreate2();

	g_WaitingDlg.ShowWindow(SW_HIDE);
	this->EnableWindow(true);
#endif
}

void CMediaCtrlDlg::ReCreate2()
{
	int i;
	char szFolderName[512];
	char szCurFolder[512];
	char szCurFile[512];
	char szFilePath[512];
	int iFolderLen = 0;
	SQList* pFileList = new SQList;
	SQList* pFolderList = new SQList;

	CString strFolderName = theApp.m_strCurPath + "MOV\\User";
	strcpy(szFolderName, strFolderName.GetString());

	iFolderLen = strlen(szFolderName) + 1;

	m_pTreeCtrlUI->SearchAllFiles(szFolderName, pFileList, pFolderList);

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		strcpy(szCurFolder, pCurItem + iFolderLen);

		InsertFolderNode(szCurFolder);
	}

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFileList->Get(i);
		strcpy(szCurFile, pCurItem);
		char* pch = strrchr(pCurItem, '\\');
		if (pch)
		{
			int iPathLen = strlen(pCurItem) - strlen(pch);
			memcpy(szFilePath, pCurItem, iPathLen);
			szFilePath[iPathLen] = '\0';
		}

		strcpy(szCurFolder, szFilePath + iFolderLen);

		InsertFileNode(szCurFolder, szCurFile);
	}

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFileList->Get(i);
		delete pCurItem;
	}
	pFileList->EnableRemoveData(false);
	pFileList->Reset();
	delete pFileList;

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		delete pCurItem;
	}
	pFolderList->EnableRemoveData(false);
	pFolderList->Reset();
	delete pFolderList;

	m_pTreeCtrlUI->Save();
	m_pIOSourceCtrlDll->SaveMediaFile();
}

void CMediaCtrlDlg::SetLogFileDll(LogFileDll* pObj)
{
	m_pLogFileDll = pObj;

	if(m_pTreeCtrlUI)
		m_pTreeCtrlUI->SetLogFileDll(pObj);
}

LRESULT CMediaCtrlDlg::OnButtonClick(WPARAM wParam, LPARAM lParam)
{
	unsigned threadID1;
	switch (wParam)
	{
		case IDC_RESET:
			{
				HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _MC_Reset_ThreadProc, this, 0, &threadID1);
			}
			break;
		case IDC_RECREATE:
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _MC_ReCreate_ThreadProc, this, 0, &threadID1);
			//ReCreate();
			break;
	}
	return true;
}

void CMediaCtrlDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pResetBtn)
		m_pResetBtn->MouseMove(point.x, point.y);

	if(m_pReCreateBtn)
		m_pReCreateBtn->MouseMove(point.x, point.y);

	m_pUTipDll->_UT_ShowTip(m_pUTipDll->GetUTObj(), this->GetSafeHwnd(), point.x, point.y);

	CDialog::OnMouseMove(nFlags, point);
}

void CMediaCtrlDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pResetBtn)
		m_pResetBtn->LButtonDown(point.x, point.y);

	if (m_pReCreateBtn)
		m_pReCreateBtn->LButtonDown(point.x, point.y);

	CDialog::OnLButtonDown(nFlags, point);
}


void CMediaCtrlDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pResetBtn)
		m_pResetBtn->LButtonUp(point.x, point.y);

	if (m_pReCreateBtn)
		m_pReCreateBtn->LButtonUp(point.x, point.y);

	CDialog::OnLButtonUp(nFlags, point);
}

__inline char* GetNextItemFromString(char ch, char* szSrc)
{
	static char g_szNumber[256];
	int iNumber = 0;
	char* pTemp = strchr(szSrc, ch);
	if (pTemp)
	{
		int iLen = strlen(szSrc) - strlen(pTemp);
		memcpy(g_szNumber, szSrc, iLen);
		g_szNumber[iLen] = '\0';

		if (strlen(pTemp) > 1)
			strcpy(szSrc, pTemp + 1);
		else
			strcpy(szSrc, "");
		return g_szNumber;
	}
	else
	{
		strcpy(g_szNumber, szSrc);
		strcpy(szSrc, "");
		return g_szNumber;
	}
}

void CMediaCtrlDlg::InsertFolderNode(char* szFolder)
{
	char szTmp[512];
	char szFolderName[512] = "";
	wchar_t wszFolderName[512];

	bool bDo = true;
	HTREEITEM hCutItem = NULL;
	HTREEITEM hLastItem = NULL;
	strcpy(szTmp, szFolder);
	
	while (bDo)
	{
		char* szCurFolder = GetNextItemFromString('\\', szTmp);
		strcpy(szFolderName,szCurFolder);
		if (strcmp(szFolderName,"") != 0)
		{
			hCutItem = m_pTreeCtrlUI->FindSubFolder(hLastItem, szFolderName);
			if (!hCutItem)
			{
				wcscpy(wszFolderName, ANSIToUnicode(szFolderName));
				m_pTreeCtrlUI->InsertNode(hLastItem, wszFolderName, true, 0);
			}
			else
				hLastItem = hCutItem;
		}
		else
			bDo = false;
	}
}

void CMediaCtrlDlg::InsertFileNode(char* szFolder,char* szFileName)
{
	char szTmp[512];
	char szItemName[512];
	char szFolderName[512];
	wchar_t wszFolderName[512];
	wchar_t wszFileName[512];

	bool bFind = false;

	bool bDo = true;
	HTREEITEM hCutItem = NULL;
	HTREEITEM hLastItem = NULL;
	strcpy(szTmp, szFolder);

	while (bDo)
	{
		char* szCurFolder = GetNextItemFromString('\\', szTmp);
		strcpy(szFolderName, szCurFolder);
		if (strcmp(szFolderName, "") != 0)
		{
			hCutItem = m_pTreeCtrlUI->FindSubFolder(hLastItem, szFolderName);
			if (!hCutItem)
				bFind = false;
			else
			{
				hLastItem = hCutItem;
				bFind = true;
			}
		}
		else
			bDo = false;
	}

	if (bFind)
	{
		char* pch = strrchr(szFileName, '\\');
		if (pch)
			strcpy(szItemName, pch + 1);
		else
			strcpy(szItemName, szFileName);

		HTREEITEM hCurFileItem = m_pTreeCtrlUI->FindTreeItem(hLastItem, szItemName);
		if (!hCurFileItem)
		{
			wcscpy(wszFileName, ANSIToUnicode(szFileName));
			m_pTreeCtrlUI->AddFile2(hLastItem, wszFileName, false);
		}
		else
		{
			m_pTreeCtrlUI->DeleteItem(hCurFileItem);

			wcscpy(wszFileName, ANSIToUnicode(szFileName));
			m_pTreeCtrlUI->AddFile2(hLastItem, wszFileName, false);
		}
	}
}

void CMediaCtrlDlg::AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom)
{
	wchar_t wszID[512];
	wchar_t wszData[512];
	wchar_t wszAppName[128];
	wchar_t wszKeyName[128];
	char szIniFile[512];
	wchar_t wszIniFile[512];
	sprintf(szIniFile, "%s\\Language\\import_%s.ini", theApp.m_strCurPath, theApp.m_strLanguage);

	wcscpy(wszIniFile, ANSIToUnicode(szIniFile));

	int iTotal = GetPrivateProfileInt("Base", "Total", 0, szIniFile);
	for (int i = 0; i < iTotal; i++)
	{
		swprintf(wszAppName, L"Item%d", i);
		GetPrivateProfileStringW(wszAppName, L"ID", L"", wszID, 512, wszIniFile);

		if (wcscmp(wszID, wszTipID) == 0)
		{
			GetPrivateProfileStringW(wszAppName, L"Str", L"", wszData, 512, wszIniFile);

			TipInfoW tip_info;
			tip_info.hWnd = this->GetSafeHwnd();
			wcscpy(tip_info.wszMsg, wszData);
			tip_info.rect.left = left;
			tip_info.rect.top = top;
			tip_info.rect.right = right;
			tip_info.rect.bottom = bottom;

			m_pUTipDll->_UT_Add(m_pUTipDll->GetUTObj(), tip_info.hWnd, tip_info.rect, tip_info.wszMsg, tip_info.ulTipID);

			break;
		}
	}
}

void CMediaCtrlDlg::SetTip()
{
	m_pUTipDll->_UT_Reset(m_pUTipDll->GetUTObj());

	AddTip(L"ReCreate", 1500, 20, 80, 20);
	AddTip(L"Reset", 1600, 20, 80, 20);
}

unsigned int __stdcall _MC_Reset_ThreadProc(void* lpvThreadParm)
{
	CMediaCtrlDlg* pObj = (CMediaCtrlDlg *)lpvThreadParm;
	pObj->Reset();
	return 0;
}

unsigned int __stdcall _MC_ReCreate_ThreadProc(void* lpvThreadParm)
{
	CMediaCtrlDlg* pObj = (CMediaCtrlDlg *)lpvThreadParm;
	pObj->ReCreate();
	return 0;
}

