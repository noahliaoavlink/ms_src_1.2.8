#pragma once

typedef struct
{
	int iHour;
	int iMinute;
	int iSecond;
	//int iCmdID;
	char szCmdID[32];
	int iParameter;
}PBItem;

// CAddDlg dialog

class CAddDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddDlg)

	bool m_bModify;

public:
	CAddDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddDlg();

	PBItem* GetPBItem();
	void SetPBItem(PBItem* pItem);
	void EnableModify(bool bEnable);
	void UpdateUIStatus();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnStnClickedStaticStartTime3();
	afx_msg void OnCbnSelchangePbCmd();
};
