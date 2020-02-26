
// CamViewerDlg.h : header file
//

#pragma once

#include "RTSPPlayerObj.h"

// CCamViewerDlg dialog
class CCamViewerDlg : public CDialogEx
{
	int m_iDlgWidth;
	int m_iDlgHeight;

	RTSPPlayerObj* m_pRTSPPlayerObj;
// Construction
public:
	CCamViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedAlwaysOnTop();
};
