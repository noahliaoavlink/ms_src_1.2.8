#pragma once

#ifndef _CIPCtrl_H_
#define _CIPCtrl_H_

class CIPCtrl : public CIPAddressCtrl
{
	DECLARE_DYNAMIC(CIPCtrl)

public:
	CIPCtrl();
	~CIPCtrl();

	void Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void SetIP(char* szIP);
	char* GetIP();

	void Enable(bool bEnable);

protected:

	//{{AFX_MSG(CIPCtrl)
	//afx_msg void OnClicked();
	//afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	COLORREF textBGColor;
	CBrush cBrush;
	COLORREF m_BGColor;
	COLORREF m_FGColor;

	char m_szIP[64];
	bool m_bEnableUpdate;
};

#endif
