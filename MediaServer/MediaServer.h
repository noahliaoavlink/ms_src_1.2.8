// MediaServer.h : PROJECT_NAME 應用程式的主要標頭檔
//
#if !defined(AFX_MEDISSERVER_H)
#define AFX_MEDISSERVER_H

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號

#include "Panel.h"
#include "LogFileDll.h"

// CMediaServerApp:
// 請參閱實作此類別的 MediaServer.cpp
//

typedef struct strSTREM_MAPPING
{
	UINT iPanelIdx;
	UINT iMediaIdx;
	UINT iObjIdx;
	UINT iWidth;
	UINT iHeight;
	BOOL bExtraVideo;
	CString strVideoPath;
	CString strObjPath;
}STREM_MAPPING;

class CMediaServerApp : public CWinApp
{
private:
	CRITICAL_SECTION m_csInUse;
public:
	CMediaServerApp();

	HANDLE	m_hShareMemory;
	CString m_strCurPath;
	BOOL m_bSampleVersion;
	CString m_strSelVideoPath;
	INT32 m_iAppStatus;
	CWnd *m_pMediaServerDlg;
	CString m_strLanguage;
	//log file
	LogFileDll* m_pLogFileDll;
	bool m_bEnableLog;
	void* m_pLogFileObj;

	bool m_bLogoMode;
	BOOL m_bEnableEdgeBlending;
	bool m_bImportAutoTesting;
	bool m_bEnableVWallDisplay;
	int m_iVWallOffsetIndex;

	int m_iVWallIndexOrder[4];

	float m_fPresentFPS;
	bool m_bSwitchToShape;

	void SaveLog(CString strLog, CString strLogPath = "D:\\Log.csv");
	STREM_MAPPING GetStreamMapByIdx(INT iPanelIdx, INT iObjIdx);
	void DeleteObject(INT iPanelIdx, INT iObjIdx);
	void SaveShapeProject(CString strProjectPath);

	//log file
	LogFileDll* GetLogFileDll();
	void SetLogFileInfo(bool bEnable, void* pObj);
	void Add2LogFile(int iLevel, char* szMsg);

	void ParseCommandLine();
// 覆寫
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()
};

#define DEF_GLOBAL_MEDIA 0x100

extern CMediaServerApp theApp;
extern CArray<CPanel*, CPanel*> g_PannelAry;
extern CArray<CRect, CRect> g_DispArys;
extern CArray<STREM_MAPPING, STREM_MAPPING> g_StreamMappingAry;
	  
#endif
