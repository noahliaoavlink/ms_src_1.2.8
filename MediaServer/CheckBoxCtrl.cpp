#include "stdafx.h"
#include "CheckBoxCtrl.h"
#include <iostream>
#include <string>

IMPLEMENT_DYNAMIC(CheckBoxCtrl, CWnd)

CheckBoxCtrl::CheckBoxCtrl()
{
	textBGColor = TRANSPARENT;
	m_BGColor = RGB(50,50,50);//BLACK;
	m_FGColor = RGB(255, 255, 255); //WHITE;
}

CheckBoxCtrl::~CheckBoxCtrl()
{
}

BEGIN_MESSAGE_MAP(CheckBoxCtrl, CWnd)
	//{{AFX_MSG_MAP(CheckBoxCtrl)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
void CheckBoxCtrl::Create(wchar_t* wszName,int x,int y,int w,int h,CWnd* pParent,unsigned long ulID)
{
	RECT rect = {x,y,x + w,y + h};
	BOOL bRet = CButton::Create(wszName, WS_CHILD | WS_VISIBLE|BS_AUTOCHECKBOX, rect, pParent, ulID);
}
*/

void CheckBoxCtrl::Create(char* szName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	RECT rect = { x,y,x + w,y + h };
	BOOL bRet = CButton::Create(szName, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, pParent, ulID);
}

void CheckBoxCtrl::Enable(bool bEnable)
{
	EnableWindow(bEnable);
}

void CheckBoxCtrl::OnClicked()
{
	bool bCheck = CButton::GetCheck();

	HWND hWnd = ::GetParent(this->GetSafeHwnd());
	::PostMessage(hWnd, WM_UPDATE_NETWORK_SETTING,0,0);
}

HBRUSH CheckBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
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

