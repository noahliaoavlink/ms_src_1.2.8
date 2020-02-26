#pragma once

typedef struct
{
	int iIndex;
	char szText[16];
}DOItem;

typedef struct
{
	int iFPS;
	int iInterval;
	char szText[16];
}FPSItem;

// CSettingsDlg dialog

class CSettingsDlg : public CDialogEx
{
	HWND m_hMSMainUIWnd;
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

	void SetMainUIHandle(HWND hWnd);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedBrowse2();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEnableMappingPf();
	afx_msg void OnBnClickedEnableTimelinePf();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBrowse3();
	afx_msg void OnBnClickedBrowse4();
	afx_msg void OnBnClickedBrowse5();
	afx_msg void OnBnClickedBrowse6();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
