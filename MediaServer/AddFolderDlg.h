#pragma once


// CAddFolderDlg dialog

class CAddFolderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddFolderDlg)

public:
	CAddFolderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddFolderDlg();

	wchar_t* GetFolderName();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_FOLDER_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
