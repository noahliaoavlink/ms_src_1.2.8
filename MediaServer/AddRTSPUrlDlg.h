#pragma once


// CAddRTSPUrlDlg dialog

class CAddRTSPUrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddRTSPUrlDlg)

public:
	CAddRTSPUrlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddRTSPUrlDlg();

	char* GetUrl();
	double GetLength();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_RTSP_URL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
