#pragma once

#ifndef _CheckBoxCtrl_H_
#define _CheckBoxCtrl_H_

#include "../../../Include/Common.h"

class CheckBoxCtrl : public CButton
{
	DECLARE_DYNAMIC(CheckBoxCtrl)

public:
	CheckBoxCtrl();
	~CheckBoxCtrl();

	//void Create(wchar_t* wszName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void Create(char* szName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void Enable(bool bEnable);

protected:
	
	//{{AFX_MSG(CheckBoxCtrl)
	afx_msg void OnClicked();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	COLORREF textBGColor;
	CBrush cBrush;
	COLORREF m_BGColor;
	COLORREF m_FGColor;
};

#endif
