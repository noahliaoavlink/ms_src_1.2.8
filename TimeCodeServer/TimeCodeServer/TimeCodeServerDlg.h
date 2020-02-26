
// TimeCodeServerDlg.h : header file
//

#pragma once


#include "IPDetector.h"
#include "TCServerAgent.h"

// CTimeCodeServerDlg dialog
class CTimeCodeServerDlg : public CDialogEx
{
	IPDetector m_IPDetector;
	TCServerAgent* m_pTCServerAgent;

	char m_szTime[64];
// Construction
public:
	CTimeCodeServerDlg(CWnd* pParent = NULL);	// standard constructor

	char* PosToTimeInfo(double dPos, bool bEnableMS);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIMECODESERVER_DIALOG };
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
	afx_msg void OnBnClickedStartNetwork();
	afx_msg void OnBnClickedStopNetwork();
	afx_msg void OnBnClickedTcStop();
	afx_msg void OnBnClickedTcPlay();
	afx_msg void OnBnClickedTcPause();

	afx_msg LRESULT OnUpdateTimeCodeStr(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
};
