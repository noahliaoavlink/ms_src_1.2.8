#pragma once

#ifndef _CStaticCtrl_H_
#define _CStaticCtrl_H_

#include "../../../Include/Common.h"

class CStaticCtrl : public CStatic
{
	DECLARE_DYNAMIC(CStaticCtrl)

public:
	CStaticCtrl();
	~CStaticCtrl();

	//void Create(wchar_t* wszName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void Create(char* szName, int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);

protected:

	//{{AFX_MSG(CStaticCtrl)
	//afx_msg void OnClicked();
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