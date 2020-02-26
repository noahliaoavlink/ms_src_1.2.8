// MediaServerDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "MediaServerDlg.h"
#include "LogoDlg.h"
#include "WaitingDlg.h"
#include "SettingsDlg.h"
#include "../Include/MidiCtrlCommon.h"
#include "../Include/SCreateWin.h"

#ifdef _TRAIL_VERSION
#include "ProbationChecker.h"
#endif

#ifdef _DISABLE_HASP
#else
#include "..\lib\HASP\hasp_api.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef _DISABLE_HASP
#else
unsigned char vendor_code[] =
"xDXLpqde16FbWnjk2jabaGaB4gE9pKtCBh5cMdmy2g8FTF6jyP0/7V6NC5ETk+7Oaw3C78YQiRFmXayY"
"cTWRh6sLq5idglcFPfbSbwGia1/98+DuK97KEAHR3ulAodJbOT4BFdBr7ewVDdPTGa2fMTcBaE28IgjP"
"9fIIz3ptTdsKXd1YeDE/N4fmclvOpHMKB/1ZDoCHoCvOcTaSh9TlMU/L7eL5U3qu7PwTqWvJXmRrGRI8"
"GvC+1WD46va87mpcXH+0gCgCuVfLN/Dz2XSr/P5xSqEjT0VIKwVGnG29QxdFJHXpaI95vGssCHgMD3bs"
"GhsfSwJ90nwOHbbHpN7M1iN8YyBlcxHUTl54/QMTcPEYTfJ3mwDOsy7oUdEinERh1X7HqIogB/WhUJTP"
"LnEiXvRCY8x57TnRXRqu2mam15tA2rpHDHlyD8Dg8+5tJWJ0bazipvfm0U7uJ8f7mThlRnsbM7sH3QTY"
"39Qv9nd4fpz47BBYt219O6N4hyrkRt9kvoNCUobjaH2oIXsMDKfAbLZSgOvfi5enT2gowyqgNdoQQN7q"
"qa3Jdd6UzssXElxLD7krPQcKQYTvGGK/JK8lysblSIZdHOK9x0VBiBxGio4r3CV1FfVSKhX0WcZuRA1h"
"SRtumJe2N+oGJJ01a/ngKQD37x3bUnPyDORQMlIbJpTc8fiq/770hOlwLflEeE/4+DoPkl5iZ6rmskM3"
"8Hub1KX510Q2CMZjxe83iRzkszC42a1Xe6qCnqQtn2bM1rtVU18V5Hdmou9P6Jcy+9WRPBBQUioKQWLk"
"5CKnDnMRuDbX7FW4yAF0E2ivtKdiktVCKgPzk9ymJ7fOMlz0mdcpoCuk4s2NCJ3uZprTqaZDFop/Rgdi"
"PeSKKuVDvX7J6KV9UdDQNtXma4IzdotKh9LN7U8ApToT8GeXGlcpanSTkMX7WZQxyhJlP4mQvbDKcQH7"
"SSvOo8AOgSY+hE6n5xIrgg==";


const hasp_feature_t feature = 1;
hasp_handle_t handle = HASP_INVALID_HANDLE_VALUE;
hasp_status_t status;
#ifdef _WIN64
	#pragma comment(lib,"..\\lib\\HASP\\libhasp_windows_x64_34720.lib")
#else
	#pragma comment(lib,"..\\lib\\HASP\\libhasp_windows_34720.lib")
#endif

#endif

//#define _ENABLE_MONITOR_SIMULATION_MODE 1
//#define _DEBUG_MONITORS 5

//simulation

// 對 App About 使用 CAboutDlg 對話方塊
PANNEL_SETTING g_PannelSetting;
CArray<CPanel*, CPanel*> g_PannelAry;
CArray<CRect, CRect> g_DispArys;
CArray<STREM_MAPPING, STREM_MAPPING> g_StreamMappingAry;

CWaitingDlg g_WaitingDlg;

unsigned int __stdcall _EnterShape_ThreadProc(void* lpvThreadParm);

#ifdef _ENABLE_CHECK_KEYPRO
unsigned int __stdcall _CheckKeyPro_ThreadProc(void* lpvThreadParm);
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMediaServerDlg 對話方塊




CMediaServerDlg::CMediaServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
	//m_pPannel = NULL;	
	m_iSplit = -1;
	m_FramePic = NULL;
	theApp.m_pMediaServerDlg = this;

	m_pKPCheckerCtrl = 0;

#ifdef _ENABLE_VIDEO_WALL
	m_pSynCtrl = 0;
#endif

	m_pSyncPBCtrl = 0;

	m_bDoSwitchToShape = false;
}

void CMediaServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMediaServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMediaServerDlg 訊息處理常式

UINT CMediaServerDlg::InitThread(LPVOID pParam)
{
	CMediaServerDlg *pThis = reinterpret_cast<CMediaServerDlg *>(pParam);
	//pThis->InitFunction();
	CLogoDlg dlgLogo;
	dlgLogo.DoModal();
	return 1;
}

void CMediaServerDlg::InitFunction()
{
	if (0)
	{
#ifdef PLAY_3D_MODE
		CPlaneScene3D::FileDecoder("D:\\Project\\MediaServer\\x64\\Debug\\MOV\\Test\\ABC.mp4", "",FALSE, TRUE);
		//CPlaneScene3D::FileEncoder("D:\\Project\\MediaServer\\x64\\Debug\\MOV\\vj-matrix-space-pack\\VJ_Matrix_Space_11!@#$%^.mov", FALSE);
#endif
		PostMessage(WM_CLOSE);
		return;
	}
	else if (0)
	{
		CString strTemp;
		CFileFind fFindDict, fFindFile;
		BOOL bWorkDict, bWorkFile;
		bWorkDict = fFindDict.FindFile(theApp.m_strCurPath + "MOV\\*.*");
		while (bWorkDict)
		{
			bWorkDict = fFindDict.FindNextFileA();
			if (fFindDict.IsDirectory() && !fFindDict.IsDots())
			{
				strTemp = fFindDict.GetFilePath() + "\\*.mov";
				bWorkFile = fFindFile.FindFile(strTemp);
				while (bWorkFile)
				{
					bWorkFile = fFindFile.FindNextFileA();
					strTemp.Format("%s, %s\n", fFindDict.GetFilePath(), fFindFile.GetFileName());
#ifdef PLAY_3D_MODE
					CPlaneScene3D::FileEncoder(fFindFile.GetFilePath(), FALSE);
					//CPlaneScene3D::FileDecoder(fFindFile.GetFilePath(), "", FALSE);
#endif
				}
			}
		}
		PostMessage(WM_CLOSE);
		return;
	}
	else if (1)
	{

#if 0
		CString strTemp;
		CFileFind fFindDict, fFindFile;
		BOOL bWorkDict, bWorkFile;
		bWorkDict = fFindDict.FindFile(theApp.m_strCurPath + "MOV\\*.*");
		while (bWorkDict)
		{
			bWorkDict = fFindDict.FindNextFileA();
			if (fFindDict.IsDirectory() && !fFindDict.IsDots())
			{
				strTemp = fFindDict.GetFilePath() + "\\*.*";
				bWorkFile = fFindFile.FindFile(strTemp);
				while (bWorkFile)
				{
					bWorkFile = fFindFile.FindNextFileA();
#ifdef PLAY_3D_MODE
					strTemp = fFindFile.GetFilePath();
					if (strTemp.Find("!@#$%^") != -1)
					{
						CPlaneScene3D::FileEncoder(fFindFile.GetFilePath(), FALSE, TRUE);
					}
#endif
				}
			}
		}
		//PostMessage(WM_CLOSE);
		//return;
#else

		wchar_t wszKeyName[256];
		wchar_t wszData[512];
		char szCurFileName[512];
		m_pSIniFileW->Open(wszIniFileName, true, false);

		wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"FileList", L"TotalOfFiles", L"0");
		int iTotalOfFiles = _wtoi(wszTotal);
		for (int j = 0; j < iTotalOfFiles; j++)
		{
			wsprintfW(wszKeyName, L"File%d", j);
			wchar_t* wszCurFileName = m_pSIniFileW->GetItemValue(L"FileList", wszKeyName, L"");

			//char* szCurFileName = WCharToChar(wszCurFileName);
			strcpy(szCurFileName, WCharToChar(wszCurFileName));

			if (PathFileExistsA(szCurFileName))
			{
#ifdef PLAY_3D_MODE
				CPlaneScene3D::FileEncoder(szCurFileName, FALSE, TRUE);
#endif
			}
		}

		m_pSIniFileW->Close();

		m_pSIniFileW->Open(wszIniFileName, true, true);
		wsprintfW(wszData, L"%d", 0);
		m_pSIniFileW->SetItemData(L"FileList", L"TotalOfFiles", wszData);
		m_pSIniFileW->Write();
		m_pSIniFileW->Close();
#endif

	}

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);

#ifdef ENABLE_UI_CTRL
	//m_pUIController = new UIController;
#else

#ifdef _MIXER2
	m_dlgMixer2.Create(IDD_TEMP_DLG, this);
	m_dlgMixer2.SetWindowPos(this, rectTemp.left, rectTemp.top,
		rectTemp.Width(), rectTemp.Height(), SWP_HIDEWINDOW);
#else	
	m_dlgMixerCtrl.Create(IDD_TEMP_DLG, this);
	m_dlgMixerCtrl.SetWindowPos(this, rectTemp.left, rectTemp.top,
		rectTemp.Width(), rectTemp.Height(), SWP_HIDEWINDOW);
#endif

#endif

	SetMessageToMSLauncher(MSLC_INIT_TIMELINE);

	m_dlgTimeLine.SetLogFileDll(m_pLogFileDll);
	m_dlgTimeLine.Create(IDD_TEMP_DLG, this);
	m_dlgTimeLine.SetWindowPos(this, rectTemp.left, rectTemp.top,
		rectTemp.Width(), rectTemp.Height(), SWP_HIDEWINDOW);

	/*
	m_dlgRadioMenu.Create(IDD_TEMP_DLG, this);
	m_dlgRadioMenu.SetWindowPos(this, rectTemp.left, rectTemp.top,
	rectTemp.Width(), rectTemp.Height(), SWP_SHOWWINDOW);*/

#ifdef _DISABLE_HASP
#else  //_DISABLE_HASP
	if (handle == HASP_INVALID_HANDLE_VALUE)
	{
		status = hasp_login(feature, vendor_code, &handle);

#if (_TRAIL_VERSION != 1)
		if (status != HASP_STATUS_OK)
			PostMessage(WM_CLOSE);
#endif  //_TRAIL_VERSION

	}

#endif //_DISABLE_HASP

#ifdef _TRAIL_VERSION
	ProbationChecker probation_checker;
	if (!probation_checker.CheckPeriodDate())
	{
		MessageBox("Trial date has expired!!", "", MB_OK | MB_ICONSTOP);
		PostMessage(WM_CLOSE);
	}
#endif

	g_PannelSetting.rectSurface.Width = 0;
	g_PannelSetting.rectSurface.Height = 0;

	SetMessageToMSLauncher(MSLC_INIT_MEDIA_CTRL);

	m_dlgMediaCtrl.SetLogFileDll(m_pLogFileDll);
	m_dlgMediaCtrl.Create(IDD_TEMP_DLG, this);
	rectTemp = g_DispArys.GetAt(0);
	m_dlgMediaCtrl.SetWindowPos(this, rectTemp.left, rectTemp.top,
		rectTemp.Width(), rectTemp.Height(), SWP_HIDEWINDOW);

#ifdef ENABLE_UI_CTRL
	m_pUIController = new UIController;
	m_pUIController->SetIOSourceCtrlDll(m_dlgMediaCtrl.GetIOSourceCtrlDll());
	m_pUIController->SetCSVReader(m_dlgMediaCtrl.GetCSVReader());
	m_pUIController->SetMediaFileManagerCallback(this);
	m_pUIController->SetMainUIHandle(this->GetSafeHwnd());

	m_pUIController->Init();
#else

	m_SimpleMediaCtrl.SetIOSourceCtrlDll(m_dlgMediaCtrl.GetIOSourceCtrlDll());
	m_SimpleMediaCtrl.SetCSVReader(m_dlgMediaCtrl.GetCSVReader());
	m_SimpleMediaCtrl.Init();

	m_SimpleMediaCtrl.SetMediaFileManagerCallback(this);
#endif

	m_dlgTimeLine.SetIOSourceCtrlDll(m_dlgMediaCtrl.GetIOSourceCtrlDll());
	
#ifdef ENABLE_UI_CTRL
	//m_pUIController = new UIController;
#else
	m_dlgMixer2.SetIOSourceCtrlDll(m_dlgMediaCtrl.GetIOSourceCtrlDll());
#endif
	
	g_WaitingDlg.Create(IDD_WAITING_DLG, 0);

	SetMessageToMSLauncher(MSLC_INIT_SHAPE);

	theApp.m_iAppStatus = 1;
//#ifdef _ENABLE_GPU
	InitShapeUI();
//#endif
	ShowWindow(SW_SHOW);

	ClipCursor(rectTemp);
}

BOOL CMediaServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	ShowWindow(SW_HIDE);


	// TODO: 在此加入額外的初始設定
	char szRTPFolder[512];
	m_pExceptionReport = new ExceptionReport;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	m_pKPCheckerCtrl = new KPCheckerCtrl;

#ifdef _ENABLE_VIDEO_WALL
	m_pSynCtrl = new SynCtrl;
#endif

	m_pSyncPBCtrl = new SyncPBCtrl;

	m_pSIniFileW = new SIniFileW;

	char szAPPath[512];
	char szFileName[512];
	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);
	sprintf(szFileName, "%s\\decrypt_file_list.txt", szAPPath);
	wcscpy(wszIniFileName, ANSIToUnicode(szFileName));

	//m_pLogFileDll = new LogFileDll;
	m_pLogFileDll = theApp.GetLogFileDll();
	std::wstring wstRTPFolder = m_pExceptionReport->GetFolderPath();
	strcpy(szRTPFolder, WCharToChar(wstRTPFolder));
	m_pLogFileDll->_Out_LGD_DoRecycle(szRTPFolder);
	CheckLogStatus();

	int iWidth = GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);
	//m_pPannel = NULL;


	//取得螢幕個數 GetSystemMetrics(SM_CMONITORS)
	//MonitorFromWindow 取得目前的螢幕
	//然後用 GetMonitorInfo 取得螢幕資訊 MONITORINFOEX 
	//rcMonitor 螢幕矩形
	//szDevice 螢幕名稱

	char cTemp[1024];
	g_PannelSetting.bDisableUI = GetPrivateProfileInt("UI", "Disable UI", 0, theApp.m_strCurPath + "Setting.ini");
	GetPrivateProfileString("UI", "Video Path", "", cTemp, 1024, theApp.m_strCurPath + "Setting.ini");
	g_PannelSetting.strVideoPath = cTemp;
	g_PannelSetting.strVideoPath.Trim();
	GetPrivateProfileString("UI", "Model Path", "", cTemp, 1024, theApp.m_strCurPath + "Setting.ini");
	g_PannelSetting.strModelPath = cTemp;
	g_PannelSetting.strModelPath.Trim();
	g_PannelSetting.iTestPattern = GetPrivateProfileInt("UI", "Test Pattern", 0, theApp.m_strCurPath + "Setting.ini");

	int tipID = GetPrivateProfileInt("UI", "TipLang", 0, theApp.m_strCurPath + "Setting.ini");
	char szIniFile[512];
	sprintf(szIniFile, "%s\\Language\\lang_config.ini", theApp.m_strCurPath);
	int iTotal = GetPrivateProfileInt("Base", "Total", 0, szIniFile);
	if (tipID > iTotal || tipID < 0) tipID = 0;
	sprintf(cTemp, "Lang%d", tipID);
	GetPrivateProfileString(cTemp, "Code", "", cTemp, 512, szIniFile);
	theApp.m_strLanguage = cTemp;

	g_PannelSetting.iTVWallMode = GetPrivateProfileInt("TV Wall", "Mode", 0, theApp.m_strCurPath + "Setting.ini");
	g_PannelSetting.iTVNumH = GetPrivateProfileInt("TV Wall", "H Pannel Num", 0, theApp.m_strCurPath + "Setting.ini");
	g_PannelSetting.iTVNumV = GetPrivateProfileInt("TV Wall", "V Pannel Num", 0, theApp.m_strCurPath + "Setting.ini");
	g_PannelSetting.iEncodeMedia = GetPrivateProfileInt("Media", "Encode", 1, theApp.m_strCurPath + "Setting.ini");

#ifdef _ENABLE_MONITOR_SIMULATION_MODE
	m_iMonitorNum = _DEBUG_MONITORS;
#else
	m_iMonitorNum = GetSystemMetrics(SM_CMONITORS);
#endif


	CString strResFolder;
	strResFolder = "UI Folder\\Main UI";

	if (!g_PannelSetting.bDisableUI)
	{
		m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + strResFolder + "\\Background.bmp");
		CItem_Class* pItem;
		pItem = new CItem_Class(this, m_hWnd, MEDIA_ITEM, theApp.m_strCurPath + strResFolder + "\\B01.bmp", true, true, true);
		pItem->OffsetObject(CPoint(330, 146));
		m_FramePic->AddItem(pItem);

		if (!theApp.m_bSampleVersion)
		{
#if (_ENABLE_VIDEO_WALL == 1|| _TRAIL_VERSION == 1)
#else
			pItem = new CItem_Class(this, m_hWnd, MIXER_ITEM, theApp.m_strCurPath + strResFolder + "\\B02.bmp", true, true, true);
			//pItem->OffsetObject(CPoint(330, 539));
			pItem->OffsetObject(CPoint(1344, 344));
			m_FramePic->AddItem(pItem);
#endif
		}

		pItem = new CItem_Class(this, m_hWnd, SHAPE_ITEM, theApp.m_strCurPath + strResFolder + "\\B03.bmp", true, true, true);
		pItem->OffsetObject(CPoint(669, 343));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, TLINE_ITEM, theApp.m_strCurPath + strResFolder + "\\B04.bmp", true, true, true);
		pItem->OffsetObject(CPoint(1007, 146));
		m_FramePic->AddItem(pItem);

		/*pItem = new CItem_Class(this, m_hWnd, OUTPT_ITEM, theApp.m_strCurPath + strResFolder + "\\B05.bmp", true, true, true);
		pItem->OffsetObject(CPoint(1007, 539));
		m_FramePic->AddItem(pItem);
		*/

#if 0
		pItem = new CItem_Class(this, m_hWnd, SYSTM_ITEM, theApp.m_strCurPath + strResFolder + "\\B06.bmp", true, true, true);
		//pItem->OffsetObject(CPoint(1344, 344));
		pItem->OffsetObject(CPoint(1007, 539));
		m_FramePic->AddItem(pItem);
#else
		pItem = new CItem_Class(this, m_hWnd, SYSTM_ITEM, theApp.m_strCurPath + strResFolder + "\\settings.bmp", true, true, true);
		//pItem->OffsetObject(CPoint(1344, 344));
		pItem->OffsetObject(CPoint(1874, 1));
		m_FramePic->AddItem(pItem);
#endif

		pItem = new CItem_Class(this, m_hWnd, POWER_ITEM, theApp.m_strCurPath + strResFolder + "\\Off.bmp", true, true, true);
		pItem->OffsetObject(CPoint(59, 871));
		m_FramePic->AddItem(pItem);
	}
	else
		m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + strResFolder + "\\BlackBK.bmp");


	m_FramePic->Initialize();

	if (iWidth != 1920 || iHeight != 1080)
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic->SetWindowRgn(NullRgn, true);

	SetMessageToMSLauncher(MSLC_INIT_PANNEL);

	if (g_PannelSetting.iTVWallMode == TVWALL_DISABLE)
	{
		iWidth = 0;
		iHeight = 0;
		for (int x = 0; x < g_DispArys.GetCount(); x++)
		{
			iWidth += g_DispArys.GetAt(x).Width();

			if (iHeight < g_DispArys.GetAt(x).Height())
				iHeight = g_DispArys.GetAt(x).Height();
		}
	}
	else
	{
		iWidth *= g_PannelSetting.iTVNumH;
		iHeight *= g_PannelSetting.iTVNumV;
	}
	SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_HIDEWINDOW);

	for (int x = 0; x < MAX_PANNEL; x++)
	{
		g_PannelSetting.fOffsetX[x] = 0;
		g_PannelSetting.fOffsetY[x] = 0;
		g_PannelSetting.fRatio[x] = 1;
		g_PannelSetting.iEffect[x] = 0;
	}

	//g_PannelSetting.bFullScreen = FALSE;
	//g_PannelSetting.bNotShowModal = FALSE;
	g_PannelSetting.dStartShowTick = 0;
	//m_pInitThread = AfxBeginThread(InitThread, this);

//#ifdef _ENABLE_GPU
	if (m_iSplit == -1)
	{
		int iWidth = GetSystemMetrics(SM_CXSCREEN);
		int iHeight = GetSystemMetrics(SM_CYSCREEN);

		CString strTemp;

		if (!g_PannelSetting.iTVWallMode)
			m_iSplit = m_iMonitorNum;
		else
			m_iSplit = g_PannelSetting.iTVNumV * g_PannelSetting.iTVNumH;

#ifdef _TRAIL_VERSION
		if (m_iSplit >= 2)
			m_iSplit = 2;
#endif
		g_PannelSetting.iPannelNum = m_iSplit;
		CStringArray strVideoAry;

		for (int x = 0; x < m_iSplit; x++)
		{
			strVideoAry.RemoveAll();

			if (g_PannelSetting.iTestPattern != 0)
			{
				strTemp.Format("%spic\\Patten%i.jpg", theApp.m_strCurPath, g_PannelSetting.iTestPattern);
				strVideoAry.Add(strTemp);
			}
			else if (!g_PannelSetting.strVideoPath.IsEmpty())
			{
				strVideoAry.Add(g_PannelSetting.strVideoPath);
			}
			else
			{
				/*
				while (fileDlg.DoModal() == IDOK)
				{
					strVideoAry.Add(fileDlg.GetPathName());
				}
				*/
			}

			//if (g_PannelAry.GetCount() == 0)
				//OnPaint();

#ifdef PLAY_3D_MODE
			if (g_PannelSetting.iTVWallMode)
			{
				if (x != 0)
					strVideoAry.RemoveAll();

				g_PannelAry[x]->ShowTempVideo(strVideoAry);
			}
			else if (0)
			{
				strVideoAry.RemoveAll();
				if (x != 0)
				{
					g_PannelAry[x]->ShowVideoAry(strVideoAry);
				}
				else
					g_PannelAry[x]->ShowTempVideo(strVideoAry);
			}
#else
//			g_PannelAry[x]->ShowVideoAry(strVideoAry);

#endif					
			}
		}
	//m_FramePic->ShowWindow(SW_SHOW);
//#endif

	InitFunction();

//#ifdef _ENABLE_GPU
	//OnShowWindow;
	for (int x = 0; x < g_DispArys.GetCount(); x++)
	{
		g_PannelAry[x]->ShowWindow(SW_HIDE);
		//g_PannelAry[x]->OnShowWindow(false);
	}
	m_FramePic->ShowWindow(SW_SHOW);
//#endif

	SetMessageToMSLauncher(MSLC_INITIALIZE_HAS_FINISHED);

#ifdef _ENABLE_VIDEO_WALL
	m_pSynCtrl->Start();
#endif

#if (_ENABLE_CHECK_KEYPRO && _TRAIL_VERSION != 1)
/*
	m_iTimeCount = 0;
	m_bTimerIsSuppend = false;
	SetTimer(103, 1000, NULL);
	*/
	m_bCheckKeyPro = true;
	unsigned threadID1;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _CheckKeyPro_ThreadProc, this, 0, &threadID1);
#endif

	if (m_pSyncPBCtrl)
	{
		m_pSyncPBCtrl->Init();
		m_pSyncPBCtrl->Start();
	}

	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		g_PannelAry[i]->ApplySetting();
	}

	if (theApp.m_bSwitchToShape)
	{
		m_bDoSwitchToShape = true;
		SetTimer(102,1000,NULL);
	}

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CMediaServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CMediaServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else if (theApp.m_iAppStatus == 1)
	{
		CDialog::OnPaint();
		CClientDC cdc(this);

		CBrush brhBlack(RGB(0,0,0));
		RECT rectCur;
		GetWindowRect(&rectCur);
		cdc.FillRect(&rectCur, &brhBlack);
		
		if (m_FramePic)
		{
			HWND hWnd = m_FramePic->GetWnd();
			if (!g_PannelSetting.bDisableUI && hWnd != NULL)
				//m_FramePic->Redraw(true, Redraw_BGImage);
				m_FramePic->DrawSpecifyImage(theApp.m_strCurPath + "UI Folder\\Main UI\\Loading.bmp");

#ifdef _ENABLE_GPU
#else
			/*
			if (m_iSplit > 0 && g_PannelAry.GetCount() == 0)
			{
				CPanel *pPannel = NULL;
				if (g_PannelSetting.iTVWallMode != TVWALL_DISABLE)
				{
					if (g_PannelSetting.iTVWallMode == TVWALL_MULTI_THREAD || g_PannelSetting.iTVWallMode == TVWALL_PARTITION)
					{
						pPannel = new CPanel(this);
						RECT rectPannel;

						rectPannel.left = 0;
						rectPannel.top = 0;

						rectPannel.right = g_DispArys.GetAt(0).Width() * g_PannelSetting.iTVNumH;
						rectPannel.bottom = g_DispArys.GetAt(0).Height() * g_PannelSetting.iTVNumV;

						pPannel->SetIndex(0);
						//m_pPannel[0].Create("", WS_CHILD | BS_ICON, rectPannel, this, 0);
						pPannel->Create(IDD_TEMP_DLG, this);
						pPannel->SetWindowPos(this, 0, 0, rectPannel.right, rectPannel.bottom, SWP_SHOWWINDOW);
						g_PannelAry.Add(pPannel);
					}
					else
					{
						//m_pPannel = new CPanel[g_PannelSetting.iTVNumH * g_PannelSetting.iTVNumV];
						RECT rectPannel;

						rectPannel.left = 0;
						rectPannel.top = 0;
						rectPannel.right = g_DispArys.GetAt(0).Width() * g_PannelSetting.iTVNumH;
						rectPannel.bottom = g_DispArys.GetAt(0).Height() * g_PannelSetting.iTVNumV;

						INT32 iIndex = 0;

						for (int x = 0; x < g_PannelSetting.iTVNumH; x++)
						for (int y = 0; y < g_PannelSetting.iTVNumV; y++)
						{
							rectPannel.left = g_DispArys.GetAt(0).Width() * x;
							rectPannel.right = g_DispArys.GetAt(0).Width() * (x + 1);
							rectPannel.top = g_DispArys.GetAt(0).Height() * y;
							rectPannel.bottom = g_DispArys.GetAt(0).Height() * (y + 1);

							pPannel = new CPanel(this);
							pPannel->SetIndex(iIndex);
							//m_pPannel[iIndex].Create("", WS_CHILD | BS_ICON, rectPannel, this, 0);
							pPannel->Create(IDD_TEMP_DLG, this);
							pPannel->SetWindowPos(this, rectPannel.left, rectPannel.top,
								g_DispArys.GetAt(0).Width(), g_DispArys.GetAt(0).Height(), SWP_SHOWWINDOW);

							g_PannelAry.Add(pPannel);
							iIndex++;
						}
					}
				}
				else
				{
					//m_pPannel = new CPanel[g_DispArys.GetCount()];
					CRect rectPannel;

					//rectPannel.left = g_DispArys.GetAt(0).Width();
					for (int x = 0; x < g_DispArys.GetCount(); x++)
					{
						rectPannel.top = 0;
						rectPannel.right = rectPannel.left + g_DispArys.GetAt(x).Width();
						rectPannel.bottom = rectPannel.top + g_DispArys.GetAt(x).Height();
							 
						pPannel = new CPanel(this);
						if (x == 0)
							pPannel->ShapeUI(TRUE);

						//m_pPannel[x].Create("", WS_CHILD | BS_ICON, rectPannel, this, x);
						pPannel->Create(IDD_TEMP_DLG, this);
						pPannel->SetWindowPos(this, rectPannel.left, rectPannel.top,
							rectPannel.Width(), rectPannel.Height(), SWP_SHOWWINDOW);

						rectPannel.left = rectPannel.right;

						pPannel->SetIndex(x);
						g_PannelAry.Add(pPannel);
					}

					for (int x = 0; x < g_DispArys.GetCount(); x++)
						g_PannelAry[x]->ShowWindow(SW_HIDE);

					g_PannelAry.GetAt(0)->LoadPorejct();
				}
				return;
			}
			*/
#endif
		}
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CMediaServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMediaServerDlg::EnterShapeMode(bool bCheckSource)
{
	if (bCheckSource)
	{
		BOOL bHadPlayed = FALSE;
		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsOpened())
			{
				bHadPlayed = TRUE;
				break;
			}
		}

		if (!bHadPlayed)
		{
			MessageBox("Need to Add Timeline, and Play Media First", "Alarm", MB_TASKMODAL); //MB_TASKMODAL | MB_TOPMOST
			GetParent()->SetFocus();
			return;
		}
	}
	
	// TODO: 在此加入控制項告知處理常式程式碼
	CFileDialog fileDlg(TRUE, "", "", OFN_FILEMUSTEXIST , "*(*.*)|*.*|", NULL, sizeof(OPENFILENAME));
							
	//if(strVideoAry.GetCount() > 0)
	if (m_iSplit == -1)
	{
		int iWidth = GetSystemMetrics(SM_CXSCREEN);
		int iHeight = GetSystemMetrics(SM_CYSCREEN);
		
		CString strTemp;

		if (!g_PannelSetting.iTVWallMode)
			m_iSplit = m_iMonitorNum;
		else
			m_iSplit = g_PannelSetting.iTVNumV * g_PannelSetting.iTVNumH;
		g_PannelSetting.iPannelNum = m_iSplit;
		CStringArray strVideoAry;

		for (int x = 0; x < m_iSplit; x++)
		{
			strVideoAry.RemoveAll();

			if (g_PannelSetting.iTestPattern != 0)
			{
				strTemp.Format("%spic\\Patten%i.jpg", theApp.m_strCurPath, g_PannelSetting.iTestPattern);
				strVideoAry.Add(strTemp);
			}
			else if (!g_PannelSetting.strVideoPath.IsEmpty())
			{
				strVideoAry.Add(g_PannelSetting.strVideoPath);
			}
			else
			{
				while (fileDlg.DoModal() == IDOK)
				{
					strVideoAry.Add(fileDlg.GetPathName());
				}
			}

			if (g_PannelAry.GetCount() == 0)
			{
				//OnPaint();
				SendMessage(WM_PAINT);
			}
						
#ifdef PLAY_3D_MODE
			if (g_PannelSetting.iTVWallMode)
			{
				if (x != 0)
					strVideoAry.RemoveAll();

				g_PannelAry[x]->ShowTempVideo(strVideoAry);
			}
			else if (0)
			{
				strVideoAry.RemoveAll();
				if (x != 0)
				{
					g_PannelAry[x]->ShowVideoAry(strVideoAry);
				}
				else
					g_PannelAry[x]->ShowTempVideo(strVideoAry);
			}
#else
			g_PannelAry[x]->ShowVideoAry(strVideoAry);

#endif					
		}
		
		m_FramePic->ShowWindow(SW_HIDE);

		for (int x = 0; x < m_iSplit; x++)
		{
			g_PannelAry[x]->RunVideo(TRUE);
			g_PannelAry[x]->ShowWindow(SW_SHOW);
			if (x == 0)
				g_PannelAry[x]->SetFocus();
		}
		
		if (0)
		{
			m_dlgUI.Create(IDD_MODIFYUI, this);
			m_dlgUI.ShowWindow(SW_SHOW);
			m_dlgUI.SetForegroundWindow();
			m_dlgUI.SetWindowPos(this, 0, 0, GetSystemMetrics(SM_CXSCREEN), 500, SWP_HIDEWINDOW);
			//CRgn NullRgn;
			//NullRgn.CreateRectRgn(0, 0, 0, 0);
			//dlgUI.SetWindowRgn(NullRgn, TRUE);
			m_dlgUI.BringWindowToTop();
		}
	}
	else
	{
		for (int x = 0; x < m_iSplit; x++)
		{
			g_PannelAry[x]->ShowWindow(SW_SHOW);
		}
	}

#ifdef _ENABLE_GPU
	for (INT x = 0; x < g_MediaStreamAry.size(); x++)
	{
		g_MediaStreamAry.at(x)->SetSourceMode(SM_SHAPE);
	}
#endif
}

void CMediaServerDlg::EnterShapeMode2()
{
	unsigned threadID1;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _EnterShape_ThreadProc, this, 0, &threadID1);
}

void CMediaServerDlg::EnterMixerMode()
{
#ifdef ENABLE_UI_CTRL
	//m_pUIController = new UIController;

	m_pUIController->OpenSimpleMixer();
	m_FramePic->ShowWindow(SW_HIDE);
#else

#ifdef _MIXER2
	BOOL bHadPlayed = FALSE;
	for (int x = 0; x < g_MediaStreamAry.size(); x++)
	{
		if (g_MediaStreamAry.at(x)->IsOpened())
		{
			bHadPlayed = TRUE;
			break;
}
	}
	if (!bHadPlayed)
	{
		MessageBox("Need to Add Timeline, and Play Media First", "Alarm", MB_TASKMODAL);
		GetParent()->SetFocus();
		return;
	}
	m_dlgMixer2.ShowWindow(SW_SHOW);;
	m_dlgMixer2.SetFocus();
#else
	m_dlgMixerCtrl.ShowWindow(SW_SHOW);
	m_dlgMixerCtrl.SetFocus();
#endif
	m_FramePic->ShowWindow(SW_HIDE);

#endif
}

void CMediaServerDlg::EnterTimeLine()
{
	m_dlgTimeLine.ShowWindow(SW_SHOW);
	m_dlgTimeLine.SetFocus();
	m_dlgTimeLine.ReDraw();
	m_dlgTimeLine.RunNetwork();
	m_FramePic->ShowWindow(SW_HIDE);

#ifdef _ENABLE_GPU
	for (INT x = 0; x < g_MediaStreamAry.size(); x++)
	{
		g_MediaStreamAry.at(x)->SetSourceMode(SM_TIMELINE);
	}
#endif
}

void CMediaServerDlg::EnterMediaCtrl(BOOL bFullScreen,void* pCallback)
{
#ifdef ENABLE_UI_CTRL
	if (pCallback)
		m_pUIController->SetMediaFileManagerCallback((MediaFileManagerCallback *)pCallback);
	else
		m_pUIController->SetMediaFileManagerCallback(this);
#else
	if (pCallback)
		m_SimpleMediaCtrl.SetMediaFileManagerCallback((MediaFileManagerCallback *)pCallback);
	else
		m_SimpleMediaCtrl.SetMediaFileManagerCallback(this);
#endif

	if (bFullScreen)
	{
		m_dlgMediaCtrl.ShowWindow(SW_SHOW);
		m_dlgMediaCtrl.SetFocus();
		m_FramePic->ShowWindow(SW_HIDE);
	}
	else
	{
#ifdef ENABLE_UI_CTRL
		m_pUIController->OpenSimpleMediaCtrl();
#else
		m_SimpleMediaCtrl.Show(true);
#endif
	}

}

void CMediaServerDlg::OnDestroy()
{
	
	// TODO: 在此加入您的訊息處理常式程式碼

	m_bCheckKeyPro = false;

	if (m_pKPCheckerCtrl)
		delete m_pKPCheckerCtrl;

	Add2LogFile(LL_INFO, "CMediaServerDlg::OnDestroy() - begin");

#ifdef _ENABLE_VIDEO_WALL
	m_pSynCtrl->Stop();
#endif

	if (m_pSyncPBCtrl)
		m_pSyncPBCtrl->Stop();

	/*
	HWND hMSLauncher = ::FindWindow("MS_WatchDog_Win", NULL);
	if (hMSLauncher)
	{
		COPYDATASTRUCT copy_data;
		copy_data.dwData = MSLC_EXIT;
		copy_data.cbData = 0;
		copy_data.lpData = 0;

		::SendMessage(hMSLauncher, WM_COPYDATA, 0, (LPARAM)&copy_data);
	}
	*/
	SetMessageToMSLauncher(MSLC_EXIT);

	//m_SimpleMediaCtrl.~SimpleMediaCtrl();
	CoUninitialize();

//	if (m_pLogFileDll)
//		delete m_pLogFileDll;

	if (m_pSIniFileW)
		delete m_pSIniFileW;

#ifdef ENABLE_UI_CTRL
	delete m_pUIController;
#endif

	if (m_pExceptionReport)
		delete m_pExceptionReport;

#ifdef _ENABLE_VIDEO_WALL
	if(m_pSynCtrl)
		delete m_pSynCtrl;
#endif

	if (m_pSyncPBCtrl)
		delete m_pSyncPBCtrl;

	CDialog::OnDestroy();

	Add2LogFile(LL_INFO, "CMediaServerDlg::OnDestroy() - end");
}

void CMediaServerDlg::ReleaseResource()
{
	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - begin");

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 1");

	for (INT x = 0; x < g_MediaStreamAry.size(); x++)
	{
		g_MediaStreamAry.at(x)->ReleaseDispManagerAry();
	}

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 2");

	if (g_PannelAry.GetCount() > 0 && g_PannelAry[0]->m_hWnd)
	{
		if (g_PannelSetting.iTVWallMode)
			g_PannelAry[0]->SendMessage(WM_CLOSE);
		else
		{
			for (int x = 0; x < g_PannelAry.GetCount(); x++)
			{
				g_PannelAry[x]->SendMessage(WM_CLOSE);
			}
		}
	}

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 3");
#ifdef _MIXER2
	if (m_dlgMixer2)
		m_dlgMixer2.SendMessage(WM_CLOSE);
#else
	if (m_dlgMixerCtrl)
		m_dlgMixerCtrl.SendMessage(WM_CLOSE);
#endif

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 4");

	if (m_dlgTimeLine)
		m_dlgTimeLine.SendMessage(WM_CLOSE);

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 5");

	if (m_dlgMediaCtrl)
		m_dlgMediaCtrl.SendMessage(WM_CLOSE);

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 6");

	if (m_dlgRadioMenu)
		m_dlgRadioMenu.SendMessage(WM_CLOSE);

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 7");

	if(g_WaitingDlg)
		g_WaitingDlg.SendMessage(WM_CLOSE);

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 8");

	if (m_FramePic)
		delete m_FramePic;

	m_FramePic = NULL;

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 9");

#ifdef _DISABLE_HASP
#else
	if (handle)
		status = hasp_logout(handle);
#endif

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 10");
	//m_SimpleMediaCtrl.SetMediaFileManagerCallback(NULL);

	if (g_PannelSetting.iEncodeMedia != 0)
	{	
		CString strTemp;
		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsExtraVideo())
				continue;
			else if (g_PannelSetting.iEncodeMedia == 1)
			{
				g_MediaStreamAry.at(x)->Close();
				//strTemp.Format("C:\\Windows\\Log\\MS%i.mov", x);

				//if (PathFileExists(g_MediaStreamAry.at(x)->FilePath()))
				//CFile::Remove(g_MediaStreamAry.at(x)->FilePath());
			}
			else if (g_PannelSetting.iEncodeMedia == 2)
			{
				CString strPath = g_MediaStreamAry.at(x)->FilePath();
				g_MediaStreamAry.at(x)->Close();
#ifdef PLAY_3D_MODE
				if (!strPath.IsEmpty())
					CPlaneScene3D::FileEncoder(g_MediaStreamAry.at(x)->FilePath(), FALSE, TRUE);
#endif
			}
		}
	}

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - 11");

	ClipCursor(NULL);

	Add2LogFile(LL_INFO, "CMediaServerDlg::ReleaseResource() - end");
}

void CMediaServerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CClientDC cdc(this);
	CString strTemp;
	strTemp.Format("(%04i, %04i)", point.x, point.y);
	//cdc.TextOutA(50, 50, strTemp);

	if (m_dlgUI && g_PannelAry.GetCount() > 0)
	{
		if (point.y < 10 &&!m_dlgUI.IsWindowVisible())
		{
			m_dlgUI.ShowWindow(SW_SHOW);
		}
		else if (point.y > 400 && m_dlgUI.IsWindowVisible())
			m_dlgUI.ShowWindow(SW_HIDE);
	}

	if (1) 
	{
		CRect rectDisp = g_DispArys.GetAt(0);
	}

	CDialog::OnMouseMove(nFlags, point);
}
  
BOOL CMediaServerDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	char szMsg[256];

	CString strTemp;
	strTemp.Format("Media Server Dlg, %x, %x, %x", message, wParam, lParam);
	theApp.SaveLog(strTemp);

	if (message == 0x82)
		message = message;

	switch (message)
	{
		case MEDIA_ITEM:
			EnterMediaCtrl(TRUE);
			if (m_dlgRadioMenu)
				m_dlgRadioMenu.BringWindowToTop();
			break;
		case MIXER_ITEM:
			if (m_FramePic)
				m_FramePic->OnEnable(false);
			EnterMixerMode();
			if (m_dlgRadioMenu)
				m_dlgRadioMenu.BringWindowToTop();
			if (m_FramePic)
				m_FramePic->OnEnable(true);
			break;
		case SHAPE_ITEM:
#ifdef _DISABLE_HASP
#else //_DISABLE_HASP

#if (_TRAIL_VERSION != 1)
			if (status != HASP_STATUS_OK)
				PostMessage(WM_CLOSE);
			else
#endif   //_TRAIL_VERSION

#endif  //_DISABLE_HASP
			{
				//EnterShapeMode();
				EnterShapeMode2();
				if (m_dlgRadioMenu)
					m_dlgRadioMenu.BringWindowToTop();
			}
			break;
		case TLINE_ITEM:
			EnterTimeLine();
			if (m_dlgRadioMenu)
				m_dlgRadioMenu.BringWindowToTop();
			break;
		case OUTPT_ITEM:
			break;
		case SYSTM_ITEM:
			{
				if (m_FramePic)
					m_FramePic->OnEnable(false);
				CSettingsDlg dlg;
				dlg.SetMainUIHandle(this->GetSafeHwnd());
				::EnableWindow(this->GetSafeHwnd(), false);
				if (dlg.DoModal() == IDOK)
				{
				}
				if (m_FramePic)
					m_FramePic->OnEnable(true);
			}
			break;
		case POWER_ITEM:
			PostMessage(WM_CLOSE);
			break;
		case MSG_MAINUI:
		{
			RECT rect;
			::GetWindowRect(this->GetSafeHwnd(), &rect);
			sprintf(szMsg, "#MS_KeyUp# MSG_MAINUI (0) [%d , %d , %d , %d]\n", rect.left, rect.top, rect.right, rect.bottom);
			OutputDebugStringA(szMsg);

			if (m_FramePic)
			{
				sprintf(szMsg, "#MS_KeyUp# MSG_MAINUI (1)\n");
				OutputDebugStringA(szMsg);

				SetWinPos(this->GetSafeHwnd(), 0, 0,true);

				m_FramePic->ShowWindow(SW_HIDE);
				m_FramePic->ShowWindow(SW_SHOW);

				sprintf(szMsg, "#MS_KeyUp# MSG_MAINUI (2)\n");
				OutputDebugStringA(szMsg);
			}

			sprintf(szMsg, "#MS_KeyUp# MSG_MAINUI (3)\n");
			OutputDebugStringA(szMsg);

			SetFocus();

			sprintf(szMsg, "#MS_KeyUp# MSG_MAINUI (4)\n");
			OutputDebugStringA(szMsg);
		}
			break;
		case MSG_SELECT_MEDIA:
			{
				EnterMediaCtrl(FALSE);
			}
			break;
		case WM_COPYDATA:
			{
				COPYDATASTRUCT * p = (COPYDATASTRUCT*)lParam;
				switch (p->dwData)
				{
					case MSG_SELECT_MEDIA:
						{
							CopyDataInfo* pCopyDataInfo = (CopyDataInfo*)p->lpData;
							EnterMediaCtrl(FALSE, (MediaFileManagerCallback*)pCopyDataInfo->pObj);

							m_dlgTimeLine.EnableWindow(false);
						}
						break;
				}
			}
			break;
		default:
			break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CMediaServerDlg::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	Add2LogFile(LL_INFO, "CMediaServerDlg::OnClose() - begin");

#ifdef _ENABLE_VIDEO_WALL
	m_pSynCtrl->Stop();
#endif

	if (m_pSyncPBCtrl)
		m_pSyncPBCtrl->Stop();

	m_dlgTimeLine.DoPause();

	ReleaseResource();
	CDialog::OnCancel();
	Add2LogFile(LL_INFO, "CMediaServerDlg::OnClose() - end");
}


void CMediaServerDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	return;
	ReleaseResource();
	CDialog::OnCancel();
}


void CMediaServerDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	return;
	ReleaseResource();
	CDialog::OnOK();
}

void CMediaServerDlg::ReturnFileName(wchar_t* wszFileName)
{
	theApp.m_strSelVideoPath = wszFileName;

	if (theApp.m_strSelVideoPath.IsEmpty())
	{
		theApp.m_strSelVideoPath = "NULL";
	}
}

//#ifdef _ENABLE_GPU
void CMediaServerDlg::InitShapeUI()
{
	if (m_iSplit > 0 && g_PannelAry.GetCount() == 0)
	{
		CPanel *pPannel = NULL;
		if (g_PannelSetting.iTVWallMode != TVWALL_DISABLE)
		{
			if (g_PannelSetting.iTVWallMode == TVWALL_MULTI_THREAD || g_PannelSetting.iTVWallMode == TVWALL_PARTITION)
			{
				pPannel = new CPanel(this);
				RECT rectPannel;

				rectPannel.left = 0;
				rectPannel.top = 0;

				rectPannel.right = g_DispArys.GetAt(0).Width() * g_PannelSetting.iTVNumH;
				rectPannel.bottom = g_DispArys.GetAt(0).Height() * g_PannelSetting.iTVNumV;

				//pPannel->SetIndex(0);
				//m_pPannel[0].Create("", WS_CHILD | BS_ICON, rectPannel, this, 0);

#ifdef _ENABLE_CWND
				pPannel->Create();
				pPannel->Init();
				pPannel->SetIndex(0);
#else
				pPannel->SetIndex(0);
				pPannel->Create(IDD_TEMP_DLG, this);
#endif
				//pPannel->SetWindowPos(this, 0, 0, rectPannel.right, rectPannel.bottom, SWP_SHOWWINDOW);
				pPannel->SetWindowPos(this, 0, 0, rectPannel.right, rectPannel.bottom, SWP_HIDEWINDOW);

				pPannel->SetSyncPBCtrl(m_pSyncPBCtrl);

				g_PannelAry.Add(pPannel);
			}
			else
			{
				//m_pPannel = new CPanel[g_PannelSetting.iTVNumH * g_PannelSetting.iTVNumV];
				RECT rectPannel;

				rectPannel.left = 0;
				rectPannel.top = 0;
				rectPannel.right = g_DispArys.GetAt(0).Width() * g_PannelSetting.iTVNumH;
				rectPannel.bottom = g_DispArys.GetAt(0).Height() * g_PannelSetting.iTVNumV;

				INT32 iIndex = 0;

				for (int x = 0; x < g_PannelSetting.iTVNumH; x++)
					for (int y = 0; y < g_PannelSetting.iTVNumV; y++)
					{
						rectPannel.left = g_DispArys.GetAt(0).Width() * x;
						rectPannel.right = g_DispArys.GetAt(0).Width() * (x + 1);
						rectPannel.top = g_DispArys.GetAt(0).Height() * y;
						rectPannel.bottom = g_DispArys.GetAt(0).Height() * (y + 1);

						pPannel = new CPanel(this);
						//pPannel->SetIndex(iIndex);
						//m_pPannel[iIndex].Create("", WS_CHILD | BS_ICON, rectPannel, this, 0);
#ifdef _ENABLE_CWND
						pPannel->Create();
						pPannel->Init();
						pPannel->SetIndex(iIndex);
						::SetWindowPos(pPannel->GetSafeHwnd(), HWND_TOPMOST, rectPannel.left, rectPannel.top, g_DispArys.GetAt(0).Width(), g_DispArys.GetAt(0).Height(), SWP_HIDEWINDOW);
#else
						pPannel->SetIndex(iIndex);
						pPannel->Create(IDD_TEMP_DLG, this);

						pPannel->SetWindowPos(this, rectPannel.left, rectPannel.top,
							//g_DispArys.GetAt(0).Width(), g_DispArys.GetAt(0).Height(), SWP_SHOWWINDOW);
							g_DispArys.GetAt(0).Width(), g_DispArys.GetAt(0).Height(), SWP_HIDEWINDOW);
#endif
						

						pPannel->SetSyncPBCtrl(m_pSyncPBCtrl);

						g_PannelAry.Add(pPannel);
						iIndex++;
					}
			}
		}
		else
		{
			//m_pPannel = new CPanel[g_DispArys.GetCount()];
			CRect rectPannel;

#ifdef _ENABLE_MONITOR_SIMULATION_MODE
//#define _DEBUG_MONITORS 3
//rectPannel.left = g_DispArys.GetAt(0).Width();

			int iDisW = 1920 / 2;
			int iDisH = 1080 / 2;
			int iDisX = 1920;
			int iDisY = 0;
			
			if (g_DispArys.GetCount() < _DEBUG_MONITORS)
			{
				for (int x = 0; x < _DEBUG_MONITORS; x++)
				{
					rectPannel.top = 0;

					//if (x >= g_DispArys.GetCount())
					{
						int iIndex = g_DispArys.GetCount() - 1;

						//rectPannel.right = rectPannel.left + g_DispArys.GetAt(iIndex).Width();
						//rectPannel.bottom = rectPannel.top + g_DispArys.GetAt(iIndex).Height();

						if (x == 0)
						{
							rectPannel.top = 0;
							rectPannel.right = rectPannel.left + g_DispArys.GetAt(x).Width();
							rectPannel.bottom = rectPannel.top + g_DispArys.GetAt(x).Height();
						}
						else if (x == 1)
						{
							rectPannel.left = iDisX;
							rectPannel.top = iDisY;

							rectPannel.right = rectPannel.left + iDisW;
							rectPannel.bottom = rectPannel.top + iDisH;
						}
						else if (x == 2)
						{
							rectPannel.left = iDisX + iDisW;
							rectPannel.top = iDisY;

							rectPannel.right = rectPannel.left + iDisW;
							rectPannel.bottom = rectPannel.top + iDisH;
						}
						else if (x == 3)
						{
							rectPannel.left = iDisX;
							rectPannel.top = iDisY + iDisH;

							rectPannel.right = rectPannel.left + iDisW;
							rectPannel.bottom = rectPannel.top + iDisH;
						}
						else if (x == 4)
						{
							rectPannel.left = iDisX + iDisW;
							rectPannel.top = iDisY + iDisH;

							rectPannel.right = rectPannel.left + iDisW;
							rectPannel.bottom = rectPannel.top + iDisH;
						}

					}
					//else
					//{
						//rectPannel.right = rectPannel.left + g_DispArys.GetAt(x).Width();
						//rectPannel.bottom = rectPannel.top + g_DispArys.GetAt(x).Height();
					//}

#ifdef _ENABLE_CWND
					pPannel = new CPanel(this);
					if (x == 0)
						pPannel->ShapeUI(TRUE);

					pPannel->Create();
					pPannel->Init();

					::SetWindowPos(pPannel->GetSafeHwnd(), HWND_TOP, rectPannel.left, rectPannel.top, rectPannel.Width(), rectPannel.Height(), SWP_HIDEWINDOW);
#else
					pPannel = new CPanel(this);
					if (x == 0)
						pPannel->ShapeUI(TRUE);

					//m_pPannel[x].Create("", WS_CHILD | BS_ICON, rectPannel, this, x);
					pPannel->Create(IDD_TEMP_DLG, this);
					pPannel->SetWindowPos(this, rectPannel.left, rectPannel.top,
						//rectPannel.Width(), rectPannel.Height(), SWP_SHOWWINDOW);
						rectPannel.Width(), rectPannel.Height(), SWP_HIDEWINDOW);
#endif
					rectPannel.left = rectPannel.right;

					pPannel->SetIndex(x);
					pPannel->SetSyncPBCtrl(m_pSyncPBCtrl);
					g_PannelAry.Add(pPannel);
				}

				for (int x = 0; x < _DEBUG_MONITORS; x++)
					g_PannelAry[x]->ShowWindow(SW_HIDE);
			}

			//for (int x = 0; x < g_DispArys.GetCount(); x++)
			//	g_PannelAry[x]->ShowWindow(SW_HIDE);
#else
			//rectPannel.left = g_DispArys.GetAt(0).Width();
			for (int x = 0; x < g_DispArys.GetCount(); x++)
			{

#ifdef _TRAIL_VERSION
				if (x >= 2)
					continue;
#endif
				rectPannel.top = 0;
				rectPannel.right = rectPannel.left + g_DispArys.GetAt(x).Width();
				rectPannel.bottom = rectPannel.top + g_DispArys.GetAt(x).Height();

				pPannel = new CPanel(this);
				if (x == 0)
					pPannel->ShapeUI(TRUE);

				//m_pPannel[x].Create("", WS_CHILD | BS_ICON, rectPannel, this, x);
#ifdef _ENABLE_CWND
				pPannel->Create();
				pPannel->Init();

				if (x == 0)
					::SetWindowPos(pPannel->GetSafeHwnd(), HWND_TOP, rectPannel.left, rectPannel.top, rectPannel.Width(), rectPannel.Height(), SWP_HIDEWINDOW);
				else
					::SetWindowPos(pPannel->GetSafeHwnd(), HWND_TOP, rectPannel.left, rectPannel.top, rectPannel.Width(), rectPannel.Height(), SWP_HIDEWINDOW);
#else
				pPannel->Create(IDD_TEMP_DLG, this);
				pPannel->SetWindowPos(this, rectPannel.left, rectPannel.top,
					//rectPannel.Width(), rectPannel.Height(), SWP_SHOWWINDOW);
					rectPannel.Width(), rectPannel.Height(), SWP_HIDEWINDOW);
#endif
				

				rectPannel.left = rectPannel.right;

				pPannel->SetIndex(x);
				pPannel->SetSyncPBCtrl(m_pSyncPBCtrl);
				g_PannelAry.Add(pPannel);
			}

			for (int x = 0; x < g_DispArys.GetCount(); x++)
				g_PannelAry[x]->ShowWindow(SW_HIDE);
#endif

			g_PannelAry.GetAt(0)->LoadPorejct();
		}
		return;
	}
}
//#endif

void CMediaServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (m_bDoSwitchToShape)
	{
		m_bDoSwitchToShape = false;
		EnterTimeLine();
		EnterShapeMode(false);

		m_dlgTimeLine.DoPlay();
	}

	__super::OnTimer(nIDEvent);
}

void CMediaServerDlg::CheckLogStatus()
{
	m_bEnableLog = GetPrivateProfileInt("LogFile", "MediaServerDlg", 0, theApp.m_strCurPath + "Setting.ini");

	if (m_bEnableLog)
	{
		if (m_pLogFileDll)
		{
			m_pLogFileObj = m_pLogFileDll->_Out_LGD_Create();
			m_pLogFileDll->_Out_LGD_Init(m_pLogFileObj, "MediaServerDlg");

			theApp.SetLogFileInfo(m_bEnableLog, m_pLogFileObj);
		}
	}
	else
		theApp.SetLogFileInfo(false, 0);
}

void CMediaServerDlg::Add2LogFile(int iLevel, char* szMsg)
{
	if (m_bEnableLog && m_pLogFileDll && m_pLogFileObj)
	{
		m_pLogFileDll->_Out_LGD_AddLog(m_pLogFileObj, iLevel, szMsg);
	}
}

void CMediaServerDlg::SetTip()
{
	m_dlgTimeLine.SetTip();
	m_dlgMediaCtrl.SetTip();
	g_PannelAry[0]->SetTip();
}

void CMediaServerDlg::SetMessageToMSLauncher(int iCommandID)
{
//	char szMsg[512];
	HWND hMSLauncher = ::FindWindow("MS_WatchDog_Win", NULL);

//	sprintf(szMsg, "#MS_Launcher# SetMessageToMSLauncher %d (0)\n", hMSLauncher);
//	OutputDebugStringA(szMsg);

	if (hMSLauncher)
	{
		COPYDATASTRUCT copy_data;
		copy_data.dwData = iCommandID;// MSLC_EXIT;
		copy_data.cbData = 0;
		copy_data.lpData = 0;

//		sprintf(szMsg, "#MS_Launcher# SetMessageToMSLauncher %d (1)\n", hMSLauncher);
//		OutputDebugStringA(szMsg);

		::SendMessage(hMSLauncher, WM_COPYDATA, 0, (LPARAM)&copy_data);
	}
}

#ifdef _ENABLE_CHECK_KEYPRO
void CMediaServerDlg::CheckKeyPro()
{
	DWORD dwLastTime = 0;
	while (m_bCheckKeyPro)
	{
		DWORD dwCurTime = timeGetTime();
		if (dwLastTime == 0)
			dwLastTime = dwCurTime;

		/*
		TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		if (m_manager->GetTCAdapter()->GetMode() == TCM_CLIENT)
			continue;
			*/
		if (dwCurTime >= dwLastTime + 1000 * 60)//40,33
		{
			dwLastTime = dwCurTime;

#if 0
			status = hasp_login(feature, vendor_code, &handle);
			if (status != HASP_STATUS_OK)
			{
				m_bCheckKeyPro = false;
				TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
				CWnd* pEditViewWnd = m_manager->GetEditViewWnd();
				::PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
				MessageBox("Cannot find dongle!!", "Warning", MB_OK | MB_TOPMOST);
				PostMessage(WM_CLOSE);
			}
#else
			if (m_pKPCheckerCtrl)
				m_pKPCheckerCtrl->Launch();
#endif
		}

		Sleep(1);
	}
}

unsigned int __stdcall _CheckKeyPro_ThreadProc(void* lpvThreadParm)
{
	CMediaServerDlg* pObj = (CMediaServerDlg *)lpvThreadParm;
	pObj->CheckKeyPro();
	return 0;
}

#endif

unsigned int __stdcall _EnterShape_ThreadProc(void* lpvThreadParm)
{
	CMediaServerDlg* pObj = (CMediaServerDlg *)lpvThreadParm;
	pObj->EnterShapeMode();
	return 0;
}


