#pragma once


// CLoadObjVdo 對話方塊

class CLoadObjVdo : public CDialog
{
	DECLARE_DYNAMIC(CLoadObjVdo)

public:
	CLoadObjVdo(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CLoadObjVdo();
	void SetVideoNum(UINT iViewNum){m_iMaxViewNum = iViewNum;};
	CString GetVideoPath()	{return m_strVideoPath;};
	CString GetObjPath() { return m_strObjPath; };
	INT32 GetSelView() { return m_iSelViewPort; };
	DOUBLE GetZoolRatio() {return m_dZoomRatio;};
	void LockLoadObj() { m_bLockLoadObj = TRUE;};
	BOOL IsExtraVideo() { return m_bExtraVideo;};
	void SetDefautlStatus(int iStatus) { m_iDefaultObjStatus = iStatus; };

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOADOBJ_DLG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	CEdit m_ObjPathEdit;
	CComboBox m_ViewComboBox;
	CComboBox m_VideoPathBox;
	CComboBox m_ZoomComboBox;
	CSliderCtrl m_ZoomSildCtrl;
	UINT m_iMaxViewNum;
	CString m_strVideoPath;
	CString m_strObjPath;
	INT32 m_iSelViewPort;
	double m_dZoomRatio;
	BOOL m_bLockLoadObj;
	BOOL m_bExtraVideo;
	INT32 m_iDefaultObjStatus;  //0: Normal, 1: Save, 2: Load

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOpenVideoPath();
	afx_msg void OnBnClickedOpenObjPath();
	afx_msg void OnCbnSelchangeVideoPathComb0();
	afx_msg void OnCbnSelchangeZoomCombo();
	afx_msg void OnNMCustomdrawZoomSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedOpenVideoPath2();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSaveDefaultChk();
};
