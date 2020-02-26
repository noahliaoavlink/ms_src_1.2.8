#pragma once

#include "BitSlider.h"
#include "MixerEffect.h"

#define IDC_STATIC_PREVIEW1	 9051
#define IDC_STATIC_PREVIEW2	 9052
#define IDC_STATIC_MIXER	 9053

#if 0
//MixerPreview inherit from CStatic
class MixerPreview: public CStatic
{
private:
	CWnd*   m_pParentWnd;

public:
	MixerPreview();
	~MixerPreview();

	void Create(const CRect& area, CWnd* Parent, int IID);
public:
	BOOL m_bPlaying;
	float m_fZoomRate;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
#else
//MixerPreview composite CStatic
class MixerPreview
{
private:
	int m_iHeight;
	int m_iWidth;
	CPoint m_ZoomedRealLT;
	CPoint m_ImageStartLT;
	int m_iZoomedSrcHeight;
	int m_iZoomedSrcWidth;
	int m_iZoomedDstHeight;
	int m_iZoomedDstWidth;
	CRect m_ZoomedRect;
	CWnd*   m_pParentWnd;
	D3DRender* m_pD3DRender;
	CShapeDispManager* m_pDisplayManager;
	CMediaStream*  m_pMediaStream;

	void SetImageStartLT(const CPoint& OriLT, const CPoint& OriRB, const CPoint& ZoomLT, const CPoint& ZoomRB);
	void SetZoomedRealLT(const CPoint& OriLT, const CPoint& OriRB, const CPoint& ZoomLT, const CPoint& ZoomRB);

public:
	CRect m_PreZmRectPos;
	int m_iListIdx;
	CStatic	m_PicCtrl;
	BOOL m_bPlaying;
	float m_fZoomRate;
	wchar_t m_sFileName[512];
	CBitSlider* m_pVideoSlider;
	//int m_iSoundPos;
	UCHAR	m_bZmPaintBit : 1;
	CPoint	m_ZoomLTpnt;
	CPoint	m_ZoomRBpnt;
	CMixerEffect m_cEffect;
	UCHAR	m_uEffItemBmp; //effect kinds and items mapping; bit position <others(...), bit 0> => <others(...), Level>

	MixerPreview();
	~MixerPreview();

	void Create(const CRect& area, CWnd* Parent, int IID, CBitSlider* video_slider);
	CShapeDispManager* GetDispManager() { return m_pDisplayManager; }
	D3DFORMAT GetDesktopD3DFormat() { return m_pD3DRender->GetSurfaceFormat(); }
	IDirect3DDevice9* GetD3DDevice() { return m_pD3DRender->GetD3DDevice(); }
	void RenderSurface(IDirect3DSurface9* pBuffer, int iWidth, int iHeight);
	void RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight);
	bool StreamExist() { return (m_pMediaStream != NULL); }
	CMediaStream& MixerStream() { return *m_pMediaStream; }
	void InitMixerStream(CMediaStream* pVal) { m_pMediaStream = pVal; }
	void ToNoZoomSym(CRect& NoZoomedSym) { m_ZoomedRect = NoZoomedSym; }
	void CalImageSiteLT(const CRect& NoZoomedSym, const CRect& ZoomedSym);
	int SrcImgWidth() { return m_iZoomedSrcWidth; }
	int SrcImgHeight() { return m_iZoomedSrcHeight; }
	int DstImgWidth() { return m_iZoomedDstWidth; }
	int DstImgHeight() { return m_iZoomedDstHeight; }
	const CPoint& ImgSrc() { return m_ImageStartLT; }
	const CPoint& ImgDst() { return m_ZoomedRealLT; }
	void SetWH(int width, int height);
	D3DRender* MxD3DRender() { return m_pD3DRender; }
};
#endif