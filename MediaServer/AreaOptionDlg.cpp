// AreaOptionDlg.cpp : ��@��
//

#include "stdafx.h"
#include "MediaServer.h"
#include "AreaOptionDlg.h"
#include "afxdialogex.h"


// CAreaOptionDlg ��ܤ��

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


// CAreaOptionDlg �T���B�z�`��

BOOL CAreaOptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �b���[�J�B�~����l��

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO6))->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX �ݩʭ����Ǧ^ FALSE
}

void CAreaOptionDlg::OnBnClickedOk()
{
	// TODO: �b���[�J����i���B�z�`���{���X

	m_uGroupEnd = ((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck();
	m_uDarkMask = ((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck();
	m_uNextAction = ((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck();


	CDialogEx::OnOK();
}

