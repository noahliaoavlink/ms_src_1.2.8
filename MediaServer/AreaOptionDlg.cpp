// AreaOptionDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "AreaOptionDlg.h"
#include "afxdialogex.h"


// CAreaOptionDlg 對話方塊

IMPLEMENT_DYNAMIC(CAreaOptionDlg, CDialogEx)

CAreaOptionDlg::CAreaOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AREA_OPTION_DLG, pParent)
{

}

CAreaOptionDlg::~CAreaOptionDlg()
{
}

void CAreaOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAreaOptionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAreaOptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAreaOptionDlg 訊息處理常式

BOOL CAreaOptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CAreaOptionDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	m_uGroupEnd = ((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck();
	m_uDarkMask = ((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck();
	m_uNextAction = ((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck();


	CDialogEx::OnOK();
}

