#pragma once


// CServerListDlg dialog
#include "../../../Include/SQList.h"
#include "../../../Include/NetCommon.h"

class CServerListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerListDlg)

	SQList* m_pServerList;
public:
	CServerListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CServerListDlg();

	void SetServerList(void* pList);
	char* GetIP();
	int GetPort();
	void UpdateList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_LIST_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
