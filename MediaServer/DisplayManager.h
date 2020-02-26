#pragma once

#ifndef _DisplayManager_H_
#define _DisplayManager_H_

#include <d3d9.h>

#include "RenderCommon.h"
#include "..\\..\\..\\Include\\SQList.h"
#include "MFRecorderDll.h"

#ifdef _ENABLE_SINGLE_EFFECT
#include "MixerEffect.h"
#endif

#ifdef _ENABLE_GPU
#include "ProcessorHD.h"
#endif

#define _MAX_RECT 10

#define _ENABLE_LINE_MARK 1

enum MarkMode
{
	MM_NONE = 0,
	MM_LINE,
	MM_COLOR_BAR,
};

typedef struct
{
	IDirect3DSurface9* pD3DSurface;
	IDirect3DTexture9* pOverlayTexture;
	IDirect3DSurface9* pOverlaySurface;
	int iWidth;
	int iHeight;
	int iPixelFormat;
	bool bI420ToYV12;
}D3DSurfaceItem;

class FPSCounter
{
	DWORD m_dwLastTime;
	int m_iCount;
	float m_fFPS;
	Average m_AvgFPS;
public:
	FPSCounter();
	~FPSCounter();

	void CalFPS();
	float GetAvgFPS();
};

class DisplayManager
{
	HWND m_hWnd;
	HWND m_hPanelWnd;
	int m_iAdapter;
	SQList* m_pD3DSurfaceList;

	int m_iWidth;
	int m_iHeight;
	D3DFORMAT m_DesktopD3DFormat;

	bool m_bDoI420ToYV12;

	RECT m_RectList[_MAX_RECT];

	int m_iItemIndex;

	D3DSurfaceItem* m_pTmpD3DSurfaceItem;

	int m_iOffset;

	DWORD m_dwLastTime;
	bool m_bIsAlive;
	HANDLE m_hThread;
	CRITICAL_SECTION m_CriticalSection;

	unsigned char* m_pRawDataBuffer;
	IDirect3DSurface9* m_pRawDataSurface;
	bool m_bEnableRecordAVI;
	MFRecorderDll* m_pMFRecorderDll;

	unsigned char* m_pBlackBuffer;

	bool m_bGPUIsEnabled;
	bool m_bIsSuspend;
	int m_iSuspendCount;

	D3DPRESENT_PARAMETERS* m_pD3Dpp;
	HWND m_hTimeLineHwnd;
	int m_iPanelIndex;
	bool m_bIsDeivceLost;

	FPSCounter m_FPSCounter[16];

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	D3DPen* m_pD3DPen[6];
	D3DPen* m_pD3DColorBarPen[6];
	int m_iMarkMode;

//	HPEN m_hPen[6];
#endif

	int m_iFrameCount;
	bool m_bIsD3D9Ex;
	double m_dCurPos[16];
	bool m_bUpdate[16];

	int m_iPresentInterval;

#ifdef _ENABLE_SINGLE_EFFECT
	CMixerEffect* m_pMixerEffect;
	IDirect3DTexture9* m_pOffscreenTexture;
	IDirect3DSurface9* m_pOffscreenSurface;

	bool m_bDoEffect;
#endif

#ifdef _ENABLE_GPU
	ProcessorHD* m_pProcessorHDs[10];
#endif

protected:
	IDirect3DDevice9* m_pD3DDevice;
	IDirect3DDevice9Ex* m_pD3DDeviceEx;
public:
	DisplayManager();
	~DisplayManager();

	void Init(HWND hWnd, IDirect3DDevice9* pD3DDevice,bool bIsD3D9Ex = false);
	void Start();
	void Stop();

	void CheckAdapterInfo();

	void SetRect(int iIndex, RECT rect);

	int CreateSurfaceItem();
	void DestroyAllD3DSurfaces();
	void DestroyOverlayResource(IDirect3DSurface9* pSurface, IDirect3DTexture9* pTexture);
	void DeleteSurface(IDirect3DSurface9* pSurface);
	D3DSurfaceItem* GetSurface(int iIndex);
	int GetTotalOfSurfaceItems();
	IDirect3DDevice9* GetD3DDevice();
	IDirect3DTexture9* CreateOverlayTexture(int iWidth, int iHeight);
	bool FormatSupported(D3DFORMAT surface);
	int GetD3DFormat(int iPixelFormat);
	IDirect3DSurface9* CreateSurfaces(int iWidth, int iHeight, D3DFORMAT format);
	void EnableI420ToYV12(bool bEnable);
	D3DFORMAT GetDesktopD3DFormat();

	int UpdateSurfaceData(IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight);
	int RenderFrame_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight, RECT rDest);
	int RenderFrame1_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rDest);
	int DrawSurface_OS(IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rDest);
	int DrawSurface_OS(IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, RECT rSrc, RECT rDest);
	int RenderFrame1_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, RECT rSrc, RECT rDest);

	int RenderFrame(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight);
	int RenderSurface(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight);

	int RenderFrame_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight, RECT rSrc, RECT rDest);

	int RenderSurface(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight, int iPanelIndex);
	int RenderFrame(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight, int iPanelIndex);

	int Merge();
	void Flip();

	bool IsAlive();
	void Suspend();
	void Resume();
	bool IsSuspend();

	void CaptureRenderTarget(IDirect3DSurface9 *pRenderTarget);
	void RecordAudio(unsigned char* pBuffer, int iLen);
	int D3DSurface2RawData(IDirect3DSurface9* pInSurface, int iWidth, int iHeight, unsigned char* pOutBuffer);
	void EnableRecordAVI(bool bEnable, float fFPS, int iChannels, int iSampleRate, int iBits);
	bool IsRecordAVI();
	void SetPanelHWND(HWND hWnd);
	HWND GetPanelHWND();
	void ClearAll();
	void Clear(int iIndex, int w, int h);
	void ResetAllD3DSurfaces();
	void EnableGPU(bool bEnable);

	void SetD3dPresentParamters(D3DPRESENT_PARAMETERS* pD3Dpp);
	void CheckDeviceState();
	void SetTimeLineHwnd(HWND hWnd);
	void SetPanelIndex(int iIndex);
	void ResetD3DDevice(IDirect3DDevice9* pD3DDevice);
	bool IsDeivceLost();
	HWND GetTimeLineHwnd();
	float GetAvgFPS(int iIndex);
	void ApplySetting();

	void TimeEvent();

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	void DrawLineMark(int iWidth, int iHigh);
//	void DrawLineMark(IDirect3DSurface9* pD3DSurface, int iWidth, int iHigh);
	void DrawColorBarMark(int iWidth, int iHigh);
	void SetMarkMode(int iMode);
#endif
	int GetFrameCount();
	void UpdateFrameCount(int iValue);
	void SetCurPos(int iIndex, double dPos);
	double GetCurPos(int iIndex);

#ifdef _ENABLE_SINGLE_EFFECT
	int CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h);
	CMixerEffect* GetMixerEffect();
	void SetEffectIndex(int iID);
	int GetEffectIndex();
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectParam(int iIndex, int iValue);
	INT32 GetEffectParam(int iIndex);
	
	void EnableToDoEffect(bool bEnable);
	bool EffectIsEnabled();
#endif

	IDirect3DSurface9* DoHDEffect(int iIndex, IDirect3DSurface9* pIn, int w, int h);
	void DoHDEffect2(int iIndex, IDirect3DSurface9* pIn, int w, int h);
	void DoHDEffect3(int iIndex, IDirect3DSurface9* pIn, int w, int h);
	void DoAllHDEffects();
	void ResetHDEffect();
	void WaitForHDEffectFinished();
	IDirect3DSurface9* GetHDSurface(int iIndex);
};

#endif
