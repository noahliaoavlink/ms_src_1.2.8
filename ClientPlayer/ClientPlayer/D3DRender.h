#pragma once

#ifndef _D3DRender_H_
#define _D3DRender_H_

#include <d3d9.h>
#include "../../Include/RenderCommon.h"

typedef LPDIRECT3D9(WINAPI *LPFND3DC)(UINT SDKVersion);

class D3DRender
{
	//IDirect3D9Ex             *m_d3d;
	//IDirect3DDevice9Ex       *m_d3dDevice;

	D3DFORMAT               m_VideoSurfaceFmt;
	int m_iWidth;
	int m_iHeight;
	HWND m_hWnd;
	HINSTANCE hD3DLib;

	//int m_iOffset;

	//mplayer
	D3DFORMAT desktop_fmt;          /**< Desktop (screen) colorspace format.
									Usually XRGB */
	int cur_backbuf_width;          /**< Current backbuffer width */
	int cur_backbuf_height;         /**< Current backbuffer height */
	IDirect3DSurface9 *d3d_backbuf; /**< Video card's back buffer (used to
									display next frame) */
	IDirect3DSurface9 *d3d_surface; /**< Offscreen Direct3D Surface. MPlayer
									renders inside it. Uses colorspace
									priv->movie_src_fmt */

	IDirect3D9* m_pD3D;
	IDirect3DDevice9* m_pD3DDevice;
	IDirect3D9Ex* m_pD3DEx;
	IDirect3DDevice9Ex* m_pD3DDeviceEx;
	D3DCAPS9 d3dCaps;
	bool m_bIsD3D9Ex;

	int m_iAdapter;
	bool m_bUseFlipEx;
	int m_iPixelFormat;
protected:
public:
	D3DRender();
	~D3DRender();

	LPFND3DC  OurDirect3DCreate9;

	int Init(HWND hWnd, int iPixelFormat, int iWidth, int iHeight);
	void Destroy();
	void DestroySurfaces();
	int GetDeviceInfo();
	int CreateDevice(HWND hWnd, bool bReset = false);
	int CreateSurfaces(int iWidth, int iHeight, D3DFORMAT format);
	int RestSurfaceFormat(int iPixelFormat, int iWidth, int iHeight);
	int RestSurfaces(int iWidth, int iHeight, D3DFORMAT format);
	void SetViewport(int iWidth, int iHeight);
	bool FormatSupported(D3DFORMAT surface, D3DFORMAT adaptor);
	void SetupDefaultRenderState();
	void Flip();
	int Reset();
	int RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight);
	int RenderFrame1(IDirect3DSurface9* pSurface, int iWidth, int iHeight);
	int RenderFrame1(IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rSrc);
	int BeginDrawSurface();
	int EndDrawSurface();
	int DrawSurface(IDirect3DSurface9* pSurface, int iWidth, int iHeight);
	int DrawSurface(IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rSrc);
	int UpdateSurfaceData(unsigned char* pBuffer, int iWidth, int iHeight);
	int UpdateSurfaceData(IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight);
	int GetD3DFormat(int iPixelFormat);
	HRESULT CheckD3D9Ex(void);
	IDirect3D9* GetDirect3D9Ptr();
	IDirect3DDevice9* GetD3DDevice();
	char* GetD3DFormatName(D3DFORMAT iFormat);
	void Clear();
	int GetWidth();
	int GetHeight();
};

#endif
