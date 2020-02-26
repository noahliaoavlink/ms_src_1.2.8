
// ClientPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientPlayer.h"
#include "ClientPlayerDlg.h"
#include "afxdialogex.h"

#include "../../Include/StrConv.h"
#include "../../Include/SString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientPlayerDlg dialog



CClientPlayerDlg::CClientPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENTPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CClientPlayerDlg message handlers

BOOL CClientPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	int iIndex = 0;
	char szAPPath[512];
	char szIniFileName[512];
	char szSourceFileName[512];
	char szIP[32] = "127.0.0.1";
	int iPort = 2007;
	char szWinName[128];
	char szLayoutStr[32];
	char szDisplayMode[32];
	LPWSTR *szArglist;
	int nArgs;
	//int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	if (nArgs > 1)
	{
		strcpy(szLayoutStr, WCharToChar((const std::wstring&)szArglist[1]));
		char* p = WCharToChar((const std::wstring&)szArglist[2]);
		iIndex = atoi(p);
	}

	int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int iLayoutID = 0;
	for (int i = 0; i < g_iTotalOfLayoutItems; i++)
	{
		if (strcmp(g_LayoutItems[i].szText, szLayoutStr) == 0)
		{
			iLayoutID = g_LayoutItems[i].iID;
			break;
		}
	}

	m_LayoutCtrl.MakeInfo(iLayoutID, iScreenWidth, iScreenHeight);

	sprintf(szWinName, "TC_Client_Player-%d", GetTickCount());
	m_pNWin = new CNWin(szWinName);

	RECT* pRect = m_LayoutCtrl.GetGridRect(iIndex);
	
	::SetWindowPos(m_pNWin->GetSafeHwnd(), HWND_TOP, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, SWP_SHOWWINDOW);

	m_pClientPlayerCtrl = new ClientPlayerCtrl;
	m_pClientPlayerCtrl->Init(m_pNWin->GetSafeHwnd());

	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniFileName,"%s\\ClientPlayerConfig.ini", szAPPath);
	GetPrivateProfileString("Source", "File", "", szSourceFileName, 512, szIniFileName);

	GetPrivateProfileString("Server", "IP", "", szIP, 32, szIniFileName);
	iPort = GetPrivateProfileInt("Server", "Port", 0, szIniFileName);

	GetPrivateProfileString("Display", "Mode", "Normal", szDisplayMode, 32, szIniFileName);

	m_pClientPlayerCtrl->SetFileName(ANSIToUnicode(szSourceFileName));
	m_pClientPlayerCtrl->SetServerInfo(szIP,iPort);
	m_pClientPlayerCtrl->SetDisplayMode(szDisplayMode);
	m_pClientPlayerCtrl->SetLayoutInfo(iLayoutID, iIndex);

	m_pClientPlayerCtrl->Connect();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientPlayerDlg::OnPaint()
{
	ShowWindow(SW_HIDE);
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientPlayerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if (m_pNWin)
		delete m_pNWin;

	if (m_pClientPlayerCtrl)
	{
		m_pClientPlayerCtrl->Stop();
		delete m_pClientPlayerCtrl;
	}
}
