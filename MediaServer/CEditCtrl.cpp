#include "stdafx.h"
#include "CEditCtrl.h"

IMPLEMENT_DYNAMIC(CEditCtrl, CEdit)

CEditCtrl::CEditCtrl()
{
	textBGColor = TRANSPARENT;
	m_BGColor = RGB(50, 50, 50);//BLACK;
	m_FGColor = RGB(255, 255, 255);
}

CEditCtrl::~CEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CIPCtrl)
	//ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditCtrl::Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CEdit::Create(WS_CHILD | WS_VISIBLE , rect, pParent, ulID);
}

HBRUSH CEditCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
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

void CEditCtrl::OnEnChange()
{
	HWND hWnd = ::GetParent(this->GetSafeHwnd());
	::PostMessage(hWnd, WM_UPDATE_NETWORK_SETTING, 0, 0);
}