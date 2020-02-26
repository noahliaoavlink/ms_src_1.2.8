#pragma once


// CSearchTagDlg dialog

class CSearchTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchTagDlg)

public:
	CSearchTagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchTagDlg();

	wchar_t* GetKeyword();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEARCH_TAG_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
