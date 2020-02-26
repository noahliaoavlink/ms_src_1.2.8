#include "stdafx.h"
#include "CIPCtrl.h"

IMPLEMENT_DYNAMIC(CIPCtrl, CIPAddressCtrl)

CIPCtrl::CIPCtrl()
{
	textBGColor = TRANSPARENT;
	m_BGColor = RGB(50, 50, 50);//BLACK;
	m_FGColor = RGB(255, 255, 255);

	m_bEnableUpdate = true;
}

CIPCtrl::~CIPCtrl()
{
}

BEGIN_MESSAGE_MAP(CIPCtrl, CIPAddressCtrl)
	//{{AFX_MSG_MAP(CIPCtrl)
	//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CIPCtrl::Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID)
{
	CIPAddressCtrl::Create(WS_CHILD | WS_VISIBLE , CRect(x, y, x + w, y + h), pParent, ulID);
}

HBRUSH CIPCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CIPCtrl::SetIP(char* szIP)
{
	int iIP[4];
	m_bEnableUpdate = false;
	sscanf(szIP, "%d.%d.%d.%d", &iIP[0], &iIP[1], &iIP[2], &iIP[3]);
	SetAddress(iIP[0], iIP[1], iIP[2], iIP[3]);
}

char* CIPCtrl::GetIP()
{
	BYTE ucIP[4];
	GetAddress(ucIP[0], ucIP[1], ucIP[2], ucIP[3]);
	sprintf(m_szIP,"%d.%d.%d.%d", ucIP[0], ucIP[1], ucIP[2], ucIP[3]);
	return m_szIP;
}

void CIPCtrl::OnEnChange()
{
	if (m_bEnableUpdate)
	{
		HWND hWnd = ::GetParent(this->GetSafeHwnd());
		::PostMessage(hWnd, WM_UPDATE_NETWORK_SETTING, 0, 0);
	}
	else
		m_bEnableUpdate = true;
}

void CIPCtrl::Enable(bool bEnable)
{
	EnableWindow(bEnable);
}

/*
HBRUSH CIPCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
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
*/