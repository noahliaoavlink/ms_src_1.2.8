#pragma once


// CAudioConfigDlg dialog

class CAudioConfigDlg : public CDialogEx
{
	bool m_bAudioConfig[10];

	DECLARE_DYNAMIC(CAudioConfigDlg)

public:
	CAudioConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAudioConfigDlg();

	void EnableAudio(int iIndex, bool bEnable);
	bool AudioIsEnabled(int iIndex);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUDIO_CONFIG_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
