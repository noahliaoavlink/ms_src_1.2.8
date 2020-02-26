#pragma once


// CPanelBlendDlg 對話方塊

class CPanelBlendDlg : public CDialog
{
	DECLARE_DYNAMIC(CPanelBlendDlg)

public:
	CPanelBlendDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CPanelBlendDlg();
	UINT GetPanelIdx() { return m_iPanelIdx; };
	BOOL IsLockBlendArea() { return m_bLockBlendArea; };
	UINT GetFinishStatus() { return m_iFinishStatus; }; //0: Not, 1: Finish, 2: Finish & Create Object

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAN_BLD_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	UINT m_iPanelIdx;
	UINT m_iFinishStatus; //0: Not, 1: Finish, 2: Finish & Create Object
	BOOL m_bLockBlendArea;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
