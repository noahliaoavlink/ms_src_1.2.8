#pragma once


// CEditEffectIndexDlg dialog

class CEditEffectIndexDlg : public CDialogEx
{
	int m_iIndex;
	DECLARE_DYNAMIC(CEditEffectIndexDlg)

public:
	CEditEffectIndexDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditEffectIndexDlg();

	int GetIndex();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_EFFECT_INDEX_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
