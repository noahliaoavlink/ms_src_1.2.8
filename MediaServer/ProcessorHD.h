#pragma once

#ifndef _ProcessorHD_H_
#define _ProcessorHD_H_

//#include <d3d9.h>
//#include <dxvahd.h>

#include "DXVAHD_Sample.h"
#include "D3DSprite.h"

#define _BG_MODE 1


typedef struct
{
	int iWidth;
	int iHeight;
	IDirect3DTexture9* pTexture;
//	IDirect3DSurface9* pTextureSurface;
//	IDirect3DSurface9* pOffscreenSurface;
	int iAlpha;
	int iDrawAlpha;
}OverlayObj;

class ProcessorHD
{
	bool m_bDXVA_SW;

	HWND m_hWnd;
	int m_iWidth;
	int m_iHeight;

	// DXVA-HD interfaces
	IDXVAHD_Device          *m_pDXVAHD;
	IDXVAHD_VideoProcessor  *m_pDXVAVP;
	IDirect3DSurface9       *m_pMainStream;
	IDirect3DSurface9*      m_ppSubStream[SUB_STREAM_COUNT];

	IDirect3DSurface9* m_pGPUSurface;

	DXVAHD_DEVICE_USAGE m_usage;    // DXVA-HD device usage (command-line parameter)

	//IDirect3D9* m_pD3D;
	IDirect3DDevice9Ex* m_pD3DDeviceEx;

	// Proc amp filtering information

	struct ProcAmpInfo
	{
		BOOL                        bSupported;
		DXVAHD_FILTER_RANGE_DATA    Range;
		INT                         CurrentValue;
		INT                         Step;
	};

	ProcAmpInfo m_Filters[NUM_FILTERS];

	INT     m_TargetWidthPercent;
	INT     m_TargetHeightPercent;
	RECT    m_rcMainVideoDestRect;      // Destination rectangle for the main video image.

	unsigned long m_ulFrameCount;
	int m_iGPUSurfaceCount;

	DXVAHD_VPDEVCAPS caps;
	int m_iSurfaceW;
	int m_iSurfaceH;

#if _BG_MODE
	IDirect3DTexture9* m_pBGTexture;
	IDirect3DSurface9* m_pBGTextureSurface;
#endif

	DXVAHD_COLOR m_CurBGColor;

	bool m_bFinished;
	float m_fAlpha;

/*	
	D3DSprite* m_pD3DSprite;
//	OverlayObj m_pFade;

	OverlayObj m_RedOverlay;
	OverlayObj m_GreenOverlay;
	OverlayObj m_BlueOverlay;
	OverlayObj m_BabyBlueOverlay; //(R-)
	OverlayObj m_YellowOverlay;   //(B-)
	OverlayObj m_PurpleOverlay;   //(G-)
	OverlayObj m_WhiteOverlay;    //(RGB)
	OverlayObj m_BlackOverlay;    //(RGB-)
	*/
protected:
public:
	ProcessorHD();
	~ProcessorHD();

	bool Init(HWND hWnd, IDirect3DDevice9Ex* pD3DDeviceEx, int w, int h);
	HRESULT AdjustFilter(DXVAHD_FILTER filter, int dy);
	HRESULT AdjustTargetRect(int dx, int dy);
	IDirect3DSurface9* GetMainStream();
	void CalculateSubstreamRect(INT frame, RECT *prect);
	int ProcessVideoFrame();
	void Flip();

	void SetBrightness(int iValue);
	void SetContrast(int iValue);
	void SetHue(int iValue);
	void SetSaturation(int iValue);

	int CreateSurface(D3DFORMAT format, int iWidth, int iHeight);
	int RestSurface(D3DFORMAT format, int iWidth, int iHeight);
	void DestroySurface();
	int GetSurfaceWidth();
	int GetSurfaceHeight();
	void UpdateSurface(IDirect3DSurface9* pSurface);

	IDirect3DTexture9* CreateOverlayTexture(int iWidth, int iHeight);
	void DestroyOverlayResource(IDirect3DSurface9* pSurface, IDirect3DTexture9* pTexture);
	//int CreateFadeOverlay(int iWidth, int iHeight);
	int CreateRGBColorOverlay();

	void SetRedAlpha(int iValue);
	void SetBabyBlueAlpha(int iValue);
	void SetGreenAlpha(int iValue);
	void SetPurpleAlpha(int iValue);
	void SetBlueAlpha(int iValue);
	void SetYellowAlpha(int iValue);
	void DrawOverlay(OverlayObj* pObj);
	void ComputeOverlayAlpha();

	IDirect3DSurface9* GetBGSurface();
	void ResetFliter();
	void SetBGColor(int r, int g, int b);
	void SetBGColor_R(int r);
	void SetBGColor_G(int g);
	void SetBGColor_B(int b);
	void SetPlanarAlpha(int iValue);

	void Reset();
	void WaitForFinished();
};

#endif
