// RoundSliderCtrl.cpp: Implementation file
//
// Copyright (c) 1999 Daniel Frey
//
// Partially based on CRoundButton, copyright (c) 1997,1998 by Chris Maunder.
//
// Distribute and use freely, except:
// a) Don't alter or remove this notice.
// b) Mark the changes you make.
//
// This file is provided "as is" with no expressed or implied warranty.
// May not be sold for profit. Use at your own risk. Expect bugs.
//
// History:
//
//  v1.2  13.09.1999  Added knob-only drag-activation (suggested by Kevin Shale)
//
//  v1.1  11.05.1999  Added/fixed mouse-wheel support
//                    Added volume-button-look (thanks to Noam Rathaus)
//                    Added color and font support (thanks to John Gann)
//                    Fixed text position in OnPaint()
//
//  v1.0  19.04.1999  Initial release

#include "stdafx.h"
#include "RoundSliderCtrl.h"

#include <math.h>

//#include "MemDC.h"
//#define USE_MEM_DC // Comment this out, if you don't want to use CMemDC

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const double pi = 3.141592653589793238462643383279;

// The following 3 functions were taken from 'CRoundButton.cpp', written and
// copyright (c) 1997,1998 by Chris Maunder.

// To be honest, I never had a look at their implementation, I just use them.
// This is cut-and-paste-programming at its best... if it works... :)   (Daniel)

// prototypes
COLORREF GetColour(double dAngle, COLORREF crBright, COLORREF crDark);
void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crColour, BOOL bDashed = FALSE);
void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crBright, COLORREF crDark);


// Calculate colour for a point at the given angle by performing a linear
// interpolation between the colours crBright and crDark based on the cosine
// of the angle between the light source and the point.
//
// Angles are measured from the +ve x-axis (i.e. (1,0) = 0 degrees, (0,1) = 90 degrees )
// But remember: +y points down!

COLORREF GetColour(double dAngle, COLORREF crBright, COLORREF crDark)
{
#define Rad2Deg	180.0/3.1415 
#define LIGHT_SOURCE_ANGLE	-2.356		// -2.356 radians = -135 degrees, i.e. From top left

	ASSERT(dAngle > -3.1416 && dAngle < 3.1416);
	double dAngleDifference = LIGHT_SOURCE_ANGLE - dAngle;

	if (dAngleDifference < -3.1415) dAngleDifference = 6.293 + dAngleDifference;
	else if (dAngleDifference > 3.1415) dAngleDifference = 6.293 - dAngleDifference;

	double Weight = 0.5*(cos(dAngleDifference)+1.0);

	BYTE Red   = (BYTE) (Weight*GetRValue(crBright) + (1.0-Weight)*GetRValue(crDark));
	BYTE Green = (BYTE) (Weight*GetGValue(crBright) + (1.0-Weight)*GetGValue(crDark));
	BYTE Blue  = (BYTE) (Weight*GetBValue(crBright) + (1.0-Weight)*GetBValue(crDark));

	//TRACE("LightAngle = %0.0f, Angle = %3.0f, Diff = %3.0f, Weight = %0.2f, RGB %3d,%3d,%3d\n", 
	//	  LIGHT_SOURCE_ANGLE*Rad2Deg, dAngle*Rad2Deg, dAngleDifference*Rad2Deg, Weight,Red,Green,Blue);

	return RGB(Red, Green, Blue);
}

void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crColour, BOOL bDashed)
{
	const int nDashLength = 1;
	LONG lError, lXoffset, lYoffset;
	int  nDash = 0;
	BOOL bDashOn = TRUE;

	//Check to see that the coordinates are valid
	ASSERT( (p.x + lRadius <= LONG_MAX) && (p.y + lRadius <= LONG_MAX) );
	ASSERT( (p.x - lRadius >= LONG_MIN) && (p.y - lRadius >= LONG_MIN) );

	//Set starting values
	lXoffset = lRadius;
	lYoffset = 0;
	lError   = -lRadius;

	do {
		if (bDashOn) {
			pDC->SetPixelV(p.x + lXoffset, p.y + lYoffset, crColour);
			pDC->SetPixelV(p.x + lXoffset, p.y - lYoffset, crColour);
			pDC->SetPixelV(p.x + lYoffset, p.y + lXoffset, crColour);
			pDC->SetPixelV(p.x + lYoffset, p.y - lXoffset, crColour);
			pDC->SetPixelV(p.x - lYoffset, p.y + lXoffset, crColour);
			pDC->SetPixelV(p.x - lYoffset, p.y - lXoffset, crColour);
			pDC->SetPixelV(p.x - lXoffset, p.y + lYoffset, crColour);
			pDC->SetPixelV(p.x - lXoffset, p.y - lYoffset, crColour);
		}

		//Advance the error term and the constant X axis step
		lError += lYoffset++;

		//Check to see if error term has overflowed
		if ((lError += lYoffset) >= 0)
			lError -= --lXoffset * 2;

		if (bDashed && (++nDash == nDashLength)) {
			nDash = 0;
			bDashOn = !bDashOn;
		}

	} while (lYoffset <= lXoffset);	//Continue until halfway point
} 

void DrawCircle(CDC* pDC, CPoint p, LONG lRadius, COLORREF crBright, COLORREF crDark)
{
	LONG lError, lXoffset, lYoffset;

	//Check to see that the coordinates are valid
	ASSERT( (p.x + lRadius <= LONG_MAX) && (p.y + lRadius <= LONG_MAX) );
	ASSERT( (p.x - lRadius >= LONG_MIN) && (p.y - lRadius >= LONG_MIN) );

	//Set starting values
	lXoffset = lRadius;
	lYoffset = 0;
	lError   = -lRadius;

	do {
		const double Pi = 3.141592654, 
					 Pi_on_2 = Pi * 0.5,
					 Three_Pi_on_2 = Pi * 1.5;
		COLORREF crColour;
		double   dAngle = atan2(lYoffset, lXoffset);

		//Draw the current pixel, reflected across all eight arcs
		crColour = GetColour(dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lXoffset, p.y + lYoffset, crColour);

		crColour = GetColour(Pi_on_2 - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lYoffset, p.y + lXoffset, crColour);

		crColour = GetColour(Pi_on_2 + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lYoffset, p.y + lXoffset, crColour);

		crColour = GetColour(Pi - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lXoffset, p.y + lYoffset, crColour);

		crColour = GetColour(-Pi + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lXoffset, p.y - lYoffset, crColour);

		crColour = GetColour(-Pi_on_2 - dAngle, crBright, crDark);
		pDC->SetPixelV(p.x - lYoffset, p.y - lXoffset, crColour);

		crColour = GetColour(-Pi_on_2 + dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lYoffset, p.y - lXoffset, crColour);

		crColour = GetColour(-dAngle, crBright, crDark);
		pDC->SetPixelV(p.x + lXoffset, p.y - lYoffset, crColour);

		//Advance the error term and the constant X axis step
		lError += lYoffset++;

		//Check to see if error term has overflowed
		if ((lError += lYoffset) >= 0)
			lError -= --lXoffset * 2;

	} while (lYoffset <= lXoffset);	//Continue until halfway point
} 

/////////////////////////////////////////////////////////////////////////////
// CRoundSliderCtrl

IMPLEMENT_DYNAMIC(CRoundSliderCtrl, CSliderCtrl)

CRoundSliderCtrl::CRoundSliderCtrl()
{
	m_iOldPos = 0;//Allen
	m_strText = "%ld";
	m_nKnobRadius = 7;
	m_nZero = 0;
	m_bInverted = false;
	m_bDragging = false;
	m_bDrawRadioButton = false;
	m_bDragByKnobOnly = false;

	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lf), &m_lf);
	m_font.CreateFontIndirect(&m_lf);
	m_crText = RGB(255,255,255);// ::GetSysColor(COLOR_WINDOWTEXT);

	//m_hKnobBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	//m_hDialBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));

	m_hKnobBrush = ::CreateSolidBrush(RGB(226,174,28));
	m_hDialBrush = ::CreateSolidBrush(RGB(255, 255, 255));

	m_hMemDC = CreateCompatibleDC(NULL);
}

CRoundSliderCtrl::~CRoundSliderCtrl()
{
	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = 0;
	}
}

BEGIN_MESSAGE_MAP(CRoundSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CRoundSliderCtrl)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRoundSliderCtrl::SetBGFileName(char* szFileName)
{
	strcpy(m_szBGFileName, szFileName);
}

void CRoundSliderCtrl::Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CSliderCtrl::Create(WS_CHILD | WS_VISIBLE, rect, pParent, ulID);

	m_ImageFile.Load(m_szBGFileName);

	SetRange(0, 359, FALSE);
	SetLineSize(1);
	SetPageSize(10);

	Init();
}

void CRoundSliderCtrl::PreSubclassWindow() 
{
	CSliderCtrl::PreSubclassWindow();

	SetRange(0, 359, FALSE);
	SetLineSize(1);
	SetPageSize(10);

	Init();
}

BOOL CRoundSliderCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CSliderCtrl::PreCreateWindow(cs)) return FALSE;
	/*
	SetRange(0, 359, FALSE);
	SetLineSize(1);
	SetPageSize(10);

	Init();
	*/
	return TRUE;
}

void CRoundSliderCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CSliderCtrl::OnSize(nType, cx, cy);
	
	Init();
}

void CRoundSliderCtrl::Init()
{
	CRect rc;
	GetClientRect(rc);

	// Resize the window to make it square
	rc.bottom = rc.right = min(rc.bottom, rc.right);

	// Get the vital statistics of the window
	m_ptCenter = rc.CenterPoint();
	//m_nRadius = rc.bottom/2-(m_nKnobRadius+1);
	m_nRadius = rc.bottom / 2 - (m_nKnobRadius + 15);

	// Set the window region so mouse clicks only activate the round section 
	// of the slider
//	m_rgn.DeleteObject(); 
//	SetWindowRgn(NULL, FALSE);
//	m_rgn.CreateEllipticRgnIndirect(rc);
//	SetWindowRgn(m_rgn, TRUE);

//	VerifyPos();
}

#pragma warning(disable:4100) // Unreferenced formal parameter
BOOL CRoundSliderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
#pragma warning(default:4100)

void CRoundSliderCtrl::OnPaint() 
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax() + 1;

	const bool bDisabled = !IsWindowEnabled();

	CPaintDC dc(this); // device context for painting

#ifdef USE_MEM_DC
	CMemDC pDC(&dc);
#else
	CDC* pDC = &dc;
#endif

	int nRadius = m_nRadius;

	int nPos = (((GetPos() - nMin) * 360 / (nMax - nMin)) + m_nZero + 360) % 360;
	if (m_bInverted) nPos = 360 - nPos;

	const double dPos = ((double)(nPos))*pi / 180.0;
	CPoint ptKnobCenter;

	DrawBMP(&m_ImageFile, 0, 0, 109, 109, 255, 255, 255);

#pragma warning(disable:4244) // Disable warning "Converting 'double' to 'int', possible loss of data"
	if (m_bDrawRadioButton)
	{
		ptKnobCenter = CPoint(m_ptCenter.x + (nRadius - m_nKnobRadius) * sin(dPos), m_ptCenter.y - (nRadius - m_nKnobRadius) * cos(dPos));
	}
	else
	{
		ptKnobCenter = CPoint(m_ptCenter.x + (nRadius)* sin(dPos), m_ptCenter.y - (nRadius)* cos(dPos));
	}
#pragma warning(default:4244)

	m_ptKnobCenter = ptKnobCenter;

	if (m_bDrawRadioButton)
	{
		nRadius += 1;// 2;
	}
	else
	{
		nRadius -= 1;// 2;
	}
	

	// Draw the knob
	int nKnobRadius = m_nKnobRadius;

	if (m_bDrawRadioButton)
	{
		nKnobRadius *= 4;
		nKnobRadius /= 5;
	}

	const CRect rcKnob(ptKnobCenter.x - nKnobRadius + 2, ptKnobCenter.y - nKnobRadius + 2, ptKnobCenter.x + nKnobRadius, ptKnobCenter.y + nKnobRadius);

	CRgn rgnKnob;
	rgnKnob.CreateEllipticRgnIndirect(rcKnob);
	if (bDisabled)
	{
		pDC->FillRgn(&rgnKnob, CBrush::FromHandle(::GetSysColorBrush(COLOR_BTNFACE)));
	}
	else
	{
		pDC->FillRgn(&rgnKnob, CBrush::FromHandle(m_hKnobBrush));
	}
	rgnKnob.DeleteObject();

	if (m_bDrawRadioButton || m_bDragging)
	{
		DrawCircle(pDC, ptKnobCenter, --nKnobRadius, RGB(204, 204, 204), RGB(255, 255, 255));
//		DrawCircle(pDC, ptKnobCenter, --nKnobRadius, RGB(255, 255, 255), RGB(255, 255, 255));
	}
	else
	{
		DrawCircle(pDC, ptKnobCenter, --nKnobRadius, RGB(204, 204, 204), RGB(204, 204, 204));
//		DrawCircle(pDC, ptKnobCenter, --nKnobRadius, RGB(255, 255, 255), RGB(255, 255, 255));
	}

	// Draw the focus circle on the inside of the knob
//	if (!m_bDrawRadioButton && (GetFocus() == this))
//	{
//		DrawCircle(pDC, ptKnobCenter, nKnobRadius - 2, RGB(0, 0, 0), TRUE);
//	}

	// Draw the text
	const CString strFormattedText = OnFormatText();

	if (!strFormattedText.IsEmpty())
	{
		CFont* pOldFont = pDC->SelectObject(&m_font);

		const CSize szExtent = pDC->GetTextExtent(strFormattedText);
		const CPoint ptText = CPoint(m_ptCenter.x - szExtent.cx / 2, m_ptCenter.y - szExtent.cy / 2);
		const int nOldTextColor = pDC->SetTextColor(m_crText);

		pDC->SetBkMode(TRANSPARENT);
		if (bDisabled)
		{
			pDC->DrawState(ptText, szExtent, strFormattedText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}
		else
		{
			pDC->TextOut(ptText.x, ptText.y, strFormattedText);
		}

		// Clean up
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(nOldTextColor);
	}
}

void CRoundSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bDragByKnobOnly)
	{
		CPoint pt = m_ptKnobCenter - point;
		if((pt.x*pt.x + pt.y*pt.y) > m_nKnobRadius*m_nKnobRadius) return; // Ignore it...
	}

	if(!m_bDragging)
	{
		m_bDragging = true;
		m_bDragChanged = false;
		SetCapture();
		SetFocus();
		if(SetKnob(point))
		{
			m_bDragChanged = true;
			PostMessageToParent(TB_THUMBTRACK);
		}
		RedrawWindow();
	}
	else
	{
		CSliderCtrl::OnLButtonDown(nFlags, point);
	}
}

void CRoundSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		if(SetKnob(point))
		{
			m_bDragChanged = true;
			PostMessageToParent(TB_THUMBTRACK);
			RedrawWindow();
		}
	}
	else
	{
		CSliderCtrl::OnMouseMove(nFlags, point);
	}
}

void CRoundSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		m_bDragging = false;
		::ReleaseCapture();
		if(SetKnob(point))
		{
			PostMessageToParent(TB_THUMBTRACK);
			m_bDragChanged = true;
		}
		if(m_bDragChanged)
		{
			PostMessageToParent(TB_THUMBPOSITION);
			m_bDragChanged = false;
		}
		RedrawWindow();
	}
	else
	{
		CSliderCtrl::OnLButtonUp(nFlags, point);
	}
}

bool CRoundSliderCtrl::SetKnob(const CPoint& pt)
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;

	CSize szDelta = pt - m_ptCenter;
	if(m_bInverted) szDelta.cx *= -1;

	double dNewPos = 0.0;

	if(szDelta.cx != 0)
	{
		dNewPos = 90.0 - atan(-(double)szDelta.cy / (double)szDelta.cx) * 180.0 / pi;
	}

	if(((szDelta.cx == 0) && (szDelta.cy >= 0)) || (szDelta.cx < 0))
	{
		dNewPos += 180.0;
	}

	dNewPos -= m_nZero;

	while(dNewPos < 0.0) 
		dNewPos += 360.0;
	while(dNewPos >= 360.0) 
		dNewPos -= 360.0;

	const int nNewPos = nMin + (int)(dNewPos*(nMax-nMin)/360.0);
	const bool bChanged = (nNewPos != GetPos());

	if(bChanged)
	{
		if(nNewPos <= 130 || nNewPos >= 230)
			SetPos(nNewPos);
	}

	return bChanged;
}

void CRoundSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;

	switch(nChar)
	{
	case VK_LEFT:
	case VK_UP:
		{
			int nNewPos = GetPos()-GetLineSize();
			while(nNewPos < nMin) nNewPos += (nMax - nMin);
			SetPos(nNewPos);
			RedrawWindow();
			PostMessageToParent(TB_LINEUP);
		}
		break;
	
	case VK_RIGHT:
	case VK_DOWN:
		{
			int nNewPos = GetPos()+GetLineSize();
			while(nNewPos >= nMax) nNewPos -= (nMax - nMin);
			SetPos(nNewPos);
			RedrawWindow();
			PostMessageToParent(TB_LINEDOWN);
		}
		break;

	case VK_PRIOR:
		{
			int nNewPos = GetPos()-GetPageSize();
			while(nNewPos < nMin) nNewPos += (nMax - nMin);
			SetPos(nNewPos);
			RedrawWindow();
			PostMessageToParent(TB_PAGEUP);
		}
		break;

	case VK_NEXT:
		{
			int nNewPos = GetPos()+GetPageSize();
			while(nNewPos >= nMax) nNewPos -= (nMax - nMin);
			SetPos(nNewPos);
			RedrawWindow();
			PostMessageToParent(TB_PAGEDOWN);
		}
		break;

	case VK_HOME:
	case VK_END:
		// Do nothing (ignore keystroke)
		break;

	default:
		CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CRoundSliderCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		PostMessageToParent(TB_ENDTRACK);
		break;

	case VK_HOME:
	case VK_END:
		// Do nothing
		break;

	default:
		CSliderCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

void CRoundSliderCtrl::PostMessageToParent(const int nTBCode) const
{
	CWnd* pWnd = GetParent();
	if(pWnd) pWnd->PostMessage(WM_HSCROLL, (WPARAM)((GetPos() << 16) | nTBCode), (LPARAM)GetSafeHwnd());
}

BOOL CRoundSliderCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL bResult = TRUE;

	ScreenToClient(&pt);

	if(ChildWindowFromPoint(pt) == this)
	{
		bResult = CSliderCtrl::OnMouseWheel(nFlags, zDelta, pt);
		RedrawWindow();
	}

	return bResult;
}

void CRoundSliderCtrl::SetText(const CString& strNewText)
{
	m_strText = strNewText;
}

CString CRoundSliderCtrl::GetText() const
{
	return m_strText;
}

void CRoundSliderCtrl::SetKnobRadius(const int nNewKnobRadius)
{
	ASSERT(nNewKnobRadius > 4);

	m_nKnobRadius = nNewKnobRadius;
}

int CRoundSliderCtrl::GetKnobRadius() const
{
	return m_nKnobRadius;
}

void CRoundSliderCtrl::SetDragByKnobOnly(const bool bDragByKnobOnly)
{
	m_bDragByKnobOnly = bDragByKnobOnly;
}

bool CRoundSliderCtrl::GetDragByKnobOnly() const
{
	return m_bDragByKnobOnly;
}

void CRoundSliderCtrl::SetZero(const int nZero)
{
	ASSERT(nZero >= 0 && nZero < 360);

	m_nZero = nZero;
}

int CRoundSliderCtrl::GetZero() const
{
	return m_nZero;
}

void CRoundSliderCtrl::SetInverted(const bool bNewInverted)
{
	m_bInverted = bNewInverted;
}

bool CRoundSliderCtrl::GetInverted() const
{
	return m_bInverted;
}

void CRoundSliderCtrl::SetRadioButtonStyle(const bool bNewRadioButtonStyle)
{
	m_bDrawRadioButton = bNewRadioButtonStyle;
}

bool CRoundSliderCtrl::GetRadioButtonStyle() const
{
	return m_bDrawRadioButton;
}

void CRoundSliderCtrl::ReconstructFont()
{
	m_font.DeleteObject();

	const BOOL bCreated = m_font.CreateFontIndirect(&m_lf);
	ASSERT(bCreated);
}

void CRoundSliderCtrl::SetDialColor(const COLORREF crBkgnd)
{
	if(m_hDialBrush) ::DeleteObject(m_hDialBrush);

	m_hDialBrush = ::CreateSolidBrush(crBkgnd);
}

void CRoundSliderCtrl::SetKnobColor(const COLORREF crBkgnd)
{
	if(m_hKnobBrush) ::DeleteObject(m_hKnobBrush);

	m_hKnobBrush = ::CreateSolidBrush(crBkgnd);
}

void CRoundSliderCtrl::SetTextColor(const COLORREF crText)
{
	m_crText = crText;
}

void CRoundSliderCtrl::SetFontBold(const bool bSet)
{
	m_lf.lfWeight = bSet;
	ReconstructFont();
}

void CRoundSliderCtrl::SetFontUnderline(const bool bSet)
{
	m_lf.lfUnderline = bSet;
	ReconstructFont();
}

void CRoundSliderCtrl::SetFontItalic(const bool bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
}

void CRoundSliderCtrl::SetFontSize(const int nSize)
{
	m_lf.lfHeight = -nSize;
	ReconstructFont();
}
/*
void CRoundSliderCtrl::SetFontName(const CString& strFont)
{
	strcpy(m_lf.lfFaceName, strFont);
	ReconstructFont();
}
*/
// Overrideables
CString CRoundSliderCtrl::OnFormatText()// const
{
	CString strFormattedText;

	int iRealPos = 0;
	int iPos = GetPos();
	if (iPos >= 230)
		iRealPos = (iPos - 230)/ 2.6;
	else
		iRealPos = iPos / 2.6 + 50;

	m_iRealPos = iRealPos;
	strFormattedText.Format(GetText(), iRealPos);

	if (m_iOldPos != m_iRealPos)//Allen add
	{
		m_iOldPos = m_iRealPos;
		GetParent()->PostMessage(WM_MIXER_VOL_NOTIFY, m_iRealPos, iPos);
	}
	return strFormattedText;
}

void CRoundSliderCtrl::DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h, int iColorKeyR, int iColorKeyG, int iColorKeyB)
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

#ifdef _DEBUG
void CRoundSliderCtrl::AssertValid() const
{
	CSliderCtrl::AssertValid();

	ASSERT(m_nZero >= 0 && m_nZero < 360);
	ASSERT(m_nKnobRadius > 4);
}

void CRoundSliderCtrl::Dump(CDumpContext& dc) const
{
	CSliderCtrl::Dump(dc);

	dc	<< "m_strText = '" << m_strText << "'\n"
		<< "m_nKnobRadius = " << m_nKnobRadius << "\n"
		<< "m_nZero = " << m_nZero << "\n"
		<< "m_bInverted = " << (m_bInverted ? "true" : "false") << "\n"
		<< "m_bDrawRadioButton = " << (m_bDrawRadioButton ? "true" : "false");

	// Some information is still missing. Add whatever you need :)
}
#endif // _DEBUG
