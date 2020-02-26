// EditEffectIndexDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "EditEffectIndexDlg.h"
#include "afxdialogex.h"
#include "ContentProcessor.h"

// CEditEffectIndexDlg dialog

IMPLEMENT_DYNAMIC(CEditEffectIndexDlg, CDialogEx)

CEditEffectIndexDlg::CEditEffectIndexDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT_EFFECT_INDEX_DLG, pParent)
{

}

CEditEffectIndexDlg::~CEditEffectIndexDlg()
{
}

void CEditEffectIndexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditEffectIndexDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditEffectIndexDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditEffectIndexDlg message handlers
BOOL CEditEffectIndexDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	char szItemText[256];
	CComboBox* pEffectIndexList = (CComboBox*)GetDlgItem(IDC_EFFECT_INDEX);
	
#ifdef _ENABLE_HD_EFFECT
	int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
	for (int i = 0; i < iTotalOfEffectItems; i++)
	{
		sprintf(szItemText, "%02d. %s", i, g_hd_effect_param_items[i].szEffectName);
		pEffectIndexList->AddString(szItemText);
	}
#else
	int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
	for (int i = 0; i < iTotalOfEffectItems; i++)
	{
		sprintf(szItemText, "%s"/*"%02d. %s",i*/, g_effect_param_items[i].szEffectName);
		pEffectIndexList->AddString(szItemText);
	}
#endif

	pEffectIndexList->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEditEffectIndexDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CComboBox* pEffectIndexList = (CComboBox*)GetDlgItem(IDC_EFFECT_INDEX);
	m_iIndex = pEffectIndexList->GetCurSel();

	CDialogEx::OnOK();
}

int CEditEffectIndexDlg::GetIndex()
{
	return m_iIndex;
}
