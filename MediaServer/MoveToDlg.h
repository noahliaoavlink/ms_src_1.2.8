#pragma once

enum DlgType
{
	DT_NORMAL = 0,
	DT_CHANGE_LEN,
};

// CMoveToDlg dialog

class CMoveToDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMoveToDlg)

	int m_iType;
public:
	CMoveToDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMoveToDlg();

	double GetTime();
	void SetType(int iType);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOVE_TO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
