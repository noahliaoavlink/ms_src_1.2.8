#pragma once


// CWaitingDlg dialog

class CWaitingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWaitingDlg)

	int m_iCurPos;
public:
	CWaitingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitingDlg();

	void UpdateStatus(char* szMsg);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAITING_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
