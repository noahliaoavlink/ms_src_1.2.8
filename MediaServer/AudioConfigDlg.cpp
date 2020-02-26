// AudioConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "AudioConfigDlg.h"
#include "afxdialogex.h"


// CAudioConfigDlg dialog

IMPLEMENT_DYNAMIC(CAudioConfigDlg, CDialogEx)

CAudioConfigDlg::CAudioConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AUDIO_CONFIG_DLG, pParent)
{
	memset(m_bAudioConfig, 1, 10);
}

CAudioConfigDlg::~CAudioConfigDlg()
{
}

void CAudioConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAudioConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAudioConfigDlg::OnBnClickedOk)
END_MESSAGE_MAP()

//m_bAudioConfig[10];
// CAudioConfigDlg message handlers
BOOL CAudioConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int i = 0; i < 10; i++)
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_SOURCE1 + i);
		if(m_bAudioConfig[i])
			pBtn->SetCheck(true);
		else
			pBtn->SetCheck(false);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioConfigDlg::EnableAudio(int iIndex, bool bEnable)
{
	m_bAudioConfig[iIndex] = bEnable;
}

bool CAudioConfigDlg::AudioIsEnabled(int iIndex)
{
	return m_bAudioConfig[iIndex];
}


void CAudioConfigDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < 10; i++)
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_SOURCE1 + i);
		if (pBtn->GetCheck())
			m_bAudioConfig[i] = true;
		else
			m_bAudioConfig[i] = false;
	}

	CDialogEx::OnOK();
}
