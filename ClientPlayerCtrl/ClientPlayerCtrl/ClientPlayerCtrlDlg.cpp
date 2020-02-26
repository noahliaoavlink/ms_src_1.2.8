
// ClientPlayerCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientPlayerCtrl.h"
#include "ClientPlayerCtrlDlg.h"
#include "afxdialogex.h"

#include "../../Include/LayoutInfo.h"

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


// CClientPlayerCtrlDlg dialog



CClientPlayerCtrlDlg::CClientPlayerCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENTPLAYERCTRL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientPlayerCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientPlayerCtrlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CClientPlayerCtrlDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_CLOSE, &CClientPlayerCtrlDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CClientPlayerCtrlDlg message handlers

BOOL CClientPlayerCtrlDlg::OnInitDialog()
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

	CComboBox* pLayout = (CComboBox*)GetDlgItem(IDC_LAYOUT);

	for (int i = 0; i < g_iTotalOfLayoutItems; i++)
		pLayout->AddString(g_LayoutItems[i].szText);

	pLayout->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientPlayerCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientPlayerCtrlDlg::OnPaint()
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
HCURSOR CClientPlayerCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientPlayerCtrlDlg::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here
	CComboBox* pLayout = (CComboBox*)GetDlgItem(IDC_LAYOUT);

	int iTotal = GetTotalOfItems(g_LayoutItems[pLayout->GetCurSel()].iID);

	if (iTotal > 0)
	{
		for (int i = 0; i < iTotal; i++)
		{
			m_APProcessCtrl[i].Launch(g_LayoutItems[pLayout->GetCurSel()].szText,i);
		}
	}
}


void CClientPlayerCtrlDlg::OnBnClickedClose()
{
	// TODO: Add your control notification handler code here
	CComboBox* pLayout = (CComboBox*)GetDlgItem(IDC_LAYOUT);

	int iTotal = GetTotalOfItems(g_LayoutItems[pLayout->GetCurSel()].iID);

	if (iTotal > 0)
	{
		for (int i = 0; i < iTotal; i++)
		{
			m_APProcessCtrl[i].Close();
		}
	}
}

int CClientPlayerCtrlDlg::GetTotalOfItems(int iLayoutID)
{
	int iTotal = 0;
	switch (iLayoutID)
	{
	case LT_1x1:
		iTotal = 1;
		break;
	case LT_2x2:
		iTotal = 4;
		break;
	case LT_3x3:
		iTotal = 9;
		break;
	case LT_4x4:
		iTotal = 16;
		break;
	case LT_3x2:
		iTotal = 6;
		break;
	case LT_4x3:
		iTotal = 12;
		break;
	case LT_5x3:
		iTotal = 15;
		break;
	}
	return iTotal;
}