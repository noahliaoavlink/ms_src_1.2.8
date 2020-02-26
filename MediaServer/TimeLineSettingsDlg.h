#pragma once


// CTimeLineSettingsDlg dialog

class CTimeLineSettingsDlg : public CDialogEx
{
	bool m_bDisableName;

	DECLARE_DYNAMIC(CTimeLineSettingsDlg)

public:
	CTimeLineSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeLineSettingsDlg();

	void SetName(char* szName);
	void SetID(int iID);
	char* GetName();
	int GetID();
	void DisableName();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIMELINE_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
