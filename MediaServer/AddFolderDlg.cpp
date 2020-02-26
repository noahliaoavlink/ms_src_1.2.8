// AddFolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "AddFolderDlg.h"
#include "afxdialogex.h"

wchar_t g_wszFolderName[512];
// CAddFolderDlg dialog

IMPLEMENT_DYNAMIC(CAddFolderDlg, CDialogEx)

CAddFolderDlg::CAddFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_FOLDER_DLG, pParent)
{

}

CAddFolderDlg::~CAddFolderDlg()
{
}

void CAddFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddFolderDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddFolderDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddFolderDlg message handlers
BOOL CAddFolderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddFolderDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItemTextW(GetSafeHwnd(),IDC_FOLDER_NAME, g_wszFolderName,512);

	CDialogEx::OnOK();
}

wchar_t* CAddFolderDlg::GetFolderName()
{
	return g_wszFolderName;
}
