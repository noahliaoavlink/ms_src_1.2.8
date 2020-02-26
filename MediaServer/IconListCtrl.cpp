#include "stdafx.h"
#include "IconListCtrl.h"
#include "../../../Include/MediaFileCommon.h"

IMPLEMENT_DYNAMIC(IconListCtrl, CListCtrl)

IconListCtrl::IconListCtrl()
{
	m_bListChg = false;
	m_ulSelID = -1;//nothing to select

	textBGColor = TRANSPARENT;
	m_BGColor = RGB(50, 50, 50);//BLACK;
	m_FGColor = RGB(255, 255, 255);
}

IconListCtrl::~IconListCtrl()
{
	if (m_hBrush)
		DeleteObject(m_hBrush);
}

BEGIN_MESSAGE_MAP(IconListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(IconListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_ERASEBKGND()
//	ON_WM_CTLCOLOR()
//	ON_WM_CTLCOLOR_REFLECT()
	//ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &IconListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, &IconListCtrl::OnNMClick)
END_MESSAGE_MAP()

void IconListCtrl::Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CListCtrl::Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ICON, rect, pParent, ulID);
	//BOOL bRet = CListCtrl::Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, rect, pParent, ulID);
	Init();
	Clean();
}

void IconListCtrl::Clean()
{
	CListCtrl* pList = this;
	pList->DeleteAllItems();

	m_ImageList.DeleteImageList();
	bool bRet = m_ImageList.Create(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, ILC_COLOR32 | ILC_MASK, 0, 1);

	m_hBrush = CreateSolidBrush(RGB(50, 50, 50));
}

void IconListCtrl::Init()
{
	CListCtrl* pList = this;//GetCListCtrl();
	pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
#ifdef _UNICODE
	pList->InsertColumn(0, L"Name");
#else
	pList->InsertColumn(0, "Name");
#endif
	
}

BOOL IconListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	CRect scrollbar_rect;
	IconListCtrl::GetClientRect(rect);
	::FillRect(pDC->m_hDC, &rect, m_hBrush);

	
	CScrollBar* pScrollBar = this->GetScrollBarCtrl(SB_VERT);
	if (pScrollBar)
	{
		pScrollBar->GetClientRect(scrollbar_rect);

		pScrollBar->GetSafeHwnd();

		HDC hDC = ::GetDC(pScrollBar->GetSafeHwnd());

		::FillRect(hDC, &scrollbar_rect, m_hBrush);

		::ReleaseDC(pScrollBar->GetSafeHwnd(), hDC);
	}
	
	return FALSE;
}

HBRUSH IconListCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (textBGColor == TRANSPARENT)
		pDC->SetBkMode(TRANSPARENT);
	else
		pDC->SetBkMode(OPAQUE);

	pDC->SetBkColor(textBGColor);
	pDC->SetTextColor(m_FGColor);

	if (cBrush.GetSafeHandle())
		cBrush.DeleteObject();
	cBrush.CreateSolidBrush(m_BGColor);

	return cBrush;
}

HBRUSH IconListCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CRect rect;

	this->GetClientRect(&rect);

	if (textBGColor == TRANSPARENT)
		pDC->SetBkMode(TRANSPARENT);
	else
		pDC->SetBkMode(OPAQUE);

	pDC->SetBkColor(textBGColor);
	pDC->SetTextColor(m_FGColor);

	if (cBrush.GetSafeHandle())
		cBrush.DeleteObject();
	cBrush.CreateSolidBrush(m_BGColor);

	return cBrush;
}

void IconListCtrl::OnNMCustomdraw(NMHDR * pNMHDR, LRESULT * pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = CDRF_DODEFAULT;

	pLVCD->clrText = RGB(255, 255, 255);
	pLVCD->clrTextBk = RGB(50,50,50);

	
	/*
	CRect scrollbar_rect;
	CScrollBar* pScrollBar = this->GetScrollBarCtrl(SB_VERT);
	if (pScrollBar)
	{
		pScrollBar->GetClientRect(scrollbar_rect);

		pScrollBar->GetSafeHwnd();

		HDC hDC = ::GetDC(pScrollBar->GetSafeHwnd());

		::FillRect(hDC, &scrollbar_rect, m_hBrush);

		::ReleaseDC(pScrollBar->GetSafeHwnd(), hDC);
	}
	*/
}

void IconListCtrl::AddItem(char* szName,unsigned char* pIconBuf, unsigned long ulID)
{
	CListCtrl* pList = this;
	int iTotal = pList->GetItemCount();
	pList->InsertItem(iTotal, szName, m_ImageList.GetImageCount());
	pList->SetItemData(iTotal, ulID);

	CBitmap bmp;
	bool bRet = bmp.CreateBitmap(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, 1, 32, pIconBuf);
	int iRet = m_ImageList.Add(&bmp, RGB(255, 0, 255));
	bmp.DeleteObject();

	pList->SetImageList(&m_ImageList, LVSIL_NORMAL);
}
/*
LRESULT IconListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_VSCROLL || message == WM_HSCROLL)
	{
	}

	if (message == WM_CTLCOLORSCROLLBAR || message == WM_CTLCOLOR)
	{
		
	}
	return CListCtrl::WindowProc(message, wParam, lParam);
}
*/

void IconListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	if ((pNMLV->uChanged & LVIF_STATE)
		&& (pNMLV->uNewState & LVIS_SELECTED))
	{
		//m_bListChg = true;
	}
	*pResult = 0;
}


void IconListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	m_iSelIdx = pNMItemActivate->iItem;

	if (m_iSelIdx >= 0)
	{
		m_ulSelID = GetItemData(m_iSelIdx);
		GetParent()->PostMessage(WM_MIXER_CLR_EXIST_ZOOM, m_iSelIdx, m_ulSelID);
	}
	else
		m_ulSelID = -1;

	m_bListChg = true;
	*pResult = 0;
}
