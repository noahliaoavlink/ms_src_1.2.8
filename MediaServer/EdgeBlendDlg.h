#pragma once


// CEdgeBlendDlg 對話方塊

typedef struct strBLEND_SETTING
{
	UINT32 uHNum;
	UINT32 uVNum;
	UINT32 uGridH;
	UINT32 uGridV;
	INT32 iPanelIdxAry[4];
	INT32 iBlendWidth;
	INT32 iBlendHeight;
	BOOL bBlendArea[4][4];
	CRect rectDispAry[4];
	INT32 iStatus; //0: Nono, 1: Draw, 2: Modify Point

} BLEND_SETTING;

class CEdgeBlendDlg : public CDialog
{
	DECLARE_DYNAMIC(CEdgeBlendDlg)

public:
	CEdgeBlendDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CEdgeBlendDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDGEBLEND_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	CDC*     m_MemoryDC;
	CBitmap  m_Bitmap;
	UINT m_uCurStep;
	CRect m_rectDrawArea;
	CRect m_rectDispAry[4];
	INT32 m_iCurPannelIdx;
	CString m_strPanelArrange;
	void SetStepUI(UINT uStep);
	void DrawMemoryDC();
	void CalcDispArea();
	void SetBlendArea(INT iItemID);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNextBtn();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	BLEND_SETTING m_BlendSetting;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedEbsSetBtn();
	afx_msg void OnBnClickedLeftChk();
	afx_msg void OnBnClickedTopChk();
	afx_msg void OnBnClickedRightChk();
	afx_msg void OnBnClickedDownChk();
	afx_msg void OnBnClickedPreBtn();
};
