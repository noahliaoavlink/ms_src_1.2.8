
// MSSchedulerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MSScheduler.h"
#include "MSSchedulerDlg.h"
#include "afxdialogex.h"
#include "AddDlg.h"
#include "../../Include/sstring.h"
#include "../../Include/StrConv.h"

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


// CMSSchedulerDlg dialog



CMSSchedulerDlg::CMSSchedulerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MSSCHEDULER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSSchedulerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMSSchedulerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMSSchedulerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADD, &CMSSchedulerDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DELETE, &CMSSchedulerDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CLEAR, &CMSSchedulerDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_MODIFY, &CMSSchedulerDlg::OnBnClickedModify)
END_MESSAGE_MAP()


// CMSSchedulerDlg message handlers

BOOL CMSSchedulerDlg::OnInitDialog()
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

	CListCtrl* pList = (CListCtrl *)GetDlgItem(IDC_LIST1);
	pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	pList->InsertColumn(0, L"Time");
	pList->InsertColumn(1, L"Command");
	pList->InsertColumn(2, L"Parameter");
	pList->SetColumnWidth(0, 200);
	pList->SetColumnWidth(1, 60);
	pList->SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	UpdateUIStatus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMSSchedulerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMSSchedulerDlg::OnPaint()
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
HCURSOR CMSSchedulerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMSSchedulerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	CButton* pEnable = (CButton*)GetDlgItem(IDC_ENABLE);

	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\Setting.ini", szAPPath);

	sprintf(szData, "%d", pEnable->GetCheck());
	WritePrivateProfileStringA("Shutdown", "Enable", szData, szIniPath);

	if (pEnable->GetCheck())
	{
		SYSTEMTIME st;
		CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
		pStartTimeCtrl->GetTime(&st);

		sprintf(szData, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
		WritePrivateProfileStringA("Shutdown", "Time", szData, szIniPath);
	}

	CButton* pEnablePBScheduleBtn = (CButton*)GetDlgItem(IDC_ENABLE_PB_SCHEDULE);
	sprintf(szData, "%d", pEnablePBScheduleBtn->GetCheck());
	WritePrivateProfileStringA("PBSchedule", "Enable", szData, szIniPath);

	if(pEnablePBScheduleBtn->GetCheck())
		WritePrivateProfileStringA("Timeline_Setting", "Repeat", "0", szIniPath);
	else
		WritePrivateProfileStringA("Timeline_Setting", "Repeat", "1", szIniPath);

	ListToFile();

	CDialogEx::OnOK();
}

void CMSSchedulerDlg::UpdateUIStatus()
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

		sscanf(szTmp, "%02d:%02d:%02d", &iHour, &iMinute, &iSecond);

		st.wHour = iHour;
		st.wMinute = iMinute;
		st.wSecond = iSecond;

		CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
		bRet = pStartTimeCtrl->SetTime(&st);
	}

	CButton* pEnablePBScheduleBtn = (CButton*)GetDlgItem(IDC_ENABLE_PB_SCHEDULE);
	int iEnablePBSchedule = GetPrivateProfileIntA("PBSchedule", "Enable", 0, szIniPath);

	pEnablePBScheduleBtn->SetCheck(iEnablePBSchedule);

	if (iEnablePBSchedule == 1)
	{
		FileToList();
	}
}

void CMSSchedulerDlg::OnBnClickedAdd()
{
	// TODO: Add your control notification handler code here
	char szData[32];
	wchar_t wszData[32];
	CAddDlg add_dlg;
	if (add_dlg.DoModal() == IDOK)
	{
		PBItem* pPBItem = add_dlg.GetPBItem();

		sprintf(szData, "%02d:%02d:%02d", pPBItem->iHour, pPBItem->iMinute, pPBItem->iSecond);

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
		int iIndex = pList->GetItemCount();
		pList->InsertItem(iIndex, ANSIToUnicode(szData), 0);
		wcscpy(wszData, ANSIToUnicode(pPBItem->szCmdID));
		pList->SetItemText(iIndex, 1, wszData);

		swprintf(wszData, L"%d", pPBItem->iParameter);
		pList->SetItemText(iIndex, 2, wszData);
	}
}


void CMSSchedulerDlg::OnBnClickedDelete()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
	int iIndex = -1;
	iIndex = pList->GetNextItem(iIndex, LVNI_SELECTED);
	if (iIndex != -1)
		pList->DeleteItem(iIndex);
}


void CMSSchedulerDlg::OnBnClickedClear()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
	pList->DeleteAllItems();
}

void CMSSchedulerDlg::ListToFile()
{
	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	char szAPPName[64];
	char szTime[64];
	char szCmd[64];
	char szParam[64];
	//char szTmp[256];
	//int iIndex = 0;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\MS_PB_Schedule.ini", szAPPath);

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
	//int iIndex = pList->GetItemCount();
	sprintf(szData, "%d", pList->GetItemCount());

	WritePrivateProfileStringA("PBSchedule", "Total", szData, szIniPath);

	for (int i = 0; i < pList->GetItemCount(); i++)
	{
		sprintf(szAPPName,"Item%d",i);
		CString sTime = pList->GetItemText(i, 0);
		CString sCmd = pList->GetItemText(i, 1);
		CString sParam = pList->GetItemText(i, 2);

		strcpy(szTime, WCharToChar(sTime.GetBuffer()));
		strcpy(szCmd, WCharToChar(sCmd.GetBuffer()));
		strcpy(szParam, WCharToChar(sParam.GetBuffer()));

		//iParameter = _wtoi(sParam.GetBuffer());

		WritePrivateProfileStringA(szAPPName, "Time", szTime, szIniPath);
		WritePrivateProfileStringA(szAPPName, "Cmd", szCmd, szIniPath);
		WritePrivateProfileStringA(szAPPName, "Param", szParam, szIniPath);
	}
}

void CMSSchedulerDlg::FileToList()
{
	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	char szAPPName[64];
	wchar_t wszTime[64];
	wchar_t wszCmd[64];
	wchar_t wszParameter[64];
	int iParameter;
	
	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\MS_PB_Schedule.ini", szAPPath);

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
	pList->DeleteAllItems();

	int iTotal = GetPrivateProfileIntA("PBSchedule", "Total", 0, szIniPath);
	if (iTotal > 0)
	{
		for (int i = 0; i < iTotal; i++)
		{
			sprintf(szAPPName, "Item%d", i);
			GetPrivateProfileStringA(szAPPName, "Time", "", szData, 100, szIniPath);
			wcscpy(wszTime, ANSIToUnicode(szData));
			GetPrivateProfileStringA(szAPPName, "Cmd", "", szData, 100, szIniPath);
			wcscpy(wszCmd, ANSIToUnicode(szData));

			iParameter = GetPrivateProfileIntA(szAPPName, "Param", 0, szIniPath);

			swprintf(wszParameter, L"%d", iParameter);

			int iIndex = pList->GetItemCount();
			pList->InsertItem(iIndex, wszTime, 0);
			pList->SetItemText(iIndex, 1, wszCmd);
			pList->SetItemText(iIndex, 2, wszParameter);
		}
	}
}

void CMSSchedulerDlg::OnBnClickedModify()
{
	// TODO: Add your control notification handler code here
	char szTime[64];
	char szData[64];
	wchar_t wszTime[64];
	wchar_t wszCmd[64];
	wchar_t wszData[32];
	wchar_t wszParameter[64];

	PBItem pb_item;
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST1);
	int iIndex = -1;
	iIndex = pList->GetNextItem(iIndex, LVNI_SELECTED);
	if (iIndex != -1)
	{
		CString sTime = pList->GetItemText(iIndex, 0);
		CString sCmd = pList->GetItemText(iIndex, 1);
		CString sParam = pList->GetItemText(iIndex, 2);

		strcpy(szTime, WCharToChar(sTime.GetBuffer()));

		sscanf(szTime, "%02d:%02d:%02d", &pb_item.iHour, &pb_item.iMinute, &pb_item.iSecond);

		strcpy(pb_item.szCmdID, WCharToChar(sCmd.GetBuffer()));
		pb_item.iParameter = _wtoi(sParam.GetBuffer());

		CAddDlg add_dlg;
		add_dlg.SetPBItem(&pb_item);
		add_dlg.EnableModify(true);
		if (add_dlg.DoModal() == IDOK)
		{
			PBItem* pPBItem = add_dlg.GetPBItem();

			sprintf(szData, "%02d:%02d:%02d", pPBItem->iHour, pPBItem->iMinute, pPBItem->iSecond);

			//pList->DeleteItem(iIndex);

			wcscpy(wszTime, ANSIToUnicode(szData));
			wcscpy(wszCmd, ANSIToUnicode(pPBItem->szCmdID));

			swprintf(wszParameter, L"%d", pPBItem->iParameter);

			//pList->InsertItem(iIndex, wszTime, 0);
			pList->SetItemText(iIndex, 0, wszTime);
			pList->SetItemText(iIndex, 1, wszCmd);
			pList->SetItemText(iIndex, 2, wszParameter);
		}
	}
}
