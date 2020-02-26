#if !defined(AFX_NWIN_H__8E6FD8B2_58B3_4A4C_BEF9_276AEED053A1__INCLUDED_)
#define AFX_NWIN_H__8E6FD8B2_58B3_4A4C_BEF9_276AEED053A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NWin.h : header file
//


//#include "..\\..\\..\\Include\\PluginObjMsg.h"
#include "..\\..\\..\\Include\\UIObjInfo.h"
#include "..\\..\\..\\Include\\MediaFileCommon.h"
/////////////////////////////////////////////////////////////////////////////
// CNWin window

class CNWin : public CWnd
{
	HBRUSH m_hBrush;
	WNDCLASS	wc;
	char m_szClassName[256];

	int m_iStyle;
	WinMsgCallback* m_pCallback;
// Construction
public:
	CNWin();
	CNWin(char* szClassName,int iStyle = WT_NORMAL);

	void SetBKColor(unsigned char r,unsigned char g,unsigned char b);
	void SetWinSize(int width,int high,bool bShow = false);
	void SetWinPos(int x,int y,bool bShow = false);

	BOOL RegisterWindowClass(char* szClassName);

	void SetCallback(WinMsgCallback* pCallback);
	void SetTimer1(int iTimeOut);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWin)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL DestroyWindow();
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNWin();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNWin)
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NWIN_H__8E6FD8B2_58B3_4A4C_BEF9_276AEED053A1__INCLUDED_)
