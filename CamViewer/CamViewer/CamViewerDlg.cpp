
// CamViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CamViewer.h"
#include "CamViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCamViewerDlg dialog



CCamViewerDlg::CCamViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCamViewerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PLAY, &CCamViewerDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CCamViewerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_ALWAYS_ON_TOP, &CCamViewerDlg::OnBnClickedAlwaysOnTop)
END_MESSAGE_MAP()


// CCamViewerDlg message handlers

BOOL CCamViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CWnd* pWnd = GetDlgItem(IDC_VIDEO);
	//pWnd->MoveWindow();
	m_pRTSPPlayerObj = new RTSPPlayerObj;
	m_pRTSPPlayerObj->Init(pWnd->GetSafeHwnd());

	SetDlgItemText(IDC_URL, "rtsp://admin:avlink1234@10.1.1.171:88/videoSub");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCamViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCamViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CCamViewerDlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDialogEx::OnSize(nType, cx, cy);
//
//	// TODO: Add your message handler code here
//	char szMsg[512];
//	sprintf(szMsg, "OnSize cx:%d cy:%d\n", cx, cy);
//	OutputDebugStringA(szMsg);
//}


void CCamViewerDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here
}


void CCamViewerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
	bool bUpdate = false;

	if (m_iDlgWidth != pRect->right - pRect->left)
	{
		m_iDlgWidth = pRect->right - pRect->left;
		bUpdate = true;
	}

	if (m_iDlgHeight != pRect->bottom - pRect->top)
	{
		m_iDlgHeight = pRect->bottom - pRect->top;
		bUpdate = true;
	}

	if (bUpdate)
	{
		char szMsg[512];
		sprintf(szMsg, "OnSizing w:%d h:%d\n", pRect->right - pRect->left, pRect->bottom - pRect->top);
		OutputDebugStringA(szMsg);

		RECT rect;
		RECT rDlgRect;
		CWnd* pWnd = GetDlgItem(IDC_VIDEO);
		pWnd->GetWindowRect(&rect);

		this->GetWindowRect(&rDlgRect);

		sprintf(szMsg, "IDC_VIDEO [%d , %d , %d , %d] Dlg [%d , %d , %d , %d]\n", rect.left, rect.top, rect.right, rect.bottom, rDlgRect.left, rDlgRect.top, rDlgRect.right, rDlgRect.bottom);
		OutputDebugStringA(szMsg);
		pWnd->MoveWindow(0, 0, pRect->right - pRect->left - 20, pRect->bottom - pRect->top - 100,true);
		Invalidate();
	}
}


void CCamViewerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if (m_pRTSPPlayerObj)
		delete m_pRTSPPlayerObj;
}


void CCamViewerDlg::OnBnClickedPlay()
{
	// TODO: Add your control notification handler code here
	char szUrl[512];
	GetDlgItemText(IDC_URL, szUrl, 512);

	//char szUrl[512] = "rtsp://admin:avlink1234@10.1.1.171:88/videoSub";
	if (m_pRTSPPlayerObj)
		m_pRTSPPlayerObj->Open(szUrl);
}


void CCamViewerDlg::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	if (m_pRTSPPlayerObj)
		m_pRTSPPlayerObj->Close();
}


void CCamViewerDlg::OnBnClickedAlwaysOnTop()
{
	// TODO: Add your control notification handler code here
	RECT rect;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_ALWAYS_ON_TOP);
	this->GetWindowRect(&rect);

	if (pBtn->GetCheck())
		::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,rect.left,rect.top,rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);//HWND_TOPMOST
	else
		::SetWindowPos(this->GetSafeHwnd(), HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);  //HWND_NOTOPMOST
}
