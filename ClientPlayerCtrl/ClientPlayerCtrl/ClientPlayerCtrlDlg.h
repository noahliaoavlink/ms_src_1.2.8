
// ClientPlayerCtrlDlg.h : header file
//

#pragma once

#include "APProcessCtrl.h"

// CClientPlayerCtrlDlg dialog
class CClientPlayerCtrlDlg : public CDialogEx
{
	APProcessCtrl m_APProcessCtrl[16];
// Construction
public:
	CClientPlayerCtrlDlg(CWnd* pParent = NULL);	// standard constructor

	int GetTotalOfItems(int iLayoutID);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENTPLAYERCTRL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedClose();
};
