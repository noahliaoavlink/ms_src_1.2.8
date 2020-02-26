// PanelBlendDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "PanelBlendDlg.h"
#include "afxdialogex.h"


// CPanelBlendDlg 對話方塊

IMPLEMENT_DYNAMIC(CPanelBlendDlg, CDialog)

CPanelBlendDlg::CPanelBlendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PAN_BLD_DLG, pParent)
{

}

CPanelBlendDlg::~CPanelBlendDlg()
{
}

void CPanelBlendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelBlendDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPanelBlendDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPanelBlendDlg 訊息處理常式


void CPanelBlendDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	CComboBox *pCombBox = (CComboBox*)GetDlgItem(IDC_IDX_COMB);

	m_iPanelIdx = pCombBox->GetCurSel() + 1;

	for (int x = 0; x < 3; x++)
	{
		if (((CButton*)GetDlgItem(IDC_RADIO1 + x))->GetCheck())
		{
			m_iFinishStatus = x;
			break;
		}
	}

	m_bLockBlendArea = ((CButton*)GetDlgItem(IDC_LOCK_BLEND_CHK))->GetCheck();
	CDialog::OnOK();
}


BOOL CPanelBlendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	CComboBox *pCombBox = (CComboBox*)GetDlgItem(IDC_IDX_COMB);
	CString strTemp;

	for (int x = 0; x < g_PannelAry.GetCount() - 1; x++)
	{
		strTemp.Format("%i", x + 1);
		pCombBox->AddString(strTemp);
	}
	pCombBox->SetCurSel(0);


	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}
