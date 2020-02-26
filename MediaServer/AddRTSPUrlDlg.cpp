// AddRTSPUrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "AddRTSPUrlDlg.h"
#include "afxdialogex.h"

char g_szUrl[256];
double g_dLen = 0.0;
// CAddRTSPUrlDlg dialog

IMPLEMENT_DYNAMIC(CAddRTSPUrlDlg, CDialogEx)

CAddRTSPUrlDlg::CAddRTSPUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_RTSP_URL_DLG, pParent)
{

}

CAddRTSPUrlDlg::~CAddRTSPUrlDlg()
{
}

void CAddRTSPUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddRTSPUrlDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddRTSPUrlDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddRTSPUrlDlg message handlers
BOOL CAddRTSPUrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	char szText[64];
	SetDlgItemText(IDC_RTSP_URL,"rtsp://admin:avlink1234@10.1.1.198:88/videoMain");

	CComboBox* pLenMins = (CComboBox*)GetDlgItem(IDC_LEN_MINS);
	for (int i = 0; i < 60; i++)
	{
		sprintf(szText,"%02d",i);
		pLenMins->AddString(szText);
	}
	pLenMins->SetCurSel(0);

	CComboBox* pLenSecs = (CComboBox*)GetDlgItem(IDC_LEN_SECS);
	for (int i = 0; i < 60; i++)
	{
		sprintf(szText, "%02d", i);
		pLenSecs->AddString(szText);
	}
	pLenSecs->SetCurSel(30);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddRTSPUrlDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str1;
	char szUrl[256];
	GetDlgItemText(IDC_RTSP_URL, szUrl,256);
	strcpy(g_szUrl, szUrl);

	CComboBox* pLenMins = (CComboBox*)GetDlgItem(IDC_LEN_MINS);

	int n = pLenMins->GetLBTextLen(pLenMins->GetCurSel());
	pLenMins->GetLBText(pLenMins->GetCurSel(), str1.GetBuffer(n));
	int iMins = atoi(str1.GetBuffer());

	CComboBox* pLenSecs = (CComboBox*)GetDlgItem(IDC_LEN_SECS);
	n = pLenSecs->GetLBTextLen(pLenSecs->GetCurSel());
	pLenSecs->GetLBText(pLenSecs->GetCurSel(), str1.GetBuffer(n));
	int iSecs = atoi(str1.GetBuffer());

	g_dLen = iSecs + iMins * 60;

	CDialogEx::OnOK();
}

char* CAddRTSPUrlDlg::GetUrl()
{
	return g_szUrl;
}

double CAddRTSPUrlDlg::GetLength()
{
	return g_dLen;
}