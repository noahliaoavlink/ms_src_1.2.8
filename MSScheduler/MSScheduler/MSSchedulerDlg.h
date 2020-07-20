
// MSSchedulerDlg.h : header file
//

#pragma once


// CMSSchedulerDlg dialog
class CMSSchedulerDlg : public CDialogEx
{
// Construction
public:
	CMSSchedulerDlg(CWnd* pParent = NULL);	// standard constructor

	void UpdateUIStatus();
	void ListToFile();
	void FileToList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSSCHEDULER_DIALOG };
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
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedModify();
};
