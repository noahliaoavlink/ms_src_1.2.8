#include "stdafx.h"
#include "ShapeDispManager.h"
#include "MixerEffect.h"


CShapeDispManager::CShapeDispManager()
{
	m_iUsingD3D = 0;

	m_iRenderStatus = RS_NORMAL;
}


CShapeDispManager::~CShapeDispManager()
{
}

void CShapeDispManager::SetRenderStatus(int iStatus)
{
	m_iRenderStatus = iStatus;
}

int CShapeDispManager::GetRenderStatus()
{
	return m_iRenderStatus;
}

void CShapeDispManager::DoMixerEffect(CMixerEffect* pMixerEffect, unsigned char* pBuffer, int w, int h, int iSurfaceIdx)
{
	IDirect3DDevice9* pCurD3DDevice = GetD3DDevice();

#ifdef _ENABLE_SINGLE_EFFECT
	if (pMixerEffect && w > 0 && h > 0 && EffectIsEnabled())
#else
	if (pMixerEffect && w > 0 && h > 0 )
#endif
	{
//		EnableToDoEffect(false);
		
		D3DXMATRIX matProj, matView, matWorld;
		(pCurD3DDevice->GetTransform(D3DTS_WORLD, &matWorld));
		(pCurD3DDevice->GetTransform(D3DTS_VIEW, &matView));
		(pCurD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
		
		if (pMixerEffect->GetWidth() != w || pMixerEffect->GetHeight() != h)
			pMixerEffect->ResetSurfaces(w, h);

#if 0
		IDirect3DSurface9* pOutSurface2 = pMixerEffect->DoVideoEffect(pBuffer, w, h);
		if (pOutSurface2)
			RenderSurface(iSurfaceIdx, VPF_RGB32, (unsigned char*)pOutSurface2, w, h);
#else
		IDirect3DSurface9* pOutSurface = pMixerEffect->DoVideoEffect2(pBuffer, w, h);
		if (pOutSurface)
			RenderSurface(iSurfaceIdx, VPF_RGB32, (unsigned char*)pOutSurface, w, h);
#endif
		
		(pCurD3DDevice->SetTransform(D3DTS_WORLD, &matWorld));
		(pCurD3DDevice->SetTransform(D3DTS_VIEW, &matView));
		(pCurD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));
		
	}
}

void CShapeDispManager::DoMixerEffect2(CMixerEffect* pMixerEffect, IDirect3DSurface9* pOutSurface, int w, int h, int iSurfaceIdx)
{
	IDirect3DDevice9* pCurD3DDevice = GetD3DDevice();

#ifdef _ENABLE_SINGLE_EFFECT
	if (pMixerEffect && w > 0 && h > 0 && EffectIsEnabled())
#else
	if (pMixerEffect && w > 0 && h > 0)
#endif
	{
//		EnableToDoEffect(false);

		if (pMixerEffect->GetWidth() != w || pMixerEffect->GetHeight() != h)
			pMixerEffect->ResetSurfaces(w, h);

		if (pOutSurface)
			RenderSurface(iSurfaceIdx, VPF_RGB32, (unsigned char*)pOutSurface, w, h);
	}
}

void CShapeDispManager::DoMixerEffect_Surface(CMixerEffect* pMixerEffect, IDirect3DSurface9* pSurface, int w, int h, int iSurfaceIdx)
{
	IDirect3DDevice9* pCurD3DDevice = GetD3DDevice();
#ifdef _ENABLE_SINGLE_EFFECT
	if (pMixerEffect && w > 0 && h > 0 && EffectIsEnabled())
#else
	if (pMixerEffect && w > 0 && h > 0)
#endif
	{
	//	EnableToDoEffect(false); 

		D3DXMATRIX matProj, matView, matWorld;
		(pCurD3DDevice->GetTransform(D3DTS_WORLD, &matWorld));
		(pCurD3DDevice->GetTransform(D3DTS_VIEW, &matView));
		(pCurD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));

		if (pMixerEffect->GetWidth() != w || pMixerEffect->GetHeight() != h)
			pMixerEffect->ResetSurfaces(w, h);

		IDirect3DSurface9* pOutSurface = pMixerEffect->DoVideoEffect_Surface(pSurface,w,h);
		if (pOutSurface)
			RenderSurface(iSurfaceIdx, VPF_RGB32, (unsigned char*)pOutSurface, w, h);

		
		(pCurD3DDevice->SetTransform(D3DTS_WORLD, &matWorld));
		(pCurD3DDevice->SetTransform(D3DTS_VIEW, &matView));
		(pCurD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));

	}
}

void CShapeDispManager::DoMixerEffect_SurfaceHD(CMixerEffect* pMixerEffect, IDirect3DSurface9* pSurface, int w, int h, int iSurfaceIdx)
{
	HRESULT hr;
#ifdef _ENABLE_HD_EFFECT
	D3DSurfaceItem* pD3DSurfaceItem = GetSurface(iSurfaceIdx);

	IDirect3DSurface9* pHDOutSurface = DoHDEffect(iSurfaceIdx,pSurface, w, h);
	if (pHDOutSurface)
	{
		hr = m_pD3DDeviceEx->StretchRect(pHDOutSurface, NULL, pD3DSurfaceItem->pOverlaySurface,NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
	}
	
#endif
}
