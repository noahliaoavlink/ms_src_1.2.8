#include "stdafx.h"
#include "FileListCtrl.h"
#include "../../../Include/StrConv.h"

NListCtrl::NListCtrl()
{
	m_pNWin = 0;
	m_hParentWnd = NULL;
	m_bSimpleMode = false;
}

NListCtrl::~NListCtrl()
{
	if (m_pNWin)
		delete m_pNWin;
}
/*
BEGIN_MESSAGE_MAP(NListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnSelChanged)
END_MESSAGE_MAP()
*/

void NListCtrl::Create(char* szName, int iID, int w, int h)
{
	m_pNWin = new CNWin(szName);
	m_pNWin->SetWinSize(w, h, false);

	CListCtrl::Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, CRect(0, 0, w, h), m_pNWin, iID);

	m_hWnd = m_pNWin->GetSafeHwnd();

	m_pNWin->SetCallback(this);

	m_pNWin->SetBKColor(50, 50, 50);
	//CListCtrl::SetTextColor(RGB(200,200,0));

	SetTextBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(200, 200, 0));
	SetBkColor(RGB(50, 50, 50));
}

void NListCtrl::SetParent(HWND hWnd)
{
	m_hParentWnd = hWnd;
	::SetParent(m_hWnd, hWnd);
}

void NListCtrl::MoveWindow(int x, int y, int w, int h)
{
	m_pNWin->MoveWindow(x, y, w, h);
	CListCtrl::MoveWindow(0, 0, w, h);
}

void NListCtrl::ShowWindow(bool bShow)
{
	if (bShow)
	{
		m_pNWin->ShowWindow(SW_SHOW);
		ReDraw();
	}
	else
		m_pNWin->ShowWindow(SW_HIDE);
}

void NListCtrl::Reset()
{
	CListCtrl::DeleteAllItems();
}

int NListCtrl::GetNextSelectItem(int iSel)
{
	return CListCtrl::GetNextItem(iSel, LVNI_SELECTED);
}

void NListCtrl::ChangeStyle(int iStyle)
{
	CListCtrl::ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
	CListCtrl::ModifyStyle(0, iStyle);
}


void NListCtrl::ReDraw()
{
	m_pNWin->Invalidate();
//	m_pNWin->UpdateWindow();
}

void NListCtrl::SetSimpleMode(bool bEnable)
{
	m_bSimpleMode = bEnable;
}

void* NListCtrl::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
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

//CListCtrl::SetImageList 
//m_cListCtrl.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
/*
void NListCtrl::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	

	if ((pNMListView->uChanged & LVIF_STATE)
		&& (pNMListView->uNewState & LVIS_SELECTED))
	{
		pNMListView->iItem;
		int kk = 0;
	}
}
*/


FileListCtrl::FileListCtrl()
{
	m_pIOSourceCtrlDll = 0;
	m_pFileInfoCtrl = 0;

	m_iMode = FIDM_LIST;
	m_hBrush = 0;

//	m_bSimpleMode = false;
	m_pEventCallback = 0;

	m_pRGB32Buf = new unsigned char[_THUMBNAIL_FRAME_W * _THUMBNAIL_FRAME_H * 4];
}

FileListCtrl::~FileListCtrl()
{
	if (m_pRGB32Buf)
		delete m_pRGB32Buf;

	if (m_hBrush)
		DeleteObject(m_hBrush);
}

BEGIN_MESSAGE_MAP(FileListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdrawMyList)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void FileListCtrl::Init()
{
	CListCtrl* pList = this;//GetCListCtrl();
	pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
#ifdef _UNICODE
	pList->InsertColumn(0, L"FileName");
	pList->InsertColumn(1, L"Format");
	pList->InsertColumn(2, L"Video");
	pList->InsertColumn(3, L"Audio");
	pList->InsertColumn(4, L"Len");
	pList->InsertColumn(5, L"Tags");
#else
	pList->InsertColumn(0, "FileName");
	pList->InsertColumn(1, "Format");
	pList->InsertColumn(2, "Video");
	pList->InsertColumn(3, "Audio");
	pList->InsertColumn(4, "Len");
	pList->InsertColumn(5, "Tags");
#endif
	pList->SetColumnWidth(0, 300);
	pList->SetColumnWidth(1, 60);
	pList->SetColumnWidth(2, 60);
	pList->SetColumnWidth(3, 60);
	pList->SetColumnWidth(4, 60);
	pList->SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);

	/*
	SetTextBkColor(RGB(100,100,100));
	SetTextColor(RGB(255, 128, 0));
	SetBkColor(RGB(100, 100, 100));
	*/
	m_hBrush = CreateSolidBrush(RGB(50,50,50));

	
	CHeaderCtrl* pHeader = NULL;
	pHeader = pList->GetHeaderCtrl();

	HDITEM hdItem;
	hdItem.mask = HDI_FORMAT;
	for (int i = 0; i<pHeader->GetItemCount(); i++)
	{
		pHeader->GetItem(i, &hdItem);
		hdItem.fmt |= HDF_OWNERDRAW;

		pHeader->SetItem(i, &hdItem);
	}
	

	bool bRet = m_ImageList.Create(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, ILC_COLOR32 | ILC_MASK,0,1);
}

void FileListCtrl::SetIOSourceCtrl(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void FileListCtrl::SetFileInfoCtrl(FileInfoCtrl* pObj)
{
	m_pFileInfoCtrl = pObj;
}

void FileListCtrl::SetSimpleMode(bool bEnable)
{
	//m_bSimpleMode = bEnable;
	NListCtrl::SetSimpleMode(bEnable);
//	m_pFileInfoCtrl->SetSimpleMode(bEnable);
}

void FileListCtrl::MoveWindow(int x, int y, int w, int h)
{
	NListCtrl::MoveWindow(x, y, w, h);
	
	CListCtrl* pList = this;
	pList->SetColumnWidth(0, 200);
	pList->SetColumnWidth(1, 60);
	pList->SetColumnWidth(2, 60);
	pList->SetColumnWidth(3, 60);
	pList->SetColumnWidth(4, 60);
	pList->SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
}

void FileListCtrl::SetDisplayMode(int iMode)
{
	m_iMode = iMode;
	switch (m_iMode)
	{
		case FIDM_LIST:
			ChangeStyle(LVS_REPORT);
			break;
		case FIDM_ICON:
			ChangeStyle(LVS_ICON);
			break;
	}
}

void FileListCtrl::Clean()
{
	CListCtrl* pList = this;// GetCListCtrl();
	pList->DeleteAllItems();

	if (m_pFileInfoCtrl)
		m_pFileInfoCtrl->ReDraw();

	m_ImageList.DeleteImageList();
	bool bRet = m_ImageList.Create(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, ILC_COLOR32 | ILC_MASK, 0, 1);
}

void FileListCtrl::InsertItem(int iIndex,wchar_t* wszFileName)
{
	wchar_t wszTag[256];
	CListCtrl* pList = this;// GetCListCtrl();
	if (m_pIOSourceCtrlDll)
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
		if (pCurItem)
		{
			wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);

#ifdef _UNICODE
			pList->InsertItem(iIndex, pwszShortFileName, m_ImageList.GetImageCount());
			pList->SetItemText(iIndex, 1, pCurItem->wszFileFormat);
			pList->SetItemText(iIndex, 2, pCurItem->wszVideoCodecName);
			pList->SetItemText(iIndex, 3, pCurItem->wszAudioCodecName);
			pList->SetItemText(iIndex, 4, pCurItem->wszLen);
			wcscpy(wszTag, pCurItem->wszTag);
			pList->SetItemText(iIndex, 5, wszTag);
#else
			pList->InsertItem(iIndex, WCharToChar(pwszShortFileName), m_ImageList.GetImageCount());
			pList->SetItemText(iIndex, 1, WCharToChar(pCurItem->wszFileFormat));
			pList->SetItemText(iIndex, 2, WCharToChar(pCurItem->wszVideoCodecName));
			pList->SetItemText(iIndex, 3, WCharToChar(pCurItem->wszAudioCodecName));
			pList->SetItemText(iIndex, 4, WCharToChar(pCurItem->wszLen));
			wcscpy(wszTag, pCurItem->wszTag);
			pList->SetItemText(iIndex, 5, WCharToChar(wszTag));
#endif

			pList->SetItemData(iIndex, pCurItem->ulID);

			ThumbnailPicture* pPic = m_pIOSourceCtrlDll->GetPicture(pCurItem);
			if (pPic)
			{
				m_yuv_converter.I420ToARGB(pPic->pBuffer, m_pRGB32Buf, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);

				CBitmap bmp;
				bool bRet = bmp.CreateBitmap(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, 1, 32, m_pRGB32Buf);
				int iRet = m_ImageList.Add(&bmp, RGB(255, 0, 255));
				bmp.DeleteObject();
			}
		}
	}
	pList->SetImageList(&m_ImageList, LVSIL_NORMAL);
}

wchar_t* FileListCtrl::GetCurSelFileName()
{
	CListCtrl* pList = this;

	int iCurSel = NListCtrl::GetNextSelectItem(-1);
	if (iCurSel != -1)
	{
		unsigned long ulID = pList->GetItemData(iCurSel);
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);
		if (pCurItem)
			return pCurItem->wszFileName;
	}
	return L"";
}

wchar_t* FileListCtrl::GetFirstFileName()
{
	CListCtrl* pList = this;

	if (m_pIOSourceCtrlDll->GetTotalOfFileList())
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(0);
		if (pCurItem)
			return pCurItem->wszFileName;
	}
	return L"";
}

void FileListCtrl::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;


	if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
	{
		CListCtrl* pList = this;
		unsigned long ulID = pList->GetItemData(pNMListView->iItem);
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);

		if (m_pFileInfoCtrl)
			m_pFileInfoCtrl->UpdateInfo(ulID);

		if (m_pEventCallback)
			m_pEventCallback->Event(MF_EVT_SEL_CHANGED, 0, 0);
	}
}

void FileListCtrl::OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;
	
#if 0
	switch (pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;

		case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
		{
			//text color
			pLVCD->clrText = RGB(255, 0, 0);
			//pLVCD->clrTextBk;
		}
		break;
	}
#endif
}

void FileListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
#if 1
	if (lpDrawItemStruct->CtlType == ODT_HEADER)
	{
		HDITEM hdi;
		const int c_cchBuffer = 256;
		TCHAR  lpBuffer[c_cchBuffer];

		hdi.mask = HDI_TEXT;
		hdi.pszText = lpBuffer;
		hdi.cchTextMax = c_cchBuffer;

		CHeaderCtrl* pHeader = NULL;
		pHeader = GetHeaderCtrl();
		if (pHeader)
		{
			pHeader->GetItem(lpDrawItemStruct->itemID, &hdi);
//		m_HeaderCustom.GetItem(lpDrawItemStruct->itemID, &hdi);

			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, m_hBrush);
			// Draw the button frame.
//		::DrawFrameControl(lpDrawItemStruct->hDC,
//			&lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);

			// Draw the items text using the text color red.
			SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC,RGB(200, 128, 0));
			::DrawText(lpDrawItemStruct->hDC, lpBuffer,
				(int)_tcsnlen(lpBuffer, c_cchBuffer),
				&lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
		}
	}
#endif
	CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL FileListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 27)
		{
			HWND hParent = ::GetParent(this->GetSafeHwnd());
			if(hParent)
				::PostMessage(hParent, WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void FileListCtrl::SetEventCallback(void* pObj)
{
	m_pEventCallback = (EventCallback*)pObj;
}

BEGIN_MESSAGE_MAP(EffectListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnSelChanged)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdrawMyList)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

EffectListCtrl::EffectListCtrl()
{
	m_hBrush = 0;

	m_pEventCallback = 0;
}

EffectListCtrl::~EffectListCtrl()
{
	if (m_hBrush)
		DeleteObject(m_hBrush);
}
/*
void EffectListCtrl::Create(char* szName, int iID, int w, int h, CWnd* pParent)
{
	CListCtrl::Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, CRect(0, 0, w, h), pParent, iID);

	SetTextBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(200, 200, 0));
	SetBkColor(RGB(50, 50, 50));
}
*/
void EffectListCtrl::MoveWindow(int x, int y, int w, int h)
{
	NListCtrl::MoveWindow(x, y, w, h);

	//m_pNWin->MoveWindow(x, y, w, h);
	//CListCtrl::MoveWindow(x, y, w, h);

	CListCtrl* pList = this;

	int iItemWidth = w / 8;

	pList->SetColumnWidth(0, iItemWidth);
	pList->SetColumnWidth(1, iItemWidth);
	pList->SetColumnWidth(2, iItemWidth);
	pList->SetColumnWidth(3, iItemWidth);
	pList->SetColumnWidth(4, iItemWidth);
	pList->SetColumnWidth(5, iItemWidth);
	pList->SetColumnWidth(6, iItemWidth);
	pList->SetColumnWidth(7, LVSCW_AUTOSIZE_USEHEADER);
}

void EffectListCtrl::Init()
{
	CListCtrl* pList = this;//GetCListCtrl();
	pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
#ifdef _UNICODE
	pList->InsertColumn(0, L"Name");
	pList->InsertColumn(1, L"Effect Name");
	pList->InsertColumn(2, L"Level");
	pList->InsertColumn(3, L"Parameter1");
	pList->InsertColumn(4, L"Parameter2");
	pList->InsertColumn(5, L"Parameter3");
	pList->InsertColumn(6, L"Parameter4");
	pList->InsertColumn(7, L"Parameter5");
#else
	pList->InsertColumn(0, "Name");
	pList->InsertColumn(1, "Effect Name");
	pList->InsertColumn(2, "Level");
	pList->InsertColumn(3, "Parameter1");
	pList->InsertColumn(4, "Parameter2");
	pList->InsertColumn(5, "Parameter3");
	pList->InsertColumn(6, "Parameter4");
	pList->InsertColumn(7, "Parameter5");
#endif
	pList->SetColumnWidth(0, 90);
	pList->SetColumnWidth(1, 90);
	pList->SetColumnWidth(2, 60);
	pList->SetColumnWidth(3, 80);
	pList->SetColumnWidth(4, 80);
	pList->SetColumnWidth(5, 80);
	pList->SetColumnWidth(6, 80);
	pList->SetColumnWidth(7, LVSCW_AUTOSIZE_USEHEADER);

	/*
	SetTextBkColor(RGB(100,100,100));
	SetTextColor(RGB(255, 128, 0));
	SetBkColor(RGB(100, 100, 100));
	*/

	m_hBrush = CreateSolidBrush(RGB(50, 50, 50));


	CHeaderCtrl* pHeader = NULL;
	pHeader = pList->GetHeaderCtrl();

	HDITEM hdItem;
	hdItem.mask = HDI_FORMAT;
	for (int i = 0; i<pHeader->GetItemCount(); i++)
	{
		pHeader->GetItem(i, &hdItem);
		hdItem.fmt |= HDF_OWNERDRAW;

		pHeader->SetItem(i, &hdItem);
	}
	//bool bRet = m_ImageList.Create(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, ILC_COLOR32 | ILC_MASK, 0, 1);
}

#if 0
void EffectListCtrl::SetSimpleMode(bool bEnable)
{
	NListCtrl::SetSimpleMode(bEnable);
}
#endif

void EffectListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType == ODT_HEADER)
	{
		HDITEM hdi;
		const int c_cchBuffer = 256;
		TCHAR  lpBuffer[c_cchBuffer];

		hdi.mask = HDI_TEXT;
		hdi.pszText = lpBuffer;
		hdi.cchTextMax = c_cchBuffer;

		CHeaderCtrl* pHeader = NULL;
		pHeader = GetHeaderCtrl();
		if (pHeader)
		{
			pHeader->GetItem(lpDrawItemStruct->itemID, &hdi);
			//		m_HeaderCustom.GetItem(lpDrawItemStruct->itemID, &hdi);

			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, m_hBrush);
			// Draw the button frame.
			//		::DrawFrameControl(lpDrawItemStruct->hDC,
			//			&lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);

			// Draw the items text using the text color red.
			SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
			COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(200, 128, 0));
			::DrawText(lpDrawItemStruct->hDC, lpBuffer,
				(int)_tcsnlen(lpBuffer, c_cchBuffer),
				&lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
		}
	}
	CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void EffectListCtrl::ReDraw()
{
	Invalidate();
}

void EffectListCtrl::AddItem(SMEffectItem* pItem)
{
	CListCtrl* pList = this;
	int iTotal = pList->GetItemCount();

	InsertItem(iTotal, pItem);
}

void EffectListCtrl::InsertItem(int iIndex, SMEffectItem* pItem)
{
	char szText[256];
	CListCtrl* pList = this;//GetCListCtrl();

	//pItem->szName;
	//pItem->szEffectName;

	sprintf(szText,"%d", pItem->iParameter1);

	pList->InsertItem(iIndex, pItem->szName, 0);
	pList->SetItemText(iIndex, 1, pItem->szEffectName);
	sprintf(szText, "%d", pItem->iLevel);
	pList->SetItemText(iIndex, 2, szText);
	sprintf(szText, "%d", pItem->iParameter1);
	pList->SetItemText(iIndex, 3, szText);
	sprintf(szText, "%d", pItem->iParameter2);
	pList->SetItemText(iIndex, 4, szText);
	sprintf(szText, "%d", pItem->iParameter3);
	pList->SetItemText(iIndex, 5, szText);
	sprintf(szText, "%d", pItem->iParameter4);
	pList->SetItemText(iIndex, 6, szText);
	sprintf(szText, "%d", pItem->iParameter5);
	pList->SetItemText(iIndex, 7, szText);

	pList->SetItemData(iIndex, pItem->iEffectID);
}

void EffectListCtrl::UpdateItem(int iIndex, SMEffectItem* pItem)
{
	char szText[256];
	CListCtrl* pList = this;//GetCListCtrl();

							//pItem->szName;
							//pItem->szEffectName;

	sprintf(szText, "%d", pItem->iParameter1);

	pList->SetItemText(iIndex, 0, pItem->szName);
	pList->SetItemText(iIndex, 1, pItem->szEffectName);
	sprintf(szText, "%d", pItem->iLevel);
	pList->SetItemText(iIndex, 2, szText);
	sprintf(szText, "%d", pItem->iParameter1);
	pList->SetItemText(iIndex, 3, szText);
	sprintf(szText, "%d", pItem->iParameter2);
	pList->SetItemText(iIndex, 4, szText);
	sprintf(szText, "%d", pItem->iParameter3);
	pList->SetItemText(iIndex, 5, szText);
	sprintf(szText, "%d", pItem->iParameter4);
	pList->SetItemText(iIndex, 6, szText);
	sprintf(szText, "%d", pItem->iParameter5);
	pList->SetItemText(iIndex, 7, szText);

	pList->SetItemData(iIndex, pItem->iEffectID);
}

SMEffectItem* EffectListCtrl::GetItem(int iIndex)
{
	CListCtrl* pList = this;//GetCListCtrl();
	m_SMEffectItem.iEffectID = pList->GetItemData(iIndex);

	CString sText = pList->GetItemText(iIndex,0);
	strcpy(m_SMEffectItem.szName, sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 1);
	strcpy(m_SMEffectItem.szEffectName, sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 2);
	m_SMEffectItem.iLevel = atoi(sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 3);
	m_SMEffectItem.iParameter1 = atoi(sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 4);
	m_SMEffectItem.iParameter2 = atoi(sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 5);
	m_SMEffectItem.iParameter3 = atoi(sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 6);
	m_SMEffectItem.iParameter4 = atoi(sText.GetBuffer());
	sText = pList->GetItemText(iIndex, 7);
	m_SMEffectItem.iParameter5 = atoi(sText.GetBuffer());

	return &m_SMEffectItem;
}

void EffectListCtrl::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

#if 0
	if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
	{
		if (m_pEventCallback)
			m_pEventCallback->Event(EF_EVT_SEL_CHANGED,0,0);
		/*
		CListCtrl* pList = this;
		unsigned long ulID = pList->GetItemData(pNMListView->iItem);
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID);

		if (m_pFileInfoCtrl)
			m_pFileInfoCtrl->UpdateInfo(ulID);
			*/
	}
#else
	if ((pNMListView->uChanged & LVIF_STATE))
	{
		if (m_pEventCallback)
			m_pEventCallback->Event(EF_EVT_SEL_CHANGED, 0, 0);
		
	}
#endif
}

void EffectListCtrl::SetEventCallback(void* pObj)
{
	m_pEventCallback = (EventCallback*)pObj;
}

BOOL EffectListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 27)
		{
			HWND hParent = ::GetParent(this->GetSafeHwnd());
			if (hParent)
				::PostMessage(hParent, WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}
