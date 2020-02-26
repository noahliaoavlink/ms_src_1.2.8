#pragma once


// CLogoDlg ��ܤ��
#include "Item_Class.h" 
#include "MediaServerDlg.h"

class CLogoDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogoDlg)

public:
	CLogoDlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CLogoDlg();


	void EnableShowLogo(bool bEnable);

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG};
#endif

protected:
	CMenu_Class* m_FramePic;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩
	CWinThread* m_pMediaServerThread;
	CMediaServerDlg m_dlgMediaServer;

	bool m_bShowLogo;

	static UINT MediaServerThread(LPVOID pParam);
	void ShowLoop();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
