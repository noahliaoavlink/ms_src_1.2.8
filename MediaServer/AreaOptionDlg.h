#pragma once


// CAreaOptionDlg ��ܤ��

class CAreaOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAreaOptionDlg)

public:
	CAreaOptionDlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CAreaOptionDlg();

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AREA_OPTION_DLG };
#endif
	UINT m_uGroupEnd;// 0:Finish, 1: Connect to First Pt, 2: Connect to Closest Pt
	UINT m_uDarkMask;// 0: Bleding Area, 1: Dark Mask
	UINT m_uNextAction; //0: None, 1: Start New Group

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩



	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
