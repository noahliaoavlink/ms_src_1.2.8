#include "stdafx.h"
#include "TxtButtonCtrl.h"

TxtButtonCtrl::TxtButtonCtrl()
{
	strcpy(m_szName, "");

	m_hBrush = CreateSolidBrush(RGB(50,50,50));
	m_hActiveBrush = CreateSolidBrush(RGB(100, 100, 100));
	//m_hDisableBrush = CreateSolidBrush(RGB(100, 100, 100));

	m_hPen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	m_hActivePen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	m_hDisablePen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	
	m_iState = UIS_NORMAL;
	m_bEnable = true;
}

TxtButtonCtrl::~TxtButtonCtrl()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);
		m_hBrush = NULL;
	}

	if (m_hActiveBrush)
	{
		DeleteObject(m_hActiveBrush);
		m_hActiveBrush = NULL;
	}

	if (m_hPen)
	{
		DeleteObject(m_hPen);
		m_hPen = NULL;
	}

	if (m_hActivePen)
	{
		DeleteObject(m_hActivePen);
		m_hActivePen = NULL;
	}
}

void TxtButtonCtrl::Create(char* szName, int x, int y, int w, int h, HWND hParentWnd, int iID)
{
	m_Rect.left = x;
	m_Rect.top = y;
	m_Rect.right = w + m_Rect.left;
	m_Rect.bottom = h + m_Rect.top;

	m_iID = iID;

	m_hParentWnd = hParentWnd;

	strcpy(m_szName, szName);
}

void TxtButtonCtrl::Enable(bool bEnable)
{
	m_bEnable = bEnable;
}

void TxtButtonCtrl::Draw()
{
	HDC hDC = GetDC(m_hParentWnd);

	int iOffset = 0;

	HPEN hOldPen;
	HBRUSH hOldBrush;

	if (!m_bEnable)
	{
		hOldPen = (HPEN)SelectObject(hDC, m_hDisablePen);
		hOldBrush = (HBRUSH)SelectObject(hDC, m_hBrush);
		SetTextColor(hDC, RGB(100, 100, 100));
	}
	else
	{
		if (m_iState == UIS_NORMAL)
		{
			hOldPen = (HPEN)SelectObject(hDC, m_hPen);
			hOldBrush = (HBRUSH)SelectObject(hDC, m_hBrush);
			SetTextColor(hDC, RGB(255, 255, 255));
		}
		else if (m_iState == UIS_ACTIVE)
		{
			hOldPen = (HPEN)SelectObject(hDC, m_hActivePen);
			hOldBrush = (HBRUSH)SelectObject(hDC, m_hActiveBrush);
			SetTextColor(hDC, RGB(200, 200, 200));
		}
		else if (m_iState == UIS_DOWN)
		{
			hOldPen = (HPEN)SelectObject(hDC, m_hActivePen);
			hOldBrush = (HBRUSH)SelectObject(hDC, m_hActiveBrush);
			SetTextColor(hDC, RGB(200, 200, 200));

			iOffset = 1;
		}
	}

	Rectangle(hDC, m_Rect.left + iOffset, m_Rect.top + iOffset, m_Rect.right + iOffset, m_Rect.bottom + iOffset);
//	DrawRect(hDC, &m_Rect);

	SetBkMode(hDC, TRANSPARENT);
	DrawTextA(hDC, m_szName, strlen(m_szName),&m_Rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (hOldPen)
		SelectObject(hDC, hOldPen);

	if (hOldBrush)
		SelectObject(hDC, hOldBrush);

	ReleaseDC(m_hParentWnd, hDC);
}

void TxtButtonCtrl::MouseMove(int x,int y)
{
	if (!m_bEnable)
		return;

	if (m_iState != UIS_DOWN)
	{
		int iState = UIS_NORMAL;
		if (x >= m_Rect.left && x <= m_Rect.right && y >= m_Rect.top && y <= m_Rect.bottom)
			iState = UIS_ACTIVE;
		else
			iState = UIS_NORMAL;

		if (m_iState != iState)
		{
			m_iState = iState;
			Draw();
		}
	}
}

void TxtButtonCtrl::LButtonDown(int x,int y)
{
	if (!m_bEnable)
		return;

	int iState = UIS_NORMAL;
	if (x >= m_Rect.left && x <= m_Rect.right && y >= m_Rect.top && y <= m_Rect.bottom)
		iState = UIS_DOWN;
	else
		iState = UIS_NORMAL;

	if (m_iState != iState)
	{
		m_iState = iState;
		Draw();
	}
}

void TxtButtonCtrl::LButtonUp(int x, int y)
{
	char szMsg[512];

	if (!m_bEnable)
		return;

	int iState = UIS_NORMAL;

	sprintf(szMsg, "TxtButtonCtrl::LButtonUp() 0 id:%d state:%d\n", m_iID, m_iState);
	OutputDebugStringA(szMsg);

	if (x >= m_Rect.left && x <= m_Rect.right && y >= m_Rect.top && y <= m_Rect.bottom)
	{
		if (m_iState == UIS_DOWN)
		{
			sprintf(szMsg, "TxtButtonCtrl::LButtonUp() 1 id:%d\n", m_iID);
			OutputDebugStringA(szMsg);
			PostMessage(m_hParentWnd, WM_BUTTON_CLICK, m_iID, 0);

			sprintf(szMsg, "TxtButtonCtrl::LButtonUp() 2 id:%d\n", m_iID);
			OutputDebugStringA(szMsg);
		}

		sprintf(szMsg, "TxtButtonCtrl::LButtonUp() 3 id:%d\n", m_iID);
		OutputDebugStringA(szMsg);

		iState = UIS_ACTIVE;
	}
	else
	{
		iState = UIS_NORMAL;

		sprintf(szMsg, "TxtButtonCtrl::LButtonUp() 4 id:%d\n", m_iID);
		OutputDebugStringA(szMsg);
	}

	if (m_iState != iState)
	{
		m_iState = iState;
		Draw();
	}
}

void TxtButtonCtrl::DrawRect(HDC hDC, RECT* pRect)
{
	MoveToEx(hDC, pRect->left, pRect->top, 0);
	LineTo(hDC, pRect->right, pRect->top);
	LineTo(hDC, pRect->right, pRect->bottom);
	LineTo(hDC, pRect->left, pRect->bottom);
	LineTo(hDC, pRect->left, pRect->top);
}

void TxtButtonCtrl::SetDown()
{
	m_iState = UIS_ACTIVE;
}