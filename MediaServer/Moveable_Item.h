#pragma once
#include "item_class.h"

class CMoveable_Item :
	public CItem_Class
{
public:
	CMoveable_Item(void);
	~CMoveable_Item(void);
	CMoveable_Item(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage = true, 
		        bool bHaveClickImage = true, bool bHaveThroughImage = true, bool bIsOptionButtion = false);
	CMoveable_Item(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, CString strItemFileName,
					CPoint BeginPt, CPoint EndPt);
public:
	void SetItemMoveLimit(CPoint BeginPt, CPoint EndPt){m_BeginPt = BeginPt; m_EndPt = EndPt;}
	CPoint GetRelativePos(){return m_ItemCurPt;}
	void SetToPosPre(CPoint DestPt);
	void SetToPos(CPoint DestPt, BOOL bInit = FALSE);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	CPoint m_CursorPos;
	CPoint m_BeginPt, m_EndPt;
	CString m_strItemPath;
	Bitmap* m_pItemImage;
	CRgn    m_ItemRegion;
	CPoint m_ItemCurPt;
	HCURSOR SetMouseCursor(int iCursorID);
};
