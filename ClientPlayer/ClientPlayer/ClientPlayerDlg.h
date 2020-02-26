
// ClientPlayerDlg.h : header file
//

#pragma once

#include "NWin.h"
#include "LayoutCtrl.h"
#include "ClientPlayerCtrl.h"

// CClientPlayerDlg dialog
class CClientPlayerDlg : public CDialogEx
{
	CNWin* m_pNWin;
	LayoutCtrl m_LayoutCtrl;
	ClientPlayerCtrl* m_pClientPlayerCtrl;
// Construction
public:
	CClientPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENTPLAYER_DIALOG };
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
	afx_msg void OnDestroy();
};
