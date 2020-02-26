#pragma once
#include "Item_Class.h"

#ifdef _ENABLE_MEDIA_STREAM_EX
#include "MediaStreamEx.h"
#else
#include "MediaStream.h"
#endif
#include "MixerEffect.h"

class CImageArea
{
private:
	CRect m_RealityRect;
	CRect m_ZoomedRect;
	CPoint m_ZoomedRealLT;
	CPoint m_ImageStartLT;
	int m_iZoomedDstWidth;
	int m_iZoomedDstHeight;
	int m_iZoomedSrcWidth;
	int m_iZoomedSrcHeight;
	CWnd*   m_pParentWnd;
	D3DRender* m_pD3DRender;
	CShapeDispManager* m_pDisplayManager;
#ifdef _ENABLE_MEDIA_STREAM_EX
	CMediaStreamEx*  m_pMediaStream;
#else
	CMediaStream*  m_pMediaStream;
#endif
	void SetZoomedRealLT(const CPoint& OriLT, const CPoint& OriRB, const CPoint& ZoomLT, const CPoint& ZoomRB)
	{
	#ifdef ori
	/*
		int IntVal_x, IntVal_y, RealIntVal_x, RealIntVal_y;

		if (ZoomLT.x<OriLT.x)
			IntVal_x = 0;
		else
			IntVal_x = abs(ZoomLT.x - OriLT.x);

		if (ZoomLT.y<OriLT.y)
			IntVal_y = 0;
		else
			IntVal_y = abs(ZoomLT.y - OriLT.y);

		RealIntVal_x = m_RealityRect.Width()*IntVal_x / (OriRB.x - OriLT.x);
		RealIntVal_y = m_RealityRect.Height()*IntVal_y / (OriRB.y - OriLT.y);

		m_ZoomedRealLT.x = m_RealityRect.left + RealIntVal_x;
		m_ZoomedRealLT.y = m_RealityRect.top + RealIntVal_y;
	*/
	#else
		int IntVal_x, IntVal_y;
		
		if (ZoomLT.x<OriLT.x)
			IntVal_x = 0;
		else
			IntVal_x = abs(ZoomLT.x - OriLT.x);
		
		if (ZoomLT.y<OriLT.y)
			IntVal_y = 0;
		else
			IntVal_y = abs(ZoomLT.y - OriLT.y);
		
		m_ZoomedRealLT.x = int(float(m_icur_backbuf_width/*m_iDecImgWidth*/*IntVal_x) / (OriRB.x - OriLT.x));
		m_ZoomedRealLT.y = int(float(m_icur_backbuf_height/*m_iDecImgHeight*/*IntVal_y) / (OriRB.y - OriLT.y));
	#endif
	}

	void SetImageStartLT(const CPoint& OriLT, const CPoint& OriRB, const CPoint& ZoomLT, const CPoint& ZoomRB)
	{
#ifdef ori //while locate on the left-lower edge will crash
		CRect OriRect(OriLT, OriRB), ZoomedRect(ZoomLT, ZoomRB);
		LONG outside_x, outside_y;

		if (!OriRect.PtInRect(ZoomLT))
		{
			outside_x = OriLT.x - ZoomLT.x;

			if (ZoomRB.y > OriRB.y) //only zoomed rect right-up vertex inside ori rect
			{
				m_ImageStartLT.x = LONG(float(m_iDecImgWidth*outside_x) / ZoomedRect.Width());
				m_ImageStartLT.y = 0;
			}
			else
			{
				if (OriRect.PtInRect(ZoomRB) && OriRect.PtInRect(CPoint(ZoomRB.x, ZoomLT.y)))//zoomed rect right 2 vertices inside ori rect
				{
					m_ImageStartLT.x = LONG(float(m_iDecImgWidth*outside_x) / ZoomedRect.Width());
					m_ImageStartLT.y = 0;;
				}
				else
				{
					outside_y = OriLT.y - ZoomLT.y;

					m_ImageStartLT.y = LONG(float(m_iDecImgHeight*outside_y) / ZoomedRect.Height());

					if (OriRect.PtInRect(CPoint(ZoomLT.x, ZoomRB.y))) //zoomed rect left down vertex inside ori rect
						m_ImageStartLT.x = 0;
					else //only zoomed rect right bottom vertex in ori rect
						m_ImageStartLT.x = LONG(float(m_iDecImgWidth*outside_x) / ZoomedRect.Width());
				}
			}
		}
		else
		{
			m_ImageStartLT.x = 0;
			m_ImageStartLT.y = 0;
		}
#else

		CRect ZoomedRect(ZoomLT, ZoomRB);

		if ((OriLT.x <= ZoomLT.x) && (ZoomLT.x <= OriRB.x))
			m_ImageStartLT.x = 0;
		else //only occur on OriLT.x > ZoomLT.x
			m_ImageStartLT.x = LONG(float(m_iDecImgWidth*(OriLT.x - ZoomLT.x)) / ZoomedRect.Width());

		if ((OriLT.y <= ZoomLT.y) && (ZoomLT.y <= OriRB.y))
			m_ImageStartLT.y = 0;
		else //only occur on OriLT.y > ZoomLT.y
			m_ImageStartLT.y = LONG(float(m_iDecImgHeight*(OriLT.y - ZoomLT.y)) / ZoomedRect.Height());
#endif
	}

public:
	UCHAR	m_bZmPaintBit:1;
	UCHAR	m_bVolPnt:1;
	BOOL	m_bPlaying;
	UCHAR	m_uEffItemBmp; //effect kinds and items mapping; bit position others(...), bit 0 => others(...), Level
	int 	m_iAngle;
	float	m_fZoomRate;
	int 	m_iDecImgWidth;
	int	 	m_iDecImgHeight;
	int		m_icur_backbuf_width;
	int		m_icur_backbuf_height;
	CPoint	m_ZoomLTpnt;
	CPoint	m_ZoomRBpnt;
	CPoint	m_VolPoint;
	CStatic	m_PicCtrl;
	CRect	m_PreZmRectPos;
	CPoint	m_PreVolPnt;

	void RenderSurface(IDirect3DSurface9* pBuffer, int iWidth, int iHeight);
	void RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight);
	CMixerEffect m_cEffect;

	//following values reserved for indirectly set into program

    //BRIGHT PAGE SETTING VALUES (logical value:0 ~ 200 mapping to true underlying value:-100 ~ 100)
	UINT	m_uBright;
	UINT	m_uContrast;
	//HUE PAGE SETTING VALUES    (logical value:0 ~ 200 mapping to true underlying value:-100 ~ 100)
	UINT	m_uHue;
	UINT	m_uSaturation;
	//RGB PAGE SETTING VALUES    (logical value:0 ~ 200 mapping to true underlying value:-100 ~ 100)
	UINT	m_uR;
	UINT	m_uG;
	UINT	m_uB;

	CImageArea(const CRect& area, CWnd* Parent/* = NULL*/, int IID);
	~CImageArea();

	D3DFORMAT GetDesktopD3DFormat() { return m_pD3DRender->GetSurfaceFormat();}
	IDirect3DDevice9* GetD3DDevice() { return m_pD3DRender->GetD3DDevice(); }
	CShapeDispManager* GetDispManager() { return m_pDisplayManager; }
	void ToNoZoomSym(CRect& NoZoomedSym) { m_ZoomedRect = NoZoomedSym; }
	const CPoint& ImgDst() { return m_ZoomedRealLT; }
	const CPoint& ImgSrc() { return m_ImageStartLT; }
	int DstImgWidth() { return m_iZoomedDstWidth; }
	int DstImgHeight() { return m_iZoomedDstHeight; }
	int SrcImgWidth() { return m_iZoomedSrcWidth; }
	int SrcImgHeight() { return m_iZoomedSrcHeight; }
	bool StreamExist() { return (m_pMediaStream != NULL); }
	CMediaStream& MixerStream() { return *m_pMediaStream; }
	const CRect& OriImageArea() { return m_RealityRect; }
#ifdef _ENABLE_MEDIA_STREAM_EX
	void InitMixerStream(CMediaStreamEx* pVal) { m_pMediaStream = pVal; }
#else
	void InitMixerStream(CMediaStream* pVal) { m_pMediaStream = pVal; }
#endif
	void CalImageSiteLT(const CRect& NoZoomedSym, const CRect& ZoomedSym)
	{
		CPoint tmpShowLT, tmpShowRB;

		//set LeftTop x,y of showed image
		if (ZoomedSym.left < NoZoomedSym.left)
			tmpShowLT.x = NoZoomedSym.left;
		else
			tmpShowLT.x = ZoomedSym.left;

		if (ZoomedSym.top < NoZoomedSym.top)
			tmpShowLT.y = NoZoomedSym.top;
		else
			tmpShowLT.y = ZoomedSym.top;

		//get RightBottom x,y of showed image
		if (ZoomedSym.right > NoZoomedSym.right)
			tmpShowRB.x = NoZoomedSym.right;
		else
			tmpShowRB.x = ZoomedSym.right;

		if (ZoomedSym.bottom > NoZoomedSym.bottom)
			tmpShowRB.y = NoZoomedSym.bottom;
		else
			tmpShowRB.y = ZoomedSym.bottom;

		m_ZoomedRect.SetRect(tmpShowLT.x, tmpShowLT.y, tmpShowRB.x, tmpShowRB.y);

#ifdef ori
		/*
		m_iZoomedSrcWidth = int(float(m_iDecImgWidth*m_ZoomedRect.Width()) / NoZoomedSym.Width());
		m_iZoomedSrcHeight = int(float(m_iDecImgHeight*m_ZoomedRect.Height()) / NoZoomedSym.Height());
		
		m_iZoomedDstWidth = int(float(m_RealityRect.Width()*m_ZoomedRect.Width()) / NoZoomedSym.Width());
		m_iZoomedDstHeight = int(float(m_RealityRect.Height()*m_ZoomedRect.Height()) / NoZoomedSym.Height());
		*/
#else
		/*
		if (m_ZoomedRect.EqualRect(&ZoomedSym))
		{
			m_iZoomedSrcWidth  = m_iDecImgWidth;
			m_iZoomedSrcHeight = m_iDecImgHeight;
		}
		else
		{
			m_iZoomedSrcWidth = int(float(m_iDecImgWidth*m_ZoomedRect.Width()) / ZoomedSym.Width());
			m_iZoomedSrcHeight = int(float(m_iDecImgHeight*m_ZoomedRect.Height()) / ZoomedSym.Height());
		}
		*/


		//src bottom right

		if ((NoZoomedSym.left <= ZoomedSym.right) && (ZoomedSym.right <= NoZoomedSym.right))
			m_iZoomedSrcWidth = m_iDecImgWidth;
		else //only occur on ZoomedSym.right > NoZoomedSym.right
			m_iZoomedSrcWidth = int(float(m_iDecImgWidth*m_ZoomedRect.Width()) / ZoomedSym.Width());

		if ((NoZoomedSym.top <= ZoomedSym.bottom) && (ZoomedSym.bottom <= NoZoomedSym.bottom))
			m_iZoomedSrcHeight = m_iDecImgHeight;
		else //only occur on zoomedSym.bottom > NoZoomedSym.bottom
			m_iZoomedSrcHeight = int(float(m_iDecImgHeight*m_ZoomedRect.Height()) / ZoomedSym.Height()); 
		

		//dst bottom right
		
		m_iZoomedDstWidth = int(float(m_icur_backbuf_width/*m_iDecImgWidth*/*(m_ZoomedRect.right-NoZoomedSym.left)) / NoZoomedSym.Width());
		m_iZoomedDstHeight = int(float(m_icur_backbuf_height/*m_iDecImgHeight*/*(m_ZoomedRect.bottom-NoZoomedSym.top)) / NoZoomedSym.Height());
#endif

		//dst top left
		
		SetZoomedRealLT(NoZoomedSym.TopLeft(), NoZoomedSym.BottomRight(), ZoomedSym.TopLeft(), ZoomedSym.BottomRight());

		//src top left
		
		SetImageStartLT(NoZoomedSym.TopLeft(), NoZoomedSym.BottomRight(), ZoomedSym.TopLeft(), ZoomedSym.BottomRight());
	}
};

// CMixerCtrlDlg 對話方塊

class CMixerCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CMixerCtrlDlg)	

	

private:
	enum 	{BRIGHT, HUE, RGB};
	enum	{LAYER1, LAYER2, MIXER, PLAY_THREAD_NUM};
	BOOL    m_bInitOK;
	BOOL 	m_bVolClicked;
	BOOL	m_bShow;
	int		m_iImageID;
	CPoint	m_PreActPnt;
	CPoint  m_OldMousePnt;	
	CPoint  m_VolCenter;
	CPoint	m_ZoomOriLTpnt;
	CPoint	m_ZoomOriRBpnt;
	float   m_fWscaling;
	float   m_fHscaling;
	float   m_fRadius;
	CClientDC* m_cdc;
	UCHAR	m_uActivePage;
	CComboBox m_EffectCombox;
	CComboBox m_PanelCombox;
	CSliderCtrl m_SysSndSldCtrl;
	CSpinButtonCtrl m_ZoomXspin;
	CSpinButtonCtrl m_ZoomYspin;
	CEdit m_ZoomXedit;
	CEdit m_ZoomYedit;
	CStatic m_ParamStatic[6];

	CStatic m_WidthStatic;
	CStatic m_HeightStatic;

	CStatic	m_SysSoundStatic;

	CStatic	m_Img1TimeStatic;
	CStatic	m_Img2TimeStatic;
	CStatic	m_MixerTimeStatic;
	
	CString m_sParmVal[6];
	CString m_sWidth;
	CString m_sHeight;
	CString	m_sSysSound;
	CString	m_sImg1Time;
	CString	m_sImg2Time;
	CString	m_sMixerTime;
	
	UCHAR	m_bDrawActIcon:1;

	CImageArea* m_cImage[PLAY_THREAD_NUM];
	CEdit m_FileNameEdit;

	
	void	VolPntPos(bool greater);
	void 	InitVolPntPos();
	void	DrawZoomRect();
	void	RefreshEffectText();
	void	RefreshEffectCtrl();
	void	RefreshRgbText();
	void	RefreshRgbCtrl();	
	void	PrintZoomXYText();
	void	AddItem(int iIDNum);
	
public:
	CMixerCtrlDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CMixerCtrlDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG };
#endif

protected:
	enum
	{
		TIMER_ITEM = WM_USER + 0x100,
		REC_ITEM,
		STOP_ITEM,
		PAUSE_ITEM,
		PLAY_ITEM,
		TO_START_ITEM,
		TO_END_ITEM,
		FORWARD_ITEM,
		BACKWARD_ITEM,
		BACK_PLAY_ITEM,

		//Image Slider Button iIDNum
		IMAGE1_SLIDER,
		IMAGE2_SLIDER,
		MIXER_SLIDER,

		//Direction iIDNum
		UP_BUTTON,
		DOWN_BUTTON,
		LEFT_BUTTON,
		RIGHT_BUTTON,

		//RGB Tab Page iIDNum
		BRIGHT_PAGE,
		HUE_PAGE,
		RGB_PAGE,
		
		//RGB Slider Button iIDNum
		COLOR1_SLIDER,
		COLOR2_SLIDER,
		COLOR3_SLIDER,

		//Effec Slider Button iIDNum
		LEVEL_SLIDER,
		PARAM1_SLIDER,
		PARAM2_SLIDER,
		PARAM3_SLIDER,
		PARAM4_SLIDER,
		PARAM5_SLIDER
	};

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	CMenu_Class* m_FramePic;
	BOOL m_bTimerEnable;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			void DrawVolPnt();
			void DrawActivePnt(CPoint point, int player_id);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnSelchange();
	afx_msg void OnSelPanelchange();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
};
