#pragma once


// CLogoDlg 對話方塊
#include "Item_Class.h" 
#include "MediaServerDlg.h"

class CLogoDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogoDlg)

public:
	CLogoDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CLogoDlg();


	void EnableShowLogo(bool bEnable);

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG};
#endif

protected:
	CMenu_Class* m_FramePic;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
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
