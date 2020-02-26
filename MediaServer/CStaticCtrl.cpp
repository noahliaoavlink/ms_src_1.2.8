#include "stdafx.h"
#include "CStaticCtrl.h"

IMPLEMENT_DYNAMIC(CStaticCtrl, CStatic)

CStaticCtrl::CStaticCtrl()
{
	textBGColor = TRANSPARENT;
	m_BGColor = RGB(50, 50, 50);//BLACK;
	m_FGColor = RGB(255, 255, 255); //WHITE;
}

CStaticCtrl::~CStaticCtrl()
{
}

BEGIN_MESSAGE_MAP(CStaticCtrl, CStatic)
	//{{AFX_MSG_MAP(CStaticCtrl)
	//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/*
void CStaticCtrl::Create(wchar_t* wszName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	CStatic::Create(wszName, WS_CHILD | WS_VISIBLE,CRect(x, y, x + w, y+h), pParent);
}
*/

void CStaticCtrl::Create(char* szName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	CStatic::Create(szName, WS_CHILD | WS_VISIBLE, CRect(x, y, x + w, y + h), pParent);
}

HBRUSH CStaticCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
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
