
#include "stdafx.h"
//#include "BitItem.h"
#include "BitSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBitSlider::CBitSlider(int MxPrwid/*Allen add*/)
  : m_bAllocFlag(FALSE),
#ifdef _ENABLE_BMP_FILE
	m_pNormalBmpFile(NULL),
	m_pActiveBmpFile(NULL),
#else
	m_lpActive(NULL),
	m_lpNormal(NULL),
#endif
	m_hHand(NULL),
	m_nOffset(0)
{
#ifdef _ENABLE_BMP_FILE
	m_hMemDC = CreateCompatibleDC(NULL);
#endif
	m_iMxPrwid = MxPrwid;//Allen add
}

CBitSlider::~CBitSlider()
{
	this->DestroyBackItem();

#ifdef _ENABLE_BMP_FILE
	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = 0;
	}
#endif
}

BEGIN_MESSAGE_MAP(CBitSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CBitSlider)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBitSlider::Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CSliderCtrl::Create(WS_CHILD | WS_VISIBLE, rect, pParent, ulID);
}

void CBitSlider::CreateV(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CSliderCtrl::Create(WS_CHILD | WS_VISIBLE | TBS_VERT, rect, pParent, ulID);
}

BOOL CBitSlider::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CBitSlider::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	return TRUE;
}

void CBitSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSliderCtrl::OnLButtonDown(nFlags, point);

	GetParent()->PostMessage(WM_MIXER_SEEK, 101, m_iMxPrwid);//Allen add
}

void CBitSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	this->Invalidate();
	this->UpdateToolTips();
	CSliderCtrl::OnLButtonUp(nFlags, point);

	GetParent()->PostMessage(WM_MIXER_SEEK, GetPosV(), m_iMxPrwid);//Allen add
}

void CBitSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDC * pDC = this->GetDC();
	this->DrawSlider(pDC, FALSE);
	this->ReleaseDC(pDC);

	this->UpdateToolTips();

	CSliderCtrl::OnMouseMove(nFlags, point);

	//if (nFlags == MK_LBUTTON) //Allen add
		//GetParent()->PostMessage(WM_MIXER_SEEK, GetPosV(), m_iMxPrwid);
}

BOOL CBitSlider::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_hHand != NULL )
	{
		::SetCursor(m_hHand);
		return TRUE;
	}
	return FALSE;
}

void CBitSlider::OnPaint() 
{
	CPaintDC	dc(this);
	this->DrawSlider(&dc, TRUE);
}

void CBitSlider::DrawSlider(CDC * pDC, BOOL bPaint)
{
#ifdef _ENABLE_BMP_FILE
	CRect		rcRect;
	CRect		rcThumb;

	this->GetClientRect(rcRect);
	this->GetNewThumbRect(rcThumb);
	if ((GetStyle() & TBS_VERT) == TBS_VERT)
	{
		DrawBMP(m_pNormalBmpFile, 0, 0, rcRect.Width(), rcRect.Height(),0,0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());
		DrawBMP(m_pActiveBmpFile, 0, rcThumb.top, rcRect.Width(), rcRect.Height(),0, rcThumb.top, rcRect.Width(), rcRect.Height());
	}
	else
	{
		DrawBMP(m_pNormalBmpFile, 0, 0, rcRect.Width(), rcRect.Height(),0,0, m_pNormalBmpFile->GetWidth(), m_pNormalBmpFile->GetHeight());
		DrawBMP(m_pActiveBmpFile, 0, 0, bPaint ? rcThumb.right : rcThumb.left, rcRect.Height(),0,0, bPaint ? rcThumb.right : rcThumb.left, rcRect.Height());
	}

	m_Thumb.Draw();

	bPaint ? m_Thumb.MoveWindow(rcThumb) : NULL;
#else
	if( m_Thumb.m_hWnd != NULL && m_lpNormal != NULL && m_lpActive != NULL )
	{
		CRect		rcRect;
		CRect		rcThumb;

		this->GetClientRect(rcRect);
		this->GetNewThumbRect(rcThumb);
		if( (GetStyle() & TBS_VERT) == TBS_VERT )
		{
			m_lpActive->CopyHoleDC(pDC, 0, 0, rcRect.Width(), bPaint ? rcThumb.bottom : rcThumb.top);
			m_lpNormal->CopyHoleDC(pDC, 0, rcThumb.bottom, rcRect.Width(), rcRect.bottom - rcThumb.bottom, 0, rcThumb.bottom);
		}
		else
		{
			m_lpActive->CopyHoleDC(pDC, 0, 0, bPaint ? rcThumb.right : rcThumb.left, rcRect.Height());
			m_lpNormal->CopyHoleDC(pDC, rcThumb.right, 0, rcRect.right - rcThumb.right, rcRect.Height(), rcThumb.right, 0);
		}
		bPaint ? m_Thumb.MoveWindow(rcThumb) : NULL;
	}
#endif
}

void CBitSlider::DestroyBackItem()
{
#ifdef _ENABLE_BMP_FILE
	if (m_pNormalBmpFile)
	{
		delete m_pNormalBmpFile;
		m_pNormalBmpFile = NULL;
	}

	if (m_pActiveBmpFile) 
	{
		delete m_pActiveBmpFile;
		m_pActiveBmpFile = NULL;
	}
#else
	if( m_lpActive != NULL && m_bAllocFlag )
	{
		delete m_lpActive;
		m_lpActive = NULL;
	}
	if( m_lpNormal != NULL && m_bAllocFlag )
	{
		delete m_lpNormal;
		m_lpNormal = NULL;
	}
#endif
	m_bAllocFlag = FALSE;
}

#ifdef _ENABLE_BMP_FILE
void CBitSlider::BuildBackItem(char* szNormalFileName, char* szActiveFileName)
{
	this->DestroyBackItem();

	m_pNormalBmpFile = new ImageFile;
	m_pNormalBmpFile->Load(szNormalFileName);

	m_pActiveBmpFile = new ImageFile;
	m_pActiveBmpFile->Load(szActiveFileName);

	m_bAllocFlag = TRUE;

	this->SetWindowPos(NULL, 0, 0, m_pNormalBmpFile->GetWidth(),
		m_pNormalBmpFile->GetHeight(), SWP_NOMOVE);
}
#else
void CBitSlider::BuildBackItem(UINT nNormalRes, UINT nActiveRes)
{
	this->DestroyBackItem();
	ASSERT( m_lpActive == NULL );
	ASSERT( m_lpNormal == NULL );

	m_lpNormal  = new CBitItem(nNormalRes, 0, 0);
	m_lpActive  = new CBitItem(nActiveRes, 0, 0);
	ASSERT( m_lpActive != NULL );
	ASSERT( m_lpNormal != NULL );

	m_bAllocFlag = TRUE;

	this->SetWindowPos( NULL, 0, 0, m_lpActive->GetImageWidth(),
						m_lpActive->GetImageHeight(), SWP_NOMOVE );
}

void CBitSlider::BuildBackItem(CBitItem * lpNBit, CBitItem * lpABit)
{
	if( this->m_hWnd == NULL || lpNBit == NULL || lpABit == NULL )
		return;

	m_lpNormal	= lpNBit;
	m_lpActive	= lpABit;
	
	this->SetWindowPos( NULL, 0, 0, m_lpActive->GetItemWidth(),
						m_lpActive->GetItemHeight(), SWP_NOMOVE );
}
#endif

void CBitSlider::UpdateToolTips()
{
	CString	  strTips;
	strTips.Format("%d%%", GetPosV());

	m_ctlTips.UpdateTipText(strTips, this);
	m_ctlTips.UpdateTipText(strTips, &m_Thumb);
}

#ifdef _ENABLE_BMP_FILE
void CBitSlider::BuildThumbItem(char* szFileName, int cx, int cy,int iState)
{
	if (iState == 0) //kNormalBit
	{
		m_Thumb.Create(WS_CHILD | WS_VISIBLE, this, 0);
		m_Thumb.ReLoadBitItem(szFileName, cx, cy, iState);
		m_Thumb.SetNotifyParent(TRUE);

		m_ctlTips.Create(this);
		m_ctlTips.AddTool(this);
		m_ctlTips.AddTool(&m_Thumb);
		m_ctlTips.SetDelayTime(100);

		this->UpdateToolTips();
	}
	else
	{
		m_Thumb.ReLoadBitItem(szFileName, cx, cy, iState);
	}
}
#else

void CBitSlider::BuildThumbItem(UINT nThumbRes, int cx, int cy)
{
	if( m_Thumb.m_hWnd != NULL || nThumbRes <= 0 )
		return;
	m_Thumb.Create(WS_CHILD | WS_VISIBLE, this, 0);
	ASSERT( m_Thumb.m_hWnd != NULL );
	m_Thumb.ReLoadBitItem(nThumbRes, cx, cy);
	m_Thumb.SetNotifyParent(TRUE);

	m_ctlTips.Create(this);
	m_ctlTips.AddTool(this);
	m_ctlTips.AddTool(&m_Thumb);
	m_ctlTips.SetDelayTime(100);

	this->UpdateToolTips();
}

void CBitSlider::BuildThumbItem(CBitItem * lpBit)
{
	if( lpBit == NULL || m_Thumb.m_hWnd != NULL )
		return;
	m_Thumb.Create(WS_CHILD | WS_VISIBLE, this, 0);
	ASSERT( m_Thumb.m_hWnd != NULL );
	m_Thumb.SetBitItem(lpBit);
	m_Thumb.SetNotifyParent(TRUE);

	m_ctlTips.Create(this);
	m_ctlTips.AddTool(this);
	m_ctlTips.AddTool(&m_Thumb);
	m_ctlTips.SetDelayTime(100);

	this->UpdateToolTips();
}
#endif

void CBitSlider::GetNewThumbRect(CRect & rcThumb)
{
	CRect	rcRect, rcOld;
	m_Thumb.GetClientRect(rcRect);
	this->GetThumbRect(&rcOld);

	if((GetStyle() & TBS_VERT) == TBS_VERT)		// Vertical slider...
	{
		rcThumb.left	= m_nOffset;
		rcThumb.right	= rcThumb.left + rcRect.Width();
		rcThumb.top		= rcOld.top + (rcRect.Height() - rcOld.Height()) / 2;
		rcThumb.bottom	= rcThumb.top + rcRect.Height();
	}
	else										// Horizon slider...
	{
		rcThumb.top		= m_nOffset;
		rcThumb.bottom	= rcThumb.top + rcRect.Height();
		rcThumb.left	= rcOld.left + (rcRect.Width() - rcOld.Width()) / 2;
		rcThumb.right	= rcThumb.left + rcRect.Width();
	}
}

BOOL CBitSlider::PreTranslateMessage(MSG* pMsg) 
{
	if(  m_ctlTips.m_hWnd != NULL  )
		m_ctlTips.RelayEvent(pMsg);
	
	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void CBitSlider::SetPosV(int iPos)
{
	if ((GetStyle() & TBS_VERT) == TBS_VERT)
	{
		int iNewPos = this->GetRangeMax() - iPos;
		SetPos(iNewPos);
	}
	else
		SetPos(iPos);
}

int CBitSlider::GetPosV()
{
	if ((GetStyle() & TBS_VERT) == TBS_VERT)
		return this->GetRangeMax() - this->GetPos();
	else
		return GetPos();
}

void CBitSlider::DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h,int iSrcX,int iSrcY,int iSrcW,int iSrcH)
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

	//if (iColorKeyR == -1 && iColorKeyG == -1 && iColorKeyB == -1)
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);

		HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, pImgFile->GetHBITMAP());

		StretchBlt(hdc,
			x,
			y,
			w,
			h,
			m_hMemDC,
			iSrcX,
			iSrcY,
			iSrcW,
			iSrcH, SRCCOPY);

		if (hOldBitmap)
			SelectObject(m_hMemDC, hOldBitmap);
	}
	/*
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
	*/
	::ReleaseDC(this->GetSafeHwnd(), hdc);
}













