#pragma once

// SplitWnd.h : implementation file
// 
class CxSplitterWnd : public CSplitterWnd
{
	// Construction
public:
	CxSplitterWnd() {};
	virtual ~CxSplitterWnd() {};

	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSplitterWnd)
	//}}AFX_VIRTUAL 

	// Implementation
public:
	// These are the methods to be overridden
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);

	// Generated message map functions
protected:
	//{{AFX_MSG(CxSplitterWnd)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};