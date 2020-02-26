#pragma once

#include "Item_Class.h"


// CRadioMenuDlg 對話方塊

class CRadioMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CRadioMenuDlg)

public:
	CRadioMenuDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CRadioMenuDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG };
#endif
	enum
	{
		MEDIA_ITEM = WM_USER + 1,
		MIXER_ITEM,
		SHAPE_ITEM,
		TLINE_ITEM,
		OUTPT_ITEM,
		SYSTM_ITEM,
		BAR_MENU = WM_USER + 100,
		DOT_MENU = WM_USER + 101
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	CMenu_Class* m_FramePic;
	CMenu_Class* m_DotPic;
	BOOL m_bShowMenu;

	DECLARE_MESSAGE_MAP()
public:
	void ShowMenu(BOOL bShowMenu);
	virtual BOOL OnInitDialog();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	virtual void OnCancel();
	virtual void OnOK();
};
