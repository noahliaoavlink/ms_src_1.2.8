#pragma once

#include "TrackContentManager.h"

// CEditCommandDlg dialog

class CEditCommandDlg : public CDialogEx
{
	TrackContentManager* m_pTrackContentManager;
	char m_szFullPath[512];
	int m_iEventCount;
	DECLARE_DYNAMIC(CEditCommandDlg)

public:
	CEditCommandDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditCommandDlg();

	int GetType();
	char* GetName();
	char* GetTargetName();
	int GetJumpType();
	void SetType(int iValue);
	void SetName(char* szName);
	void SetTargetName(char* szName);
	void SetJumpType(int iType);
	void SetTrackContentManager(TrackContentManager* pObj);
	void SetFullPath(char* szPath);
	void UpdateEventList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_CMD_DLG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCmdList();
};
