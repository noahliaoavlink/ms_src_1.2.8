// SplitWnd.cpp : implementation file
// 
#include "stdafx.h"
#include "xSplitterWnd.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

// HitTest return values (values and spacing between values is important)
// Had to adopt this because it has module scope 

/////////////////////////////////////////////////////////////////////////////
// CxSplitterWnd 

BEGIN_MESSAGE_MAP(CxSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CxSplitterWnd)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitterWnd command routing 
BOOL CxSplitterWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (CWnd::OnCommand(wParam, lParam))
		return TRUE;
	// route commands to the splitter to the parent frame window
	return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
}

BOOL CxSplitterWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CWnd::OnNotify(wParam, lParam, pResult))
		return TRUE;
	// route commands to the splitter to the parent frame window
	*pResult = GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	return TRUE;
}

BOOL CxSplitterWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// The code line below is necessary if using CxSplitterWnd in a regular dll
	// AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

void CxSplitterWnd::OnDrawSplitter(CDC* pDC,ESplitType nType,const CRect& rect)
{
	if (splitBar == nType)
	{
		if (pDC)
		{
			//CBrush brushBlue(RGB(250, 193, 18));
			CBrush brushBlue(RGB(101, 101, 101));
			CBrush* pOldBrush = pDC->SelectObject(&brushBlue);
			pDC->Rectangle(rect.left, rect.top + 2, rect.right, rect.bottom - 2);
			pDC->SelectObject(pOldBrush);
		}
	}
}