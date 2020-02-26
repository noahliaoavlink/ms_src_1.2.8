#pragma once

#ifndef _TxtButtonCtrl_H_
#define _TxtButtonCtrl_H_

enum UIState
{
	UIS_NORMAL = 0,
	UIS_ACTIVE,
	UIS_DOWN,
	UIS_DISABLE,
};

class TxtButtonCtrl //: public CSpinButtonCtrl
{
	RECT m_Rect;
	int m_iID;

	HWND m_hParentWnd;
	char m_szName[256];

	HBRUSH m_hBrush;
	HBRUSH m_hActiveBrush;
	HBRUSH m_hDisableBrush;
	HPEN m_hPen;
	HPEN m_hActivePen;
	HPEN m_hDisablePen;

	bool m_bEnable;

	int m_iState;
public:
	TxtButtonCtrl();
	~TxtButtonCtrl();

	void Create(char* szName, int x, int y, int w, int h, HWND hParentWnd, int iID);
	void Enable(bool bEnable);
	void Draw();
	void DrawRect(HDC hDC, RECT* pRect);
	void SetDown();

	void MouseMove(int x, int y);
	void LButtonDown(int x, int y);
	void LButtonUp(int x, int y);

protected:


};

#endif