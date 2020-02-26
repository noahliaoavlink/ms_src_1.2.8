#pragma once
#include "ui_class.h"
#include "afxwin.h"
#include "UTipDll.h"

class CMenu_Class :
	public CUI_Class
{
	DECLARE_DYNAMIC(CMenu_Class)
public:
	CMenu_Class(){};
	~CMenu_Class(void);
	CMenu_Class(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage = true);
	int CreateMenu(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage = true);
	virtual void OffsetObject(CPoint OffsetPt);
	virtual int  Initialize();
	virtual CRgn* GetRegion(void);
	int RemoveAll();
	int AddItem(CUI_Class* pItem);
	int DeleteItem(int iIDNum, BOOL bAddRegion = TRUE);
	virtual void OnWindowMove(int ix, int iy);
	virtual int OnZoom(int iStartX, int iStartY, float fScaleWidth, float fScaleHeight);
	virtual int  ChangeToOtherBKImage();
	int     ResetEffectRegion();
	void    AddRegion(CRgn* AddRgn);
	virtual void DoItemClickFunction(int iIDNum, int iParam);
	virtual afx_msg void OnPaint();
	CUI_Class* GetItemByID(INT32 iID);
	void SetGroupItem(CArray<INT32, INT32> iIDAry, INT32 iGroupIndex);
	INT32 GetSelectedItem(INT32 iGroupIndex);
	void EnableMenuMove(BOOL bMoveable) { m_bMoveable = bMoveable;};
	void EnableItem(BOOL bEnable, INT32 iExceptID = -1);
protected:
	int m_iItemNum;	
	CPoint m_CursorPos;
	BOOL m_bMoveable;
	INT32 m_iExceptID;
	CTypedPtrList<CObList, CUI_Class*>m_ItemList;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnable(BOOL bEnable);
};
