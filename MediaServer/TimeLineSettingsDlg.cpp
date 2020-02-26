// TimeLineSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "TimeLineSettingsDlg.h"
#include "afxdialogex.h"

#define _MAX_TIME_LINE_ID 64
char g_szTimeLineName[128];
int g_iTimeLineID = 0;

// CTimeLineSettingsDlg dialog

IMPLEMENT_DYNAMIC(CTimeLineSettingsDlg, CDialogEx)

CTimeLineSettingsDlg::CTimeLineSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TIMELINE_SETTINGS_DLG, pParent)
{
	m_bDisableName = false;
}

CTimeLineSettingsDlg::~CTimeLineSettingsDlg()
{
}

void CTimeLineSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimeLineSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTimeLineSettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTimeLineSettingsDlg message handlers
BOOL CTimeLineSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	char szText[16];
	::SetDlgItemText(GetSafeHwnd(), IDC_TIMELINE_NAME, g_szTimeLineName);

	if (m_bDisableName)
		::EnableWindow(GetDlgItem(IDC_TIMELINE_NAME)->GetSafeHwnd(),false);

	CComboBox* pTimeLineID = (CComboBox*)GetDlgItem(IDC_TIMELINE_ID);
	for (int i = 0; i < _MAX_TIME_LINE_ID; i++)
	{
		sprintf(szText,"%d",i);
		pTimeLineID->AddString(szText);
	}

	if(g_iTimeLineID < _MAX_TIME_LINE_ID)
		pTimeLineID->SetCurSel(g_iTimeLineID);
	else
		pTimeLineID->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTimeLineSettingsDlg::SetName(char* szName)
{
	strcpy(g_szTimeLineName, szName);
}

void CTimeLineSettingsDlg::SetID(int iID)
{
	g_iTimeLineID = iID;
}

char* CTimeLineSettingsDlg::GetName()
{
	return g_szTimeLineName;
}

int CTimeLineSettingsDlg::GetID()
{
	return g_iTimeLineID;
}

void CTimeLineSettingsDlg::DisableName()
{
	m_bDisableName = true;
}

void CTimeLineSettingsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	::GetDlgItemText(GetSafeHwnd(), IDC_TIMELINE_NAME, g_szTimeLineName, 128);
	CComboBox* pTimeLineID = (CComboBox*)GetDlgItem(IDC_TIMELINE_ID);
	g_iTimeLineID = pTimeLineID->GetCurSel();

	CDialogEx::OnOK();
}
