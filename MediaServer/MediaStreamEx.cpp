#include "stdafx.h"
#include "MediaStreamEx.h"
#include "MixerEffect.h"

CMediaStreamEx::CMediaStreamEx(HWND hWnd)
	:CMediaStream(hWnd)
{
	m_pD3DRender = 0;
//	m_pMixerEffect = 0;
}

CMediaStreamEx::~CMediaStreamEx()
{
	if (m_pD3DRender)
		delete m_pD3DRender;

//	if (m_pMixerEffect)
	//	delete m_pMixerEffect;
}

void CMediaStreamEx::Init(HWND hWnd)
{
//	m_pMixerEffect = new CMixerEffect;
	CMediaStream::Init(hWnd);
	CMediaStream::SetVideoRawDataCallback(this);
//	InitRender(hWnd);
}

void CMediaStreamEx::InitRender(HWND hWnd, void* pVoid)
{
	if (m_pD3DRender)
		delete m_pD3DRender;

	m_pD3DRender = new D3DRender();

	CMediaStream::SetVideoRawDataCallback(this);

	m_iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_pD3DRender->Init(hWnd, VPF_RGB32, m_iScreenWidth, m_iScreenHeight); //1920, 1080
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pMixerEffect->Create(m_pD3DRender->GetD3DDevice(), m_pD3DRender->GetSurfaceFormat(), m_pD3DRender->GetWidth(), m_pD3DRender->GetHeight());
}
/*
void CMediaStreamEx::SetMixerEffect(CMixerEffect* pObj)
{
	m_pMixerEffect = pObj;
}
*/
void CMediaStreamEx::SetEffectIndex(int iID)
{
	m_iEffectIdx = iID;
	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectID(iID);
}

int CMediaStreamEx::GetEffectIndex()
{
	if (m_pMixerEffect)
		return m_pMixerEffect->GetEffectID();
	return 0;
}

void CMediaStreamEx::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
{
	sEffectParam effect_param;
	effect_param.iLevel = iLevel;
	effect_param.iParm1 = iParm1;
	effect_param.iParm2 = iParm2;
	effect_param.iParm3 = iParm3;
	effect_param.iParm4 = iParm4;
	effect_param.iParm5 = iParm5;

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(effect_param);
}

void CMediaStreamEx::DisplayVideoRawData(unsigned char* pBuffer, int w, int h)
{
	if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
		{
			int iPixelFormat = VPF_RGB32;
			int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
			
			//if (m_pMixerEffect)
				//m_pMixerEffect->ResetSurfaces(w,h);
			}
		/*
		if (m_pMixerEffect && m_pMixerEffect->GetEffectID() != 0)
		{
			m_pD3DRender->UpdateSurfaceData(pBuffer, w, h);
			IDirect3DSurface9 *pOutFrameSurface = m_pMixerEffect->DoVideoEffect(m_pD3DRender->GetSurface());
			m_pD3DRender->RenderFrame1(pOutFrameSurface, w, h);
		}
		else */
			m_pD3DRender->RenderFrame(pBuffer, w, h);
		m_pD3DRender->Flip();
	}
}

void CMediaStreamEx::DisplayVideoSurfaceData(void* pSurface, int w, int h)
{
	if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
		{
			int iPixelFormat = VPF_RGB32;
			int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
		}

		m_pD3DRender->RenderFrame1((IDirect3DSurface9 *)pSurface, w, h);

		m_pD3DRender->Flip();
	}
}
void CMediaStreamEx::DisplayNV12SurfaceData(void* pSurface, int w, int h)
{

}