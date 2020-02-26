#include "stdafx.h"
#include "UTooltip.h"

#define WAIT_TIME 5
#define SHOW_TIME 30
#define WIDTH_OFFSET 8
#define HEIGHT_OFFSET 12
#define TOP_OFFSET 24
#define TEXT_X 4
#define TEXT_Y 6

UTooltip::UTooltip()
{
	m_pNWin = 0;

	m_ucTextColor[0] = 87;
	m_ucTextColor[1] = 87;
	m_ucTextColor[2] = 87;

	m_ucBKColor[0] = 241;
	m_ucBKColor[1] = 242;
	m_ucBKColor[2] = 247;

	m_iX = 0;
	m_iY = 0;
	m_iCurIndex = -1;
	m_iOldIndex = -1;
	m_iWaitCount = 0;
	m_iShowCount = 0;
	m_iStatus = MUI_TS_NORMAL;

	m_hBrush = 0;
	m_hPen = 0;

	m_pTips = new SQList;
	m_pTips->EnableRemoveData(false);
}

UTooltip::~UTooltip()
{
	if (m_pNWin)
		delete m_pNWin;

	Reset();

	if (m_pTips)
		delete m_pTips;

	if (m_hBrush)
	{
		DeleteObject(m_hBrush);
		m_hBrush = 0;
	}

	if (m_hPen)
	{
		DeleteObject(m_hPen);
		m_hPen = 0;
	}
}

void UTooltip::SetTextColor(BYTE ucR, BYTE ucG, BYTE ucB)
{
	m_ucTextColor[0] = ucR;
	m_ucTextColor[1] = ucG;
	m_ucTextColor[2] = ucB;

//	m_Text.SetTextColor(ucR, ucG, ucG);
}

void UTooltip::SetBKColor(BYTE ucR, BYTE ucG, BYTE ucB)
{
	m_ucBKColor[0] = ucR;
	m_ucBKColor[1] = ucG;
	m_ucBKColor[2] = ucB;
}

void UTooltip::Init()
{
	char szTipWinName[128];
	sprintf(szTipWinName,"UTooltipCtrl%d", GetTickCount());
	m_pNWin = new CNWin(szTipWinName);
	m_pNWin->SetCallback(this);

	m_pNWin->SetBKColor(m_ucBKColor[0], m_ucBKColor[1], m_ucBKColor[2]);

	m_Text.SetTextColor(m_ucTextColor[0], m_ucTextColor[1], m_ucTextColor[2]);
	m_Text.SetBKColor(m_ucBKColor[0], m_ucBKColor[1], m_ucBKColor[2]);
	m_Text.SetFont("MS Sans Serif", 12);

	m_hBrush = CreateSolidBrush(RGB(m_ucBKColor[0], m_ucBKColor[1], m_ucBKColor[2]));
	m_hPen = CreatePen(PS_SOLID, 1, RGB(118, 118, 118));

	SetTimer(m_pNWin->GetSafeHwnd(),101,100,NULL);
}

void UTooltip::Add(TipInfoW* pTip)
{
	TipInfoW* pNewTip = new TipInfoW;
	memcpy(pNewTip, pTip, sizeof(TipInfoW));
	m_pTips->Add(pNewTip);
}

void UTooltip::Reset()
{
	for (int i = 0; i < m_pTips->GetTotal(); i++)
	{
		TipInfoW* pCurTipInfo = (TipInfoW*)m_pTips->Get(i);
		if (pCurTipInfo)
			delete pCurTipInfo;
	}

	m_pTips->Reset();
}

bool UTooltip::CheckTipInfo(HWND hWnd, int x, int y, TipInfoW* pTip)
{
	if (pTip->hWnd == hWnd)
	{
		if (pTip->rect.left <= x
			&& pTip->rect.left + pTip->rect.right >= x
			&& pTip->rect.top <= y
			&& pTip->rect.top + pTip->rect.bottom >= y)
			return true;
	}
	return false;
}

void UTooltip::ShowWin()
{
	SIZE size;
	HDC hDC = GetDC(m_pNWin->GetSafeHwnd());

	int iScreenW = GetDeviceCaps(hDC, HORZRES);
	int iScreenH = GetDeviceCaps(hDC, VERTRES);
	m_Text.GetTextSizeW(hDC, m_wszCurData, &size);

	ReleaseDC(m_pNWin->GetSafeHwnd(), hDC);

	int x;
	int y;
	int w = size.cx + WIDTH_OFFSET;
	int h = size.cy + HEIGHT_OFFSET;

	if (w + m_iX > iScreenW)
		x = m_iX - (w + m_iX - iScreenW);
	else
		x = m_iX;

	if (m_iY + TOP_OFFSET + h > iScreenH)
		y = m_iY + TOP_OFFSET - (m_iY + TOP_OFFSET + h - iScreenH);
	else
		y = m_iY + TOP_OFFSET;

	SetWindowPos(m_pNWin->GetSafeHwnd(), HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void UTooltip::ShowTip(HWND hWnd, int x, int y)
{
	int iIndex = -1;
	for (int i = 0; i < m_pTips->GetTotal(); i++)
	{
		TipInfoW* pCurTip = (TipInfoW*)m_pTips->Get(i);
		if (CheckTipInfo(hWnd, x, y, pCurTip))
		{
			iIndex = i;
			break;
		}
	}

	if (iIndex == -1)
	{
		m_iStatus = MUI_TS_NORMAL;
		m_iCurIndex = -1;
		ShowWindow(m_pNWin->GetSafeHwnd(), SW_HIDE);
	}
	else
	{
		if (iIndex != m_iCurIndex)
		{
			m_iCurIndex = iIndex;
			TipInfoW* pCurTip = (TipInfoW*)m_pTips->Get(iIndex);
			if (pCurTip->ulTipID != 0)
			{
				wcscpy(m_wszCurData, pCurTip->wszMsg);
			}
			else
				wcscpy(m_wszCurData, pCurTip->wszMsg);

			if (wcscmp(m_wszCurData, L"") == 0)
			{
				m_iStatus = MUI_TS_NORMAL;
				m_iCurIndex = -1;
			}
			else
				m_iStatus = MUI_TS_WAIT;

			m_iShowCount = 0;
			m_iWaitCount = 0;
			ShowWindow(m_pNWin->GetSafeHwnd(), SW_HIDE);
		}

		if (m_iStatus != MUI_TS_SHOW)
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);
			m_iX = rect.left + x;
			m_iY = rect.top + y;
		}
	}
}

void UTooltip::ShowData(HDC hDC)
{
	RECT rect;
	//RECT rect2;
	//GetWindowRect(m_pNWin->GetSafeHwnd(), &rect);
	GetClientRect(m_pNWin->GetSafeHwnd(), &rect);
	FillRect(hDC, &rect, m_hBrush);

	HPEN hOldPen = (HPEN)SelectObject(hDC, m_hPen);

	MoveToEx(hDC, rect.left, rect.top, 0);
	LineTo(hDC, rect.right - 1, rect.top);
	LineTo(hDC, rect.right - 1, rect.bottom - 1);
	LineTo(hDC, rect.left, rect.bottom - 1);
	LineTo(hDC, rect.left, rect.top);

	SelectObject(hDC, hOldPen);

	m_Text.DrawTextW(hDC, TEXT_X, TEXT_Y, m_wszCurData);
}

void UTooltip::TimeEvt()
{
	if (m_iStatus == MUI_TS_WAIT)
	{
		m_iWaitCount++;
		if (m_iWaitCount >= WAIT_TIME)
		{
			m_iStatus = MUI_TS_SHOW;
			m_iWaitCount = 0;

			ShowWin();
			HDC hDC = GetDC(m_pNWin->GetSafeHwnd());
			ShowData(hDC);
			ReleaseDC(m_pNWin->GetSafeHwnd(), hDC);
		}
	}
	else if (m_iStatus == MUI_TS_SHOW)
	{
		m_iShowCount++;
		if (m_iShowCount >= SHOW_TIME)
		{
			m_iShowCount = 0;
			m_iStatus = MUI_TS_NORMAL;
			ShowWindow(m_pNWin->GetSafeHwnd(), SW_HIDE);
		}
	}
}

void* UTooltip::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case WM_TIMER:
			TimeEvt();
			break;
	}
	return 0;
}