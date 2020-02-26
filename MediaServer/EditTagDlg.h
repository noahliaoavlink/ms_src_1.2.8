#pragma once


// CEditTagDlg dialog

class CEditTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditTagDlg)

public:
	CEditTagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditTagDlg();

	void SetTag(wchar_t* wszText);
	wchar_t* GetTag();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_TAG_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
