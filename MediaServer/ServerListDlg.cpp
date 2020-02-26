// ServerListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "ServerListDlg.h"
#include "afxdialogex.h"


// CServerListDlg dialog
char g_szIP[64];
int g_iPort;

IMPLEMENT_DYNAMIC(CServerListDlg, CDialogEx)

CServerListDlg::CServerListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_LIST_DLG, pParent)
{

}

CServerListDlg::~CServerListDlg()
{
}

void CServerListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerListDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CServerListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CServerListDlg message handlers
BOOL CServerListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	strcpy(g_szIP,"");
	g_iPort = 0;

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_SERVER_LIST);
	//CListCtrl* pList = this;//GetCListCtrl();
	pList->SetExtendedStyle(pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	pList->InsertColumn(0, "IP");
	pList->InsertColumn(1, "Port");
	pList->SetColumnWidth(0, 200);
	pList->SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	UpdateList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerListDlg::SetServerList(void* pList)
{
	m_pServerList = (SQList*)pList;
}

char* CServerListDlg::GetIP()
{
	return g_szIP;
}

int CServerListDlg::GetPort()
{
	return g_iPort;
}

void CServerListDlg::UpdateList()
{
	char szText[256];
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_SERVER_LIST);
	int iTotal = m_pServerList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		BCUDPServerInfo* pCurItem = (BCUDPServerInfo*)m_pServerList->Get(i);
		if (pCurItem)
		{
			pList->InsertItem(i, pCurItem->szIP, 0);
			sprintf(szText,"%d", pCurItem->iPort);
			pList->SetItemText(i, 1, szText);
		}
	}
}

void CServerListDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_SERVER_LIST);
	int iCurSel = pList->GetNextItem(-1, LVNI_SELECTED);
	if (iCurSel != -1)
	{
		BCUDPServerInfo* pInfo = (BCUDPServerInfo*)m_pServerList->Get(iCurSel);
		if (pInfo)
		{
			strcpy(g_szIP, pInfo->szIP);
			g_iPort = pInfo->iPort;
		}
	}


	CDialogEx::OnOK();
}
