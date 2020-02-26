
// TimeCodeServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TimeCodeServer.h"
#include "TimeCodeServerDlg.h"
#include "afxdialogex.h"

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


// CTimeCodeServerDlg dialog



CTimeCodeServerDlg::CTimeCodeServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TIMECODESERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeCodeServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTimeCodeServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_NETWORK, &CTimeCodeServerDlg::OnBnClickedStartNetwork)
	ON_BN_CLICKED(IDC_STOP_NETWORK, &CTimeCodeServerDlg::OnBnClickedStopNetwork)
	ON_BN_CLICKED(IDC_TC_STOP, &CTimeCodeServerDlg::OnBnClickedTcStop)
	ON_BN_CLICKED(IDC_TC_PLAY, &CTimeCodeServerDlg::OnBnClickedTcPlay)
	ON_BN_CLICKED(IDC_TC_PAUSE, &CTimeCodeServerDlg::OnBnClickedTcPause)

	ON_MESSAGE(WM_UPDATE_TIME_CODE_STR, OnUpdateTimeCodeStr)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTimeCodeServerDlg message handlers

BOOL CTimeCodeServerDlg::OnInitDialog()
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
	m_IPDetector.CheckIPAddress();

	//IDC_HOURS , IDC_MINUTES , IDC_SECONDS , 
	//IDC_IPADDRESS1 , IDC_PORT
	//IDC_TIME_CODE_STR , IDC_PROGRESS1
	SetDlgItemInt(IDC_HOURS,0);
	SetDlgItemInt(IDC_MINUTES, 0);
	SetDlgItemInt(IDC_SECONDS, 0);
	SetDlgItemInt(IDC_MS, 0);

	SetDlgItemInt(IDC_PORT, 2007);

	if (m_IPDetector.GetCount() > 0)
	{
		char* szIP = m_IPDetector.GetIPAddress(0);
		CIPAddressCtrl* pIPAddressCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);

		int iIP[4];
		sscanf(szIP, "%d.%d.%d.%d", &iIP[0], &iIP[1], &iIP[2], &iIP[3]);
		pIPAddressCtrl->SetAddress(iIP[0], iIP[1], iIP[2], iIP[3]);
	}

	m_pTCServerAgent = new TCServerAgent;
	m_pTCServerAgent->Init(this->GetSafeHwnd());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTimeCodeServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTimeCodeServerDlg::OnPaint()
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
HCURSOR CTimeCodeServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTimeCodeServerDlg::OnBnClickedStartNetwork()
{
	// TODO: Add your control notification handler code here
	int iPort = GetDlgItemInt(IDC_PORT);

	int iHours = GetDlgItemInt(IDC_HOURS);
	int iMinutes = GetDlgItemInt(IDC_MINUTES);
	int iSecs = GetDlgItemInt(IDC_SECONDS);
	int iMS = GetDlgItemInt(IDC_MS);

	unsigned long ulTimeCode = iMS + iSecs * 1000 + iMinutes * 1000 * 60 + iHours * 1000 * 60 * 60;

	m_pTCServerAgent->SetTotalOfTimeCode(ulTimeCode);
	
	CProgressCtrl* pProgressCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pProgressCtrl->SetRange32(0,ulTimeCode);
	pProgressCtrl->SetPos(0);
	
	m_pTCServerAgent->StartServer(iPort);
}


void CTimeCodeServerDlg::OnBnClickedStopNetwork()
{
	// TODO: Add your control notification handler code here
	m_pTCServerAgent->StopServer();
}


void CTimeCodeServerDlg::OnBnClickedTcStop()
{
	// TODO: Add your control notification handler code here
	m_pTCServerAgent->TC_Stop();
}


void CTimeCodeServerDlg::OnBnClickedTcPlay()
{
	// TODO: Add your control notification handler code here
	m_pTCServerAgent->TC_Start();
}


void CTimeCodeServerDlg::OnBnClickedTcPause()
{
	// TODO: Add your control notification handler code here
	m_pTCServerAgent->TC_Pause();
}

LRESULT CTimeCodeServerDlg::OnUpdateTimeCodeStr(WPARAM wParam, LPARAM lParam)
{
	double dTmp = wParam;
	double dTimeCode = dTmp / 1000.0;

	char* szTimeCodeStr = PosToTimeInfo(dTimeCode, true);

	CWnd* pWnd = GetDlgItem(IDC_TIME_CODE_STR);
	::SetWindowTextA(pWnd->GetSafeHwnd(), szTimeCodeStr);

	CProgressCtrl* pProgressCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pProgressCtrl->SetPos(wParam);

	return true;
}

char* CTimeCodeServerDlg::PosToTimeInfo(double dPos, bool bEnableMS)
{
	int iHour = dPos / (60 * 60);

	double dNum = dPos - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;

	if (bEnableMS)
	{
		double dNum2 = dNum1 - iSecond;
		int iMSec = dNum2 * 1000;

		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d:%03d", iHour, iMinute, iSecond, iMSec);
		else
			sprintf(m_szTime, "%02d:%02d:%03d", iMinute, iSecond, iMSec);
	}
	else
	{
		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d", iHour, iMinute, iSecond);
		else
			sprintf(m_szTime, "%02d:%02d", iMinute, iSecond);
	}
	return m_szTime;
}


void CTimeCodeServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if (m_pTCServerAgent)
		delete m_pTCServerAgent;
}
