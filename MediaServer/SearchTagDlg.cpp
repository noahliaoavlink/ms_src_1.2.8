// SearchTagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "SearchTagDlg.h"
#include "afxdialogex.h"

wchar_t g_wszKeyword[128] = L"";
// CSearchTagDlg dialog

IMPLEMENT_DYNAMIC(CSearchTagDlg, CDialogEx)

CSearchTagDlg::CSearchTagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SEARCH_TAG_DLG, pParent)
{

}

CSearchTagDlg::~CSearchTagDlg()
{
}

void CSearchTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSearchTagDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSearchTagDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSearchTagDlg message handlers


void CSearchTagDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItemTextW(this->GetSafeHwnd(), IDC_KEYWORD, g_wszKeyword, 128);
	CDialogEx::OnOK();
}

wchar_t* CSearchTagDlg::GetKeyword()
{
	return g_wszKeyword;
}
