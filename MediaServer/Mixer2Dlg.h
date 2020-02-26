#pragma once

#include "Item_Class.h"
#include "Moveable_Item.h"
#include "ImageFile.h"
#include "RoundSliderCtrl.h"
#include "BitSlider.h"
#include "CEditCtrl.h"
#include "IconListCtrl.h"
#include "IOSourceCtrlDll.h"
#include "MixerPreview.h"

#define _MAX_SOURCE 10

// CMixer2Dlg dialog

class CMixer2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMixer2Dlg)

	typedef struct
	{
		CRect Rgn;

	}PrevRgn;

	enum { LAYER1, LAYER2, MIXER, PREVIEW_NUM };

private:
	int m_iXedit;
	int m_iYedit;
	CComboBox m_EffectCombox;
	CStatic m_ParamStatic[6];
	CString m_sParmVal[6];
	CPoint  m_OldMousePnt;
	CString m_sHeight;
	CString m_sWidth;
	BOOL    m_bInitOK;
	CClientDC* m_cdc;
	int	m_iImageID;
	MixerPreview* m_pMxPrw[PREVIEW_NUM];
	CRect m_rPreviewRgn[PREVIEW_NUM];
	YUVConverter m_yuv_converter;
	unsigned char* m_pRGB32Buf;
	CImageList m_ImageList;
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	bool m_bPreviewChg;
	bool m_bUpdateVol;
	CPoint	m_ZoomOriLTpnt;
	CPoint	m_ZoomOriRBpnt;

	int m_iStreamIndexMapping[_MAX_SOURCE];

	void DrawActivePnt(int previewID);
	void DrawRect(HWND hWnd, HDC hDC, HPEN hPen);
	void DrawZoomRect();
	void AddItem(int iIDNum);
	void RefreshEffectCtrl();
	void RefreshEffectText();
	int  VolToPos(int vol);
	void RedrawDlgUICtrl();
public:
	CMixer2Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMixer2Dlg();

	void DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h, int iColorKeyR = -1, int iColorKeyG = -1, int iColorKeyB = -1);
	void DrawUI();
	void DrawText();
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG };
#endif

protected:

	enum
	{
		TIMER_ITEM = WM_USER + 0x100,
		V1_STOP_ITEM,
		V1_PAUSE_ITEM,
		V1_PLAY_ITEM,
		V1_TO_START_ITEM,
		V1_TO_END_ITEM,
		V1_SLIDER,

		V2_STOP_ITEM,
		V2_PAUSE_ITEM,
		V2_PLAY_ITEM,
		V2_TO_START_ITEM,
		V2_TO_END_ITEM,
		V2_SLIDER,

		V3_STOP_ITEM,
		V3_PAUSE_ITEM,
		V3_PLAY_ITEM,
		V3_TO_START_ITEM,
		V3_TO_END_ITEM,
		V3_SLIDER,

		//Direction iIDNum
		UP_BUTTON,
		DOWN_BUTTON,
		LEFT_BUTTON,
		RIGHT_BUTTON,

		ROTATE,

		//Effec Slider Button iIDNum
		LEVEL_SLIDER,
		PARAM1_SLIDER,
		PARAM2_SLIDER,
		PARAM3_SLIDER,
		PARAM4_SLIDER,
		PARAM5_SLIDER
	};

	CMenu_Class* m_FramePic;
	ImageFile m_ZoomCtrlBmp;
	ImageFile m_VolumeCtrlBmp;
	HDC m_hMemDC;

	CRoundSliderCtrl* m_pVolumeCtrl;
	CBitSlider* m_pVideo1Slider;
	CBitSlider* m_pVideo2Slider;
	CBitSlider* m_pVideo3Slider;
	CBitSlider* m_pVolumeSlider;
	CEditCtrl* m_pX_EditCtrl;
	CEditCtrl* m_pY_EditCtrl;
	CEditCtrl* m_pWidth_EditCtrl;
	CEditCtrl* m_pHeight_EditCtrl;
	CEditCtrl* m_pAngle_EditCtrl;
	IconListCtrl* m_pIconListCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnVolChgNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSeek(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClrExistZoomRect(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSelchange();
};
