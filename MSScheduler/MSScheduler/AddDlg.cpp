// AddDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MSScheduler.h"
#include "AddDlg.h"
#include "afxdialogex.h"
#include "../../Include/StrConv.h"

PBItem g_PBItem;
// CAddDlg dialog

IMPLEMENT_DYNAMIC(CAddDlg, CDialogEx)

CAddDlg::CAddDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_DLG, pParent)
{
	m_bModify = false;
}

CAddDlg::~CAddDlg()
{
}

void CAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddDlg::OnBnClickedOk)
//	ON_STN_CLICKED(IDC_STATIC_START_TIME3, &CAddDlg::OnStnClickedStaticStartTime3)
ON_CBN_SELCHANGE(IDC_PB_CMD, &CAddDlg::OnCbnSelchangePbCmd)
END_MESSAGE_MAP()


// CAddDlg message handlers
BOOL CAddDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	// TODO: Add extra initialization here
	wchar_t wszParamStr[64];
	CComboBox* pPBCmd = (CComboBox *)GetDlgItem(IDC_PB_CMD);
	pPBCmd->AddString(L"Play");
	pPBCmd->AddString(L"Stop");
	pPBCmd->AddString(L"SwitchTo");
	pPBCmd->SetCurSel(0);

	CComboBox* pPBParam = (CComboBox *)GetDlgItem(IDC_PB_PARAM);
	for (int i = 0; i < 10; i++)
	{
		//swprintf(wszParamStr, L"%d", i + 1);
		swprintf(wszParamStr, L"%d", i);
		pPBParam->AddString(wszParamStr);
	}
	pPBParam->SetCurSel(0);

	if (m_bModify)
		UpdateUIStatus();

	OnCbnSelchangePbCmd();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strData;
	SYSTEMTIME st;
	CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
	pStartTimeCtrl->GetTime(&st);

	CComboBox* pPBCmd = (CComboBox *)GetDlgItem(IDC_PB_CMD);

	//sprintf(szData, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	g_PBItem.iHour = st.wHour;
	g_PBItem.iMinute = st.wMinute;
	g_PBItem.iSecond = st.wSecond;

	pPBCmd->GetLBText(pPBCmd->GetCurSel(), strData);

	char* szCmdStr = WCharToChar(strData.GetBuffer());

	strcpy(g_PBItem.szCmdID, szCmdStr);

	CComboBox* pPBParam = (CComboBox *)GetDlgItem(IDC_PB_PARAM);
	g_PBItem.iParameter = pPBParam->GetCurSel();

	CDialogEx::OnOK();
}

PBItem* CAddDlg::GetPBItem()
{
	return &g_PBItem;
}

void CAddDlg::SetPBItem(PBItem* pItem)
{
	memcpy(&g_PBItem, pItem,sizeof(PBItem));
}

void CAddDlg::EnableModify(bool bEnable)
{
	m_bModify = bEnable;
}

void CAddDlg::UpdateUIStatus()
{
	SYSTEMTIME st;
	CComboBox* pPBCmd = (CComboBox *)GetDlgItem(IDC_PB_CMD);

	if (strcmp(g_PBItem.szCmdID, "Play") == 0)
		pPBCmd->SetCurSel(0);
	else if (strcmp(g_PBItem.szCmdID, "Stop") == 0)
		pPBCmd->SetCurSel(1);
	else if (strcmp(g_PBItem.szCmdID, "SwitchTo") == 0)
		pPBCmd->SetCurSel(2);

	GetSystemTime(&st);

	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;

	st.wHour = g_PBItem.iHour;
	st.wMinute = g_PBItem.iMinute;
	st.wSecond = g_PBItem.iSecond;

	CDateTimeCtrl* pStartTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_START_TIME);
	bool bRet = pStartTimeCtrl->SetTime(&st);

	CComboBox* pPBParam = (CComboBox *)GetDlgItem(IDC_PB_PARAM);
	pPBParam->SetCurSel(g_PBItem.iParameter);
}

void CAddDlg::OnCbnSelchangePbCmd()
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetDlgItem(IDC_PB_PARAM);

	CComboBox* pPBCmd = (CComboBox *)GetDlgItem(IDC_PB_CMD);
	if (pPBCmd->GetCurSel() == 2)
		pWnd->EnableWindow(true);
	else
		pWnd->EnableWindow(false);
}
