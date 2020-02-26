
#include "stdafx.h"
#include "BitWnd.h"
//#include "resource.h"
//#include "BitItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBitWnd::CBitWnd()
#ifdef _ENABLE_BMP_FILE
	: m_pNormalBmpFile(NULL),
	  m_pActiveBmpFile(NULL),
	  m_pHoverBmpFile(NULL),
	m_nOldState(kNormalBit),
#else
  : m_lpBit(NULL),
#endif
	m_bMute(FALSE),
	m_bCanMute(FALSE),
	m_hCursor(NULL),
	m_bNotify(FALSE),
	m_bTracking(FALSE),
	m_bAllocFlag(FALSE),
	m_nState(kNormalBit),
	m_bLDownNotify(FALSE)
{
#ifdef _ENABLE_BMP_FILE
	m_hMemDC = CreateCompatibleDC(NULL);
#endif
}

CBitWnd::~CBitWnd()
{
#ifdef _ENABLE_BMP_FILE
	if (m_pNormalBmpFile)
		delete m_pNormalBmpFile;

	if (m_pActiveBmpFile)
		delete m_pActiveBmpFile;

	if (m_pHoverBmpFile)
		delete m_pHoverBmpFile;

	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = 0;
	}
#else
	if( m_lpBit != NULL && m_bAllocFlag )
	{
		delete m_lpBit;
		m_lpBit = NULL;
	}
#endif
}

BEGIN_MESSAGE_MAP(CBitWnd, CWnd)
	//{{AFX_MSG_MAP(CBitWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

BOOL CBitWnd::Create(UINT wStyle, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR	lpWndName = NULL;
	lpWndName = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
									AfxGetApp()->LoadStandardCursor(IDC_ARROW),
									(HBRUSH)GetStockObject(BLACK_BRUSH), NULL);
	if( lpWndName == NULL )
		return FALSE;
	//
	// 2.0 Create the window directly...
	//wStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CWnd::Create(lpWndName, NULL, wStyle, CRect(0, 0, 0, 0), pParentWnd, nID);
}

#ifdef _ENABLE_BMP_FILE
void CBitWnd::ReLoadBitItem(char* szFileName, int cx, int cy,int iState)
{
	if (kNormalBit == iState)
	{
		if (m_pNormalBmpFile)
			delete m_pNormalBmpFile;

		m_iItemNums = 1;

		m_pNormalBmpFile = new ImageFile;
		m_pNormalBmpFile->Load(szFileName);
		this->MoveWindow(0, 0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());
	}
	else if (kHoverBit == iState)
	{
		if (m_pHoverBmpFile)
			delete m_pHoverBmpFile;

		m_iItemNums = 2;

		m_pHoverBmpFile = new ImageFile;
		m_pHoverBmpFile->Load(szFileName);
	}
	else if (kActiveBit == iState)
	{
		if (m_pActiveBmpFile)
			delete m_pActiveBmpFile;

		m_iItemNums = 3;

		m_pActiveBmpFile = new ImageFile;
		m_pActiveBmpFile->Load(szFileName);
	}
}
#else
void CBitWnd::ReLoadBitItem(UINT nResID, int cx, int cy)
{
	if( m_lpBit != NULL && m_bAllocFlag )
	{
		delete m_lpBit;
		m_lpBit = NULL;
		m_bAllocFlag = FALSE;
	}
	ASSERT( m_hWnd != NULL );
	m_lpBit = new CBitItem(nResID, cx, cy);
	m_bAllocFlag = TRUE;
	ASSERT( m_lpBit != NULL );
	this->MoveWindow(0, 0, m_lpBit->GetItemWidth(), m_lpBit->GetItemHeight());
}

void CBitWnd::SetBitItem(CBitItem * lpBit)
{
	ASSERT( m_lpBit == NULL );
	ASSERT( this->m_hWnd != NULL);
	m_lpBit = lpBit;
	this->MoveWindow(0, 0, m_lpBit->GetItemWidth(), m_lpBit->GetItemHeight());
}
#endif

BOOL CBitWnd::OnEraseBkgnd(CDC* pDC) 
{
#ifdef _ENABLE_BMP_FILE
	//if (kNormalBit == m_nState)
		//DrawBMP(m_pNormalBmpFile, 0, 0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());
	if (kHoverBit == m_nState)
		DrawBMP(m_pHoverBmpFile, 0, 0, m_pHoverBmpFile->GetWidth(), m_pHoverBmpFile->GetHeight());
	else if (kActiveBit == m_nState)
		DrawBMP(m_pActiveBmpFile, 0, 0, m_pActiveBmpFile->GetWidth(), m_pActiveBmpFile->GetHeight());
	else
		DrawBMP(m_pNormalBmpFile, 0, 0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());

	m_nOldState = m_nState;
#else
	(m_lpBit != NULL) ? m_lpBit->CopyItemIndex(pDC, m_nState, FALSE) : NULL;
#endif
	return TRUE;
}

BOOL CBitWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_hCursor != NULL )
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return FALSE;
}

LRESULT CBitWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_nState = m_bMute ? kActiveBit : kNormalBit;
#ifdef _ENABLE_BMP_FILE
	m_nState = (m_iItemNums > 1) ? m_nState : kNormalBit;
#else
	m_nState = (m_lpBit->GetItemNums() > 1) ? m_nState : kNormalBit;
#endif

	m_bTracking = FALSE;
	this->Invalidate();
	return 0;
}

LRESULT CBitWnd::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
#ifdef _ENABLE_BMP_FILE
	m_nState = (m_iItemNums > 1) ? kHoverBit : kNormalBit;
#else
	m_nState = (m_lpBit->GetItemNums() > 1) ? kHoverBit : kNormalBit;
#endif
	m_nState = m_bMute ? kActiveBit : m_nState;

	this->Invalidate();
	return 0;
}

void CBitWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	/*
	if (nFlags != MK_LBUTTON)
		SetFlipCursor(AfxGetApp()->LoadCursor(IDC_HAND_OPEN));
	else
		SetFlipCursor(AfxGetApp()->LoadCursor(IDC_HAND_CLOSE)); //Allen: no action ??
	*/

	if( !m_bTracking )
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	if( m_bNotify )
	{
		ASSERT( GetParent() != NULL );
		this->ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));
	}
}

void CBitWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//Allen: no action ??
#ifdef _ENABLE_BMP_FILE
	m_nState = (m_iItemNums > 2) ? kActiveBit : kNormalBit;
#else
	m_nState = (m_lpBit->GetItemNums() > 2) ? kActiveBit : kNormalBit;
#endif
	m_nState = m_bMute ? kNormalBit : m_nState;
	m_bMute	 = m_bCanMute ? !m_bMute : m_bMute;
	this->Invalidate();

	if( m_bLDownNotify )
	{
		ASSERT( GetParent() != NULL );
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	}
	
	if( m_bNotify )
	{
		ASSERT( GetParent() != NULL );
		this->ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
	}
}

void CBitWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !m_bMute )
	{
#ifdef _ENABLE_BMP_FILE
		m_nState = (m_iItemNums > 1) ? kHoverBit : kNormalBit;
#else
		m_nState = (m_lpBit->GetItemNums() > 1) ? kHoverBit : kNormalBit;
#endif
		this->Invalidate();
	}
	if( !m_bLDownNotify )
	{
		ASSERT( GetParent() != NULL );
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	}
	if( m_bNotify )
	{
		ASSERT( GetParent() != NULL );
		this->ClientToScreen(&point);
		GetParent()->ScreenToClient(&point);
		GetParent()->SendMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));
	}
}

void CBitWnd::Draw()
{
	if (kHoverBit == m_nState)
		DrawBMP(m_pHoverBmpFile, 0, 0, m_pHoverBmpFile->GetWidth(), m_pHoverBmpFile->GetHeight());
	else if (kActiveBit == m_nState)
		DrawBMP(m_pActiveBmpFile, 0, 0, m_pActiveBmpFile->GetWidth(), m_pActiveBmpFile->GetHeight());
	else
		DrawBMP(m_pNormalBmpFile, 0, 0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());
}

void CBitWnd::DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h, int iColorKeyR, int iColorKeyG, int iColorKeyB)
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

	if (iColorKeyR == -1 && iColorKeyG == -1 && iColorKeyB == -1)
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, pImgFile->GetHBITMAP());

		StretchBlt(hdc,
			x,
			y,
			w,
			h,
			m_hMemDC,
			0,
			0,
			pImgFile->GetWidth(),
			pImgFile->GetHeight(), SRCCOPY);

		if (hOldBitmap)
			SelectObject(m_hMemDC, hOldBitmap);
	}
	else
	{
		COLORREF ColorKey = RGB(iColorKeyR, iColorKeyG, iColorKeyB);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, pImgFile->GetHBITMAP());

		TransparentBlt(hdc,
			x,
			y,
			w,
			h,
			m_hMemDC,
			0,
			0,
			pImgFile->GetWidth(),
			pImgFile->GetHeight(),
			ColorKey);  //³z©ú


		if (hOldBitmap)
			SelectObject(m_hMemDC, hOldBitmap);
	}
	::ReleaseDC(this->GetSafeHwnd(), hdc);
}