#pragma once

#ifndef _CEditCtrl_H_
#define _CEditCtrl_H_

class CEditCtrl : public CEdit
{
	DECLARE_DYNAMIC(CEditCtrl)

public:
	CEditCtrl();
	~CEditCtrl();

	void Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);

protected:

	//{{AFX_MSG(CIPCtrl)
	//afx_msg void OnClicked();
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnEnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	COLORREF textBGColor;
	CBrush cBrush;
	COLORREF m_BGColor;
	COLORREF m_FGColor;
};

#endif
