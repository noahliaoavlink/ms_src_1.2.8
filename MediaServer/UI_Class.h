#pragma once
#include "Gdiplus.h"
#include "afxwin.h"
#include "atltypes.h"
#include "gdipluscolor.h"
#include "UTipDll.h"

using namespace Gdiplus;


//------------------ UI Used------------------//
#define Redraw_BGImage		1
#define Redraw_ClickImage	2
#define Redraw_ThroughImage	3

#define Err_No_BGImage      -1
#define Err_No_ClickImage   -2
#define Err_No_ThroughImage -3

#define HaveBGImage       true
#define NoBGImage         false
#define HaveClickImage    true
#define NoClickImage      false
#define HaveThroughtImage true
#define NoThroughtImage   false
#define IsOptionType      true
#define NotOptionType     false

#define OK  0
#define Err -1

#define DEF_UI_CLICK_MSG 1200
//--------------------------------------------//

// CUI_Class

class CUI_Class : public CWnd  
{
	DECLARE_DYNAMIC(CUI_Class)

public:
	CUI_Class();
	virtual ~CUI_Class();
	

public:
	BOOL IsPtInside(CPoint CurrentPt);
	virtual CRgn* GetRegion(void){return &m_region;}
	void SetClipRgn(CRgn *DrawRgn){m_cliprgn = DrawRgn;}
	virtual void OffsetObject(CPoint OffsetPt);
	void SetParentWindow(CWnd* ParetnWnd){m_ParentWnd = ParetnWnd;}
	CPoint GetObjectOffset(){return m_ObjectOffset;}

	virtual int RedrawBGImage(bool bErase);	
	virtual int Redraw(bool bErase, int DrawType);	
	virtual int  Initialize();
	virtual int DrawSpecifyImage(CString strImgPath);

	bool IsHaveBackgroundImage(){return m_bBGImage;}
	bool IsHaveClickImage(){return m_bClickImage;}
	bool IsHaveThroughImage(){return m_bThroughImage;}
	bool IsHaveInitialized(){return m_bHaveInitialized;}
	int  GetIDNumber(){return m_iID;}
	virtual int OnZoom(int iStartX, int iStartY, float fScaleWidth, float fScaleHeight);	
	virtual int  SetDlgRegion(CWnd* DlgWnd, BOOL bSetPlacement = TRUE);
	CRgn* GetSacleRegion(bool bContainItems);
	CWnd* GetParentWnd(){return m_ParentWnd;}
	void IsPassiveDraw(bool bPassive){m_bPassiveDraw = bPassive;}
	CString ChangeToAbsolutePath(CString strFile);
	HWND GetWnd() { return m_hWnd; }
	void SetUTipDll(UTipDll* pObj, CWnd* pParetnWnd);

protected:
	Bitmap* m_pImage;
	CString m_strFileName;
	HWND    m_hWnd;
	bool    m_bBGImage;         //是否有背景圖
	bool    m_bClickImage;      //是否有滑鼠按下的圖片
	bool    m_bThroughImage;    //是否有滑鼠經過的圖片
	bool    m_bHaveInitialized; //是否已經初始化過了
	bool    m_bPassiveDraw;

	CRgn    m_region;           //物件的Region範圍
	CRgn    *m_cliprgn;          //限制物件只繪製指定的Region範圍
	CPoint  m_ObjectOffset;		//物件在上一層的位置，如按鈕在Dialog中的位置。位置的資訊藏在Bitmap的第一個Pixel。
	CPoint  m_ImageOffset;		//影像的Offset。因得到的Bitmap可能有留邊。繪圖時依這資訊把邊去掉。	
	CWnd*   m_ParentWnd;
	int     m_iID;
	CDC*     m_MemoryDC;
	CBitmap  m_Bitmap;
	Graphics* m_pGraphics;
	float    m_fScale_Width;
	float    m_fScale_Height;	
	CRgn*    m_SacleRgn;	

	UTipDll* m_pUTipDll;
protected:
	int GetInfoFromImageFile(CString strFileName);
	const Color m_BGEraseColor;
	WCHAR* AsWideString(const char* str, bool bChangeToAbsolutePath = false);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual afx_msg void OnPaint();
	afx_msg void OnBnClicked();
	afx_msg void OnBnDoubleclicked();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual int  ChangeToOtherBKImage();
public:
	virtual afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnable(BOOL bEnable);
};


