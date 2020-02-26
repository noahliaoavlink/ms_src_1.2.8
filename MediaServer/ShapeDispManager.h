#pragma once
#include "DisplayManager.h"

enum RenderStatus
{
	RS_WAIT = -1,
	RS_NORMAL = 0,
	RS_DRAW,
	RS_PRESENT,
};

//class CMixerEffect;

class CShapeDispManager :
	public DisplayManager
{
public:
	CShapeDispManager();
	~CShapeDispManager();
	void DoMixerEffect(CMixerEffect* pMixerEffect, unsigned char* pBuffer, int w, int h, int iSurfaceIdx);
	void DoMixerEffect2(CMixerEffect* pMixerEffect, IDirect3DSurface9* pOutSurface, int w, int h, int iSurfaceIdx);
	void DoMixerEffect_Surface(CMixerEffect* pMixerEffect, IDirect3DSurface9* pSurface, int w, int h, int iSurfaceIdx);
	void DoMixerEffect_SurfaceHD(CMixerEffect* pMixerEffect, IDirect3DSurface9* pSurface, int w, int h, int iSurfaceIdx);
	void SetPannelIdx(INT32 iPannelIdx) { m_iPannelIdx = iPannelIdx; };
	int IsD3DUsing() { return m_iUsingD3D; };
	void UsingD3D(int iUsingD3D) { m_iUsingD3D = iUsingD3D; };
	INT32 GetPannelIdx() { return m_iPannelIdx; };

	void SetRenderStatus(int iStatus);
	int GetRenderStatus();
private:
	int m_iPannelIdx;
	int m_iUsingD3D; //0: No Use, 1: Preview, 2: Loadding
	int m_iRenderStatus;
};

