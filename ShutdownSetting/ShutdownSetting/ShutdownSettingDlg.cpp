
// ShutdownSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShutdownSetting.h"
#include "ShutdownSettingDlg.h"
#include "afxdialogex.h"
#include "../../Include/sstring.h"

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


// CShutdownSettingDlg dialog



CShutdownSettingDlg::CShutdownSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SHUTDOWNSETTING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShutdownSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShutdownSettingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CShutdownSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CShutdownSettingDlg message handlers

BOOL CShutdownSettingDlg::OnInitDialog()
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
	UpdateUIStatus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CShutdownSettingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShutdownSettingDlg::OnPaint()
{
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
HCURSOR CShutdownSettingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CShutdownSettingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	CButton* pEnable = (CButton*)GetDlgItem(IDC_ENABLE);
	
	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath,"%s\\Setting.ini", szAPPath);

	sprintf(szData,"%d", pEnable->GetCheck());
	WritePrivateProfileStringA("Shutdown", "Enable", szData, szIniPath);

	if (pEnable->GetCheck())
	{
		SYSTEMTIME st;
		CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
		pStartTimeCtrl->GetTime(&st);
		
		sprintf(szData, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
		WritePrivateProfileStringA("Shutdown", "Time", szData, szIniPath);
	}
	CDialogEx::OnOK();
}

void CShutdownSettingDlg::UpdateUIStatus()
{
	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	char szTmp[256];
	int iIndex = 0;

	int iHour = 0;
	int iMinute = 0;
	int iSecond = 0;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\Setting.ini", szAPPath);

	int iEnable = GetPrivateProfileIntA("Shutdown", "Enable", 0, szIniPath);

	CButton* pEnableBtn = (CButton*)GetDlgItem(IDC_ENABLE);

	pEnableBtn->SetCheck(iEnable);

	BOOL bRet;
	if (iEnable == 1)
	{
		SYSTEMTIME st;

		GetSystemTime(&st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;

		GetPrivateProfileStringA("Shutdown", "Time", "", szData, 100, szIniPath);
		strcpy(szTmp, szData);

		sscanf(szTmp,"%02d:%02d:%02d", &iHour, &iMinute, &iSecond);

		st.wHour = iHour;
		st.wMinute = iMinute;
		st.wSecond = iSecond;
		
		CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
		bRet = pStartTimeCtrl->SetTime(&st);
	}
}
