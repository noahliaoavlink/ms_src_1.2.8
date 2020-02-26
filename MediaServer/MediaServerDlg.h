// MediaServerDlg.h : 標頭檔
//

#pragma once
#include "ModifyUI.h"
#include "Item_Class.h"
#include "Moveable_Item.h"

#ifdef _MIXER2
#include "Mixer2Dlg.h"
#else
#include "MixerCtrlDlg.h"
#endif

#include "TimeLineDlg.h"
#include "MediaCtrlDlg.h"
#include "RadioMenuDlg.h"
#include "SimpleMediaCtrl.h"
#include "LogFileDll.h"
#include "SIniFileW.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/sstring.h"

#include "ExceptionReport.h"

#include "SynCtrl.h"
#include "KPCheckerCtrl.h"
#include "SyncPBCtrl.h"

#ifdef ENABLE_UI_CTRL
#include "UIController.h"
#endif

// CMediaServerDlg 對話方塊
class CMediaServerDlg : public CDialog , public MediaFileManagerCallback
{
	ExceptionReport* m_pExceptionReport;

#ifdef _ENABLE_CHECK_KEYPRO
//	int m_iTimeCount;
//	bool m_bTimerIsSuppend;
	bool m_bCheckKeyPro;

	KPCheckerCtrl* m_pKPCheckerCtrl;
#endif

	bool m_bDoSwitchToShape;

// 建構
public:
	CMediaServerDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_MEDIASERVER_DIALOG };

	enum
	{
		MEDIA_ITEM = WM_USER + 1,
		MIXER_ITEM,
		SHAPE_ITEM,
		TLINE_ITEM,
		OUTPT_ITEM,
		SYSTM_ITEM,
		POWER_ITEM,
		TESTM_ITEM
	};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;
	//CPanel *m_pPannel;
	
#ifdef _MIXER2
	CMixer2Dlg m_dlgMixer2;
#else
	CMixerCtrlDlg m_dlgMixerCtrl;
#endif
	CTimeLineDlg m_dlgTimeLine;
	CMediaCtrlDlg m_dlgMediaCtrl;
	CRadioMenuDlg m_dlgRadioMenu;

#ifdef ENABLE_UI_CTRL
	UIController* m_pUIController;
#else
	SimpleMediaCtrl m_SimpleMediaCtrl;
#endif
	INT32 m_iSplit;
	INT32 m_iMonitorNum;
	CModifyUI m_dlgUI;
	CMenu_Class* m_FramePic;
	LogFileDll* m_pLogFileDll;
	bool m_bEnableLog;
	void* m_pLogFileObj;
	SIniFileW* m_pSIniFileW;
	wchar_t wszIniFileName[512];

#ifdef _ENABLE_VIDEO_WALL
	SynCtrl* m_pSynCtrl;
#endif

	SyncPBCtrl* m_pSyncPBCtrl;

	void EnterMixerMode();
	void EnterTimeLine();
	void EnterMediaCtrl(BOOL bFullScreen = TRUE, void* pCallback = 0);
	void ReleaseResource();
	void EnterShapeMode2();

	CWinThread* m_pInitThread;
	static UINT InitThread(LPVOID pParam);
    void InitFunction();
	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual void OnOK();

	void EnterShapeMode(bool bCheckSource = true);
	void ReturnFileName(wchar_t* wszFileName);
	void SetMessageToMSLauncher(int iCommandID);

#ifdef _ENABLE_CHECK_KEYPRO
	void CheckKeyPro();
#endif

//#ifdef _ENABLE_GPU
	void InitShapeUI();
//#endif
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void CheckLogStatus();
	void Add2LogFile(int iLevel, char* szMsg);
	void SetTip();
};
