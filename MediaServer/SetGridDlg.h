#pragma once


// CSetGridDlg 對話方塊
enum
{
	WM_SETGRID = WM_USER + 0x800,
	WM_SETCURVE,
	WM_SETGAMMA,
	WM_SETGAIN,
	WM_PRESETGAIN,
	WM_SETTEXTURE,
	WM_INIT2DMAPPING,
	WM_SETGRID_CLOSE
};

class CSetGridDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetGridDlg)

public:
	CSetGridDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSetGridDlg();
	INT32 m_iGridNumX;
	INT32 m_iGridNumY;

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETGRID_DLG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	CDC*     m_MemoryDC;
	CDC*     m_DirectionDC;
	CBitmap  m_Bitmap;
	CBitmap  m_DirectionBMP;
	CRect m_rectChart, m_rectDirection;
	BOOL m_bAddCurPt;
	BOOL m_bClickDirectionPt;
	CArray<CPoint, CPoint> m_CurvePtAry;
	CArray<CPoint, CPoint> m_CurvePtTable[5][MAX_PTAREA];
	CPoint m_ptDarkTable[5][MAX_PTAREA];
	INT32 m_iTextureIdx;
	INT32 m_iSelPannel;
	INT32 m_iSelArea;
	void SavePtAry();
	void LoadPtAry();
	void PtToAry(BOOL bNeedSave);
	BOOL SetDarkPt(CPoint ptMouse);
	DECLARE_MESSAGE_MAP()
public:
	DOUBLE m_ColorAry[100];
	UINT m_uPanelGain[5];
	CPoint m_ptDark;
	UINT m_clrGain;
	UINT m_uGammaVal;
	INT m_iSelCurvePt;
	afx_msg void OnBnClickedOk();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	void RedrawMemoryDC();
	void CreateDefultGama();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetcurveBtn();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSetgainBtn();
	afx_msg void OnBnClickedSetgridBtn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeGammaCombo();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCbnSelchangePannelCombo();
	afx_msg void OnCbnSelchangeAreaCombo();
	virtual void OnCancel();
};
