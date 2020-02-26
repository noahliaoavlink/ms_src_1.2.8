#pragma once

#ifndef _UTooltip_H_
#define _UTooltip_H_

#include "NWin.h"
#include "TextBase.h"
#include "..\\..\\Include\\SQList.h"

enum TipStatus
{
	MUI_TS_NORMAL = 0,
	MUI_TS_WAIT,
	MUI_TS_SHOW,
};

class UTooltip : public WinMsgCallback
{
	CNWin* m_pNWin;

	BYTE m_ucTextColor[3];
	BYTE m_ucBKColor[3];

	TextBase m_Text;
	SQList* m_pTips;

	int m_iX;
	int m_iY;
	int m_iShowCount;
	int m_iWaitCount;
	int m_iStatus;
	int m_iCurIndex;
	int m_iOldIndex;

	wchar_t m_wszCurData[256];
	HBRUSH m_hBrush;
	HPEN m_hPen;
public:
	UTooltip();
	~UTooltip();

	void Init();
	void SetTextColor(BYTE ucR, BYTE ucG, BYTE ucB);
	void SetBKColor(BYTE ucR, BYTE ucG, BYTE ucB);
	void Add(TipInfoW* pTip);
	void Reset();
	bool CheckTipInfo(HWND hWnd, int x, int y, TipInfoW* pTip);

	void ShowWin();
	void TimeEvt();
	void ShowTip(HWND hWnd, int x, int y);
	void ShowData(HDC hDC);

	void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);

protected:

};

#endif