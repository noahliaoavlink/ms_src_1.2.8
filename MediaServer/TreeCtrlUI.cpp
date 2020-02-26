#include "stdafx.h"
#include "TreeCtrlUI.h"

#include "AddFolderDlg.h"
#include "EditTagDlg.h"
#include "SearchTagDlg.h"
#include "MediaSerVer.h"
#include "AddRTSPUrlDlg.h"

#include "../../../Include/StrConv.h"

#define IDC_TREE1 1001

#define _SKIP_FILE_NODE 1

unsigned int __stdcall _TestThreadProc(void* lpvThreadParm);
unsigned int __stdcall _AddFileThreadProc(void* lpvThreadParm);

__inline void GetExecutionPath(HMODULE hModule, char* szPath)
{
	char szExecutionPath[256];
	char szFullPath[256];
	GetModuleFileNameA(hModule, szFullPath, 256);//AfxGetInstanceHandle()

	char* pTemp = strrchr(szFullPath, '\\');
	int iLen = strlen(szFullPath) - strlen(pTemp);
	memcpy(szExecutionPath, szFullPath, iLen);
	szExecutionPath[iLen] = '\0';

	strcpy(szPath, szExecutionPath);
}

TreeCtrlUI::TreeCtrlUI()
{
	wcscpy(m_wszParentPath,L"");
	m_hBitmap = 0;
	m_pSIniFileW = new SIniFileW;
	m_pIOSourceCtrlDll = 0;
	m_pEventCallback = 0;
	m_pFileListCtrl = 0;
	m_pWin = 0;
	m_hParentWnd = NULL;
	m_iDialogMode = MFDM_EDIT;
	m_pLogFileDll = NULL;
	m_pLogFileObj = NULL;
	m_bSimpleMode = false;
	m_bEnableSearchTag = true;
}

TreeCtrlUI::~TreeCtrlUI()
{
	if (m_pLogFileObj)
	{
		if (m_pLogFileDll)
			m_pLogFileDll->_Out_LMC_Destroy(m_pLogFileObj);
		m_pLogFileObj = NULL;
	}

	if (m_pSIniFileW)
		delete m_pSIniFileW;

	if (m_hBitmap)
		DeleteObject(m_hBitmap);

	if(m_pWin)
		delete m_pWin;
}

void TreeCtrlUI::Init()
{
	char szAPPath[512];
	char szFileName[512];

	m_pWin = new CNWin("TreeCtrlUI");
	//m_pWin->SetWinSize(310,500,true);
	m_pWin->SetWinSize(500,900,false);

	// Create a 32bits ImageList
	m_imageList.Create(24, 24, ILC_COLOR32, 0, 0);

	//Add the bitmap to the ImageList
	//m_bitmap.LoadBitmap(IDB_BITMAP2); //IDB_CAM_ICON_LIST

	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);
	sprintf(szFileName, "%s\\UI Folder\\Media\\tree_icon.bmp", szAPPath);

	m_hBitmap = (HBITMAP)LoadImageA(NULL, szFileName ,IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	m_bitmap.Attach(m_hBitmap);
	m_imageList.Add(&m_bitmap, RGB(255,255,255));

	m_TreeCtrl.SetEventCallback(this);

	m_TreeCtrl.Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER
		  | TVS_LINESATROOT | TVS_FULLROWSELECT | WS_VISIBLE
		//| TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_HASBUTTONS
		| TVS_NOTOOLTIPS,// | TVS_EDITLABELS,
		CRect(0, 0, 500, 900), m_pWin, IDC_TREE1);

	m_pCTreeCtrl = &m_TreeCtrl;
	m_lTreeCtrlID = IDC_TREE1;
	m_hWnd = m_pWin->GetSafeHwnd();

	m_TreeCtrl.SetBkColor(RGB(50,50,50));
	m_pWin->SetBKColor(50,50,50);
	m_TreeCtrl.SetTextColor(RGB(200,200,0));

	m_pWin->SetCallback(this);

	m_TreeCtrl.SetImageList(&m_imageList, LVSIL_NORMAL);

	if (m_pLogFileDll)
	{
		m_pLogFileObj = m_pLogFileDll->_Out_LMC_Create();
		m_pLogFileDll->_Out_LMC_Init(m_pLogFileObj);
	}

	std::string sCSVFileName = theApp.m_strCurPath + "MOV\\Media-Tag-LUT.csv";

	m_CSVReader.Open((char*)sCSVFileName.c_str());
	m_CSVReader.ParseData();
	m_CSVReader.Close();
}

void TreeCtrlUI::SetDialogMode(int iMode)
{
	m_iDialogMode = iMode;
}

void TreeCtrlUI::SetParent(HWND hWnd)
{
	m_hParentWnd = hWnd;
	::SetParent(m_hWnd,hWnd);
}

void TreeCtrlUI::SetIOSourceCtrl(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void TreeCtrlUI::SetEventCallback(EventCallback* pObj)
{
	m_pEventCallback = pObj;
}

void TreeCtrlUI::SetFileListCtrl(FileListCtrl* pObj)
{
	m_pFileListCtrl = pObj;
}

void TreeCtrlUI::SetLogFileDll(LogFileDll* pObj)
{
	m_pLogFileDll = pObj;
}

void TreeCtrlUI::SetSimpleMode(bool bEnable)
{
	m_bSimpleMode = bEnable;
}

void TreeCtrlUI::MoveWindow(int x,int y,int w,int h)
{
	m_pWin->MoveWindow(x,y,w,h);
	m_TreeCtrl.MoveWindow(0,0,w,h);
}

void TreeCtrlUI::ShowWindow(bool bShow)
{
	if (bShow)
	{
		m_pWin->ShowWindow(SW_SHOW);
		ReDraw();
	}
	else
		m_pWin->ShowWindow(SW_HIDE);
}

void TreeCtrlUI::CheckCurSelItem()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

	CString sText = m_TreeCtrl.GetItemText(hItem);
	if ((hItem != NULL) && m_TreeCtrl.ItemHasChildren(hItem))
	{
		//TraverseTreeBranch(&m_TreeCtrl, hItem);
	}
}

void TreeCtrlUI::TraverseAllNodes(bool bDoWrite)
{
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = hRootItem;

	if (!hRootItem)
		return;

	wcscpy(m_wszParentPath, L"");
	TraverseTreeBranch(&m_TreeCtrl, hRootItem, m_wszParentPath, bDoWrite);

	while (hCurItem)
	{
		hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

		if (hCurItem)
		{
			wcscpy(m_wszParentPath, L"");
			TraverseTreeBranch(&m_TreeCtrl, hCurItem, m_wszParentPath,bDoWrite);
		}
	}
}

void TreeCtrlUI::TraverseAllNodes2(int iWriteLevel)
{
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = hRootItem;

	if (!hRootItem)
		return;

	wcscpy(m_wszParentPath, L"");
	TraverseTreeBranch2(&m_TreeCtrl, hRootItem, m_wszParentPath, iWriteLevel);

	while (hCurItem)
	{
		hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

		if (hCurItem)
		{
			wcscpy(m_wszParentPath, L"");
			TraverseTreeBranch2(&m_TreeCtrl, hCurItem, m_wszParentPath, iWriteLevel);
		}
	}
}

void TreeCtrlUI::TraverseTreeBranch(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath, bool bDoWrite)
{
	// Do stuff with hItem here
	CString sText = pTreeCtrl->GetItemText(hItem);
	if (bDoWrite)
	{
		bool bIsFolder = false;
		if (m_TreeCtrl.ItemHasChildren(hItem))
			bIsFolder = true;

		unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
		
		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if(pCurItem)
				WriteToFile(pCurItem->wszFileName, wszParentPath, bIsFolder, ulID);
		}
		else
		{
#ifdef _UNICODE
			WriteToFile(sText.GetBuffer(), wszParentPath, bIsFolder, ulID);
#else
			wchar_t wszItemText[256];
			wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
			WriteToFile(wszItemText, wszParentPath, bIsFolder, ulID);
#endif
		}
	}

	HTREEITEM hItemTmp = pTreeCtrl->GetChildItem(hItem);
	if (hItemTmp)
	{
		CString sParentText = pTreeCtrl->GetItemText(hItem);
		if (wcscmp(m_wszParentPath, L"") != 0)
			wcscat(m_wszParentPath, L"/");
#ifdef _UNICODE
		wcscat(m_wszParentPath, sParentText);
#else
		wcscat(m_wszParentPath, ANSIToUnicode(sParentText.GetBuffer()));
#endif
	}

	char szItemPath[512] = "";
	char szParentPath[512] = "";

	bool bIsRTSPUrl = false;
	while (hItemTmp != NULL)
	{
		bIsRTSPUrl = false;
		unsigned long ulID = m_TreeCtrl.GetItemData(hItemTmp);
		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if (pCurItem)
			{
			//	pCurItem->wszFileName;

				if (IsRTSPUrl(WCharToChar(pCurItem->wszFileName)))
					bIsRTSPUrl = true;
			}
		}

		if (!bIsRTSPUrl)
		{
			strcpy(szItemPath, "");
			FindFullPath(hItemTmp, szItemPath, true);

			char* pch = strrchr(szItemPath, '/');
			if (pch)
			{
				int iLen = strlen(szItemPath) - strlen(pch);

				memcpy(szParentPath, szItemPath, iLen);
				szParentPath[iLen] = '\0';
			}
			else
			{
				strcpy(szParentPath, "");
			}
			wcscpy(m_wszParentPath, ANSIToUnicode(szParentPath));
		}

		TraverseTreeBranch(pTreeCtrl, hItemTmp, m_wszParentPath, bDoWrite);
		hItemTmp = pTreeCtrl->GetNextSiblingItem(hItemTmp);
	}
}

void TreeCtrlUI::TraverseTreeBranch2(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath, int iWriteLevel)
{
	CString sText = pTreeCtrl->GetItemText(hItem);
	if (iWriteLevel == WL_ALL)
	{
		bool bIsFolder = false;
		if (m_TreeCtrl.ItemHasChildren(hItem))
			bIsFolder = true;

		unsigned long ulID = m_TreeCtrl.GetItemData(hItem);

		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if (pCurItem)
				WriteToFile(pCurItem->wszFileName, wszParentPath, bIsFolder, ulID);
		}
		else
		{
#ifdef _UNICODE
			WriteToFile(sText.GetBuffer(), wszParentPath, bIsFolder, ulID);
#else
			wchar_t wszItemText[256];
			wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
			WriteToFile(wszItemText, wszParentPath, bIsFolder, ulID);
#endif
		}
	}
	else if (iWriteLevel == WL_FOLDER)
	{
		bool bIsFolder = false;
		if (m_TreeCtrl.ItemHasChildren(hItem))
			bIsFolder = true;

		unsigned long ulID = m_TreeCtrl.GetItemData(hItem);

		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if (pCurItem && bIsFolder)
				WriteToFile(pCurItem->wszFileName, wszParentPath, bIsFolder, ulID);
		}
		else
		{
#ifdef _UNICODE
			WriteToFile(sText.GetBuffer(), wszParentPath, bIsFolder, ulID);
#else
			wchar_t wszItemText[256];
			wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
			if(bIsFolder)
				WriteToFile(wszItemText, wszParentPath, bIsFolder, ulID);
#endif
		}
	}
	else if (iWriteLevel == WL_FILE)
	{
		bool bIsFolder = false;
		if (m_TreeCtrl.ItemHasChildren(hItem))
			bIsFolder = true;

		unsigned long ulID = m_TreeCtrl.GetItemData(hItem);

		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if (pCurItem && !bIsFolder)
				WriteToFile(pCurItem->wszFileName, wszParentPath, bIsFolder, ulID);
		}
		else
		{
#ifdef _UNICODE
			WriteToFile(sText.GetBuffer(), wszParentPath, bIsFolder, ulID);
#else
			wchar_t wszItemText[256];
			wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
			if (!bIsFolder)
				WriteToFile(wszItemText, wszParentPath, bIsFolder, ulID);
#endif
		}
	}

	HTREEITEM hItemTmp = pTreeCtrl->GetChildItem(hItem);
	if (hItemTmp)
	{
		CString sParentText = pTreeCtrl->GetItemText(hItem);
		if (wcscmp(m_wszParentPath, L"") != 0)
			wcscat(m_wszParentPath, L"/");
#ifdef _UNICODE
		wcscat(m_wszParentPath, sParentText);
#else
		wcscat(m_wszParentPath, ANSIToUnicode(sParentText.GetBuffer()));
#endif
	}

	char szItemPath[512] = "";
	char szParentPath[512] = "";

	bool bIsRTSPUrl = false;
	while (hItemTmp != NULL)
	{
		bIsRTSPUrl = false;
		unsigned long ulID = m_TreeCtrl.GetItemData(hItemTmp);
		if (ulID != 0)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
			if (pCurItem)
			{
				//	pCurItem->wszFileName;

				if (IsRTSPUrl(WCharToChar(pCurItem->wszFileName)))
					bIsRTSPUrl = true;
			}
		}

		if (!bIsRTSPUrl)
		{
			strcpy(szItemPath, "");
			FindFullPath(hItemTmp, szItemPath, true);

			char* pch = strrchr(szItemPath, '/');
			if (pch)
			{
				int iLen = strlen(szItemPath) - strlen(pch);

				memcpy(szParentPath, szItemPath, iLen);
				szParentPath[iLen] = '\0';
			}
			else
			{
				strcpy(szParentPath, "");
			}
			wcscpy(m_wszParentPath, ANSIToUnicode(szParentPath));
		}

		TraverseTreeBranch2(pTreeCtrl, hItemTmp, m_wszParentPath, iWriteLevel);
		hItemTmp = pTreeCtrl->GetNextSiblingItem(hItemTmp);
	}
}

bool TreeCtrlUI::CheckTreeNode(wchar_t* wszItemPath)
{
	int iItemCount = 0;
	int iPassCount = 0;
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = hRootItem;

	if (!hRootItem)
		return false;
	//m_wszTmpParentPath
	wcscpy(m_wszTmpParentPath, L"");
	iItemCount++;
	bool bRet = CheckTreeBranchNode(&m_TreeCtrl, hRootItem, m_wszTmpParentPath, wszItemPath);
	if (bRet)
		iPassCount++;

	while (hCurItem)
	{
		hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

		if (hCurItem)
		{
			wcscpy(m_wszTmpParentPath, L"");
			bRet = CheckTreeBranchNode(&m_TreeCtrl, hCurItem, m_wszTmpParentPath, wszItemPath);
			if (bRet)
				iPassCount++;
			iItemCount++;
		}
	}

	if (iItemCount == iPassCount)
		return true;

	return false;
}

bool TreeCtrlUI::CheckTreeBranchNode(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath, wchar_t* wszItemPath)
{
	int iItemCount = 0;
	int iPassCount = 0;
	CString sText = pTreeCtrl->GetItemText(hItem);

	bool bIsFolder = false;
	if (m_TreeCtrl.ItemHasChildren(hItem))
		bIsFolder = true;

	unsigned long ulID = m_TreeCtrl.GetItemData(hItem);

	if (ulID != 0)
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if (pCurItem)
		{
			wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);

#ifdef _UNICODE
			if (wcscmp(pwszShortFileName, sText.GetBuffer()) == 0)
#else
			wchar_t wszItemText[256];
			wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
			if (wcscmp(pwszShortFileName, wszItemText) == 0)
#endif
			{
				return true;
			}
		}
	}

	HTREEITEM hItemTmp = pTreeCtrl->GetChildItem(hItem);
	if (hItemTmp)
	{
		CString sParentText = pTreeCtrl->GetItemText(hItem);
		if (wcscmp(m_wszTmpParentPath, L"") != 0)
			wcscat(m_wszTmpParentPath, L"/");
#ifdef _UNICODE
		wcscat(m_wszTmpParentPath, sParentText);
#else
		wcscat(m_wszTmpParentPath, ANSIToUnicode(sParentText.GetBuffer()));
#endif
	}

	while (hItemTmp != NULL)
	{
		iItemCount++;
		bool bRet = CheckTreeBranchNode(pTreeCtrl, hItemTmp, m_wszTmpParentPath, wszItemPath);
		if (bRet)
			iPassCount++;
		hItemTmp = pTreeCtrl->GetNextSiblingItem(hItemTmp);
	}

	if (iItemCount == iPassCount)
		return true;
	return false;
}

HTREEITEM TreeCtrlUI::InsertNode(HTREEITEM hParent,wchar_t* wszText,bool bHasChild ,unsigned long ulID )
{
	TV_INSERTSTRUCT TreeItem;
	int iBMPIndex = 0;
	TreeItem.hParent = hParent;
#ifdef _UNICODE	
	TreeItem.itemex.pszText = wszText;
#else
	TreeItem.itemex.pszText = WCharToChar(wszText);
#endif
	if (bHasChild)
	{
		TreeItem.hInsertAfter = TVI_LAST;
		TreeItem.itemex.mask = TVIF_TEXT | TVIF_CHILDREN;
		TreeItem.itemex.cChildren = 1;
		iBMPIndex = 1;
	}
	else
	{
		TreeItem.itemex.mask = TVIF_TEXT;
		TreeItem.hInsertAfter = TVI_LAST;// TVI_FIRST;
		TreeItem.itemex.cChildren = 0;
		iBMPIndex = 0;
	}
	HTREEITEM hItem = m_TreeCtrl.InsertItem(&TreeItem);
	m_TreeCtrl.SetItemImage(hItem, iBMPIndex, iBMPIndex);
	m_TreeCtrl.SetItemData(hItem, ulID);
	return hItem;
}

void TreeCtrlUI::RemoveNode()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

	RemoveNode(hItem);

	/*
	wchar_t wszFolderName[256];
	wchar_t wszFileName[512];
	char szFileName[512];
	char szFolderName[512];
	char szMsg[512];
	SQList* pFolderList = new SQList;

	bool bIsOpened = false;

	if (hItem)
	{
		if (m_pLogFileDll)
		{
			if (m_TreeCtrl.ItemHasChildren(hItem))
			{
				int i;
				
				CString sText = m_TreeCtrl.GetItemText(hItem);
				wcscpy(wszFolderName, ANSIToUnicode(sText.GetBuffer()));

				SQList* pFileList = new SQList;

				char szItemPath[512] = "";
				FindFullPath(hItem, szItemPath);

				CString strFolderName = theApp.m_strCurPath + "MOV\\User\\" + szItemPath;// m_TreeCtrl.GetItemText(hItem);
				strcpy(szFolderName, strFolderName.GetString());

				SearchAllFiles(szFolderName, pFileList, pFolderList);

				for (i = 0; i < pFileList->GetTotal(); i++)
				{
					//sprintf(szFileName,"%s\\%s", szFolderName, (char*)pFileList->Get(i));
					strcpy(szFileName,(char*)pFileList->Get(i));
					if (!IsOpened(szFileName))
					{
						BOOL bRet = DeleteFile(szFileName);
					}
					else
					{
						bIsOpened = true;
						sprintf(szMsg, "The action can't be completed because the '%s' is open in another module.", szFileName);
						MessageBoxA(NULL, szMsg, "Warning", MB_OK);
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

				//RemoveDirectory(szFolderName);

				char* szNewFolder = new char[strlen(szFolderName) + 1];
				strcpy(szNewFolder, szFolderName);
				pFolderList->Add(szNewFolder);

				if (m_pLogFileObj)
					m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DEL_FOLDER, wszFolderName, L"", 0);
			}
			else
			{
				unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
				if (ulID > 0)
				{
					//wchar_t wszFolderName[256];
					
					HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(hItem);
					CString sParentText = m_TreeCtrl.GetItemText(hParentItem);

					wcscpy(wszFolderName, ANSIToUnicode(sParentText.GetBuffer()));

					MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
					if (pCurItem)
					{
						wcscpy(wszFileName, pCurItem->wszFileName);


						strcpy(szFileName, WCharToChar(pCurItem->wszFileName));
						if (!IsOpened(szFileName))
						{
							BOOL bRet = DeleteFileW(pCurItem->wszFileName);
							m_pIOSourceCtrlDll->DeleteFile(pCurItem->wszFileName);
							m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DEL_FILE, wszFolderName, wszFileName, 0);
						}
						else
						{
							bIsOpened = true;
							sprintf(szMsg,"The action can't be completed because the '%s' is open in another module.", szFileName);
							MessageBoxA(NULL, szMsg, "Warning", MB_OK);
						}
					}
				}
			}
		}

		if(!bIsOpened)
			m_TreeCtrl.DeleteItem(hItem);
	}

	//remove all folder
	int i;
	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		strcpy(szFolderName, (char*)pFolderList->Get(pFolderList->GetTotal() - i - 1));
		BOOL bRet = RemoveDirectory(szFolderName);
	}

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		strcpy(szFolderName, (char*)pFolderList->Get(i));
		BOOL bRet = RemoveDirectory(szFolderName);
	}

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		delete pCurItem;
	}
	pFolderList->EnableRemoveData(false);
	pFolderList->Reset();
	delete pFolderList;
	*/
}

void TreeCtrlUI::RemoveNode(HTREEITEM hItem)
{
	wchar_t wszFolderName[256];
	wchar_t wszFileName[512];
	char szFileName[512];
	char szFolderName[512];
	char szMsg[512];
	SQList* pFolderList = new SQList;

	bool bIsOpened = false;

	try
	{
		if (hItem)
		{
			if (m_pLogFileDll)
			{
				if (m_TreeCtrl.ItemHasChildren(hItem))
				{
					int i;

					CString sText = m_TreeCtrl.GetItemText(hItem);
					wcscpy(wszFolderName, ANSIToUnicode(sText.GetBuffer()));

					SQList* pFileList = new SQList;

					char szItemPath[512] = "";
					FindFullPath(hItem, szItemPath);

					CString strFolderName = theApp.m_strCurPath + "MOV\\User\\" + szItemPath;// m_TreeCtrl.GetItemText(hItem);
					strcpy(szFolderName, strFolderName.GetString());

					SearchAllFiles(szFolderName, pFileList, pFolderList);

					for (i = 0; i < pFileList->GetTotal(); i++)
					{
						//sprintf(szFileName,"%s\\%s", szFolderName, (char*)pFileList->Get(i));
						strcpy(szFileName, (char*)pFileList->Get(i));
						if (!IsOpened(szFileName))
						{
							BOOL bRet = DeleteFile(szFileName);
						}
						else
						{
							bIsOpened = true;
							sprintf(szMsg, "The action can't be completed because the '%s' is open in another module.", szFileName);
							MessageBoxA(NULL, szMsg, "Warning", MB_OK| MB_TOPMOST);
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

					//RemoveDirectory(szFolderName);

					char* szNewFolder = new char[strlen(szFolderName) + 1];
					strcpy(szNewFolder, szFolderName);
					pFolderList->Add(szNewFolder);

					if (m_pLogFileObj)
						m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DEL_FOLDER, wszFolderName, L"", 0);
				}
				else
				{
					unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
					if (ulID > 0)
					{
						//wchar_t wszFolderName[256];

						HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(hItem);
						CString sParentText = m_TreeCtrl.GetItemText(hParentItem);

						wcscpy(wszFolderName, ANSIToUnicode(sParentText.GetBuffer()));

						MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
						if (pCurItem)
						{
							wcscpy(wszFileName, pCurItem->wszFileName);


							strcpy(szFileName, WCharToChar(pCurItem->wszFileName));
							if (!IsOpened(szFileName))
							{
								BOOL bRet = DeleteFileW(pCurItem->wszFileName);
								m_pIOSourceCtrlDll->DeleteFile(pCurItem->wszFileName);
								m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DEL_FILE, wszFolderName, wszFileName, 0);
							}
							else
							{
								bIsOpened = true;
								sprintf(szMsg, "The action can't be completed because the '%s' is open in another module.", szFileName);
								MessageBoxA(NULL, szMsg, "Warning", MB_OK| MB_TOPMOST);
							}
						}
					}
				}
			}

			if (!bIsOpened)
				m_TreeCtrl.DeleteItem(hItem);
		}

		//remove all folder
		int i;
		for (i = 0; i < pFolderList->GetTotal(); i++)
		{
			strcpy(szFolderName, (char*)pFolderList->Get(pFolderList->GetTotal() - i - 1));
			BOOL bRet = RemoveDirectory(szFolderName);
		}

		for (i = 0; i < pFolderList->GetTotal(); i++)
		{
			strcpy(szFolderName, (char*)pFolderList->Get(i));
			BOOL bRet = RemoveDirectory(szFolderName);
		}

		for (i = 0; i < pFolderList->GetTotal(); i++)
		{
			char* pCurItem = (char*)pFolderList->Get(i);
			delete pCurItem;
		}
		pFolderList->EnableRemoveData(false);
		pFolderList->Reset();
		delete pFolderList;
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in RemoveNode()!! \n");
		MessageBoxA(NULL, "RemoveNode() Failed!!", "", MB_OK| MB_TOPMOST);
	}
}

void TreeCtrlUI::DeleteItem(HTREEITEM hItem)
{
	unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
	if (ulID > 0)
	{
		wchar_t wszFolderName[256];
		wchar_t wszFileName[512];
		HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(hItem);
		CString sParentText = m_TreeCtrl.GetItemText(hParentItem);

		wcscpy(wszFolderName, ANSIToUnicode(sParentText.GetBuffer()));

		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if (pCurItem)
		{
			wcscpy(wszFileName, pCurItem->wszFileName);
//			BOOL bRet = DeleteFileW(pCurItem->wszFileName);
			m_pIOSourceCtrlDll->DeleteFile(pCurItem->wszFileName);
			m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DEL_FILE, wszFolderName, wszFileName, 0);
		}
	}

	m_TreeCtrl.DeleteItem(hItem);
}

void TreeCtrlUI::Save()
{
	char szAPPath[512];
	char szFileName[512];
	wchar_t wszFileName[512];// = L"tree_ctrl.txt";
	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);

	sprintf(szFileName,"%s\\tree_ctrl.txt", szAPPath);
	wcscpy(wszFileName,ANSIToUnicode(szFileName));
	
	m_pSIniFileW->Open(wszFileName,true,true);
#if 0
	TraverseAllNodes(true);
#else
	TraverseAllNodes2(WL_FOLDER);
	TraverseAllNodes2(WL_FILE);
#endif
	m_pSIniFileW->Write();
	m_pSIniFileW->Close();
}

void TreeCtrlUI::Load()
{
	char szAPPath[512];
	char szFileName[512];
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszCurRoot[256];
	//wchar_t szFileName[512] = L"C:\\ProgramData\\tree_ctrl.txt";
	wchar_t wszFileName[512];// = L"tree_ctrl.txt";

	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);

	sprintf(szFileName, "%s\\tree_ctrl.txt", szAPPath);
	wcscpy(wszFileName, ANSIToUnicode(szFileName));

	m_TreeCtrl.DeleteAllItems();

	m_pSIniFileW->Open(wszFileName, true, false);

	wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"Base", L"Total", L"0");
	int iTotalOfRoots = _wtoi(wszTotal);

	wcscpy(m_wszParentPath, L"");

	for (int i = 0; i < iTotalOfRoots; i++)
	{
		wsprintfW(wszKeyName, L"Root%d", i);
		wchar_t* wszRoot = m_pSIniFileW->GetItemValue(L"Base", wszKeyName, L"0");
		wcscpy(wszCurRoot, wszRoot);

		char* szFileFolder = WCharToChar(wszRoot);
		HTREEITEM hFolderItem = FindFolder(szFileFolder);

		HTREEITEM hCurRoot = 0;
		if(!hFolderItem)
			hCurRoot = InsertNode(NULL, wszCurRoot, true,0);
		else
			hCurRoot = hFolderItem;
		//wsprintf(wszAppName, L"Root%d", i);
		wcscpy(wszAppName, wszCurRoot);
		wcscpy(m_wszParentPath, wszCurRoot);

		LoadSubNodeFromFile(hCurRoot, wszCurRoot);
	}

	m_pSIniFileW->Close();
}

void TreeCtrlUI::LoadSubNodeFromFile(HTREEITEM hRoot, wchar_t* wszParentPath)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszCurFileName[512];
	wchar_t wszLocalParentPath[512];
	//wchar_t wszCurRoot[256];

	wcscpy(wszLocalParentPath, wszParentPath);

	//folders
	wchar_t* wszTotalOfDirectorys = m_pSIniFileW->GetItemValue(wszParentPath, L"TotalOfDirectorys", L"0");
	int iTotalOfDirectorys = _wtoi(wszTotalOfDirectorys);
	if (iTotalOfDirectorys > 0)
	{
		for (int j = 0; j < iTotalOfDirectorys; j++)
		{
			wsprintfW(wszKeyName, L"Dir%d", j);
			wchar_t* wszCurDir = m_pSIniFileW->GetItemValue(wszParentPath, wszKeyName, L"");

			char* szFileFolder = WCharToChar(wszCurDir);
			HTREEITEM hFolderItem = FindFolder(szFileFolder);

			HTREEITEM hCurRoot = 0;
			if (!hFolderItem)
				hCurRoot = InsertNode(hRoot, wszCurDir, true, 0);
			else
				hCurRoot = hFolderItem;

			if (wcscmp(m_wszParentPath, L"") != 0)
				wcscat(m_wszParentPath, L"/");
			//wcscat(m_wszParentPath, wszCurDir);
			wsprintfW(m_wszParentPath, L"%s/%s", wszLocalParentPath, wszCurDir);
			LoadSubNodeFromFile(hCurRoot, m_wszParentPath);
		}
	}

	//files
	wchar_t* wszTotalOfFiles = m_pSIniFileW->GetItemValue(wszLocalParentPath, L"TotalOfFiles", L"0");
	int iTotalOfFiles = _wtoi(wszTotalOfFiles);
	if (iTotalOfFiles > 0)
	{
		for (int j = 0; j < iTotalOfFiles; j++)
		{
			wsprintfW(wszKeyName, L"File%d", j);
			wchar_t* wszFile = m_pSIniFileW->GetItemValue(wszLocalParentPath, wszKeyName, L"");

			wcscpy(wszCurFileName, wszFile);

			//id
			wsprintfW(wszKeyName, L"FileID%d", j);
			wchar_t* wszFileID = m_pSIniFileW->GetItemValue(wszLocalParentPath, wszKeyName, L"");
			//unsigned long ulID = _wtol(wszFileID);
			unsigned long ulID = _wtof(wszFileID);

			wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(wszCurFileName);
			char* pszShortFileName = WCharToChar(pwszShortFileName);

			CString sText = m_TreeCtrl.GetItemText(hRoot);
			HTREEITEM hItem = FindTreeItem(sText.GetBuffer(), pszShortFileName);

			if(!hItem)
				InsertNode(hRoot, pwszShortFileName, false, ulID);
		}
	}
}

bool TreeCtrlUI::CheckSubNode(wchar_t* wszParentPath)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszCurFileName[512];
	//wchar_t wszCurRoot[256];

	int iCount = 0;

	//folders
	wchar_t* wszTotalOfDirectorys = m_pSIniFileW->GetItemValue(wszParentPath, L"TotalOfDirectorys", L"0");
	int iTotalOfDirectorys = _wtoi(wszTotalOfDirectorys);
	if (iTotalOfDirectorys > 0)
	{
		for (int j = 0; j < iTotalOfDirectorys; j++)
		{
			wsprintfW(wszKeyName, L"Dir%d", j);
			wchar_t* wszCurDir = m_pSIniFileW->GetItemValue(wszParentPath, wszKeyName, L"");

			if (wcscmp(m_wszTmpParentPath2, L"") != 0)
				wcscat(m_wszTmpParentPath2, L"/");
			wcscat(m_wszTmpParentPath2, wszCurDir);

			wsprintfW(m_wszTmpParentPath2, L"%s/%s", wszParentPath, wszCurDir);

			CheckSubNode(m_wszTmpParentPath2);
		}
	}

	//files
	wchar_t* wszTotalOfFiles = m_pSIniFileW->GetItemValue(wszParentPath, L"TotalOfFiles", L"0");
	int iTotalOfFiles = _wtoi(wszTotalOfFiles);
	if (iTotalOfFiles > 0)
	{
		for (int j = 0; j < iTotalOfFiles; j++)
		{
			wsprintfW(wszKeyName, L"File%d", j);
			wchar_t* wszFile = m_pSIniFileW->GetItemValue(wszParentPath, wszKeyName, L"");

			wsprintfW(wszCurFileName, L"%s/%s", wszParentPath, wszFile);

			bool bRet = CheckTreeNode(wszCurFileName);
			if (bRet)
				iCount++;
		}
	}

	if (iCount == iTotalOfFiles)
		return true;
	return false;
}

void TreeCtrlUI::WriteToFile(wchar_t* wszItem, wchar_t* wszParentPath,bool bIsFolder, unsigned long ulID)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	if (wcscmp(m_wszParentPath, L"") != 0)
	{
		if (bIsFolder)
		{
			wchar_t* wszTotal = m_pSIniFileW->GetItemValue(wszParentPath, L"TotalOfDirectorys", L"0");
			int iTotal = _wtoi(wszTotal);

			wsprintfW(wszData, L"%d", iTotal + 1);
			m_pSIniFileW->SetItemData(wszParentPath, L"TotalOfDirectorys", wszData);
			wsprintfW(wszKeyName, L"Dir%d", iTotal);
			m_pSIniFileW->SetItemData(wszParentPath, wszKeyName, wszItem);
		}
		else
		{
			wchar_t* wszTotal = m_pSIniFileW->GetItemValue(wszParentPath, L"TotalOfFiles", L"0");
			int iTotal = _wtoi(wszTotal);

			wsprintfW(wszData, L"%d", iTotal + 1);
			m_pSIniFileW->SetItemData(wszParentPath, L"TotalOfFiles", wszData);
			wsprintfW(wszKeyName, L"File%d", iTotal);
			m_pSIniFileW->SetItemData(wszParentPath, wszKeyName, wszItem);
			wsprintfW(wszData, L"%u", ulID);
			wsprintfW(wszKeyName, L"FileID%d", iTotal);
			m_pSIniFileW->SetItemData(wszParentPath, wszKeyName, wszData);
		}
	}
	else
	{
		wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"Base", L"Total", L"0");
		int iTotal = _wtoi(wszTotal);

		wsprintfW(wszData, L"%d", iTotal + 1);
		m_pSIniFileW->SetItemData(L"Base", L"Total", wszData);
		wsprintfW(wszKeyName, L"Root%d",iTotal);
		m_pSIniFileW->SetItemData(L"Base", wszKeyName, wszItem);	
	}
	m_pSIniFileW->Write();
}

void TreeCtrlUI::PopupMenu(HTREEITEM hItem,POINT* pPoint)
{
	CMenu *menu = new CMenu;
	menu->CreatePopupMenu();

#ifdef _UNICODE	
	if (m_iDialogMode == MFDM_SELECT)
	{
		if (hItem)
		{
			if (m_TreeCtrl.ItemHasChildren(hItem))
				menu->AppendMenu(MF_STRING, IDC_SEARCH_TAG, L"Search Tag");	
		}
	}
	else
	{
		if (hItem)
		{
			if (m_TreeCtrl.ItemHasChildren(hItem))
			{
				menu->AppendMenu(MF_STRING, IDC_ADD_FOLDER, L"Add Folder");
				menu->AppendMenu(MF_STRING, IDC_ADD_FILE, L"Add File");
				menu->AppendMenu(MF_STRING, IDC_SEARCH_TAG, L"Search Tag");
				menu->AppendMenu(MF_STRING, IDC_DEL, L"Delete");
			}
			else
			{
				menu->AppendMenu(MF_STRING, IDC_EDIT_TAG, L"Edit Tag");
				menu->AppendMenu(MF_STRING, IDC_DEL, L"Delete");
			}

		}
		else
		{
			menu->AppendMenu(MF_STRING, IDC_ADD_FOLDER, L"Add Folder");
			//menu->AppendMenu(MF_STRING, IDC_ADD_FILE, L"Add File");
		}
	}
#else
	if (m_iDialogMode == MFDM_SELECT)
	{
		if (hItem)
		{
			if (m_bEnableSearchTag)
			{
				if (m_TreeCtrl.ItemHasChildren(hItem))
					menu->AppendMenu(MF_STRING, IDC_SEARCH_TAG, "Search Tag");
			}
		}
	}
	else
	{
		if (hItem)
		{
			bool bIsReadOnly = IsReadOnly(hItem);
			if (m_TreeCtrl.ItemHasChildren(hItem))
			{
				if (bIsReadOnly)
					menu->AppendMenu(MF_STRING, IDC_SEARCH_TAG, "Search Tag");
				else
				{
					menu->AppendMenu(MF_STRING, IDC_ADD_FOLDER, "Add Folder");
					menu->AppendMenu(MF_STRING, IDC_ADD_FILE, "Add File");
					menu->AppendMenu(MF_STRING, IDC_ADD_RTSP_URL, "Add RTSP Url");
					menu->AppendMenu(MF_STRING, IDC_SEARCH_TAG, "Search Tag");
					menu->AppendMenu(MF_STRING, IDC_DEL, "Delete");
				}

				if(theApp.m_bImportAutoTesting)
					menu->AppendMenu(MF_STRING, IDC_TEST, "Test");
			}
			else
			{
				if (bIsReadOnly)
					menu->AppendMenu(MF_STRING, IDC_EDIT_TAG, "Edit Tag");
				else
				{
					menu->AppendMenu(MF_STRING, IDC_EDIT_TAG, "Edit Tag");
					menu->AppendMenu(MF_STRING, IDC_DEL, "Delete");
				}
			}
		}
		else
		{
			menu->AppendMenu(MF_STRING, IDC_ADD_FOLDER, "Add Folder");
			//menu->AppendMenu(MF_STRING, IDC_ADD_FILE, L"Add File");
		}
	}
#endif

	
	UINT nCode = menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, pPoint->x, pPoint->y, &m_TreeCtrl);
	delete menu;

	switch (nCode)
	{
		case IDC_ADD_FOLDER:
			AddFolder(hItem);
			break;
		case IDC_ADD_FILE:
			//AddFile(hItem);
			AddFile_Thread(hItem);
			break;
		case IDC_DEL:
			DeleteItem();
			break;
		case IDC_EDIT_TAG:
			EditTag(hItem);
			break;
		case IDC_SEARCH_TAG:
			SearchTag(hItem);
			break;
		case IDC_ADD_RTSP_URL:
			AddRTSPUrl(hItem);
			break;
			
		case IDC_TEST:
			Test();
			break;
			
	}
}

void TreeCtrlUI::AddFolder(HTREEITEM hItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	char szFolderName[512];
	CAddFolderDlg dlg;
	EnableWindow(m_hWnd, false);
	EnableWindow(m_hParentWnd, false);
	if (dlg.DoModal() == IDOK)
	{
		wchar_t* wszFolderName = dlg.GetFolderName();

		if (wcscmp(wszFolderName, L"") == 0)
		{
			MessageBoxA(NULL, "Folder name cannot be empty!", "Warning", MB_OK| MB_TOPMOST);
			EnableWindow(m_hWnd, true);
			EnableWindow(m_hParentWnd, true);
			return;
		}

		
		strcpy(szFolderName, WCharToChar(wszFolderName));

		AddFolder(hItem, szFolderName);
		
		/*
		HTREEITEM hFolderItem = FindFolder(szFolderName);

		if (hFolderItem)
		{
			MessageBox(NULL, "Cannot have the same folder name!!", "Warning", MB_OK);
			return;
		}

		InsertNode(hItem, wszFolderName, true,0);

		if (m_pLogFileDll)
		{
			if (m_pLogFileObj)
				m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_ADD_FOLDER, wszFolderName,L"",0);
		}

		Save();
#if 0
		CreateDirectory(theApp.m_strCurPath + "MOV\\"+ wszFolderName, NULL);
#else

		char szItemPath[512] = "";
		FindFullPath(hItem, szItemPath);

		if (strcmp(szItemPath, "") == 0)
		{
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\", NULL);
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\" + wszFolderName, NULL);
		}
		else
		{
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\", NULL);
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\" + szItemPath + "\\" + wszFolderName, NULL);
		}
#endif
*/
	}
	EnableWindow(m_hWnd, true);
	EnableWindow(m_hParentWnd, true);
}

void TreeCtrlUI::AddFolder(HTREEITEM hItem,char* szFolderName)
{
	try
	{
		wchar_t wszFolderName[512];
		HTREEITEM hFolderItem = FindFolder(szFolderName);

		if (hFolderItem)
		{
			MessageBox(NULL, "Cannot have the same folder name!!", "Warning", MB_OK| MB_TOPMOST);
			return;
		}

		wcscpy(wszFolderName, ANSIToUnicode(szFolderName));

		InsertNode(hItem, wszFolderName, true, 0);

		if (m_pLogFileDll)
		{
			if (m_pLogFileObj)
				m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_ADD_FOLDER, wszFolderName, L"", 0);
		}

		Save();
#if 0
		CreateDirectory(theApp.m_strCurPath + "MOV\\" + wszFolderName, NULL);
#else

		char szItemPath[512] = "";
		FindFullPath(hItem, szItemPath);

		if (strcmp(szItemPath, "") == 0)
		{
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\", NULL);
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\" + szFolderName, NULL);
		}
		else
		{
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\", NULL);
			CreateDirectory(theApp.m_strCurPath + "MOV\\User\\" + szItemPath + "\\" + szFolderName, NULL);
		}
#endif
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in AddFolder()!! \n");

		MessageBoxA(NULL, "AddFolder() Failed!!", "", MB_OK| MB_TOPMOST);
	}
}

void TreeCtrlUI::DeleteItem()
{
	RemoveNode();

	try
	{
		Save();
		m_pIOSourceCtrlDll->SaveMediaFile();
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in DeleteItem() - Save!! \n");
		MessageBoxA(NULL, "Save() Failed!!", "", MB_OK);
	}

	UpdateList();
}

void TreeCtrlUI::AddFile(HTREEITEM hItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef _UNICODE
	wchar_t wszFile[512];
	CFileDialog fileDlg(TRUE, _T("txt"), L"", OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		//_T("MP4 Files (*.mp4)|*.mp4|MKV Files (*.mkv)|*.mkv|MPEG Files (*.mpg)|*.mpg|MOV Files (*.mov)|*.mov|WMV Files (*.wmv)|*.wmv|VOB Files (*.vob)|*.vob|RMVB Files (*.rmvb)|*.rmvb|ALL Files (*.*)|*.*||"));
	_T("Video Files (*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb)|*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb|Audio Files (*.ac3;*.wav;*.flac;*.ape;*.wma;*.ogg;*.midi;*.mp3;*.ogg;*.flac;*.aac)|*.ac3;*.wav;*.flac;*.ape;*.wma;*.ogg;*.midi;*.mp3;*.ogg;*.flac;*.aac||"));

	if (fileDlg.DoModal() == IDOK)
	{
		wcscpy(wszFile, fileDlg.GetPathName());

		int iRet = m_pIOSourceCtrlDll->AddMediaFile(wszFile);

		if (iRet != -1)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFile);
			if (pCurItem)
			{
				wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
				InsertNode(hItem, pwszShortFileName, false, pCurItem->ulID);
			}

			UpdateList();
			//if(m_pEventCallback)
				//m_pEventCallback->Event(OBJ_MSG_UPDATE_FILE_LIST,0,0);
		}
		else
			MessageBoxA(NULL, "Unsupported", "", MB_OK);
	}
#else
	wchar_t wszFile[512];
	//CFileDialog fileDlg(TRUE, "", "", OFN_EXPLORER | OFN_OVERWRITEPROMPT,
	CWnd Cur_Wnd;
	Cur_Wnd.Attach(m_hWnd);

	CFileDialog fileDlg(TRUE, "", "", OFN_EXPLORER,
		//"ALL Files (*.*)|*.*|MP4 Files (*.mp4)|*.mp4|MKV Files (*.mkv)|*.mkv|MPEG Files (*.mpg)|*.mpg|MOV Files (*.mov)|*.mov|WMV Files (*.wmv)|*.wmv|VOB Files (*.vob)|*.vob|RMVB Files (*.rmvb)|*.rmvb||");

		//"Video Files (*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb)|*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb|Audio Files (*.ac3;*.wav;*.flac;*.ape;*.wma;*.ogg;*.midi;*.mp3;*.ogg;*.flac;*.aac)|*.ac3;*.wav;*.flac;*.ape;*.wma;*.ogg;*.midi;*.mp3;*.ogg;*.flac;*.aac||", &Cur_Wnd);
		"Video Files (*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb)|*.asf;*.avi;*.divx;*.flv;*.m1v;*.m2ts;*.m2v;*.m4v;*.mp4;*.mpeg;*.mpg;*.mkv;*.mpg;*.mov;*.rm;*.rmvb;*.qt;*.webm;*.wmv;*.vob;*.rmvb||", &Cur_Wnd);
	
	EnableWindow(m_hWnd,false);
	EnableWindow(m_hParentWnd, false);

	if (fileDlg.DoModal() == IDOK)
	{
		/*
		//CString strNewFilePath;
		//strNewFilePath.Format();
		CString strDest;
		CFileFind fFind;
		strDest = theApp.m_strCurPath + "MOV\\" + m_TreeCtrl.GetItemText(hItem) + 
			      "\\" + fileDlg.GetFileName();

		if (fFind.FindFile(strDest))
		{
			AfxMessageBox("The File Has Existed!");
			return;
		}

		CopyFile(fileDlg.GetPathName(), strDest, FALSE);

		wcscpy(wszFile, ANSIToUnicode(strDest));

		
		int iRet = AddFile2(hItem, wszFile);
		if (iRet != -1)
			UpdateList();
		else
			MessageBoxA(NULL, "Unsupported", "", MB_OK);
		Save();
		m_pIOSourceCtrlDll->SaveMediaFile();
		CPlaneScene3D::FileEncoder(strDest, FALSE);

//		CheckSaveFile();
*/
//CString strNewFilePath;
//strNewFilePath.Format();

		CString strDest;
		CString strDestPath;
		char szPathName[512];
		char szFileName[512];

		strcpy(szPathName, fileDlg.GetPathName());
		strcpy(szFileName, fileDlg.GetFileName());

		if (g_PannelSetting.iEncodeMedia == 0)
		{
			wcscpy(wszFile, ANSIToUnicode(szPathName));

			int iRet = AddFile(hItem, wszFile);
			if (iRet != -1)
				UpdateList();
			else
				MessageBoxA(NULL, "Unsupported", "", MB_OK| MB_TOPMOST);
		}
		else
		{
#if 0
			strDest = theApp.m_strCurPath + "MOV\\" + m_TreeCtrl.GetItemText(hItem) +
				"\\" + fileDlg.GetFileName();
#else

			char szItemPath[512] = "";
			FindFullPath(hItem, szItemPath);

			if (strcmp(szItemPath, "") == 0)
			{
				strDestPath = theApp.m_strCurPath + "MOV\\User\\" + m_TreeCtrl.GetItemText(hItem);
				strDest = theApp.m_strCurPath + "MOV\\User\\" + m_TreeCtrl.GetItemText(hItem) +
					"\\" + szFileName;
			}
			else
			{
				strDestPath = theApp.m_strCurPath + "MOV\\User\\" + szItemPath;
				strDest = theApp.m_strCurPath + "MOV\\User\\" + szItemPath +
					"\\" + szFileName;
			}

#endif
			if (!PathFileExistsA(strDestPath))
			{
				EnableWindow(m_hWnd, true);
				EnableWindow(m_hParentWnd, true);
				Cur_Wnd.Detach();
				return;
			}

			BOOL bRet = CopyFile(szPathName, strDest, FALSE);
			
			if(bRet)
			{
				/*
				CFileStatus fsCur;
				CFile::GetStatus(strDest, fsCur);

				fsCur.m_attribute &= ~CFile::readOnly;
				CFile::SetStatus(strDest, fsCur);
				*/

				if (m_pLogFileDll)
				{
					if (m_pLogFileObj)
					{
						wchar_t wszSrc[256];
						wchar_t wszDest[256];
						wcscpy(wszSrc, ANSIToUnicode(szPathName));
						wcscpy(wszDest, ANSIToUnicode(strDest));
						m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_COPY_FILE, wszSrc, wszDest, 0);
					}
				}

				wcscpy(wszFile, ANSIToUnicode(strDest));

				int iRet = AddFile2(hItem, wszFile, false);
				if (iRet != -1)
					UpdateList();
				else
					MessageBoxA(NULL, "Unsupported", "", MB_OK| MB_TOPMOST);
			}
			else
			{
				MessageBoxA(NULL, "CopyFile() Failed!!", "", MB_OK| MB_TOPMOST);
			}
		}
		Save();
		m_pIOSourceCtrlDll->SaveMediaFile();

		

		/*  noah-disable_encrypt
#ifdef PLAY_3D_MODE
		if (g_PannelSetting.iEncodeMedia != 0)
			CPlaneScene3D::FileEncoder(strDest, FALSE);
#endif
*/

	}
	EnableWindow(m_hWnd, true);
	EnableWindow(m_hParentWnd, true);
	Cur_Wnd.Detach();
#endif
}

void TreeCtrlUI::AddFile_Thread(HTREEITEM hItem)
{
	unsigned threadID1;
	//TreeCtrlUIThreadParam thread_param;
	m_thread_param.pObj = this;
	m_thread_param.hItem = hItem;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _AddFileThreadProc, &m_thread_param, 0, &threadID1);
}

int TreeCtrlUI::AddFile(HTREEITEM hItem, wchar_t* wszFileName)
{
	//HTREEITEM hFolderItem = m_TreeCtrl.GetSelectedItem();
	int iRet;
	try
	{
		CString sText = m_TreeCtrl.GetItemText(hItem);
		iRet = m_pIOSourceCtrlDll->AddMediaFile(wszFileName);
		if (iRet != -1)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
			if (pCurItem)
			{
				wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
				char* pszShortFileName = WCharToChar(pwszShortFileName);
				HTREEITEM hCurItem = FindTreeItem(sText.GetBuffer(), pszShortFileName);
				if (!hCurItem)
				{
					InsertNode(hItem, pwszShortFileName, false, pCurItem->ulID);

					if (m_pLogFileDll)
					{
						if (m_pLogFileObj)
						{
							wchar_t wszItemText[256];
							wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
							m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_ADD_FILE, wszItemText, pCurItem->wszFileName, pCurItem->ulID);
						}
					}
				}
			}
		}
		else
		{
			char szMsg[256];
			sprintf_s(szMsg, "#TreeCtrl# AddFile Failed!! \n");
			OutputDebugStringA(szMsg);
		}
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in AddFile !! \n");

		MessageBoxA(NULL, "AddFile() Failed!!", "", MB_OK| MB_TOPMOST);
	}
	return iRet;
}

int TreeCtrlUI::AddFile2(HTREEITEM hItem, wchar_t* wszFileName,bool IsEncrypted)
{
	int iRet;
	try
	{
		CString sText = m_TreeCtrl.GetItemText(hItem);
		iRet = m_pIOSourceCtrlDll->AddMediaFile2(wszFileName, IsEncrypted);  //g_PannelSetting.iEncodeMedia
		if (iRet != -1)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
			if (pCurItem)
			{
				wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
				char* pszShortFileName = WCharToChar(pwszShortFileName);
				HTREEITEM hCurItem = FindTreeItem(sText.GetBuffer(), pszShortFileName);
				if (!hCurItem)
				{
					InsertNode(hItem, pwszShortFileName, false, pCurItem->ulID);

					if (m_pLogFileDll)
					{
						if (m_pLogFileObj)
						{
							wchar_t wszItemText[256];
							wcscpy(wszItemText, ANSIToUnicode(sText.GetBuffer()));
							m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_ADD_FILE, wszItemText, pCurItem->wszFileName, pCurItem->ulID);
						}
					}
				}
			}
		}
		else
		{
			char szMsg[256];
			sprintf_s(szMsg, "#TreeCtrl# AddFile2 Failed!! \n");
			OutputDebugStringA(szMsg);
		}
	}
	catch (...)
	{
		OutputDebugString("HEError - Catching an exception in AddFile2 !! \n");
		MessageBoxA(NULL, "AddFile2() Failed!!", "", MB_OK| MB_TOPMOST);
	}
	return iRet;
}

void TreeCtrlUI::AddRTSPUrl(HTREEITEM hItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	wchar_t wszUrl[256];
	char szShortUrl[256];
	CAddRTSPUrlDlg dlg;

	CString sText = m_TreeCtrl.GetItemText(hItem);
	if (dlg.DoModal() == IDOK)
	{
		wcscpy(wszUrl, ANSIToUnicode(dlg.GetUrl()));
		int iRet = m_pIOSourceCtrlDll->AddRTSPUrl(wszUrl, dlg.GetLength());
		if (iRet != -1)
		{
			MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszUrl);
			if (pCurItem)
			{
				wchar_t* pwszShortUrl = m_pIOSourceCtrlDll->GetShortUrl(wszUrl);
				strcpy(szShortUrl, WCharToChar(pwszShortUrl));
				HTREEITEM hCurItem = FindTreeItem(sText.GetBuffer(), szShortUrl);
				if (!hCurItem)
				{
					InsertNode(hItem, pwszShortUrl, false, pCurItem->ulID);
				}
			}
		}

		Save();
		m_pIOSourceCtrlDll->SaveMediaFile();
	}
}

void TreeCtrlUI::EditTag(HTREEITEM hItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CEditTagDlg dlg;

	unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
	if (ulID > 0)
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if(pCurItem)
			dlg.SetTag(pCurItem->wszTag);
	}

	if (dlg.DoModal() == IDOK)
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if (pCurItem)
			wcscpy(pCurItem->wszTag, dlg.GetTag());

		m_pIOSourceCtrlDll->SaveMediaFile();
	}
}

void TreeCtrlUI::SearchTag(HTREEITEM hItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSearchTagDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		SearchChild(dlg.GetKeyword(), true);
	}
}

void TreeCtrlUI::SearchChild(wchar_t* wszText,bool bDoSearchTag)
{
	int iIndex = 0;
	if (m_pFileListCtrl)
	{
		m_pFileListCtrl->Clean();

		HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
		CString sText = m_TreeCtrl.GetItemText(hItem);
		if ((hItem != NULL) && m_TreeCtrl.ItemHasChildren(hItem))
		{
			HTREEITEM hCurItem = m_TreeCtrl.GetChildItem(hItem);
			if (hCurItem)
			{
				if (!m_TreeCtrl.ItemHasChildren(hCurItem))
				{
					CString sText = m_TreeCtrl.GetItemText(hCurItem);
					unsigned long ulID = m_TreeCtrl.GetItemData(hCurItem);

					MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);

					if (pCurItem)
					{
						if (bDoSearchTag)
						{
							if (wcsstr(pCurItem->wszTag, wszText))
							{
								m_pFileListCtrl->InsertItem(iIndex, pCurItem->wszFileName);
								iIndex++;
							}
						}
						else
						{
							m_pFileListCtrl->InsertItem(iIndex, pCurItem->wszFileName);
							iIndex++;
						}
					}
				}

				while (hCurItem)
				{
					hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

					if (hCurItem)
					{
						if (!m_TreeCtrl.ItemHasChildren(hCurItem))
						{
							CString sText = m_TreeCtrl.GetItemText(hCurItem);

							unsigned long ulID = m_TreeCtrl.GetItemData(hCurItem);
							MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
							if (pCurItem)
							{
								if (bDoSearchTag)
								{
									if (wcsstr(pCurItem->wszTag, wszText))
									{
										m_pFileListCtrl->InsertItem(iIndex, pCurItem->wszFileName);
										iIndex++;
									}
								}
								else
								{
									m_pFileListCtrl->InsertItem(iIndex, pCurItem->wszFileName);
									iIndex++;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if (hItem == NULL)
			{
				HTREEITEM hCurItem = m_TreeCtrl.GetNextItem(TVI_ROOT, TVGN_ROOT);
				if (hCurItem)
					m_TreeCtrl.SelectItem(hCurItem);
			}
		}
	}
}

void TreeCtrlUI::DoUpdateList()
{
	SearchChild();
}

void TreeCtrlUI::UpdateList()
{
//	SearchChild();
	::PostMessage(m_TreeCtrl.GetSafeHwnd(), WM_TREE_UPDATE_LIST,0,0);
}

void TreeCtrlUI::SelChanged()
{
	UpdateList();
}

void TreeCtrlUI::ReDraw()
{
	m_pWin->Invalidate();
//	m_pWin->UpdateWindow();
}

HTREEITEM TreeCtrlUI::FindFolder(char* szFolder)
{
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = hRootItem;

	CString sText = m_TreeCtrl.GetItemText(hCurItem);
	if (sText.Compare(szFolder) == 0)
		return hCurItem;

	while (hCurItem)
	{
		hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

		if (hCurItem)
		{
			CString sText = m_TreeCtrl.GetItemText(hCurItem);
			if (sText.Compare(szFolder) == 0)
				return hCurItem;
		}
	}
	return 0;
}

HTREEITEM TreeCtrlUI::FindSubFolder(HTREEITEM hItem,char* szSubFolder)
{
	if (hItem == NULL)
		return FindFolder(szSubFolder);

	HTREEITEM hCurItem = m_TreeCtrl.GetNextItem(hItem, TVGN_CHILD);
	
	CString sText = m_TreeCtrl.GetItemText(hCurItem);
	if (sText.Compare(szSubFolder) == 0)
		return hCurItem;

	while (hCurItem)
	{
		hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

		if (hCurItem)
		{
			CString sText = m_TreeCtrl.GetItemText(hCurItem);
			if (sText.Compare(szSubFolder) == 0)
				return hCurItem;
		}
	}

	return 0;
}

HTREEITEM TreeCtrlUI::FindTreeItem(char* szFolder,char* szItemName)
{
#if 0
	HTREEITEM hFolder = FindFolder(szFolder);
	HTREEITEM hCurItem = m_TreeCtrl.GetChildItem(hFolder);
	if (hCurItem)
	{
		CString sText = m_TreeCtrl.GetItemText(hCurItem);
		if (sText.Compare(szItemName) == 0)
			return hCurItem;

		while (hCurItem)
		{
			hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

			if (hCurItem)
			{
				sText = m_TreeCtrl.GetItemText(hCurItem);
				if (sText.Compare(szItemName) == 0)
					return hCurItem;
			}
		}
	}
	return 0;
#else
	HTREEITEM hFolder = FindFolder(szFolder);
	return FindTreeItem(hFolder,szItemName);
#endif
}

HTREEITEM TreeCtrlUI::FindTreeItem(HTREEITEM hFolderItem, char* szItemName)
{
	HTREEITEM hCurItem = m_TreeCtrl.GetChildItem(hFolderItem);
	if (hCurItem)
	{
		CString sText = m_TreeCtrl.GetItemText(hCurItem);
		if (sText.Compare(szItemName) == 0)
			return hCurItem;

		while (hCurItem)
		{
			hCurItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);

			if (hCurItem)
			{
				sText = m_TreeCtrl.GetItemText(hCurItem);
				if (sText.Compare(szItemName) == 0)
					return hCurItem;
			}
		}
	}
	return 0;
}

void TreeCtrlUI::AddCSVNode(char* szFullFilePath,char* szFileFolder,char* szFileName, char* szTag)
{
	wchar_t wszTag[512];
	wchar_t wszFileFolder[512];
	wchar_t wszFullFilePath[512];
	HTREEITEM hFolderItem = FindFolder(szFileFolder);

	wcscpy(wszTag, ANSIToUnicode(szTag));
	wcscpy(wszFullFilePath, ANSIToUnicode(szFullFilePath));
	wcscpy(wszFileFolder, ANSIToUnicode(szFileFolder));

	MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFullFilePath);
	if (pCurItem)
	{
		if (hFolderItem)
		{
			wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
			char* pszShortFileName = WCharToChar(pwszShortFileName);
			HTREEITEM hItem = FindTreeItem(szFileFolder, pszShortFileName);
			if(hItem)
				return;
			else
				InsertNode(hFolderItem, pwszShortFileName, false, pCurItem->ulID);
		}
		else
		{
			HTREEITEM hRootItem = NULL;//m_TreeCtrl.GetRootItem();
			HTREEITEM hCurItem = InsertNode(hRootItem, wszFileFolder, true, 0);

			wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
			InsertNode(hCurItem, pwszShortFileName, false, pCurItem->ulID);
		}
		return ;
	}

	if (hFolderItem)
	{
		//int iRet = AddFile(hTreeItem, wszFullFilePath);
		int iRet = AddFile2(hFolderItem, wszFullFilePath,true);
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFullFilePath);
		if (pCurItem)
			wcscpy(pCurItem->wszTag, wszTag);
	}
	else
	{
		HTREEITEM hRootItem = NULL;//m_TreeCtrl.GetRootItem();
		HTREEITEM hCurItem = InsertNode(hRootItem, wszFileFolder, true, 0);
		//int iRet = AddFile(hCurItem, wszFullFilePath);
		int iRet = AddFile2(hCurItem, wszFullFilePath,true);

		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFullFilePath);
		if (pCurItem)
			wcscpy(pCurItem->wszTag, wszTag);
	}
}

int TreeCtrlUI::RemoveAll()
{
	int i;
	char szFolderName[512];
	char szFileName[515];
	char szItemPath[512] = "";
	char szMsg[512];

	bool bIsOpened = false;

	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = hRootItem;
	HTREEITEM hNextItem;

	SQList* pFolderList = new SQList;

	CString sText = m_TreeCtrl.GetItemText(hCurItem);

	//IsRTSPUrl

	while (hCurItem)
	{
		hNextItem = m_TreeCtrl.GetNextItem(hCurItem, TVGN_NEXT);
		CString sText = m_TreeCtrl.GetItemText(hCurItem);

		if (hCurItem)
		{
			if (m_TreeCtrl.ItemHasChildren(hCurItem))
			{
				/*
				SQList* pFileList = new SQList;

				strcpy(szItemPath,"");
				FindFullPath(hCurItem, szItemPath);

				CString strFolderName = theApp.m_strCurPath + "MOV\\User\\" + szItemPath;// m_TreeCtrl.GetItemText(hCurItem);
				strcpy(szFolderName, strFolderName.GetString());

				SearchAllFiles(szFolderName, pFileList, pFolderList);

				for (i = 0; i < pFileList->GetTotal(); i++)
				{
					//sprintf(szFileName, "%s\\%s", szFolderName, (char*)pFileList->Get(i));
					strcpy(szFileName,(char*)pFileList->Get(i));
					BOOL bRet = DeleteFile(szFileName);
				}

				for (i = 0; i < pFileList->GetTotal(); i++)
				{
					char* pCurItem = (char*)pFileList->Get(i);
					delete pCurItem;
				}
				pFileList->EnableRemoveData(false);
				pFileList->Reset();
				delete pFileList;

				//char* szNewFolder = new char[strlen(szFolderName) + 1];
				//strcpy(szNewFolder, szFolderName);
				//pFolderList->Add(szNewFolder);
				*/
			}

			m_TreeCtrl.DeleteItem(hCurItem);
			if (hNextItem)
				hCurItem = hNextItem;
			else
				hCurItem = NULL;
		}
	}
	
	SQList* pFileList = new SQList;

	CString strFolderName = theApp.m_strCurPath + "MOV\\User";
	strcpy(szFolderName, strFolderName.GetString());

	SearchAllFiles(szFolderName, pFileList, pFolderList);
	//if (PathFileExists(szFolderName))

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		//sprintf(szFileName, "%s\\%s", szFolderName, (char*)pFileList->Get(i));
		strcpy(szFileName, (char*)pFileList->Get(i));

		if (!IsOpened(szFileName))
			BOOL bRet = DeleteFile(szFileName);
		else
		{
			bIsOpened = true;
//			sprintf(szMsg, "The action can't be completed because the '%s' is open in another module.", szFileName);
//			MessageBoxA(NULL, szMsg, "Warning", MB_OK);
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

	//remove all folder
	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		strcpy(szFolderName, (char*)pFolderList->Get(pFolderList->GetTotal() - i - 1));
		BOOL bRet = RemoveDirectory(szFolderName);
	}

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		strcpy(szFolderName, (char*)pFolderList->Get(i));
		BOOL bRet = RemoveDirectory(szFolderName);
	}

	for (i = 0; i < pFolderList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFolderList->Get(i);
		delete pCurItem;
	}
	pFolderList->EnableRemoveData(false);
	pFolderList->Reset();
	delete pFolderList;

	UpdateList();

	if (bIsOpened)
		return -1;
	return 1;
}

wchar_t* TreeCtrlUI::GetCurSelFileName()
{
	/*
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	CString sText = m_TreeCtrl.GetItemText(hItem);
	if ((hItem != NULL) && m_TreeCtrl.ItemHasChildren(hItem))
	{
	}

	unsigned long ulID = m_TreeCtrl.GetItemData(hCurItem);
	MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
	if (pCurItem)
	{
	}
	*/
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if ((hItem != NULL) && !m_TreeCtrl.ItemHasChildren(hItem))
	{
		unsigned long ulID = m_TreeCtrl.GetItemData(hItem);
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if (pCurItem)
		{
			return pCurItem->wszFileName;
		}
	}
	return L"";
}

void* TreeCtrlUI::GetLogFileObj()
{
	return m_pLogFileObj;
}

void TreeCtrlUI::CheckSaveFile()
{
	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszCurRoot[256];
	//wchar_t szFileName[512] = L"C:\\ProgramData\\tree_ctrl.txt";
	wchar_t wszFileName[512];// = L"tree_ctrl.txt";
	int iCount = 0;

	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);

	sprintf(szFileName, "%s\\tree_ctrl.txt", szAPPath);
	wcscpy(wszFileName, ANSIToUnicode(szFileName));

	m_pSIniFileW->Open(wszFileName, true, false);

	wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"Base", L"Total", L"0");
	int iTotalOfRoots = _wtoi(wszTotal);

	wcscpy(m_wszTmpParentPath2, L"");

	for (int i = 0; i < iTotalOfRoots; i++)
	{
		wsprintfW(wszKeyName, L"Root%d", i);
		wchar_t* wszRoot = m_pSIniFileW->GetItemValue(L"Base", wszKeyName, L"0");
		wcscpy(wszCurRoot, wszRoot);
		wcscpy(wszAppName, wszCurRoot);

		wcscpy(m_wszTmpParentPath2, wszCurRoot);
		bool bRet = CheckTreeNode(m_wszTmpParentPath2);

		if(bRet)
			iCount++;
	}

	m_pSIniFileW->Close();

	if (iCount == iTotalOfRoots)
	{
		sprintf_s(szMsg, "#TreeCtrl# CheckSaveFile Passed!! \n");
		OutputDebugStringA(szMsg);
	}
	else
	{
		//failed!!
		sprintf_s(szMsg, "#TreeCtrl# CheckSaveFile Failed!! \n");
		OutputDebugStringA(szMsg);
	}
}

void TreeCtrlUI::SearchAllFiles(char* szFolder, SQList* pList, SQList* pFolderList)
{
	BOOL bFind = TRUE;
	char szNewFolder[512];
	WIN32_FIND_DATAA FindData;
	FindData.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
	char* szPath = (char*) new char[512];  //MAX_PATH
	sprintf(szPath, "%s\\*.*", szFolder);
	HANDLE hFind = FindFirstFileA(szPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL,"Find first file error!!","error",MB_OK);
		delete szPath;
		return;
	}
	else
	{
		bFind = FindNextFileA(hFind, &FindData);  //remove the .. sub folder
		while (bFind)
		{
			bFind = FindNextFileA(hFind, &FindData);
			if (bFind)
			{
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					sprintf(szNewFolder, "%s\\%s", szFolder, FindData.cFileName);

					if (pFolderList)
					{
						char* szNewFolder2 = new char[strlen(szNewFolder) + 1];
						strcpy(szNewFolder2, szNewFolder);
						pFolderList->Add(szNewFolder2);
					}

					SearchAllFiles(szNewFolder, pList, pFolderList);
				}
				else
				{
					char* szNewFile = new char[strlen(FindData.cFileName) + strlen(szFolder) + 2];
					sprintf(szNewFile, "%s\\%s", szFolder, FindData.cFileName);
					pList->Add(szNewFile);
				}
			}
			//Add(m_iCount++,FindData.cFileName);
		}
	}
	FindClose(hFind);
	delete szPath;
}

void TreeCtrlUI::FindFullPath(HTREEITEM hItem, char* szCurItemPath,bool bTreeNode)
{
	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(hItem);
	if (hParentItem)
	{
		char szNewPath[512];

		CString sItemText = m_TreeCtrl.GetItemText(hItem);
		if (strcmp(szCurItemPath, "") == 0)
			strcpy(szCurItemPath, sItemText.GetString());
		else
		{
			if(bTreeNode)
				sprintf(szNewPath, "%s/%s", sItemText.GetString(), szCurItemPath);
			else
				sprintf(szNewPath, "%s\\%s", sItemText.GetString(), szCurItemPath);
			strcpy(szCurItemPath, szNewPath);
		}
		
		FindFullPath(hParentItem, szCurItemPath, bTreeNode);
	}
	else
	{
		if (hItem)
		{
			char szNewPath[512];
			CString sItemText = m_TreeCtrl.GetItemText(hItem);
			if (strcmp(szCurItemPath, "") == 0)
				strcpy(szCurItemPath, sItemText.GetString());
			else
			{
				if (bTreeNode)
					sprintf(szNewPath, "%s/%s", sItemText.GetString(), szCurItemPath);
				else
					sprintf(szNewPath, "%s\\%s", sItemText.GetString(), szCurItemPath);
				strcpy(szCurItemPath, szNewPath);
			}
		}
	}
}

bool TreeCtrlUI::IsOpened(char* szFileName)
{
	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		CString strTemp = g_MediaStreamAry.at(y)->FilePath();
		if (strcmp(szFileName, (char*)strTemp.GetBuffer()) == 0)
		{
			if (g_MediaStreamAry.at(y)->IsOpened())
				return true;
		}
	}
	return false;
}

void TreeCtrlUI::EnableEnableSearchTag(bool bEnable)
{
	m_bEnableSearchTag = bEnable;
}

bool TreeCtrlUI::IsReadOnly(HTREEITEM hItem)
{
	char szItemPath[512] = "";
	strcpy(szItemPath, "");
	FindFullPath(hItem, szItemPath, true);

	for (int i = 0; i < m_CSVReader.GetTotal(); i++)
	{
		TagInfo* pInfo = m_CSVReader.GetTagInfo(i);
		if (pInfo)
		{
			char* pch = strstr(szItemPath, pInfo->szFileFolder);
			if (pch)
				return true;
		}
	}

	return false;
}

void* TreeCtrlUI::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case OBJ_MSG_RBUTTON_DOWN:
			PopupMenu((HTREEITEM)pParameter1, (POINT*)pParameter2);
			break;
		case OBJ_MSG_SEL_CHANGED:
			SelChanged();
			//m_pWin->Invalidate();
			//m_pWin->UpdateWindow();
			break;
		case OBJ_MSG_UPDATE_LIST:
			DoUpdateList();
			break;
	}
	return 0;
}

void* TreeCtrlUI::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case WM_KEYDOWN:
		{
			if ((int)pParameter1 == 27)
			{
				::PostMessage(m_hParentWnd, WM_KEYDOWN, (int)pParameter1, (int)pParameter2);

				if(m_bSimpleMode)
					::ShowWindow(m_hParentWnd, SW_HIDE);
				else
					::PostMessage(m_hParentWnd, WM_CLOSE, 0, 0);
			}
		}
		break;
	}

	return 0;
}

unsigned int __stdcall _AddFileThreadProc(void* lpvThreadParm)
{
	TreeCtrlUIThreadParam* pThreadParam = (TreeCtrlUIThreadParam *)lpvThreadParm;
	((TreeCtrlUI*)pThreadParam->pObj)->AddFile(pThreadParam->hItem);
	return 0;
}

//for debug
void TreeCtrlUI::TestAddFile(HTREEITEM hItem,char* szFilePath)
{
	CString strDest;
	wchar_t wszFileName[512];

	char szFileName[512];
	char* pch = strrchr(szFilePath,'\\');
	if (pch)
		strcpy(szFileName,pch+1);

	char szItemPath[512] = "";
	FindFullPath(hItem, szItemPath);

	if (strcmp(szItemPath, "") == 0)
	{
		strDest = theApp.m_strCurPath + "MOV\\User\\" + m_TreeCtrl.GetItemText(hItem) +
			"\\" + szFileName;
	}
	else
	{
		strDest = theApp.m_strCurPath + "MOV\\User\\" + szItemPath +
			"\\" + szFileName;
	}

	BOOL bRet = CopyFile(szFilePath, strDest, FALSE);
	if (bRet)
	{
		CFileStatus fsCur;
		CFile::GetStatus(strDest, fsCur);

		fsCur.m_attribute &= ~CFile::readOnly;
		CFile::SetStatus(strDest, fsCur);

		if (m_pLogFileDll)
		{
			if (m_pLogFileObj)
			{
				wchar_t wszSrc[256];
				wchar_t wszDest[256];
				wcscpy(wszSrc, ANSIToUnicode(szFilePath));
				wcscpy(wszDest, ANSIToUnicode(strDest));
				m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_COPY_FILE, wszSrc, wszDest, 0);
			}
		}

		wcscpy(wszFileName, ANSIToUnicode(strDest));

		int iRet = AddFile2(hItem, wszFileName, false);
	}
}

void TreeCtrlUI::TestAddFolder(HTREEITEM hItem, char* szFolderName)
{
	AddFolder(hItem,szFolderName);
}

void TreeCtrlUI::TestRemoveFolder(char* szParent, char* szChild)
{
	HTREEITEM hItem = FindTreeItem(szParent, szChild);
	if (hItem)
		RemoveNode(hItem);
}

void TreeCtrlUI::DoAutoTest()
{
	char szMsg[1024];
	int iTimes = 0;
	int iTotal = 0;
	int iInterval = 0;
	char szAppName[128];
	char szFolder[128];
	char szCurFolder[128];
	wchar_t wszFileName[512];
	TestItem items[100];

	iTotal = GetPrivateProfileInt("Base", "Total", 0, theApp.m_strCurPath + "TestTreeCtrl.ini");
	iInterval = GetPrivateProfileInt("Base", "Interval", 0, theApp.m_strCurPath + "TestTreeCtrl.ini");
	iTimes = GetPrivateProfileInt("Base", "Times", 0, theApp.m_strCurPath + "TestTreeCtrl.ini");
	GetPrivateProfileString("Base", "Folder", "", szFolder, 128, theApp.m_strCurPath + "TestTreeCtrl.ini");

	HTREEITEM hFolder = FindFolder(szFolder);

	strcpy(szCurFolder, szFolder);

	for (int i = 0; i < iTotal; i++)
	{
		sprintf(szAppName, "Item%d", i);
		GetPrivateProfileString(szAppName, "CMD", "", items[i].szCmd, 64, theApp.m_strCurPath + "TestTreeCtrl.ini");
		GetPrivateProfileString(szAppName, "FileName", "", items[i].szFileName, 512, theApp.m_strCurPath + "TestTreeCtrl.ini");
		GetPrivateProfileString(szAppName, "Folder", "", items[i].szFolderName, 512, theApp.m_strCurPath + "TestTreeCtrl.ini");
	}

	for (int j = 0; j < iTimes; j++)
	{
		for (int i = 0; i < iTotal; i++)
		{
			if (strcmp(items[i].szCmd, "add") == 0)
			{
				/*
				wcscpy(wszFileName, ANSIToUnicode(items[i].szFileName));
				//AddFile(hFolder, wszFileName);

				int iRet = AddFile2(hFolder, wszFileName, false);
				*/

				if(strcmp(items[i].szFolderName, szFolder) == 0)
					hFolder = FindFolder(items[i].szFolderName);
				else
					hFolder = FindTreeItem(szFolder,items[i].szFolderName);
				TestAddFile(hFolder, items[i].szFileName);
				
			}
			else if (strcmp(items[i].szCmd, "add_folder") == 0)
			{
				hFolder = FindFolder(items[i].szFolderName);
				TestAddFolder(hFolder, items[i].szFileName);
				
			}
			else if (strcmp(items[i].szCmd, "del_folder") == 0)
			{
				
				TestRemoveFolder(items[i].szFolderName,items[i].szFileName);
				
			}
			else if (strcmp(items[i].szCmd, "delete") == 0)
			{
				HTREEITEM hItem = FindTreeItem(items[i].szFolderName, items[i].szFileName);
				if (hItem)
					RemoveNode(hItem);
					//m_TreeCtrl.DeleteItem(hItem);
			}

			m_TreeCtrl.SelectItem(hFolder);
			UpdateList();

			Save();
			m_pIOSourceCtrlDll->SaveMediaFile();

			CheckSaveFile();

			Sleep(iInterval);
		}
	}

	sprintf_s(szMsg, "#TreeCtrl# Test Finished!! \n");
	OutputDebugStringA(szMsg);
}

void TreeCtrlUI::Test()
{
	unsigned threadID1;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _TestThreadProc, this, 0, &threadID1);
}

unsigned int __stdcall _TestThreadProc(void* lpvThreadParm)
{
	TreeCtrlUI* pObj = (TreeCtrlUI *)lpvThreadParm;
	pObj->DoAutoTest();
	return 0;
}

/*
CString strDest;

if (g_PannelSetting.iEncodeMedia == 0)
{
	wcscpy(wszFile, ANSIToUnicode(fileDlg.GetPathName()));

	int iRet = AddFile(hItem, wszFile);
	if (iRet != -1)
		UpdateList();
	else
		MessageBoxA(NULL, "Unsupported", "", MB_OK);
}
else
{
#if 0
	strDest = theApp.m_strCurPath + "MOV\\" + m_TreeCtrl.GetItemText(hItem) +
		"\\" + fileDlg.GetFileName();
#else

	char szItemPath[512] = "";
	FindFullPath(hItem, szItemPath);

	if (strcmp(szItemPath, "") == 0)
	{
		strDest = theApp.m_strCurPath + "MOV\\User\\" + m_TreeCtrl.GetItemText(hItem) +
			"\\" + fileDlg.GetFileName();
	}
	else
	{
		strDest = theApp.m_strCurPath + "MOV\\User\\" + szItemPath +
			"\\" + fileDlg.GetFileName();
	}

#endif
	CopyFile(fileDlg.GetPathName(), strDest, FALSE);
	CFileStatus fsCur;
	CFile::GetStatus(strDest, fsCur);

	fsCur.m_attribute &= ~CFile::readOnly;
	CFile::SetStatus(strDest, fsCur);

	if (m_pLogFileDll)
	{
		if (m_pLogFileObj)
		{
			wchar_t wszSrc[256];
			wchar_t wszDest[256];
			wcscpy(wszSrc, ANSIToUnicode(fileDlg.GetPathName()));
			wcscpy(wszDest, ANSIToUnicode(strDest));
			m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_COPY_FILE, wszSrc, wszDest, 0);
		}
	}

	wcscpy(wszFile, ANSIToUnicode(strDest));

	*/
