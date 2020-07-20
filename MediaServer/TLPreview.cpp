#include "stdafx.h"
#include "TLPreview.h"
#include "MediaServer.h"

TLPreview::TLPreview(HWND hWnd)
{
#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager = 0;
	m_iSurfaceItemIndex = -1;
#else
	m_pD3DRender = 0;
#endif

	//m_pMedisStream = new CMediaStreamEx(hWnd);
	m_pMedisStream = new CMediaStream(hWnd);
	
	INT32 iIndex = g_MediaStreamAry.size();
	g_MediaStreamAry.resize(iIndex + 1);
	g_MediaStreamAry.at(iIndex) = m_pMedisStream;

	m_iStreamAryIndex = iIndex;
	
	m_bEnableUpdatePos = true;

	m_fScaleW = 1.0;
	m_fScaleH = 1.0;

	m_effect_param.iLevel = g_EffectParam.iLevel;
	m_effect_param.iParm1 = 50;
	m_effect_param.iParm2 = 50;
	m_effect_param.iParm3 = 50;
	m_effect_param.iParm4 = 50;
	m_effect_param.iParm5 = 50;

	m_iFrameCount = 0;
}

TLPreview::~TLPreview()
{
#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager = 0;
#else
	if (m_pD3DRender)
		delete m_pD3DRender;	
#endif

	if (m_pMedisStream)
	{
		delete m_pMedisStream;
		m_pMedisStream = 0;
	}
}

void TLPreview::SetDisplayManager(DisplayManager* pObj)
{
	m_pDisplayManager = pObj;
}

void TLPreview::Init(HWND hWnd, int iIndex)
{
	m_iIndex = iIndex;

	//m_pMedisStream->Init(hWnd, iIndex);
	//m_pMedisStream->InitRender(hWnd);

	LoadSourceScaleFile();

#ifdef _ENABLE_DISPLAY_MANAGER
	m_iSurfaceItemIndex = m_pDisplayManager->CreateSurfaceItem();

	m_pMedisStream->SetVideoRawDataCallback(this);
	m_pMedisStream->CreateMixEffect(m_pDisplayManager->GetD3DDevice(), m_pDisplayManager->GetDesktopD3DFormat(), 1920, 1080);
	m_pMedisStream->SetSurfaceItemIndex(m_iSurfaceItemIndex);

	/*
	bool bEnableGPU = GetPrivateProfileInt("Media", "EnableGPU", 0, theApp.m_strCurPath + "Setting.ini");

	if(bEnableGPU)
		m_pMedisStream->EnableGPU(true, m_pDisplayManager->GetD3DDevice());
	else
		m_pMedisStream->EnableGPU(false, m_pDisplayManager->GetD3DDevice());
		*/

	/*
	if (m_iIndex == 0)
		m_pMedisStream->EnableAudio(true);
	else
		m_pMedisStream->EnableAudio(false);
	*/	
#ifdef _ENABLE_GPU
	m_pMedisStream->EnableGPU(true, m_pDisplayManager->GetD3DDevice());
#else
	m_pMedisStream->EnableGPU(false, m_pDisplayManager->GetD3DDevice());
#endif

#else
	m_pD3DRender = new D3DRender;

	int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//m_pD3DRender->Init(hWnd, VPF_I420, iScreenWidth, iScreenHeight);
	m_pD3DRender->Init(hWnd, VPF_RGB32, iScreenWidth, iScreenHeight);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pMedisStream->SetVideoRawDataCallback(this);
	m_pMedisStream->CreateMixEffect(m_pD3DRender->GetD3DDevice(), m_pD3DRender->GetSurfaceFormat(), 1920, 1080);
	//m_pMedisStream->EnableGPU(false, m_pD3DRender->GetD3DDevice());

#endif

	m_pMedisStream->SetScale(m_fScaleW, m_fScaleH);
}

void TLPreview::Open(char* szFileName)
{
	m_pDisplayManager->ResetHDEffect();
	m_pMedisStream->Open(szFileName);
}

void TLPreview::Close()
{
	m_pMedisStream->Close();
}

void TLPreview::Play()
{
	m_pMedisStream->Play();
}

void TLPreview::Stop()
{
	m_pMedisStream->Stop();
}

/*
void TLPreview::Stop2()
{
	m_pMedisStream->Stop2();
}
*/
void TLPreview::Seek(float fPos)
{
	m_pMedisStream->Seek(fPos);
}

void TLPreview::EnableAudio(bool bEnable)
{
	m_pMedisStream->EnableAudio(bEnable);
}

void TLPreview::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

void TLPreview::SpeedUp()
{
	m_pMedisStream->SpeedUp();
}

void TLPreview::SpeedDown()
{
	m_pMedisStream->SpeedDown();
}

void TLPreview::SetVolume(int iValue)
{
	m_pMedisStream->SetVolume(iValue);
}
/*
int TLPreview::GetSystemVoluem()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetSystemVolume(m_iIndex);
	return 0;
}

void TLPreview::SetSystemVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSystemVolume(m_iIndex, iValue);
}
*/
void TLPreview::SetBrightness(int iValue)
{
	m_pMedisStream->SetBrightness(iValue);
}

void TLPreview::SetContrast(int iValue)
{
	m_pMedisStream->SetContrast(iValue);
}

void TLPreview::SetSaturation(int iValue)
{
	m_pMedisStream->SetSaturation(iValue);
}

void TLPreview::SetHue(int iValue)
{
	m_pMedisStream->SetHue(iValue);
}

void TLPreview::SetR(int iValue)
{
	m_pMedisStream->SetR(iValue);
}

void TLPreview::SetG(int iValue)
{
	m_pMedisStream->SetG(iValue);
}

void TLPreview::SetB(int iValue)
{
	m_pMedisStream->SetB(iValue);
}

void TLPreview::SetGray(int iValue)
{
	if (m_pMedisStream)
		m_pMedisStream->SetGray(iValue);
}

void TLPreview::SetNegative(int iValue)
{
	if (m_pMedisStream)
		m_pMedisStream->SetNegative(iValue);
}

void TLPreview::PlayBackward()
{
	m_pMedisStream->PlayBackward();
}

int TLPreview::GetPlaybackMode()
{
	return m_pMedisStream->GetPlaybackMode();
}

void TLPreview::Pause()
{
	m_pMedisStream->Pause();
}

int TLPreview::GetVoluem()
{
	return 0;//m_pMedisStream->GetVoluem();
}

void TLPreview::SetEffectIndex(int iID)
{
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if(pMixerEffect)
		pMixerEffect->SetEffectID(iID);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectID(iID);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectID(iID);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectID(iID);
		}
	}
}

int TLPreview::GetEffectIndex()
{
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if(pMixerEffect)
	return pMixerEffect->GetEffectID();
	return 0;
}

//void TLPreview::SetEffectParam(int iID, int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
void TLPreview::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
{
	sEffectParam effect_param;
	
	if (iLevel != 65535)
		effect_param.iLevel = iLevel;
	else
		effect_param.iLevel = g_EffectParam.iLevel;

	if (iParm1 != 65535)
		effect_param.iParm1 = iParm1;
	else
		effect_param.iParm1 = 50;

	if (iParm2 != 65535)
		effect_param.iParm2 = iParm2;
	else
		effect_param.iParm2 = 50;

	if (iParm3 != 65535)
		effect_param.iParm3 = iParm3;
	else
		effect_param.iParm3 = 50;

	if (iParm4 != 65535)
		effect_param.iParm4 = iParm4;
	else
		effect_param.iParm4 = 50;

	if (iParm5 != 65535)
		effect_param.iParm5 = iParm5;
	else
		effect_param.iParm5 = 50;

	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if(pMixerEffect)
		pMixerEffect->SetEffectParam(effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(effect_param);
		}
	}
}

void TLPreview::SetEffectLevel(int iLevel)
{
	m_effect_param.iLevel = iLevel;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iEffectIndex = GetEffectIndex();
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
			{
				if (iLevel != 0)
				{
					pCurMixerEffect->GetEffectID();
					if (pCurMixerEffect->GetEffectID() != iEffectIndex)
						pCurMixerEffect->SetEffectID(iEffectIndex);
				}
				pCurMixerEffect->SetEffectParam(m_effect_param);
			}
		}
	}
}

void TLPreview::SetEffectParam1(int iParm)
{
	m_effect_param.iParm1 = iParm;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
}

void TLPreview::SetEffectParam2(int iParm)
{
	m_effect_param.iParm2 = iParm;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
}

void TLPreview::SetEffectParam3(int iParm)
{
	m_effect_param.iParm3 = iParm;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
}

void TLPreview::SetEffectParam4(int iParm)
{
	m_effect_param.iParm4 = iParm;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
}

void TLPreview::SetEffectParam5(int iParm)
{
	m_effect_param.iParm5 = iParm;
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	if (pMixerEffect)
		pMixerEffect->SetEffectParam(m_effect_param);

	//shape effect
	int iTotal = m_pMedisStream->GetTotalOfShapeDisplayManagers();
	if (iTotal == 1)
	{
		CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(0);
		if (pCurMixerEffect)
			pCurMixerEffect->SetEffectParam(m_effect_param);
		else
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
	else if (iTotal > 1)
	{
		for (int i = 0; i < iTotal; i++)
		{
			CMixerEffect* pCurMixerEffect = m_pMedisStream->GetShapeMixerEffect(i + 1);
			if (pCurMixerEffect)
				pCurMixerEffect->SetEffectParam(m_effect_param);
		}
	}
}

void TLPreview::SetFadeDuration(float fValue)
{
	m_pMedisStream->SetFadeDuration(fValue);
}

void TLPreview::EnableFadeIn(bool bEnable)
{
	m_pMedisStream->EnableFadeIn(bEnable);
}

void TLPreview::EnableFadeOut(bool bEnable)
{
	m_pMedisStream->EnableFadeOut(bEnable);
}

bool TLPreview::IsOpened()
{
	return m_pMedisStream->IsOpened();
}

void TLPreview::ResetFilter()
{
	m_pMedisStream->ResetFilter();
	m_pDisplayManager->ResetHDEffect();
}

void TLPreview::SetStartTime(double dTime)
{
	m_pMedisStream->SetStartTime(dTime);
}

void TLPreview::UpdateGlobalTimeCode(double dTimeCode)
{
	m_pMedisStream->UpdateGlobalTimeCode(dTimeCode);
}

double TLPreview::GetCurPos()
{ 
	return m_pMedisStream->GetCurPos();
}

void TLPreview::DisplayVideoRawData(unsigned char* pBuffer, int w, int h)
{
#ifdef _ENABLE_DISPLAY_MANAGER
	//m_pDisplayManager->RenderFrame(m_iSurfaceItemIndex, VPF_RGB32, pBuffer, w, h);

	if(m_iFrameCount % 2 == 0)
		m_pDisplayManager->RenderFrame(m_iSurfaceItemIndex, VPF_I420, pBuffer, w, h);

	m_iFrameCount++;
#else
	if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
		{
			int iPixelFormat = VPF_RGB32;
			int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
		}
		m_pD3DRender->RenderFrame(pBuffer, w, h);
		m_pD3DRender->Flip();
	}
#endif
}

void TLPreview::DisplayVideoSurfaceData(void* pSurface, int w, int h)
{
#ifdef _ENABLE_DISPLAY_MANAGER
	if (m_iFrameCount % 2 == 0)
		m_pDisplayManager->RenderSurface(m_iSurfaceItemIndex, VPF_RGB32, (unsigned char*)pSurface, w, h);

	m_iFrameCount++;
#else
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
#endif
}

void TLPreview::DisplayNV12SurfaceData(void* pSurface, int w, int h)
{
#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager->RenderSurface(m_iSurfaceItemIndex, VPF_NV12, (unsigned char*)pSurface, w, h);
	m_pDisplayManager->SetCurPos(m_iSurfaceItemIndex, m_pMedisStream->GetCurPos());

	m_iFrameCount++;
#else
	if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		m_pD3DRender->RenderFrame1((IDirect3DSurface9 *)pSurface, w, h);
		m_pD3DRender->Flip();
	}
#endif
}

void TLPreview::LoadSourceScaleFile()
{
	char szAppName[128];
	char szScaleW[10];
	char szScaleH[10];
	sprintf(szAppName, "SrcScale%d", m_iIndex);

	GetPrivateProfileString(szAppName, "W", "1.0", szScaleW, 10, theApp.m_strCurPath + "SourceScale.ini");
	GetPrivateProfileString(szAppName, "H", "1.0", szScaleH, 10, theApp.m_strCurPath + "SourceScale.ini");

	m_fScaleW = atof(szScaleW);
	m_fScaleH = atof(szScaleH);
}

void TLPreview::ResetMixerEffectD3DDevice()
{
	CMixerEffect* pMixerEffect = m_pMedisStream->GetMixerEffect();
	pMixerEffect->ResetD3DDevice(m_pDisplayManager->GetD3DDevice());
}
