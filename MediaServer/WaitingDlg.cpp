// WaitingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "WaitingDlg.h"
#include "afxdialogex.h"


// CWaitingDlg dialog

IMPLEMENT_DYNAMIC(CWaitingDlg, CDialogEx)

CWaitingDlg::CWaitingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WAITING_DLG, pParent)
{

}

CWaitingDlg::~CWaitingDlg()
{
}

void CWaitingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaitingDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitingDlg message handlers
BOOL CWaitingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CRect rect;
	CRect parent_rect;
	CWnd* pParentWnd = this->GetParent();
	pParentWnd->GetWindowRect(&parent_rect);

	GetWindowRect(&rect);

	int x = (parent_rect.Width() - rect.Width()) / 2;
	int y = (parent_rect.Height() - rect.Height()) / 2;

	MoveWindow(x,y,rect.Width(),rect.Height(),false);

	CProgressCtrl* pPrgCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pPrgCtrl->SetRange(0,100);
	pPrgCtrl->SetPos(0);

	SetTimer(101,10,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CWaitingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	m_iCurPos += 3;

	if (m_iCurPos >= 100)
		m_iCurPos = 0;

	CProgressCtrl* pPrgCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pPrgCtrl->SetPos(m_iCurPos);

	CDialog::OnTimer(nIDEvent);
}

void CWaitingDlg::UpdateStatus(char* szMsg)
{
	GetDlgItem(IDC_STATIC_MSG)->SetWindowTextA(szMsg);
}

void CWaitingDlg::OnCancel()
{
}

void CWaitingDlg::OnOK()
{
}
