// EditCommandDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "EditCommandDlg.h"
#include "afxdialogex.h"


int g_iCmdType;
int g_iJumpType;
char g_szCmdName[128];
char g_szCmdTargetName[128];

// CEditCommandDlg dialog

IMPLEMENT_DYNAMIC(CEditCommandDlg, CDialogEx)

CEditCommandDlg::CEditCommandDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT_CMD_DLG, pParent)
{
	strcpy(g_szCmdName,"");
	g_iCmdType = 0;
	g_iJumpType = JT_NONE;
	strcpy(g_szCmdTargetName, "");
	m_pTrackContentManager = 0;
}

CEditCommandDlg::~CEditCommandDlg()
{
}

void CEditCommandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditCommandDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditCommandDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMD_LIST, &CEditCommandDlg::OnCbnSelchangeCmdList)
END_MESSAGE_MAP()


// CEditCommandDlg message handlers
BOOL CEditCommandDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CComboBox* pCmdList = (CComboBox*)GetDlgItem(IDC_CMD_LIST);
	pCmdList->AddString("None");
	pCmdList->AddString("Stop");
	pCmdList->AddString("Jump");
	pCmdList->SetCurSel(0);

	pCmdList->SetCurSel(g_iCmdType);

	CComboBox* pEventList = (CComboBox*)GetDlgItem(IDC_EVENT_LIST);
	//pEventList->AddString("Event1");
	//pEventList->SetCurSel(0);

	UpdateEventList();

	int iTotalOfEvents = pEventList->GetCount();
	if (iTotalOfEvents > 0 && strcmp(g_szCmdTargetName,"") != 0)
	{
		int iIndex = -1;
		char szCurItemText[128];
		for (int i = 0; i < iTotalOfEvents; i++)
		{
			pEventList->GetLBText(i, szCurItemText);
			if (strcmp(szCurItemText, g_szCmdTargetName) == 0)
			{
				iIndex = i;
				break;
			}
		}
		if(iIndex != -1)
			pEventList->SetCurSel(iIndex);
		else
			pEventList->SetCurSel(0);
	}

	SetDlgItemText(IDC_CMD_NAME, g_szCmdName);

	OnCbnSelchangeCmdList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CEditCommandDlg::GetType()
{
	return g_iCmdType;
}

char* CEditCommandDlg::GetName()
{
	return g_szCmdName;
}

char* CEditCommandDlg::GetTargetName()
{
	return g_szCmdTargetName;
}

int CEditCommandDlg::GetJumpType()
{
	return g_iJumpType;
}

void CEditCommandDlg::SetType(int iValue)
{
	g_iCmdType = iValue;
}

void CEditCommandDlg::SetName(char* szName)
{
	strcpy(g_szCmdName, szName);
}

void CEditCommandDlg::SetTargetName(char* szName)
{
	strcpy(g_szCmdTargetName, szName);
}

void CEditCommandDlg::SetJumpType(int iType)
{
	g_iJumpType = iType;
}

void CEditCommandDlg::SetTrackContentManager(TrackContentManager* pObj)
{
	m_pTrackContentManager = pObj;
}

void CEditCommandDlg::SetFullPath(char* szPath)
{
	strcpy(m_szFullPath, szPath);
}

void CEditCommandDlg::UpdateEventList()
{
	if (m_pTrackContentManager)
	{
		int iItemIndex, iTrackIndex;
		int iRet = m_pTrackContentManager->GetItemTackIndex(m_szFullPath,iItemIndex,iTrackIndex);
		if (iRet != -1)
		{
			CComboBox* pEventList = (CComboBox*)GetDlgItem(IDC_EVENT_LIST);
			pEventList->ResetContent();
			m_iEventCount = 0;

			int iTotalOfNodes = m_pTrackContentManager->GetTotalOfNodes(iItemIndex,iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				CommandNode* pCurNode = (CommandNode*)m_pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex,i);
				if (pCurNode)
				{
					if (pCurNode->iType == CT_NONE)
					{
						pEventList->AddString(pCurNode->szName);
						m_iEventCount++;
					}
				}
			}

			//
			m_pTrackContentManager->GetTotalOfItems();
			for (int j = 0; j < m_pTrackContentManager->GetTotalOfItems(); j++)
			{
				if (j != iItemIndex)
				{
					char* szCurItem = m_pTrackContentManager->GetItemName(j);
					pEventList->AddString(szCurItem);
				}
			}
		}
	}
		
}

void CEditCommandDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CComboBox* pCmdList = (CComboBox*)GetDlgItem(IDC_CMD_LIST);
	g_iCmdType = pCmdList->GetCurSel();

	GetDlgItemText(IDC_CMD_NAME, g_szCmdName,128);

	CComboBox* pEventList = (CComboBox*)GetDlgItem(IDC_EVENT_LIST);
	int iSelEvent = pEventList->GetCurSel();
	pEventList->GetLBText(iSelEvent, g_szCmdTargetName);

	if (g_iCmdType == CT_JUMP)
	{
		if(iSelEvent < m_iEventCount)
			g_iJumpType = JT_EVENT;
		else
			g_iJumpType = JT_TIMELINE;
	}
	else
		g_iJumpType = JT_NONE;

	CDialogEx::OnOK();
}


void CEditCommandDlg::OnCbnSelchangeCmdList()
{
	// TODO: Add your control notification handler code here
	CComboBox* pCmdList = (CComboBox*)GetDlgItem(IDC_CMD_LIST);
	CComboBox* pEventList = (CComboBox*)GetDlgItem(IDC_EVENT_LIST);
	g_iCmdType = pCmdList->GetCurSel();

	if (g_iCmdType == CT_JUMP)
		pEventList->EnableWindow(true);
	else
		pEventList->EnableWindow(false);
}
