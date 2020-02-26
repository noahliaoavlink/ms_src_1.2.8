// MoveToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "MoveToDlg.h"
#include "afxdialogex.h"

double g_dSelTime;
// CMoveToDlg dialog

IMPLEMENT_DYNAMIC(CMoveToDlg, CDialogEx)

CMoveToDlg::CMoveToDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MOVE_TO_DLG, pParent)
{
	m_iType = DT_NORMAL;
}

CMoveToDlg::~CMoveToDlg()
{
}

void CMoveToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMoveToDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMoveToDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMoveToDlg message handlers
//IDC_DATETIMEPICKER1
BOOL CMoveToDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_iType == DT_CHANGE_LEN)
		SetWindowText("Changle Length");

	CDateTimeCtrl* pDTC = (CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER1);
	pDTC->SetFormat("HH:mm:ss");

	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(sysTime));
	sysTime.wYear = 1998;
	sysTime.wMonth = 1;
	sysTime.wDay = 1;
	sysTime.wHour = 0;
	sysTime.wMinute = 1;
	sysTime.wSecond = 0;
	sysTime.wMilliseconds = 0;
	pDTC->SetTime(&sysTime);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMoveToDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME DTCTime;
	CDateTimeCtrl* pDTC = (CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER1);

	pDTC->GetTime(&DTCTime);
	g_dSelTime = DTCTime.wSecond + DTCTime.wMinute * 60 + DTCTime.wHour * 60 * 60;

	CDialogEx::OnOK();
}

double CMoveToDlg::GetTime()
{
	return g_dSelTime;
}

void CMoveToDlg::SetType(int iType)
{
	m_iType = iType;
}
