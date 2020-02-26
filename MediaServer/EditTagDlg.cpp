// EditTagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "EditTagDlg.h"
#include "afxdialogex.h"

wchar_t g_wszTag[128] = L"";
// CEditTagDlg dialog

IMPLEMENT_DYNAMIC(CEditTagDlg, CDialogEx)

CEditTagDlg::CEditTagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT_TAG_DLG, pParent)
{

}

CEditTagDlg::~CEditTagDlg()
{
}

void CEditTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditTagDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditTagDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditTagDlg message handlers
BOOL CEditTagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemTextW(GetSafeHwnd(),IDC_TAG, g_wszTag);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEditTagDlg::SetTag(wchar_t* wszText)
{
	wcscpy(g_wszTag, wszText);
}

wchar_t* CEditTagDlg::GetTag()
{
	return g_wszTag;
}

void CEditTagDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItemTextW(this->GetSafeHwnd(),IDC_TAG, g_wszTag,128);

	CDialogEx::OnOK();
}
