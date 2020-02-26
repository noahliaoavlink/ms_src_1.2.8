#pragma once
#include "ui_class.h"
#include "Menu_Class.h"

class CItem_Class:
	public CUI_Class
{
	DECLARE_DYNAMIC(CItem_Class)

public:
	~CItem_Class(void);
	CItem_Class(){};
	CItem_Class(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage = true, bool bHaveClickImage = true, bool bHaveThroughImage = true, UINT iGroupOption = 0);
	int CreateItem(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage = true, bool bHaveClickImage = true, bool bHaveThroughImage = true, bool bIsOptionButtion = false);
	virtual int Redraw(bool bErase, int DrawType);
	int CreateWindowsItem(CWnd* ParentWnd, HWND hwnd, int iIDNum, CRect WindowsRect);
	bool IsHaveClickImage(){return m_bClickImage;}
	bool IsHaveThroughImage(){return m_bThroughImage;}
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void OnBnClicked();
	virtual int  Initialize();
	void SetParentMenu(CMenu_Class* pParentMenu){m_ParentMenu = pParentMenu;}
	CMenu_Class*  m_ParentMenu;

	INT32 m_iGroupIndex;
	bool m_bIsOptionType;
	bool    m_bClicked;         //記錄是否滑鼠已經押下
	bool m_WindowsItem;	
private:
	DECLARE_MESSAGE_MAP()

#define Is_Button_Down  0x01
#define Is_Option_Item  0x02
#define Cur_ItemStatus  m_bIsOptionType<< 1 | nFlags
public:
	afx_msg void OnEnable(BOOL bEnable);
};
