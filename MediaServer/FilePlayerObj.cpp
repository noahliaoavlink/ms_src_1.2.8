#include "stdafx.h"
#include "FilePlayerObj.h"
#include "../../../Include/StrConv.h"
#include "MediaServer.h"
#include "MixerEffect.h"

FilePlayerObj::FilePlayerObj()
{
	m_pD3DRender = 0;
	m_pMFilePlayerDll = 0;

	//m_iDisplayMode = DM_NORMAL;

	m_pMixerEffect = 0;
	m_pRGBBuffer = 0;

	m_effect_param.iLevel = 0;
	m_effect_param.iParm1 = 50;
	m_effect_param.iParm2 = 50;
	m_effect_param.iParm3 = 50;
	m_effect_param.iParm4 = 50;
	m_effect_param.iParm5 = 50;

	m_bEnableUpdatePos = true;

	m_pEventCallback = 0;
	m_iFrameCount = 0;

#ifdef _ENABLE_GPU
	m_pProcessorHD = 0;
#endif
}

FilePlayerObj::~FilePlayerObj()
{
	Stop();
	Close();

#ifdef _ENABLE_GPU
	if (m_pProcessorHD)
		delete m_pProcessorHD;
#endif

	if (m_pD3DRender)
		delete m_pD3DRender;

	if (m_pMFilePlayerDll)
		delete m_pMFilePlayerDll;

	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();

	if (m_pRGBBuffer)
		delete m_pRGBBuffer;
}

void FilePlayerObj::Init(HWND hWnd)
{
	m_pD3DRender = new D3DRender;
	m_pD3DRender->Init(hWnd, VPF_I420,1920,1080);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	CreateMixEffect(m_pD3DRender->GetD3DDevice(), m_pD3DRender->GetDesktopD3DFormat(), 1920, 1080);

	m_pMFilePlayerDll = new MFilePlayerDll;
	m_pMFilePlayerDll->LoadLib(theApp.m_strCurPath + "MFilePlayer.dll");
	m_pMFilePlayerDll->Create(0);
	m_pMFilePlayerDll->Init(0,hWnd, this);
	m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_I420);
	m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_LOCAL);

#ifdef _ENABLE_GPU
	m_pProcessorHD = new ProcessorHD;
	m_pProcessorHD->Init(hWnd, (IDirect3DDevice9Ex*)m_pD3DRender->GetD3DDevice(), 1920, 1080);

	m_pMFilePlayerDll->EnableGPU(0, true);
	m_pMFilePlayerDll->SetupDXVA2(0, m_pD3DRender->GetD3DDevice());
#endif
}

void FilePlayerObj::SetEventCallback(void* pObj)
{
	m_pEventCallback = (EventCallback*)pObj;
}

void FilePlayerObj::ResetD3D()
{
	m_pD3DRender->Reset();
}

void FilePlayerObj::Open(wchar_t* wszFileName)
{
	std::string sFileName = UnicodeToUTF8(wszFileName);
	m_pMFilePlayerDll->Open(0,(char*)sFileName.c_str());

	float fFPS = m_pMFilePlayerDll->GetVideoFPS(0);
	int iTotalOfFrames = m_pMFilePlayerDll->GetTotalOfFrames(0);
	int w = m_pMFilePlayerDll->GetVideoWidth(0);
	int h = m_pMFilePlayerDll->GetVideoHeight(0);

	if (m_pRGBBuffer)
		delete m_pRGBBuffer;

	m_pRGBBuffer = new BYTE[w * h * 4];

	m_pMFilePlayerDll->SetRepeat(0,true);
}

void FilePlayerObj::Close()
{
	if(m_pMFilePlayerDll)
		m_pMFilePlayerDll->Close(0);
}

void FilePlayerObj::Play()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Play(0);
}

void FilePlayerObj::Stop()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Stop(0);
}

void FilePlayerObj::Seek(double dPos)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Seek(0, dPos);
}

void FilePlayerObj::SpeedUp()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedUp(0);
}

void FilePlayerObj::SpeedDown()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedDown(0);
}

int FilePlayerObj::GetVoluem()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetVolume(0);
	return 0;
}

void FilePlayerObj::SetVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetVolume(0,iValue);
}

int FilePlayerObj::GetSystemVoluem()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetSystemVolume(0);
	return 0;
}

void FilePlayerObj::SetSystemVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSystemVolume(0,iValue);
}

void FilePlayerObj::SetBrightness(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		//-16 ~ 16
		int iCurValue = 0;
		iCurValue = (iValue - 50) * 2;

		float fValue = iCurValue;// 6.25;
		//float fValue = iCurValue / 6.25;
		m_pProcessorHD->SetBrightness(fValue);
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetBrightness(0, iValue);
	}
}

void FilePlayerObj::SetContrast(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		//0~16
		int iCurValue = (iValue - 50) * 2;
		float fValue = (iCurValue + 100.0);//(6.25 * 2);
		//float fValue = (iCurValue + 100.0) / (6.25 * 2);
		m_pProcessorHD->SetContrast(fValue);
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetContrast(0, iValue);
	}
}

void FilePlayerObj::SetSaturation(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		//0~16
		int iCurValue = (iValue - 50) * 2;
		float fValue = (iCurValue + 100.0);//(6.25 * 2);
		//float fValue = (iCurValue + 100.0) / (6.25 * 2);
		m_pProcessorHD->SetSaturation(fValue);
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetSaturation(0, iValue);
	}
}

void FilePlayerObj::SetHue(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		//-16 ~ 16
		int iCurValue = (iValue - 50) * 2;
		//float fValue = iCurValue / 3.125;// 6.25;
		float fValue = iCurValue;
		m_pProcessorHD->SetHue(fValue);
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetHue(0, iValue);
	}
}

void FilePlayerObj::SetR(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (iValue - 50) * 2;

		if (iCurValue == 0)
		{
			m_pProcessorHD->SetRedAlpha(0);
			m_pProcessorHD->SetBabyBlueAlpha(0);
		}
		else if (iCurValue > 0)
		{
			float fValue = iCurValue * 1.5;
			m_pProcessorHD->SetRedAlpha(fValue);
			m_pProcessorHD->SetBabyBlueAlpha(0);
		}
		else
		{
			float fValue = abs(iCurValue) * 1.5;
			m_pProcessorHD->SetBabyBlueAlpha(fValue);
			m_pProcessorHD->SetRedAlpha(0);
		}
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetR(0, iValue);
	}
}

void FilePlayerObj::SetG(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (iValue - 50) * 2;

		if (iCurValue == 0)
		{
			m_pProcessorHD->SetGreenAlpha(0);
			m_pProcessorHD->SetPurpleAlpha(0);
		}
		else if (iCurValue > 0)
		{
			float fValue = iCurValue * 1.5;
			m_pProcessorHD->SetGreenAlpha(fValue);
			m_pProcessorHD->SetPurpleAlpha(0);
		}
		else
		{
			float fValue = abs(iCurValue) * 1.5;
			m_pProcessorHD->SetPurpleAlpha(fValue);
			m_pProcessorHD->SetGreenAlpha(0);
		}
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetG(0, iValue);
	}
}

void FilePlayerObj::SetB(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (iValue - 50) * 2;

		if (iCurValue == 0)
		{
			m_pProcessorHD->SetBlueAlpha(0);
			m_pProcessorHD->SetYellowAlpha(0);
		}
		else if (iCurValue > 0)
		{
			float fValue = abs(iCurValue) * 1.5;
			m_pProcessorHD->SetBlueAlpha(fValue);
			m_pProcessorHD->SetYellowAlpha(0);
		}
		else
		{
			float fValue = abs(iCurValue) * 1.5;
			m_pProcessorHD->SetYellowAlpha(fValue);
			m_pProcessorHD->SetBlueAlpha(0);
		}
	}
	else
#endif
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetB(0, iValue);
	}
}

void FilePlayerObj::SetBGColor_R(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (float)iValue * 2.55;
		m_pProcessorHD->SetBGColor_R(iCurValue);
	}
#endif
}

void FilePlayerObj::SetBGColor_G(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (float)iValue * 2.55;
		m_pProcessorHD->SetBGColor_G(iCurValue);
	}
#endif
}

void FilePlayerObj::SetBGColor_B(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (float)iValue * 2.55;
		m_pProcessorHD->SetBGColor_B(iCurValue);
	}
#endif
}

void FilePlayerObj::SetPlanarAlpha(int iValue)
{
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		int iCurValue = (float)iValue * 2.55;
		m_pProcessorHD->SetPlanarAlpha(iCurValue);
	}
#endif
}

void FilePlayerObj::PlayBackward()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->PlayBackward(0);
}

int FilePlayerObj::GetPlaybackMode()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetPlaybackMode(0);
	return 0;
}

void FilePlayerObj::Pause()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Pause(0);
}

void FilePlayerObj::UpdateGlobalTimeCode(double dTimeCode)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->UpdateGlobalTimeCode(0,dTimeCode);
}

int FilePlayerObj::CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h)
{
	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();
	else
		m_pMixerEffect = new CMixerEffect();

	m_pMixerEffect->Create(pD3DDev, foramt, w, h);
	return 0;
}

void FilePlayerObj::SetEffectIndex(int iID)
{

#ifdef _ENABLE_HD_EFFECT
	m_pProcessorHD->ResetFliter();
#endif

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectID(iID);

	m_AvgEffect.Reset();
}

void FilePlayerObj::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
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

	m_effect_param.iLevel = iLevel;
	m_effect_param.iParm1 = iParm1;
	m_effect_param.iParm2 = iParm2;
	m_effect_param.iParm3 = iParm3;
	m_effect_param.iParm4 = iParm4;
	m_effect_param.iParm5 = iParm5;

#ifdef _ENABLE_HD_EFFECT
	SetEffectLevel(iLevel);

	if (m_pMixerEffect)
	{
		bool bIsHDEffect = false;

		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
			{
				bIsHDEffect = true;
				break;
			}
		}

		if (bIsHDEffect)
		{
			switch (m_pMixerEffect->GetEffectID())
			{
				case HD_EID_RGBA:
					SetBGColor_R(iParm1);
					SetBGColor_G(iParm2);
					SetBGColor_B(iParm3);
					break;
			}
		}
	}
#endif

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(effect_param);
}

void FilePlayerObj::SetEffectLevel(int iLevel)
{
	m_effect_param.iLevel = iLevel;

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);

#ifdef _ENABLE_HD_EFFECT
	if (m_pMixerEffect)
	{
		bool bIsHDEffect = false;

		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
			{
				bIsHDEffect = true;
				break;
			}
		}

		if (bIsHDEffect)
		{
			switch (m_pMixerEffect->GetEffectID())
			{
				case HD_EID_BRIGHTNESS:
					SetBrightness(iLevel);
					break;
				case HD_EID_CONTRAST:
					SetContrast(iLevel);
					break;
				case HD_EID_HUE:
					SetHue(iLevel);
					break;
				case HD_EID_SATURATION:
					SetSaturation(iLevel);
					break;
				case HD_EID_R:
					SetR(iLevel);
					break;
				case HD_EID_G:
					SetG(iLevel);
					break;
				case HD_EID_B:
					SetB(iLevel);
					break;
				case HD_EID_RGBA:
					SetPlanarAlpha(iLevel);
					break;
			}
		}
	}
#endif
}

void FilePlayerObj::SetEffectParam1(int iParm)
{
	m_effect_param.iParm1 = iParm;

#ifdef _ENABLE_HD_EFFECT
	
	if (m_pMixerEffect)
	{
		bool bIsHDEffect = false;

		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
			{
				bIsHDEffect = true;
				break;
			}
		}

		if (bIsHDEffect)
		{
			switch (m_pMixerEffect->GetEffectID())
			{
			case HD_EID_RGBA:
				SetBGColor_R(iParm);
				break;
			}
		}
	}
#endif

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);
}

void FilePlayerObj::SetEffectParam2(int iParm)
{
	m_effect_param.iParm2 = iParm;

#ifdef _ENABLE_HD_EFFECT

	if (m_pMixerEffect)
	{
		bool bIsHDEffect = false;

		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
			{
				bIsHDEffect = true;
				break;
			}
		}

		if (bIsHDEffect)
		{
			switch (m_pMixerEffect->GetEffectID())
			{
			case HD_EID_RGBA:
				SetBGColor_G(iParm);
				break;
			}
		}
	}
#endif

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);
}

void FilePlayerObj::SetEffectParam3(int iParm)
{
	m_effect_param.iParm3 = iParm;

#ifdef _ENABLE_HD_EFFECT

	if (m_pMixerEffect)
	{
		bool bIsHDEffect = false;

		int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
		for (int i = 0; i < iTotalOfEffectItems; i++)
		{
			if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
			{
				bIsHDEffect = true;
				break;
			}
		}

		if (bIsHDEffect)
		{
			switch (m_pMixerEffect->GetEffectID())
			{
			case HD_EID_RGBA:
				SetBGColor_B(iParm);
				break;
			}
		}
	}
#endif

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);
}

void FilePlayerObj::SetEffectParam4(int iParm)
{
	m_effect_param.iParm4 = iParm;

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);
}

void FilePlayerObj::SetEffectParam5(int iParm)
{
	m_effect_param.iParm5 = iParm;

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(m_effect_param);
}

void FilePlayerObj::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

//format - 00:00 / 02:00
void FilePlayerObj::UpdatePosInfo(char* szCurPos, char* szTotalLen)
{
//	gUpdatePosInfo(szCurPos, szTotalLen);
	if(m_pEventCallback)
	m_pEventCallback->Event(OBJ_MSG_UPDATE_POS_INFO,szCurPos,0);
}

//total len
void FilePlayerObj::SetSliderCtrlRange(double dPos)
{
//	gSetSliderCtrlRange(dPos);
	long lPos = dPos * 1000.0;
	if(m_pEventCallback)
		m_pEventCallback->Event(OBJ_MSG_SET_SLIDER_RANGE, (void*)lPos,0);
	//OBJ_MSG_UPDATE_POS_INFO,
		//OBJ_MSG_SET_SLIDER_POS,
		//OBJ_MSG_SET_SLIDER_RANGE,
}

//cur pos
void FilePlayerObj::SetSliderCtrlPos(double dPos)
{
	long lPos = dPos * 1000.0;
	if(m_bEnableUpdatePos && m_pEventCallback)
		m_pEventCallback->Event(OBJ_MSG_SET_SLIDER_POS, (void*)lPos, 0);
//	if (m_bEnableUpdatePos)
	//	gSetSliderCtrlPos(dPos);
}

void FilePlayerObj::DisplayVideo(unsigned char* pBuffer, int w, int h)
{
	char szMsg[256];

	if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
	{
		int iPixelFormat = VPF_I420;
		int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
	}

	bool bDoEffect = false;
	if (m_pMixerEffect)
	{
		sEffectParam ep = m_pMixerEffect->GetEffectParam();
		if (m_pMixerEffect->GetEffectID() != 0 && ep.iLevel != 0)
			bDoEffect = true;
	}

	if (bDoEffect)
	{
		if (m_pMixerEffect->GetWidth() != w || m_pMixerEffect->GetHeight() != h)
			m_pMixerEffect->ResetSurfaces(w,h);
		
		if (m_pMixerEffect->GetEffectID() >= HD_EID_BRIGHTNESS)
		{
			if (m_iFrameCount % 2 == 0)
				m_pD3DRender->RenderFrame(pBuffer, w, h);
		}
		else
		{
			DWORD dwTime1 = timeGetTime();
			IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect2(pBuffer, w, h);

			DWORD dwTime2 = timeGetTime();

			m_AvgEffect.Calculate(dwTime2 - dwTime1);

			if (m_iFrameCount % 10 == 9)
			{
				sprintf(szMsg, "#FPO# FilePlayerObj::DisplayVideo: [%d , %3.2F] \n", dwTime2 - dwTime1, m_AvgEffect.GetAverage());
				OutputDebugStringA(szMsg);
			}

			if (pOutSurface)
			{
				if (m_iFrameCount % 2 == 0)
					m_pD3DRender->RenderFrame1(pOutSurface, w, h);
			}
		}
	}
	else
	{
		if (m_iFrameCount % 2 == 0)
			m_pD3DRender->RenderFrame(pBuffer, w, h);
	}

	if (m_iFrameCount % 2 == 0)
	{
		int iRet = m_pD3DRender->Flip();
		if (iRet == 2)
		{
			m_pMixerEffect->ResetD3DDevice(m_pD3DRender->GetD3DDevice());
		}
	}

	m_iFrameCount++;
}

void FilePlayerObj::DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{
	bool bDoEffect = false;
	if (m_pMixerEffect)
	{
		sEffectParam ep = m_pMixerEffect->GetEffectParam();
		if (m_pMixerEffect->GetEffectID() != 0 && ep.iLevel != 0)
			bDoEffect = true;
	}

	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		if (bDoEffect)
		{
#ifdef _ENABLE_HD_EFFECT
			if (m_pProcessorHD->GetSurfaceWidth() != w || m_pProcessorHD->GetSurfaceHeight() != h)
			{
				int iFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
				m_pProcessorHD->RestSurface((D3DFORMAT)iFormat, w, h);
			}

			m_pProcessorHD->UpdateSurface((IDirect3DSurface9*)pBuffer);
			m_pProcessorHD->ProcessVideoFrame();

			m_pD3DRender->DrawSurface((IDirect3DSurface9*)m_pProcessorHD->GetBGSurface(), w, h);

#else
			if (m_pMixerEffect->GetWidth() != w || m_pMixerEffect->GetHeight() != h)
			{
				m_pMixerEffect->ResetSurfaces(w, h);
			}
			IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect_Surface((IDirect3DSurface9*)pBuffer, w, h);
			if (pOutSurface)
			{
				if (m_iFrameCount % 2 == 0)
					m_pD3DRender->RenderFrame1(pOutSurface, w, h);
			}
#endif
		}
		else
		{
			if (m_iFrameCount % 2 == 0)
				m_pD3DRender->RenderFrame1((IDirect3DSurface9*)pBuffer, w, h);
		}
	}

	if (m_iFrameCount % 2 == 0)
	{
		/*
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll->GPUIsEnabled(0))
		{
			if (bDoEffect)
			{
				//m_pProcessorHD->Flip();
				m_pD3DRender->Flip();
			}
			else
				m_pD3DRender->Flip();
		}
		else
#endif
*/
		{
			int iRet = m_pD3DRender->Flip();
			if (iRet == 2)
			{
				m_pMixerEffect->ResetD3DDevice(m_pD3DRender->GetD3DDevice());
			}
		}
	}
	m_iFrameCount++;
}

void FilePlayerObj::PlayAudio(unsigned char* pBuffer, int iLen)
{

}

void FilePlayerObj::Event(int iType)
{
}
