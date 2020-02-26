#include "stdafx.h"
#include "MixerEffect.h"

CMixerEffect::CMixerEffect()
{
	m_ieffectID = 0;
	m_pTargetSurface = NULL;

#ifdef	_ENABLE_MEDIA_STREAM_EX
	m_BufTexture = NULL;
	m_surBuffaces = NULL;
	m_Buf3DTexture = NULL;
	m_sur3DBuf = NULL;
#endif 
	m_pEffectSurface = NULL;
	m_pImgBuf = NULL;

	m_rectSrc = CRect(0, 0, 0, 0);
	m_rectDest = CRect(0, 0, 0, 0);

	//m_EffectParam = g_EffectParam;
	m_EffectParam.iLevel = m_EffectParam.iParm1 = m_EffectParam.iParm2 = m_EffectParam.iParm3 
		                 = m_EffectParam.iParm4 = m_EffectParam.iParm5 = 50;

	m_D3DParam.pVideoStream = 0;
	m_D3DParam.pzSurface = 0;
	m_pTempSurface = 0;
	m_pYUVSurface = 0;

	m_pOutSurface = 0;

	m_bInit3DEffect = true;

	m_bBusyFlag = FALSE;
	InitializeCriticalSection(&m_csInUse);
}

CMixerEffect::~CMixerEffect()
{	
	DestroySurfaces();

	m_ImgEffect3D.Destroy();
}

int CMixerEffect::GetEffectID()
{
	return	m_ieffectID;
}

void CMixerEffect::SetEffectID(int id)
{
	m_ieffectID = id;
}

sEffectParam& CMixerEffect::GetEffectParam()
{
	return m_EffectParam;
}

void CMixerEffect::SetEffectParam(const sEffectParam& val)
{	
//	while (m_bBusyFlag)
//		Sleep(1);

	m_EffectParam = val;
}

void CMixerEffect::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
{
//	while (m_bBusyFlag)
//		Sleep(1);

	m_EffectParam.iLevel = iLevel;
	m_EffectParam.iParm1 = iParm1;
	m_EffectParam.iParm2 = iParm2;
	m_EffectParam.iParm3 = iParm3;
	m_EffectParam.iParm4 = iParm4;
	m_EffectParam.iParm5 = iParm5;
}

void CMixerEffect::SetEffectParam(int iIndex, int iValue)
{
//	while (m_bBusyFlag)
//		Sleep(1);

	if (iIndex == 0) m_EffectParam.iLevel = iValue;
	else if (iIndex == 1) m_EffectParam.iParm1 = iValue;
	else if (iIndex == 2) m_EffectParam.iParm2 = iValue;
	else if (iIndex == 3) m_EffectParam.iParm3 = iValue;
	else if (iIndex == 4) m_EffectParam.iParm4 = iValue;
	else if (iIndex == 5) m_EffectParam.iParm5 = iValue;
}

INT32 CMixerEffect::GetEffectParam(int iIndex)
{
	if (iIndex == 0) return m_EffectParam.iLevel;
	else if (iIndex == 1) return m_EffectParam.iParm1;
	else if (iIndex == 2) return m_EffectParam.iParm2;
	else if (iIndex == 3) return m_EffectParam.iParm3;
	else if (iIndex == 4) return m_EffectParam.iParm4;
	else if (iIndex == 5) return m_EffectParam.iParm5;
}

int CMixerEffect::CreateSurfaces(IDirect3DDevice9* pD3DDev, int iWidth, int iHeight)
{  
	char szMsg[512];
	HRESULT hr;
	D3DDISPLAYMODE dm;

	m_D3DParam.d3ddev = pD3DDev;
	V(pD3DDev->GetDisplayMode(NULL, &dm));

	void* pData = NULL;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_rectSrc = CRect(0, 0, m_iWidth, m_iHeight);
	m_rectDest = CRect(0, 0, m_iWidth, m_iHeight);

	if (m_pImgBuf)
	{
		delete m_pImgBuf;
		m_pImgBuf = NULL;
	}

	m_pImgBuf = new BYTE[m_iWidth * m_iHeight * 4];

	V(pD3DDev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_D3DParam.pOutFrameTexture, NULL));
	//V(pD3DDev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_D3DParam.pOffscreenTexture3D, NULL));
	V(pD3DDev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_D3DParam.pOffscreenTexture3D, NULL));


	//V(pD3DDev->CreateOffscreenPlainSurface(iWidth, iHeight, dm.Format, D3DPOOL_SYSTEMMEM, &m_D3DParam.pOffscreenSurface2D, NULL));
	V(pD3DDev->CreateOffscreenPlainSurface(iWidth, iHeight, dm.Format, D3DPOOL_DEFAULT, &m_D3DParam.pVideoStream, NULL));
	V(pD3DDev->CreateDepthStencilSurface(iWidth, iHeight, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_D3DParam.pzSurface, NULL));
	V(pD3DDev->CreateOffscreenPlainSurface(iWidth, iHeight, dm.Format, D3DPOOL_SYSTEMMEM, &m_pEffectSurface, NULL));
	V(pD3DDev->CreateOffscreenPlainSurface(iWidth, iHeight, dm.Format, D3DPOOL_DEFAULT, &m_pTempSurface, NULL));

	V(pD3DDev->CreateOffscreenPlainSurface(iWidth, iHeight, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DPOOL_DEFAULT, &m_pYUVSurface, NULL));

	if (m_bInit3DEffect)
	{
		m_bInit3DEffect = false;
		m_ImgEffect3D.Create(pD3DDev);
	}

	m_D3DParam.d3ddev->GetRenderTarget(0, &m_pTargetSurface);

	return 0;
}

#ifdef	_ENABLE_MEDIA_STREAM_EX
int CMixerEffect::CreateSurfaces(D3DRender* pD3DRender)
{
	pD3DRender->m_bIsD3D9Ex? ASSERT(pD3DRender->m_pD3DDeviceEx): ASSERT(pD3DRender->m_pD3DDevice);

	int iWidth = pD3DRender->m_iWidth;
	int iHeight= pD3DRender->m_iHeight;
	IDirect3DDevice9* m_D3DDev = pD3DRender->m_bIsD3D9Ex? pD3DRender->m_pD3DDeviceEx: pD3DRender->m_pD3DDevice;
	HRESULT hr;
	D3DDISPLAYMODE dm;
	D3DSURFACE_DESC rectSurface;

	V( m_D3DDev->GetDisplayMode(NULL,  &dm));

	void* pData=NULL;

	if (iWidth == 0)
		pD3DRender->d3d_surface->GetDesc(&rectSurface);
	else
	{
		rectSurface.Width = iWidth;
		rectSurface.Height = iHeight;
	}
	V(m_D3DDev->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, pD3DRender->m_VideoSurfaceFmt, D3DPOOL_DEFAULT, &m_BufTexture, NULL));
	V(m_D3DDev->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, pD3DRender->m_VideoSurfaceFmt, D3DPOOL_DEFAULT, &m_Buf3DTexture, NULL));
	V(m_D3DDev->CreateOffscreenPlainSurface(rectSurface.Width, rectSurface.Height, pD3DRender->m_VideoSurfaceFmt, D3DPOOL_SYSTEMMEM, &m_surBuffaces, NULL));
	V(m_D3DDev->CreateDepthStencilSurface(rectSurface.Width, rectSurface.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_sur3DBuf, NULL));

	//pD3DRender->m_VideoSurfaceFmt
	//d3d_surface

	m_ImgEffect3D.Create(m_D3DDev);
	return 0;
}
#endif

void CMixerEffect::DestroySurfaces()
{
#ifdef	_ENABLE_MEDIA_STREAM_EX
	if (m_BufTexture)
	{
		m_BufTexture->Release();
		m_BufTexture = NULL;
	}
	if (m_surBuffaces)
	{
		m_surBuffaces->Release();
		m_surBuffaces = NULL;
	}
	if (m_Buf3DTexture)
	{
		m_Buf3DTexture->Release();
		m_Buf3DTexture = NULL;
	}
	if (m_sur3DBuf)
	{
		m_sur3DBuf->Release();
		m_sur3DBuf = NULL;
	}
#endif

	if (m_pEffectSurface)
	{
		m_pEffectSurface->Release();
		m_pEffectSurface = NULL;
	}

	if (m_pImgBuf)
	{
		delete m_pImgBuf;
		m_pImgBuf = NULL;
	}

	if (m_D3DParam.pOutFrameTexture)
	{  
		m_D3DParam.pOutFrameTexture->Release();
		m_D3DParam.pOutFrameTexture = NULL;
	}
	
	if (m_D3DParam.pOffscreenTexture3D)
	{ 
		m_D3DParam.pOffscreenTexture3D->Release();
		m_D3DParam.pOffscreenTexture3D = NULL;
	}
	/*
	if (m_D3DParam.pOffscreenSurface2D)
	{
		m_D3DParam.pOffscreenSurface2D->Release();
		m_D3DParam.pOffscreenSurface2D = NULL;
	}
	*/
	if (m_D3DParam.pVideoStream)
	{
		m_D3DParam.pVideoStream->Release();
		m_D3DParam.pVideoStream = NULL;
	}

	if (m_D3DParam.pzSurface)
	{
		m_D3DParam.pzSurface->Release();
		m_D3DParam.pzSurface = NULL;
	}

	if (m_pEffectSurface)
	{
		m_pEffectSurface->Release();
		m_pEffectSurface = NULL;
	}

	if (m_pTempSurface)
	{
		m_pTempSurface->Release();
		m_pTempSurface = NULL;
	}

	if (m_pYUVSurface)
	{
		m_pYUVSurface->Release();
		m_pYUVSurface = NULL;
	}

	m_D3DParam.d3ddev = NULL;

	m_rectSrc = CRect(0, 0, 0, 0);
	m_rectDest = CRect(0, 0, 0, 0);
//	m_ImgEffect3D.Destroy();
	if(m_pTargetSurface)
		m_pTargetSurface->Release();
	m_pTargetSurface = NULL;
}

void CMixerEffect::VideoEffect(BYTE* pData, INT32 iWidth, INT32 iHeight)
{
	EnterCriticalSection(&m_csInUse);
	VideoEffect2(pData, iWidth,iHeight);
	LockSurface();
	LeaveCriticalSection(&m_csInUse);
}

void CMixerEffect::VideoEffect2(BYTE* pData, INT32 iWidth, INT32 iHeight)
{
	HRESULT hr = S_OK;
	D3DLOCKED_RECT d3drect;
 
	m_bBusyFlag = TRUE;

#if 0

	{
		V(m_pTempSurface->LockRect(&d3drect, NULL, 0));

		unsigned char *pDest = (unsigned char*)d3drect.pBits;

		if (pDest)
		{
			if (d3drect.Pitch / 4 != m_iWidth)
			{
				unsigned char *src = pData;
				int iBytesbyPerLine = m_iWidth * 4;
				for (int i = 0; i < m_iHeight; i++)
				{
					memcpy(pDest, src, iBytesbyPerLine);
					src += iBytesbyPerLine;
					pDest += d3drect.Pitch;
				}
			}
			else
				memcpy(pDest, pData, m_iWidth * m_iHeight * 4);
		}

		V(m_pTempSurface->UnlockRect());

		V(m_D3DParam.d3ddev->StretchRect(m_pTempSurface, &m_rectSrc,
			m_pTargetSurface, &m_rectDest, D3DTEXF_NONE));

		m_D3DParam.pVideoStream = m_pTargetSurface;
	}
#else
	CRect rectOrigin = CRect(0, 0, m_iWidth, m_iHeight);

	if (m_rectSrc != rectOrigin || m_rectDest != rectOrigin)
	{
		V(m_pTempSurface->LockRect(&d3drect, NULL, 0));
		unsigned char *pDest = (unsigned char*)d3drect.pBits;

		memcpy(pDest, pData, m_iWidth * m_iHeight * 4);
		V(m_pTempSurface->UnlockRect());

		V(m_D3DParam.d3ddev->StretchRect(m_pTempSurface, &m_rectSrc,
			m_pTargetSurface, &m_rectDest, D3DTEXF_NONE));

		m_D3DParam.pVideoStream = m_pTargetSurface;
	}
	else
	{
		V(m_D3DParam.pVideoStream->LockRect(&d3drect, NULL, 0));
		unsigned char *pDest = (unsigned char*)d3drect.pBits;

		if (d3drect.Pitch / 4 != m_iWidth)
		{
			unsigned char *src = pData;
			int iBytesbyPerLine = m_iWidth * 4;
			for (int i = 0; i < m_iHeight; i++)
			{
				memcpy(pDest, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				pDest += d3drect.Pitch;
			}
		}
		else
			memcpy(pDest, pData, m_iWidth * m_iHeight * 4);

		V(m_D3DParam.pVideoStream->UnlockRect());
	}

#endif

	D3DXMATRIX matProj, matView, matWorld;

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_PROJECTION, &matProj));
	}

	switch (m_ieffectID)
	{
	case 0: m_ImgEffect.NormalVideo(m_D3DParam, m_EffectParam);				break;
	case 1: m_ImgEffect3D.Negative(m_D3DParam, m_EffectParam);				break;
	case 2: m_ImgEffect3D.Gray(m_D3DParam, m_EffectParam);					break;
	case 3: m_ImgEffect3D.RGBColor(m_D3DParam, m_EffectParam);				break;
		//case 4:	m_ImgEffect.Brightness( m_D3DParam, m_EffectParam );				break;
	case 4:	m_ImgEffect3D.BrightnessFlicker(m_D3DParam, m_EffectParam);		break;
	case 5:	m_ImgEffect.SleazyEmboss(m_D3DParam, m_EffectParam);			break;
	case 6:	m_ImgEffect3D.RectangleBrightDark(m_D3DParam, m_EffectParam);	break;
	case 7: m_ImgEffect.DelayMovie(m_D3DParam, m_EffectParam);				break;
	case 8: m_ImgEffect3D.MultiMovie(m_D3DParam, m_EffectParam);			break;
	case 9: m_ImgEffect3D.Pixelate(m_D3DParam, m_EffectParam);				break;
	case 10: m_ImgEffect3D.RotateScale(m_D3DParam, m_EffectParam);			break;
		//case 12: m_ImgEffect.ColorConvert( m_D3DParam, m_EffectParam );			break;
	case 11: m_ImgEffect3D.Crosshatching(m_D3DParam, m_EffectParam);			break;
	case 12: m_ImgEffect3D.SwirlTexture(m_D3DParam, m_EffectParam);		break;
	case 13: m_ImgEffect3D.BlindTexture(m_D3DParam, m_EffectParam);			break;
	case 14: m_ImgEffect3D.NeonLightTexture(m_D3DParam, m_EffectParam);		break;
	case 15: m_ImgEffect3D.HSL_RGB(m_D3DParam, m_EffectParam);				break;
	case 16: m_ImgEffect3D.WindowsNormal(m_D3DParam, m_EffectParam);			break;
	case 17: m_ImgEffect3D.Ripple(m_D3DParam, m_EffectParam);				break;
	case 18: m_ImgEffect3D.FishEye(m_D3DParam, m_EffectParam);				break;
	case 19: m_ImgEffect3D.Halftone(m_D3DParam, m_EffectParam);				break;
	case 20: m_ImgEffect3D.WoodNoise(m_D3DParam, m_EffectParam);				break;
	case 21: m_ImgEffect3D.Edge(m_D3DParam, m_EffectParam);					break;
	case 22: m_ImgEffect3D.Wave(m_D3DParam, m_EffectParam);					break;
	case 23: m_ImgEffect3D.WaterColor(m_D3DParam, m_EffectParam);			break;
	case 24: m_ImgEffect3D.Kaleidoscope(m_D3DParam, m_EffectParam);			break;
	case 25: m_ImgEffect3D.CircleCenter(m_D3DParam, m_EffectParam);			break;
	case 26: m_ImgEffect3D.CylinderRotate(m_D3DParam, m_EffectParam);		break;
	case 27: m_ImgEffect3D.PlaneRotate(m_D3DParam, m_EffectParam);			break;
	case 28: m_ImgEffect3D.DotMosaics(m_D3DParam, m_EffectParam);			break;
	case 29: m_ImgEffect3D.SingleColor(m_D3DParam, m_EffectParam);			break;
	case 30: m_ImgEffect3D.Blur(m_D3DParam, m_EffectParam);			        break;
	case 31: m_ImgEffect3D.Mirage(m_D3DParam, m_EffectParam);			    break;
	default:		LOG("Error");		break;
	}

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_PROJECTION, &matProj));
	}

	m_bBusyFlag = FALSE;
}

void CMixerEffect::VideoEffect3(BYTE* pData, INT32 iWidth, INT32 iHeight,int iForamt)
{
	HRESULT hr = S_OK;
	D3DLOCKED_RECT d3drect;

	CRect rectOrigin = CRect(0, 0, m_iWidth, m_iHeight);

	IDirect3DSurface9 *offTextureSurface3D = 0;

	//m_pYUVSurface
	
	if(iForamt == PF_YUV)
	//I420 -> YV12
	{
		hr = m_pYUVSurface->LockRect(&d3drect, NULL, 0);

		if (d3drect.Pitch == 0)
		{
			m_ImgEffect3D.SetStatus(-3);
			hr = m_pYUVSurface->UnlockRect();
			return;
		}

		unsigned char *pDest = (unsigned char*)d3drect.pBits;
		//memcpy(pDest, pData, m_iWidth * m_iHeight * 3 /2);

		int i = 0;
		unsigned char *dst = (unsigned char*)d3drect.pBits;
		unsigned char *src = pData;

		int chroma_width = iWidth >> 1;
		int chroma_height = iHeight >> 1;
		int chroma_pitch = d3drect.Pitch >> 1;

		for (i = 0; i < iHeight; i++)
		{
			memcpy(dst, src, iWidth);
			dst += d3drect.Pitch;
			src += iWidth;
		}

		dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch);
		src = pData + (iHeight * iWidth * 5 / 4);

		for (i = 0; i < chroma_height; i++)
		{
			memcpy(dst, src, chroma_width);
			dst += chroma_pitch;
			src += chroma_width;
		}

		dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch * 5 / 4);
		src = pData + (iHeight * iWidth);

		for (i = 0; i < chroma_height; i++)
		{
			memcpy(dst, src, chroma_width);
			dst += chroma_pitch;
			src += chroma_width;
		}

		hr = m_pYUVSurface->UnlockRect();

		if (m_rectSrc != rectOrigin || m_rectDest != rectOrigin)
		{
			hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)m_pYUVSurface, &m_rectSrc, m_pTempSurface, &m_rectDest, D3DTEXF_NONE);

			V(m_D3DParam.d3ddev->StretchRect(m_pTempSurface, &m_rectSrc,
				m_pTargetSurface, &m_rectDest, D3DTEXF_NONE));

			m_D3DParam.pVideoStream = m_pTargetSurface;
		}
		else
		{
			//hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)m_pYUVSurface, &m_rectSrc, m_D3DParam.pVideoStream, &m_rectDest, D3DTEXF_NONE);

			m_D3DParam.pOffscreenTexture3D->GetSurfaceLevel(0, &offTextureSurface3D);
			hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)m_pYUVSurface, &m_rectSrc, offTextureSurface3D, &m_rectDest, D3DTEXF_NONE);
		}
	}
	else if (iForamt == PF_RGB32)
	{
		/*
		hr = m_D3DParam.pVideoStream->LockRect(&d3drect, NULL, 0);
		unsigned char *pDest = (unsigned char*)d3drect.pBits;
		//memcpy(pDest, pData, m_iWidth * m_iHeight * 3 /2);

		int i = 0;
		unsigned char *dst = (unsigned char*)d3drect.pBits;
		unsigned char *src = pData;

		if (d3drect.Pitch == iWidth)
			memcpy(dst, src, iWidth * iHeight * 4);
		else
		{
			int iBytesbyPerLine = iWidth * 4;
			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				dst += d3drect.Pitch;
			}
		}

		hr = m_D3DParam.pVideoStream->UnlockRect();
		*/

		V(m_pTempSurface->LockRect(&d3drect, NULL, 0));
		unsigned char *pDest = (unsigned char*)d3drect.pBits;

		if (d3drect.Pitch / 4 != m_iWidth)
		{
			unsigned char *src = pData;
			int iBytesbyPerLine = m_iWidth * 4;
			for (int i = 0; i < m_iHeight; i++)
			{
				memcpy(pDest, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				pDest += d3drect.Pitch;
			}
		}
		else
			memcpy(pDest, pData, m_iWidth * m_iHeight * 4);

		V(m_pTempSurface->UnlockRect());

		if (m_rectSrc != rectOrigin || m_rectDest != rectOrigin)
		{
			V(m_D3DParam.d3ddev->StretchRect(m_pTempSurface, &m_rectSrc,m_pTargetSurface, &m_rectDest, D3DTEXF_NONE));

			m_D3DParam.pVideoStream = m_pTargetSurface;
		}
		else
		{
			//hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)m_pTempSurface, &m_rectSrc, m_D3DParam.pVideoStream, &m_rectDest, D3DTEXF_NONE);

			m_D3DParam.pOffscreenTexture3D->GetSurfaceLevel(0, &offTextureSurface3D);
			hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)m_pTempSurface, &m_rectSrc, offTextureSurface3D, &m_rectDest, D3DTEXF_NONE);
		}
	}

	D3DXMATRIX matProj, matView, matWorld;

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_PROJECTION, &matProj));
	}

	switch (m_ieffectID)
	{
	case 0: m_ImgEffect.NormalVideo(m_D3DParam, m_EffectParam);				break;
	case 1: m_ImgEffect3D.Negative(m_D3DParam, m_EffectParam);				break;
	case 2: m_ImgEffect3D.Gray(m_D3DParam, m_EffectParam);					break;
	case 3: m_ImgEffect3D.RGBColor(m_D3DParam, m_EffectParam);				break;
		//case 4:	m_ImgEffect.Brightness( m_D3DParam, m_EffectParam );				break;
	case 4:	m_ImgEffect3D.BrightnessFlicker(m_D3DParam, m_EffectParam);		break;
	case 5:	m_ImgEffect.SleazyEmboss(m_D3DParam, m_EffectParam);			break;
	case 6:	m_ImgEffect3D.RectangleBrightDark(m_D3DParam, m_EffectParam);	break;
	case 7: m_ImgEffect.DelayMovie(m_D3DParam, m_EffectParam);				break;
	case 8: m_ImgEffect3D.MultiMovie(m_D3DParam, m_EffectParam);			break;
	case 9: m_ImgEffect3D.Pixelate(m_D3DParam, m_EffectParam);				break;
	case 10: m_ImgEffect3D.RotateScale(m_D3DParam, m_EffectParam);			break;
		//case 12: m_ImgEffect.ColorConvert( m_D3DParam, m_EffectParam );			break;
	case 11: m_ImgEffect3D.Crosshatching(m_D3DParam, m_EffectParam);			break;
	case 12: m_ImgEffect3D.SwirlTexture(m_D3DParam, m_EffectParam);		break;
	case 13: m_ImgEffect3D.BlindTexture(m_D3DParam, m_EffectParam);			break;
	case 14: m_ImgEffect3D.NeonLightTexture(m_D3DParam, m_EffectParam);		break;
	case 15: m_ImgEffect3D.HSL_RGB(m_D3DParam, m_EffectParam);				break;
	case 16: m_ImgEffect3D.WindowsNormal(m_D3DParam, m_EffectParam);			break;
	case 17: m_ImgEffect3D.Ripple(m_D3DParam, m_EffectParam);				break;
	case 18: m_ImgEffect3D.FishEye(m_D3DParam, m_EffectParam);				break;
	case 19: m_ImgEffect3D.Halftone(m_D3DParam, m_EffectParam);				break;
	case 20: m_ImgEffect3D.WoodNoise(m_D3DParam, m_EffectParam);				break;
	case 21: m_ImgEffect3D.Edge(m_D3DParam, m_EffectParam);					break;
	case 22: m_ImgEffect3D.Wave(m_D3DParam, m_EffectParam);					break;
	case 23: m_ImgEffect3D.WaterColor(m_D3DParam, m_EffectParam);			break;
	case 24: m_ImgEffect3D.Kaleidoscope(m_D3DParam, m_EffectParam);			break;
	case 25: m_ImgEffect3D.CircleCenter(m_D3DParam, m_EffectParam);			break;
	case 26: m_ImgEffect3D.CylinderRotate(m_D3DParam, m_EffectParam);		break;
	case 27: m_ImgEffect3D.PlaneRotate(m_D3DParam, m_EffectParam);			break;
	case 28: m_ImgEffect3D.DotMosaics(m_D3DParam, m_EffectParam);			break;
	case 29: m_ImgEffect3D.SingleColor(m_D3DParam, m_EffectParam);			break;
	case 30: m_ImgEffect3D.Blur(m_D3DParam, m_EffectParam);			        break;
	case 31: m_ImgEffect3D.Mirage(m_D3DParam, m_EffectParam);			    break;
	default:		LOG("Error");		break;
	}

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_PROJECTION, &matProj));
	}

	m_bBusyFlag = FALSE;

	offTextureSurface3D->Release();
}

void CMixerEffect::VideoEffect_Surface(IDirect3DSurface9* pSurface, INT32 iWidth, INT32 iHeight)
{
	HRESULT hr = S_OK;

	IDirect3DSurface9 *offTextureSurface3D = 0;

	CRect rectOrigin = CRect(0, 0, m_iWidth, m_iHeight);

	//hr = m_D3DParam.d3ddev->StretchRect(pSurface, &m_rectSrc, m_pTempSurface, &m_rectDest, D3DTEXF_NONE);

	if (m_rectSrc != rectOrigin || m_rectDest != rectOrigin)
	{
		hr = m_D3DParam.d3ddev->StretchRect(pSurface, &m_rectSrc, m_pTempSurface, &m_rectDest, D3DTEXF_NONE);

		V(m_D3DParam.d3ddev->StretchRect(m_pTempSurface, &m_rectSrc,
			m_pTargetSurface, &m_rectDest, D3DTEXF_NONE));

		m_D3DParam.pVideoStream = m_pTargetSurface;
	}
	else
	{
		//hr = m_D3DParam.d3ddev->StretchRect(pSurface, &m_rectSrc, m_D3DParam.pVideoStream, &m_rectDest, D3DTEXF_NONE);

		m_D3DParam.pOffscreenTexture3D->GetSurfaceLevel(0, &offTextureSurface3D);
		hr = m_D3DParam.d3ddev->StretchRect((IDirect3DSurface9*)pSurface, &m_rectSrc, offTextureSurface3D, &m_rectDest, D3DTEXF_NONE);

		if (FAILED(hr))
		{
			D3DSURFACE_DESC d3d_surface_desc;
			if(pSurface)
				pSurface->GetDesc(&d3d_surface_desc);
		}
	}

	D3DXMATRIX matProj, matView, matWorld;

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->GetTransform(D3DTS_PROJECTION, &matProj));
	}

	switch (m_ieffectID)
	{
	case 0: m_ImgEffect.NormalVideo(m_D3DParam, m_EffectParam);				break;
	case 1: m_ImgEffect3D.Negative(m_D3DParam, m_EffectParam);				break;
	case 2: m_ImgEffect3D.Gray(m_D3DParam, m_EffectParam);					break;
	case 3: m_ImgEffect3D.RGBColor(m_D3DParam, m_EffectParam);				break;
		//case 4:	m_ImgEffect.Brightness( m_D3DParam, m_EffectParam );				break;
	case 4:	m_ImgEffect3D.BrightnessFlicker(m_D3DParam, m_EffectParam);		break;
	case 5:	m_ImgEffect.SleazyEmboss(m_D3DParam, m_EffectParam);			break;
	case 6:	m_ImgEffect3D.RectangleBrightDark(m_D3DParam, m_EffectParam);	break;
	case 7: m_ImgEffect.DelayMovie(m_D3DParam, m_EffectParam);				break;
	case 8: m_ImgEffect3D.MultiMovie(m_D3DParam, m_EffectParam);			break;
	case 9: m_ImgEffect3D.Pixelate(m_D3DParam, m_EffectParam);				break;
	case 10: m_ImgEffect3D.RotateScale(m_D3DParam, m_EffectParam);			break;
		//case 12: m_ImgEffect.ColorConvert( m_D3DParam, m_EffectParam );			break;
	case 11: m_ImgEffect3D.Crosshatching(m_D3DParam, m_EffectParam);			break;
	case 12: m_ImgEffect3D.SwirlTexture(m_D3DParam, m_EffectParam);		break;
	case 13: m_ImgEffect3D.BlindTexture(m_D3DParam, m_EffectParam);			break;
	case 14: m_ImgEffect3D.NeonLightTexture(m_D3DParam, m_EffectParam);		break;
	case 15: m_ImgEffect3D.HSL_RGB(m_D3DParam, m_EffectParam);				break;
	case 16: m_ImgEffect3D.WindowsNormal(m_D3DParam, m_EffectParam);			break;
	case 17: m_ImgEffect3D.Ripple(m_D3DParam, m_EffectParam);				break;
	case 18: m_ImgEffect3D.FishEye(m_D3DParam, m_EffectParam);				break;
	case 19: m_ImgEffect3D.Halftone(m_D3DParam, m_EffectParam);				break;
	case 20: m_ImgEffect3D.WoodNoise(m_D3DParam, m_EffectParam);				break;
	case 21: m_ImgEffect3D.Edge(m_D3DParam, m_EffectParam);					break;
	case 22: m_ImgEffect3D.Wave(m_D3DParam, m_EffectParam);					break;
	case 23: m_ImgEffect3D.WaterColor(m_D3DParam, m_EffectParam);			break;
	case 24: m_ImgEffect3D.Kaleidoscope(m_D3DParam, m_EffectParam);			break;
	case 25: m_ImgEffect3D.CircleCenter(m_D3DParam, m_EffectParam);			break;
	case 26: m_ImgEffect3D.CylinderRotate(m_D3DParam, m_EffectParam);		break;
	case 27: m_ImgEffect3D.PlaneRotate(m_D3DParam, m_EffectParam);			break;
	case 28: m_ImgEffect3D.DotMosaics(m_D3DParam, m_EffectParam);			break;
	case 29: m_ImgEffect3D.SingleColor(m_D3DParam, m_EffectParam);			break;
	case 30: m_ImgEffect3D.Blur(m_D3DParam, m_EffectParam);			        break;
	case 31: m_ImgEffect3D.Mirage(m_D3DParam, m_EffectParam);			    break;
	default:		LOG("Error");		break;
	}

	if (Is3DEffect(m_ieffectID))
	{
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_WORLD, &matWorld));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_VIEW, &matView));
		V(m_D3DParam.d3ddev->SetTransform(D3DTS_PROJECTION, &matProj));
	}

	m_bBusyFlag = FALSE;
	if (offTextureSurface3D)
		offTextureSurface3D->Release();
}

IDirect3DSurface9* CMixerEffect::DoVideoEffect(BYTE* pData, INT32 iWidth, INT32 iHeight)
{
	HRESULT hr = S_OK;
	VideoEffect2(pData,iWidth,iHeight);

	if (m_pOutSurface)
		m_pOutSurface->Release();

	V(m_D3DParam.pOutFrameTexture->GetSurfaceLevel(0, &m_pOutSurface));
	return m_pOutSurface;
}

IDirect3DSurface9* CMixerEffect::DoVideoEffect2(BYTE* pData, INT32 iWidth, INT32 iHeight,int iForamt)
{
	HRESULT hr = S_OK;
	VideoEffect3(pData, iWidth, iHeight, iForamt);

	if (m_ImgEffect3D.GetStatus() < 0)
	{
		char szMsg[512];
		sprintf(szMsg, "Effect3D - CMixerEffect::DoVideoEffect2  (%d)!!\n", m_ImgEffect3D.GetStatus());
		OutputDebugStringA(szMsg);

		return 0;
	}

	if (m_pOutSurface)
		m_pOutSurface->Release();

	V(m_D3DParam.pOutFrameTexture->GetSurfaceLevel(0, &m_pOutSurface));
	return m_pOutSurface;
}

IDirect3DSurface9* CMixerEffect::DoVideoEffect_Surface(IDirect3DSurface9* pSurface, INT32 iWidth, INT32 iHeight)
{
	HRESULT hr = S_OK;
	VideoEffect_Surface(pSurface, iWidth, iHeight);

	if (m_pOutSurface)
		m_pOutSurface->Release();

	V(m_D3DParam.pOutFrameTexture->GetSurfaceLevel(0, &m_pOutSurface));
	return m_pOutSurface;
}

BOOL CMixerEffect::Is3DEffect(INT32 iEffectIdx)
{
	BOOL bResult = TRUE;
	SHORT sNot3DEffectAry[200] = { 0, 1, 2, 3, 4, 5, 6, 7, 9 };

	if (iEffectIdx == 0)
		return FALSE;

	for (int x = 0; x < 200; x++)
	{
		if (!sNot3DEffectAry[x] == iEffectIdx)
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

HRESULT CMixerEffect::LockSurface()
{
	D3DLOCKED_RECT d3drect;
	HRESULT hr = S_OK;

	IDirect3DSurface9 *pVideoSurface = NULL;
	
	V(m_D3DParam.pOutFrameTexture->GetSurfaceLevel(0, &pVideoSurface));
	V(m_D3DParam.d3ddev->GetRenderTargetData(pVideoSurface, m_pEffectSurface));

	V(m_pEffectSurface->LockRect(&d3drect, NULL, 0));
	unsigned char *pSrc = (unsigned char*)d3drect.pBits;
	if (pSrc == NULL)
		return E_FAIL;

	memcpy(m_pImgBuf, pSrc, m_iWidth * m_iHeight * 4);
	V(m_pEffectSurface->UnlockRect());
	pVideoSurface->Release();

	return hr;
};

void CMixerEffect::Create(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int iWidth, int iHeight)
{
	m_pD3DDev = pD3DDev;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_VideoSurfaceFmt = foramt;

	CreateSurfaces();
}

int CMixerEffect::GetWidth()
{
	return m_iWidth;
}

int CMixerEffect::GetHeight()
{
	return m_iHeight;
}

int CMixerEffect::CreateSurfaces()
{
	CreateSurfaces(m_pD3DDev, m_iWidth, m_iHeight);
	return 0;
}

void CMixerEffect::ResetSurfaces(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
	DestroySurfaces();
	CreateSurfaces();
}

void CMixerEffect::ResetD3DDevice(IDirect3DDevice9* pD3DDev)
{
	m_pD3DDev = pD3DDev;

	m_bInit3DEffect = true;

	m_ImgEffect3D.Destroy();

	DestroySurfaces();
	CreateSurfaces();
}
