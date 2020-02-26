#include "stdafx.h"
#include "MediaStream.h"
#include "Panel.h"
#include "MediaServer.h"
#include "MixerEffect.h"
#include "ServiceProvider.h"
#include "../../../Include/EffectParam.h"

extern CArray<CPanel*, CPanel*> g_PannelAry;

CMediaStream::CMediaStream(HWND hWnd)
{
#ifdef _ENABLE_MEDIA_STREAM_EX
	m_pVideoRawDataCallback = 0;
	m_pD3DRender = 0;
#endif 
	m_pMixerEffect = 0;
	m_pMFilePlayerDll = 0;
	m_dCurPos = 0.0;
	m_dTotalPos = 0.0;
	m_pBuffer = NULL;
	m_pYUVBuffer = NULL;
	m_iWidth = 0;
	m_iHeight = 0;

	m_bEnableUpdatePos = true;
	m_bExtraVideo = false;
	m_iEffectIdx = 0;
	m_bBusyFlag = 0;
	m_strFilePath = "";

	m_pRTSPPlayerDll = 0;
	m_pRTSPObj = 0;

	m_bDoClear = false;

#ifdef _ENABLE_GPU
	m_iSourceMode = SM_TIMELINE;
#endif

	Init(hWnd);

	m_pVideoRawDataCallback = 0;	
	m_bLocked = false;
	m_bLoseVideo = false;
	m_bPlayerWaitForDisplay = false;

#ifdef _ENABLE_DISPLAY_MANAGER
//		m_iSurfaceItemIndex = -1;
	for (int i = 0; i < _MAX_DISPLAY_INDEX; i++)
	{
		m_iDisplayMappingIndex[i] = -1;
		m_iMixerEffectPannelIdx[i] = -1;
	}
#endif

	InitializeCriticalSection(&m_csInUse);
}

CMediaStream::~CMediaStream()
{
	Stop();
	Close();

	m_bBusyFlag = 0;

#ifdef _ENABLE_MEDIA_STREAM_EX
	if (m_pD3DRender)
		delete m_pD3DRender;
#endif

	if (m_pMFilePlayerDll)
	{
		//m_pMFilePlayerDll->FreeLib();
		delete m_pMFilePlayerDll;
	}

	if (m_pRTSPPlayerDll)
	{
		m_pRTSPPlayerDll->_In_RTSP_Destroy(m_pRTSPObj);
		delete m_pRTSPPlayerDll;
	}

	if (m_pBuffer)
		delete m_pBuffer;

	if (m_pYUVBuffer)
		delete m_pYUVBuffer;

	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();
}

ULONG CMediaStream::Release()
{
	ULONG ret = InterlockedDecrement(&m_refCount);	

	if (ret == 0)
	{
		delete this;
	}

	return ret;
}

ULONG CMediaStream::AddRef()
{
    return InterlockedIncrement(& m_refCount);
}


void CMediaStream::Init(HWND hWnd)
{  
	m_pMFilePlayerDll = new MFilePlayerDll;
	m_pMFilePlayerDll->LoadLib(theApp.m_strCurPath + "MFilePlayer.dll");
#ifdef _ENABLE_GPU
	/*
	for (int i = 0; i < 2; i++)
	{
		m_pMFilePlayerDll->Create(i);
		m_pMFilePlayerDll->Init(i, hWnd, this);
		//m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_RGB32);
		m_pMFilePlayerDll->SetOutputVideoFormat(i, VPF_I420);
		m_pMFilePlayerDll->SetDisplayIntervalMode(i, DIM_GOBAL);//DIM_NORMAL , DIM_LOCAL , DIM_GOBAL
	}
	*/

	m_pMFilePlayerDll->Create(0);
	m_pMFilePlayerDll->Init(0, hWnd, this);
	//m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_RGB32);
	m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_I420);
	m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_GOBAL);//DIM_NORMAL , DIM_LOCAL , DIM_GOBAL

	for (int i = 1; i < _MAX_MF_OBJS; i++)
	{
		m_pMFilePlayerDll->Create(i);
		m_pMFilePlayerDll->Init(i, hWnd, this);
		//m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_RGB32);
		m_pMFilePlayerDll->SetOutputVideoFormat(i, VPF_I420);
		m_pMFilePlayerDll->SetDisplayIntervalMode(i, DIM_GOBAL);//DIM_NORMAL , DIM_LOCAL , DIM_GOBAL
	}
	
#else
	m_pMFilePlayerDll->Create(0);
	m_pMFilePlayerDll->Init(0, hWnd, this);
	//m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_RGB32);
	m_pMFilePlayerDll->SetOutputVideoFormat(0, VPF_I420);
	m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_GOBAL);//DIM_NORMAL , DIM_LOCAL , DIM_GOBAL
#endif

	m_iWidth = 0;
	m_iHeight = 0;
	m_bFromShape = FALSE;

	m_iFrameCount = 0;

	m_pRTSPPlayerDll = new RTSPPlayerDll;
	m_pRTSPPlayerDll->LoadLib();
	m_pRTSPObj = m_pRTSPPlayerDll->_In_RTSP_Create();
	m_pRTSPPlayerDll->_In_RTSP_Init(m_pRTSPObj,this);
	m_pRTSPPlayerDll->_In_RTSP_SetOutputVideoFormat(m_pRTSPObj, VPF_I420);

	for(int i = 0;i < 4;i++)
		m_iDisplayOrder[i] = i;
}

#ifdef _ENABLE_MEDIA_STREAM_EX
void CMediaStream::InitRender(HWND hWnd, void* pVoid)
{
	if (m_pD3DRender)
		delete m_pD3DRender;

	m_pD3DRender = new D3DRender(pVoid);

	m_iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_pD3DRender->Init(hWnd, VPF_RGB32, m_iScreenWidth, m_iScreenHeight); //1920, 1080
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();
}

void CMediaStream::SetMixerEffect(CMixerEffect* pObj)
{
	m_pMixerEffect = pObj;
}
#endif

void CMediaStream::SetVideoRawDataCallback(void* pObj)
{
	m_pVideoRawDataCallback = (VideoRawDataCallback*)pObj;
}

void CMediaStream::EnableGPU(bool bEnable, void* pD3DDevice)
{
	m_pMFilePlayerDll->EnableGPU(0, bEnable);
	m_pMFilePlayerDll->SetupDXVA2(0, pD3DDevice);
}

#ifdef _ENABLE_GPU
void CMediaStream::SetSourceMode(int iMode)
{
	if (iMode > 1)
		return;
	m_iSourceMode = iMode;// SM_TIMELINE
}

void CMediaStream::EnableGPU2(bool bEnable, int iIndex, void* pD3DDevice)
{
	m_pMFilePlayerDll->EnableGPU(iIndex, bEnable);
	m_pMFilePlayerDll->SetupDXVA2(iIndex, pD3DDevice);
	m_pMFilePlayerDll->EnableAudio(iIndex, false);
}
#endif

CMixerEffect* CMediaStream::GetMixerEffect()
{
	return m_pMixerEffect;
}

void CMediaStream::SetEffectIndex(int iID)
{
	m_iEffectIdx = iID;
	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectID(iID);

	for (int x = 0; x < m_pMixerEffectAry.GetCount(); x++)
		m_pMixerEffectAry.GetAt(x)->SetEffectID(iID);
}

int CMediaStream::GetEffectIndex()
{
	if (m_pMixerEffect)
		return m_pMixerEffect->GetEffectID();
	return 0;
}

void CMediaStream::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
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

	for (int x = 0; x < m_pMixerEffectAry.GetCount(); x++)
		m_pMixerEffectAry.GetAt(x)->SetEffectParam(effect_param);
}

void CMediaStream::SetEffectParam(int iIndex, int iValue)
{
	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(iIndex, iValue);

	for (int x = 0; x < m_pMixerEffectAry.GetCount(); x++)
		m_pMixerEffectAry.GetAt(x)->SetEffectParam(iIndex, iValue);
}

INT32 CMediaStream::GetEffectParam(int iIndex)
{
	INT32 iResult = 0;

	if (m_pMixerEffect)
		iResult = m_pMixerEffect->GetEffectParam(iIndex);

	return iResult;
}


void CMediaStream::Open(char* szFileName, BOOL bFromShape)
{
	strcpy(m_szFileName, szFileName);

	if (IsRTSPUrl(m_szFileName))
		OpenFromRTSP(m_szFileName, bFromShape);
	else
		OpenFromFile(szFileName, bFromShape);
}

void CMediaStream::OpenFromFile(char* szFileName, BOOL bFromShape)
{
	m_strFilePath.SetString(szFileName);
	INT32 iStrPos = 0;

	//bool bEnableRepeat = GetPrivateProfileInt("Media", "EnableRepeat", 1, theApp.m_strCurPath + "Setting.ini");

	int iTimeCodeType = GetPrivateProfileInt("Timeline_Setting", "TimeCodeType", 0, theApp.m_strCurPath + "Setting.ini");

	if(iTimeCodeType == 0)
		m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_GOBAL);
	else
		m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_LOCAL);

	if (bFromShape)
	{
		m_bFromShape = TRUE;
#ifdef _ENABLE_GPU

		m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_LOCAL);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			m_pMFilePlayerDll->SetDisplayIntervalMode(1 + i, DIM_LOCAL);
		}
#else
		m_pMFilePlayerDll->SetDisplayIntervalMode(0, DIM_LOCAL);
#endif
	}

	for (int x = 0; x < m_strFilePath.GetLength(); x++)
	{
		if (m_strFilePath.GetAt(m_strFilePath.GetLength() - x - 1) == '\\')
		{
			iStrPos = x;
			break;
		}
	}

	m_strFileName = m_strFilePath.Right(iStrPos);

#ifdef _ENABLE_GPU

	m_pMFilePlayerDll->Open(0, szFileName);


	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		if (i > 0)
		{
			m_pMFilePlayerDll->EnableGPU(i,true);
			DisplayManager* pCurDisplayManager = g_PannelAry[i]->GetDisplayManager();
			if(pCurDisplayManager)
				m_pMFilePlayerDll->SetupDXVA2(i, pCurDisplayManager->GetD3DDevice());
			m_pMFilePlayerDll->Open(i, szFileName);
		}
		//m_pMFilePlayerDll->Open(1 + i, szFileName);
	}

#else
	m_pMFilePlayerDll->Open(0, szFileName);
#endif

	float fFPS = m_pMFilePlayerDll->GetVideoFPS(0);
	int iTotalOfFrames = m_pMFilePlayerDll->GetTotalOfFrames(0);
	int w = m_pMFilePlayerDll->GetVideoWidth(0);
	int h = m_pMFilePlayerDll->GetVideoHeight(0);

	m_iWidth = w;
	m_iHeight = h;

#ifdef _ENABLE_GPU
	if (bFromShape || iTimeCodeType == 1)
		m_pMFilePlayerDll->SetRepeat(0, true);
	else
		m_pMFilePlayerDll->SetRepeat(0, false);

	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		
		if (i > 0)
		{
			if (bFromShape || iTimeCodeType == 1)
				m_pMFilePlayerDll->SetRepeat(i, true);
			else
				m_pMFilePlayerDll->SetRepeat(i, false);
		}
		
		/*
		if (bFromShape)
			m_pMFilePlayerDll->SetRepeat(1 + i, true);
		else
			m_pMFilePlayerDll->SetRepeat(1 + i, false);
			*/
	}

#else
	if (bFromShape || iTimeCodeType == 1)
		m_pMFilePlayerDll->SetRepeat(0, true);
	else
		m_pMFilePlayerDll->SetRepeat(0, false);
#endif

	m_iIndex = g_MediaStreamAry.size();
	m_iPannelIdx = g_PannelSetting.iSelIdx;

	if (!m_pVideoRawDataCallback)
	{
#ifndef MULTI_STREAM
		if (g_PannelAry.GetCount() > 0)
			CreateMixEffect(g_PannelAry[iPannelIdx]->GetD3DDevice());
#endif
	}

	//	m_iWidth = m_iScreenWidth;// 1920;
	//	m_iHeight = m_iScreenHeight;// 1080;

	if (bFromShape)
	{
		UINT iStreamMapIdx = g_StreamMappingAry.GetCount() - 1;
		g_StreamMappingAry.GetAt(iStreamMapIdx).iWidth = w;// m_iWidth;
		g_StreamMappingAry.GetAt(iStreamMapIdx).iHeight = h;// m_iHeight;
	}

	/*g_MediaStreamAry.resize(m_iIndex + 1);
	g_MediaStreamAry.at(m_iIndex) = this;

	STREM_MAPPING StreamMap;
	StreamMap.iPanelIdx = m_iPannelIdx;

	StreamMap.strObjPath = "";
	StreamMap.strVideoPath = m_strFilePath;
	StreamMap.bExtraVideo = FALSE;
	StreamMap.iWidth = w;
	StreamMap.iHeight = h;
	StreamMap.iMediaIdx = m_iIndex;
	StreamMap.iObjIdx = 0;
	for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
	{
	if (g_StreamMappingAry.GetAt(x).iPanelIdx == m_iPannelIdx)
	StreamMap.iObjIdx++;
	}

	g_StreamMappingAry.Add(StreamMap);*/

	g_PannelSetting.rectSurface.Width = m_iWidth;
	g_PannelSetting.rectSurface.Height = m_iHeight;

	if (m_pBuffer)
		delete m_pBuffer;

	m_pBuffer = new BYTE[w * h * 4];

	if (m_pYUVBuffer)
		delete m_pYUVBuffer;
		
	m_pYUVBuffer = new BYTE[w * h * 3 / 2];
}

void CMediaStream::OpenFromRTSP(char* szUrl, BOOL bFromShape)
{
	m_strFilePath.SetString(szUrl);

	if (m_pRTSPPlayerDll && m_pRTSPObj)
	{
		if (IsOpened())
		{
			m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
			m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);
		}

		int iRet = m_pRTSPPlayerDll->_In_RTSP_CheckNetworkStatus(m_pRTSPObj, szUrl);
		m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
		m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);

		m_pRTSPPlayerDll->_In_RTSP_SetOutputVideoFormat(m_pRTSPObj, VPF_I420);
		if(iRet == 1)
			m_pRTSPPlayerDll->_In_RTSP_Open(m_pRTSPObj, szUrl);

		if (bFromShape)
		{
			Sleep(1000);

			m_iWidth = 1920;
			m_iHeight = 1080;

			if(m_pRTSPPlayerDll->_In_RTSP_GetVideoWidth(m_pRTSPObj) > 0)
				m_iWidth = m_pRTSPPlayerDll->_In_RTSP_GetVideoWidth(m_pRTSPObj);
			if(m_pRTSPPlayerDll->_In_RTSP_GetVideoHeight(m_pRTSPObj) > 0)
				m_iHeight = m_pRTSPPlayerDll->_In_RTSP_GetVideoHeight(m_pRTSPObj);

			g_PannelSetting.rectSurface.Width = m_iWidth;
			g_PannelSetting.rectSurface.Height = m_iHeight;

			UINT iStreamMapIdx = g_StreamMappingAry.GetCount() - 1;
			g_StreamMappingAry.GetAt(iStreamMapIdx).iWidth = m_iWidth;// m_iWidth;
			g_StreamMappingAry.GetAt(iStreamMapIdx).iHeight = m_iHeight;// m_iHeight;
		}

		m_iIndex = g_MediaStreamAry.size();
		m_iPannelIdx = g_PannelSetting.iSelIdx;
	}
}

void CMediaStream::Close(BOOL bCloseFromShape)
{
	if (bCloseFromShape && !m_bFromShape)
		return;

	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
		{
			m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
			m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);
		}
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Close(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			
			if (i > 0)
			{
				if (m_pMFilePlayerDll)
					m_pMFilePlayerDll->Close(i);
			}
			/*
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->Close(1+i);
				*/
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Close(0);
#endif
	}

	if (m_pBuffer)
		delete m_pBuffer;

	if (m_pYUVBuffer)
		delete m_pYUVBuffer;

	m_strFileName = "";
	m_strFilePath = "";
	m_pBuffer = 0;
	m_pYUVBuffer = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}

void CMediaStream::Play()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Play(m_pRTSPObj);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Play(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			
			if (i > 0)
			{
				if (m_pMFilePlayerDll)
					m_pMFilePlayerDll->Play(i);
			}
			/*

			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->Play(1 + i);
				*/
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Play(0);
#endif
	}
}

void CMediaStream::Stop()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Stop(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			
			if (i > 0)
			{
				if (m_pMFilePlayerDll)
					m_pMFilePlayerDll->Stop(i);
			}
			/*
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->Stop(1 + i);
				*/
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Stop(0);
#endif
	}

	m_iFrameCount++;
}

void CMediaStream::Stop2()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Stop(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{

			if (i > 0)
			{
				if (m_pMFilePlayerDll)
					m_pMFilePlayerDll->Stop(i);
			}
			/*
			if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Stop(1 + i);
			*/
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Stop2(0);
#endif
	}

	m_iFrameCount++;
}

void CMediaStream::Seek(int iPos)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Seek(0, iPos);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->Seek(1 + i, iPos);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Seek(0, iPos);
#endif
	}
}

void CMediaStream::EnableAudio(bool bEnable)
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_EnableAudio(m_pRTSPObj, bEnable);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableAudio(0, bEnable);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				//m_pMFilePlayerDll->EnableAudio(1 + i, bEnable);
				m_pMFilePlayerDll->EnableAudio(1 + i, false);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableAudio(0, bEnable);
#endif
	}
}

void CMediaStream::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

void CMediaStream::SpeedUp()
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SpeedUp(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SpeedUp(1 + i);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SpeedUp(0);
#endif
	}
}

void CMediaStream::SpeedDown()
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SpeedDown(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SpeedDown(1 + i);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SpeedDown(0);
#endif
	}
}

int CMediaStream::GetVolume()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			return m_pRTSPPlayerDll->_In_RTSP_GetVolume(m_pRTSPObj);
	}
	else
	{
		if (m_pMFilePlayerDll)
			return m_pMFilePlayerDll->GetVolume(0);
	}
	return 0;
}

int CMediaStream::GetSystemVolume()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetSystemVolume(0);
	return 0;
}

void CMediaStream::SetVolume(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_SetVolume(m_pRTSPObj, iValue);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetVolume(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetVolume(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetVolume(0, iValue);
#endif
	}
}

void CMediaStream::SetSystemVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSystemVolume(0, iValue);
}

void CMediaStream::SetBrightness(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetBrightness(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetBrightness(1 + i, iValue);
		}

#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetBrightness(0, iValue);
#endif
	}
}

void CMediaStream::SetContrast(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetContrast(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetContrast(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetContrast(0, iValue);
#endif
	}
}

void CMediaStream::SetSaturation(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetSaturation(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetSaturation(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetSaturation(0, iValue);
#endif
	}
}

void CMediaStream::SetHue(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetHue(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetHue(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetHue(0, iValue);
#endif
	}
}

void CMediaStream::SetR(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetR(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetR(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetR(0, iValue);
#endif
	}
}

void CMediaStream::SetG(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetG(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetG(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetG(0, iValue);
#endif
	}
}

void CMediaStream::SetB(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetB(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetB(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetB(0, iValue);
#endif
	}
}

void CMediaStream::SetGray(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetGray(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetGray(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetGray(0, iValue);
#endif
	}
}

void CMediaStream::SetNegative(int iValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetNegative(0, iValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetNegative(1 + i, iValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetNegative(0, iValue);
#endif
	}
}

void CMediaStream::PlayBackward()
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->PlayBackward(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->PlayBackward(1 + i);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->PlayBackward(0);
#endif
	}
}

int CMediaStream::GetPlaybackMode()
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if(m_iSourceMode == SM_SHAPE)
			return m_pMFilePlayerDll->GetPlaybackMode(1);
		else
			return m_pMFilePlayerDll->GetPlaybackMode(0);
#else
		if (m_pMFilePlayerDll)
			return m_pMFilePlayerDll->GetPlaybackMode(0);
#endif
	}
	return 0;
}

void CMediaStream::Pause()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Pause(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->Pause(1 + i);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->Pause(0);
#endif
	}
}

void CMediaStream::SetFadeDuration(float fValue)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetFadeDuration(0, fValue);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetFadeDuration(1 + i, fValue);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetFadeDuration(0, fValue);
#endif
	}
}

void CMediaStream::EnableFadeIn(bool bEnable)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableFadeIn(0, bEnable);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->EnableFadeIn(1 + i, bEnable);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableFadeIn(0, bEnable);
#endif
	}
}

void CMediaStream::EnableFadeOut(bool bEnable)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableFadeOut(0, bEnable);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->EnableFadeOut(1 + i, bEnable);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableFadeOut(0, bEnable);
#endif
	}
}

bool CMediaStream::IsOpened()
{
	if (IsRTSPUrl(m_szFileName))
	{
		if (m_pRTSPPlayerDll && m_pRTSPObj)
			return m_pRTSPPlayerDll->_In_RTSP_IsOpened(m_pRTSPObj);
	}
	else
	{
		if (m_pMFilePlayerDll)
			return m_pMFilePlayerDll->IsOpened(0);
	}
	return false;
}

void CMediaStream::SetScale(float fW, float fH)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetScale(0, fW, fH);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetScale(1 + i, fW, fH);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetScale(0, fW, fH);
#endif
	}
}

void CMediaStream::ResetFilter()
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->ResetFilter(0);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->ResetFilter(1 + i);
		}
#else
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->ResetFilter(0);
#endif
	}
}

float CMediaStream::GetFPS()
{
	if (IsRTSPUrl(m_szFileName))
	{
		return 0.0;
	}
	else
	{
		return m_pMFilePlayerDll->GetVideoFPS(0);
	}
}

void CMediaStream::SetReferenceTimeCode(double dNewRefTime)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
		m_pMFilePlayerDll->SetReferenceTimeCode(0, dNewRefTime);

}

int CMediaStream::GetAudioInfo(int& iChannelLayout, int& iSampleRate, int& iNBSamples)
{
	if (IsRTSPUrl(m_szFileName))
	{
		return 0;
	}
	else
		return m_pMFilePlayerDll->GetAudioInfo(0, iChannelLayout,iSampleRate, iNBSamples);
}

void CMediaStream::UpdateGlobalTimeCode(double dTimeCode)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
//		if (m_iSourceMode == SM_SHAPE)
		{
			int iCount = g_PannelAry.GetCount();
			for (int i = 0; i < g_PannelAry.GetCount(); i++)
			{
				if (m_pMFilePlayerDll)
				{
					if (m_iDisplayMappingIndex[i] != -1)
						//m_pMFilePlayerDll->UpdateGlobalTimeCode(1 + i, dTimeCode);
						m_pMFilePlayerDll->UpdateGlobalTimeCode(i, dTimeCode);
				}
			}
		}
//		else
			m_pMFilePlayerDll->UpdateGlobalTimeCode(0, dTimeCode);
#else
		m_pMFilePlayerDll->UpdateGlobalTimeCode(0, dTimeCode);
#endif
	}
}

void CMediaStream::SetStartTime(double dTime)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetStartTime(0, dTime);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->SetStartTime(1 + i, dTime);
		}
#else
		m_pMFilePlayerDll->SetStartTime(0, dTime);
#endif
	}
}

//format - 00:00 / 02:00
void CMediaStream::UpdatePosInfo(char* szCurPos, char* szTotalLen)
{
	m_sCurPos.SetString(szCurPos);
	m_sTotalLen.SetString(szTotalLen);

	//char szMsg[80];
	//sprintf(szMsg, "TC CMediaStream::UpdatePosInfo - Cur Pos %s, Total Len %s\n", szCurPos, szTotalLen);
	//OutputDebugStringA(szMsg);
}

//total len
void CMediaStream::SetSliderCtrlRange(double dPos)
{
	m_dTotalPos = dPos;

	//char szMsg[512];
	//sprintf(szMsg, "CMediaStream::SetSliderCtrlRange - %4.3f\n", m_dTotalPos);
	//OutputDebugStringA(szMsg);
}

//cur pos
void CMediaStream::SetSliderCtrlPos(double dPos)
{
	m_dCurPos = dPos;
	if (m_bEnableUpdatePos)
	{
		m_dCurPos = dPos;

		//char szMsg[512];
		//sprintf(szMsg, "CMediaStream::SetSliderCtrlPos - %4.3f\n", m_dCurPos);
		//OutputDebugStringA(szMsg);
	}
}

void CMediaStream::DisplayVideoWithEffect(unsigned char* pBuffer, int w, int h)
{
	DWORD dwDisTime1 = timeGetTime();

#ifdef _ENABLE_DISPLAY_MANAGER
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		if (m_pVideoRawDataCallback)
			m_pVideoRawDataCallback->DisplayNV12SurfaceData(pBuffer, w, h);
		/*
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		m_pDisplayManagerAry.GetAt(x)->RenderSurface(m_iSurfaceMappingAry.GetAt(x),
		VPF_NV12, (unsigned char*)pBuffer, w, h);
		*/
	}
	else
	{
		if (m_pMixerEffect->GetWidth() != w || m_pMixerEffect->GetHeight() != h)
			m_pMixerEffect->ResetSurfaces(w, h);
		//			ResetFilter();

		sEffectParam effect_param = m_pMixerEffect->GetEffectParam();

		memcpy(m_pYUVBuffer, pBuffer, w * h * 3 / 2);

		auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		DisplayManager* pDisplayManager = tl_manager->GetDisplayManager();

		if (m_pMixerEffect && m_pMixerEffect->GetEffectID() != 0 /*|| m_pMixerEffect->IsZoomed()) */ && effect_param.iLevel != 0)
		{
			if (pDisplayManager->IsSuspend())
			{
				LeaveCriticalSection(&m_csInUse);
				return;
			}

			pDisplayManager->Suspend();

			if (m_pVideoRawDataCallback)
			{
				if (!pDisplayManager->IsDeivceLost())
				{
					IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect2(pBuffer, w, h);
					if (pOutSurface)
						m_pVideoRawDataCallback->DisplayVideoSurfaceData(pOutSurface, w, h);
				}
			}

			for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
			{
				DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
				if (pCurDisplayManager)
				{
					if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->IsD3DUsing() == 0)
						//if (!pCurDisplayManager->IsSuspend())
						DrawPannel(x, pBuffer, w, h, true);
				}
			}
			pDisplayManager->Resume();
		}
		else
		{

			for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
			{
				DrawPannel(x, pBuffer, w, h, false);
			}

			if (m_pVideoRawDataCallback)
				m_pVideoRawDataCallback->DisplayVideoRawData(pBuffer, w, h);
		}
		pDisplayManager->UpdateFrameCount(m_iFrameCount);
	}

	DWORD dwDisTime2 = timeGetTime();

	//	sprintf(szMsg, "TC CMediaStream::DisplayVideo - spend_time : %d\n", dwDisTime2 - dwDisTime1);
	//	OutputDebugStringA(szMsg);
#else

	

#endif
}

void CMediaStream::DisplayVideoWithoutEffect(unsigned char* pBuffer, int w, int h)
{
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
	{
		if (m_pVideoRawDataCallback)
			m_pVideoRawDataCallback->DisplayNV12SurfaceData(pBuffer, w, h);
	}
	else
	{
	
		if (m_pMixerEffect->GetWidth() != w || m_pMixerEffect->GetHeight() != h)
			m_pMixerEffect->ResetSurfaces(w, h);
		//			ResetFilter();

		sEffectParam effect_param = m_pMixerEffect->GetEffectParam();

		memcpy(m_pYUVBuffer, pBuffer, w * h * 3 / 2);
	

		auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		DisplayManager* pDisplayManager = tl_manager->GetDisplayManager();

#if 0
		if (m_pMixerEffect && m_pMixerEffect->GetEffectID() != 0 /*|| m_pMixerEffect->IsZoomed()) */ && effect_param.iLevel != 0)
		{
			if (pDisplayManager->IsSuspend())
			{
				LeaveCriticalSection(&m_csInUse);
				return;
			}

			pDisplayManager->Suspend();

			if (m_pVideoRawDataCallback)
			{
				if (!pDisplayManager->IsDeivceLost())
				{
					IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect2(pBuffer, w, h);
					if (pOutSurface)
						m_pVideoRawDataCallback->DisplayVideoSurfaceData(pOutSurface, w, h);
				}
			}

			for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
			{
				DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
				if (pCurDisplayManager)
				{
					if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->IsD3DUsing() == 0)
						//if (!pCurDisplayManager->IsSuspend())
						DrawPannel(x, pBuffer, w, h, true);
				}
			}
			pDisplayManager->Resume();
		}
		else
		{
#endif
			for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
			{
				DrawPannel(x, pBuffer, w, h, false);
			}

			if (m_pVideoRawDataCallback)
				m_pVideoRawDataCallback->DisplayVideoRawData(pBuffer, w, h);
			pDisplayManager->UpdateFrameCount(m_iFrameCount);
//		}
	}
}

void CMediaStream::DisplayVideo(unsigned char* pBuffer, int w, int h)
{
	char szMsg[512];

	EnterCriticalSection(&m_csInUse);

	if (IsRTSPUrl(m_szFileName))
	{
		if (m_iWidth != w || m_iHeight != h)
		{
			m_iWidth = w;
			m_iHeight = h;

			if (m_pBuffer)
				delete m_pBuffer;

			m_pBuffer = new BYTE[w * h * 4];

			if (m_pYUVBuffer)
				delete m_pYUVBuffer;

			m_pYUVBuffer = new BYTE[w * h * 3 / 2];

			g_PannelSetting.rectSurface.Width = m_iWidth;
			g_PannelSetting.rectSurface.Height = m_iHeight;
		}
	}

#ifdef _ENABLE_SINGLE_EFFECT
	DisplayVideoWithoutEffect(pBuffer,w,h);
#else
	DisplayVideoWithEffect(pBuffer,w,h);
#endif

	LeaveCriticalSection(&m_csInUse);
	m_iFrameCount++;
}

void CMediaStream::DisplayVideo_HW_WithEffect(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{
#ifdef _ENABLE_DISPLAY_MANAGER

	bool bDoEffect = false;
	if (m_pMixerEffect)
	{
		sEffectParam ep = m_pMixerEffect->GetEffectParam();
		if (m_pMixerEffect->GetEffectID() != 0 && ep.iLevel != 0)
			bDoEffect = true;
	}

	if (m_pMFilePlayerDll->GetD3DDevice(0) == pD3DDevice)
	{
		if (m_pMFilePlayerDll->GPUIsEnabled(0))
		{
			auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
			DisplayManager* pDisplayManager = tl_manager->GetDisplayManager();

#ifdef _ENABLE_HD_EFFECT
			if (bDoEffect)
				pDisplayManager->DoHDEffect2(m_iSurfaceItemIndex, (IDirect3DSurface9*)pBuffer, w, h);
			else
			{
				if (m_pVideoRawDataCallback)
					m_pVideoRawDataCallback->DisplayNV12SurfaceData(pBuffer, w, h);
			}
#else
			if (bDoEffect)
			{
				if (pDisplayManager->IsSuspend())
				{
					return;
				}

				pDisplayManager->Suspend();

				if (m_pMixerEffect->GetWidth() != w || m_pMixerEffect->GetHeight() != h)
				{
					m_pMixerEffect->ResetSurfaces(w, h);
				}
				IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect_Surface((IDirect3DSurface9*)pBuffer, w, h);
				if (pOutSurface)
				{
					m_pVideoRawDataCallback->DisplayVideoSurfaceData(pOutSurface, w, h);
				}

				pDisplayManager->Resume();
			}
			else
			{
				if (m_pVideoRawDataCallback)
				{
					m_pVideoRawDataCallback->DisplayNV12SurfaceData(pBuffer, w, h);
				}
			}
#endif

			pDisplayManager->UpdateFrameCount(m_iFrameCount);
		}
	}
#ifdef _ENABLE_GPU
	//	if (m_iSourceMode == SM_SHAPE)
	{
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
			if (pCurDisplayManager)
			{
				if (pCurDisplayManager->GetD3DDevice() == pD3DDevice)
				{
					if (bDoEffect)
					{
						if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->IsD3DUsing() == 0)
							//if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->GetRenderStatus() != RS_DRAW)
							DrawPannel_HW(x, pBuffer, w, h, bDoEffect);
					}
					else
					{
						DrawPannel_HW(x, pBuffer, w, h, bDoEffect);
					}
					break;
				}
			}
		}
	}
#endif

#endif
}

void CMediaStream::DisplayVideo_HW_WithoutEffect(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{
	bool bDoEffect = false;
	if (m_pMixerEffect)
	{
		sEffectParam ep = m_pMixerEffect->GetEffectParam();
		if (m_pMixerEffect->GetEffectID() != 0 && ep.iLevel != 0)
			bDoEffect = true;
	}

	if (m_pMFilePlayerDll->GetD3DDevice(0) == pD3DDevice)
	{
		if (m_pMFilePlayerDll->GPUIsEnabled(0))
		{
			auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
			DisplayManager* pDisplayManager = tl_manager->GetDisplayManager();

			if (m_pVideoRawDataCallback)
			{
				m_pVideoRawDataCallback->DisplayNV12SurfaceData(pBuffer, w, h);
			}

			pDisplayManager->UpdateFrameCount(m_iFrameCount);
		}
	}
#ifdef _ENABLE_GPU
	//	if (m_iSourceMode == SM_SHAPE)
	{
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
			if (pCurDisplayManager)
			{
				if (pCurDisplayManager->GetD3DDevice() == pD3DDevice)
				{
					if (bDoEffect)
					{
						if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->IsD3DUsing() == 0)
							//if (!pCurDisplayManager->IsSuspend() && ((CShapeDispManager*)pCurDisplayManager)->GetRenderStatus() != RS_DRAW)
							DrawPannel_HW(x, pBuffer, w, h, false);
					}
					else
					{
						DrawPannel_HW(x, pBuffer, w, h, false);
					}
					break;
				}
			}
		}
	}
#endif
}

void CMediaStream::DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{

#ifdef _ENABLE_SINGLE_EFFECT
	
#ifdef _ENABLE_HD_EFFECT
	DisplayVideo_HW_WithEffect(pD3DDevice,pBuffer, w, h);
#else  //_ENABLE_HD_EFFECT  
	DisplayVideo_HW_WithoutEffect(pD3DDevice,pBuffer, w, h);
#endif  //_ENABLE_HD_EFFECT

#else
	DisplayVideo_HW_WithEffect(pD3DDevice,pBuffer, w, h);
#endif

	m_iFrameCount++;
}

void CMediaStream::DrawPannel(int iPannelIndex, unsigned char* pBuffer, int w, int h, bool bDoEffect)
{
	CMixerEffect * pMixerEffect = 0;
	//	FindObjIndex(iPannelIndex);

	if (m_bLocked)
	{
		m_bLoseVideo = true;
		return;
	}

	int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(iPannelIndex);// m_iDisplayMappingIndex[iPannelIndex];

	if (iSurfaceIdx == -1)
		return;

	DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(iPannelIndex);

	if (m_pMixerEffectAry.GetCount() > iPannelIndex)
	{
		pMixerEffect = m_pMixerEffectAry.GetAt(iPannelIndex);
	}

	pCurDisplayManager->SetCurPos(iSurfaceIdx, m_dCurPos);

	if (pCurDisplayManager->IsSuspend())
		return;

	if (bDoEffect)
	{
		pCurDisplayManager->Suspend();
		if (pMixerEffect)
		{
			sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
			pMixerEffect->SetEffectID(m_pMixerEffect->GetEffectID());
			pMixerEffect->SetEffectParam(effect_param);

			((CShapeDispManager*)pCurDisplayManager)->DoMixerEffect(pMixerEffect, pBuffer, w, h, iSurfaceIdx);
		}
		pCurDisplayManager->Resume();
	}
	else
	{
		pCurDisplayManager->UpdateFrameCount(m_iFrameCount);

		if (theApp.m_bEnableVWallDisplay)
		{
			if (theApp.m_iVWallOffsetIndex == -1)
				pCurDisplayManager->RenderFrame(iSurfaceIdx, VPF_I420, (unsigned char*)pBuffer, w, h, theApp.m_iVWallIndexOrder[iPannelIndex]);
			else
				pCurDisplayManager->RenderFrame(iSurfaceIdx, VPF_I420, (unsigned char*)pBuffer, w, h, iPannelIndex + theApp.m_iVWallOffsetIndex);
		}
		else
			pCurDisplayManager->RenderFrame(iSurfaceIdx,VPF_I420, (unsigned char*)pBuffer, w, h);	

#ifdef _ENABLE_SINGLE_EFFECT
		pCurDisplayManager->EnableToDoEffect(true);
#endif
	}

//	pCurDisplayManager->SetCurPos(iSurfaceIdx,m_dCurPos);
	UpdateProcessFPS();
}

void CMediaStream::DrawPannel_HW(int iPannelIndex, unsigned char* pBuffer, int w, int h, bool bDoEffect)
{
	CMixerEffect * pMixerEffect = 0;
//	FindObjIndex(iPannelIndex);

	if (m_bLocked)
		return;

	int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(iPannelIndex); //m_iDisplayMappingIndex[iPannelIndex];

	if (iSurfaceIdx == -1)
		return;

	DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(iPannelIndex);

	if (m_pMixerEffectAry.GetCount() > iPannelIndex)
	{
		pMixerEffect = m_pMixerEffectAry.GetAt(iPannelIndex);
	}

//	D3DSurfaceItem* pD3DSurfaceItem = pCurDisplayManager->GetSurface(iSurfaceIdx);
//	pD3DSurfaceItem->pD3DSurface = (IDirect3DSurface9*)pBuffer;

	pCurDisplayManager->SetCurPos(iSurfaceIdx, m_dCurPos);

//	if (pCurDisplayManager->IsSuspend())
//		return;

	if (bDoEffect)
	{
		pCurDisplayManager->UpdateFrameCount(m_iFrameCount);

#ifdef _ENABLE_HD_EFFECT
//		if (m_pMFilePlayerDll)
//			m_pMFilePlayerDll->EnableSuspendDisplay(iPannelIndex, true);

		pCurDisplayManager->Suspend();
		if (pMixerEffect)
		{
			DWORD dwTime1 = timeGetTime();

			((CShapeDispManager*)pCurDisplayManager)->DoHDEffect2(iSurfaceIdx, (IDirect3DSurface9*)pBuffer, w, h);

			//IDirect3DSurface9* pHDOutSurface = ((CShapeDispManager*)pCurDisplayManager)->DoHDEffect(iSurfaceIdx, (IDirect3DSurface9*)pBuffer, w, h);
//			if(pHDOutSurface)
//				g_PannelAry[iPannelIndex]->DisplayVideo_HDEffect(iSurfaceIdx, pHDOutSurface, w, h);

		   // ((CShapeDispManager*)pCurDisplayManager)->DoMixerEffect_SurfaceHD(pMixerEffect, (IDirect3DSurface9*)pBuffer, w, h, iSurfaceIdx);
			DWORD dwTime2 = timeGetTime();

			if (dwTime2 - dwTime1 > 0)
			{
				char szMsg[512];
				sprintf(szMsg, "#HD_effect# CMediaStream::DrawPannel_HW(): [%d]\n", dwTime2 - dwTime1);
				OutputDebugStringA(szMsg);
			}
		}
		pCurDisplayManager->Resume();

//		if (m_pMFilePlayerDll)
//			m_pMFilePlayerDll->EnableSuspendDisplay(iPannelIndex, false);
#else
		pCurDisplayManager->Suspend();
		if (pMixerEffect)
		{
			sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
			pMixerEffect->SetEffectID(m_pMixerEffect->GetEffectID());
			pMixerEffect->SetEffectParam(effect_param);

			((CShapeDispManager*)pCurDisplayManager)->DoMixerEffect_Surface(pMixerEffect, (IDirect3DSurface9*) pBuffer, w, h,iSurfaceIdx);
		}
		pCurDisplayManager->Resume();
#endif
	}
	else
	{
		pCurDisplayManager->UpdateFrameCount(m_iFrameCount);
		if (theApp.m_bEnableVWallDisplay)
		{
			if (theApp.m_iVWallOffsetIndex == -1)
				pCurDisplayManager->RenderSurface(iSurfaceIdx, VPF_NV12, (unsigned char*)pBuffer, w, h, theApp.m_iVWallIndexOrder[iPannelIndex]);
			else
				pCurDisplayManager->RenderSurface(iSurfaceIdx, VPF_NV12, (unsigned char*)pBuffer, w, h, iPannelIndex + theApp.m_iVWallOffsetIndex);
		}
		else
			pCurDisplayManager->RenderSurface(iSurfaceIdx,VPF_NV12, (unsigned char*)pBuffer, w, h);

#ifdef _ENABLE_SINGLE_EFFECT
		pCurDisplayManager->EnableToDoEffect(true);
#endif

	}

//	pCurDisplayManager->SetCurPos(iSurfaceIdx,m_dCurPos);

	UpdateProcessFPS();
}

void CMediaStream::PlayAudio(unsigned char* pBuffer, int iLen)
{
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		CShapeDispManager* pShapeDispManager = m_pDisplayManagerAry.GetAt(x);
		if(pShapeDispManager)
			pShapeDispManager->RecordAudio(pBuffer, iLen);
	}

	//if (m_pDisplayManager)
		//m_pDisplayManager->RecordAudio(pBuffer,iLen);
}

void CMediaStream::EnableReSampleRate(bool bEnable)
{
	if (IsRTSPUrl(m_szFileName))
	{
	}
	else
	{
#ifdef _ENABLE_GPU
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableReSampleRate(0, bEnable);

		for (int i = 0; i < g_PannelAry.GetCount(); i++)
		{
			if (m_pMFilePlayerDll)
				m_pMFilePlayerDll->EnableReSampleRate(1 + i, bEnable);
		}
#else
		m_pMFilePlayerDll->EnableReSampleRate(0, bEnable);
#endif
	}
}

bool CMediaStream::GPUIsEnabled()
{
	if (m_pMFilePlayerDll->GPUIsEnabled(0))
		return true;
	return false;
}

void CMediaStream::Event(int iType)
{
	switch (iType)
	{
		case FPE_DO_REPEAT:
			{
				for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
				{
					CShapeDispManager* pShapeDispManager = m_pDisplayManagerAry.GetAt(x);
					if (pShapeDispManager && pShapeDispManager->IsRecordAVI())
					{
						HWND hPanelWnd = pShapeDispManager->GetPanelHWND();
						if (hPanelWnd)
							::PostMessage(hPanelWnd, WM_STOP_AVI,0,0);
						/*
						INPUT ip;

						// Set up a generic keyboard event.
						ip.type = INPUT_KEYBOARD;
						ip.ki.wScan = 0; // hardware scan code for key
						ip.ki.time = 0;
						ip.ki.dwExtraInfo = 0;

						// Press the "S" key
						ip.ki.wVk = 0x53; // virtual-key code for the "s" key
						ip.ki.dwFlags = 0; // 0 for key press
						SendInput(1, &ip, sizeof(INPUT));

						// Release the "S" key
						ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
						SendInput(1, &ip, sizeof(INPUT));
						*/
					}
				}
			}
			break;
	}
}

BYTE* CMediaStream::GetVideoBuffer(BOOL bReleaseBusy)
{
	HRESULT hr = S_OK;
	BYTE *pBuffer = m_pBuffer;
//	do
//	{
	if (m_iWidth == 0 || !m_pMixerEffect || (m_pMixerEffect->GetEffectID() == 0
		&& !m_pMixerEffect->IsZoomed()))
		return pBuffer; //break;

		EnterCriticalSection(&m_csInUse);

//		while (m_bBusyFlag == 2)
	//		Sleep(1);

		m_bBusyFlag = 1;

		pBuffer = m_pMixerEffect->GetImageBuffer();

		if (bReleaseBusy)
			m_bBusyFlag = 0;

		LeaveCriticalSection(&m_csInUse);

//	} while (false);

	return pBuffer;
} 

#ifdef _ENABLE_DISPLAY_MANAGER

BOOL CMediaStream::SetDisplayManager(CShapeDispManager* pObj, INT32 iPannelIdx)
{
	BOOL bHadExist = FALSE;
	if (pObj)
	{
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iPannelIdx)
			{
				return TRUE;
			}
		}

		pObj->SetPannelIdx(iPannelIdx);
		if(GPUIsEnabled())
			pObj->EnableGPU(true);
		int iSurfaceIdx = pObj->CreateSurfaceItem();
//		m_iSurfaceItemIndex = iSurfaceIdx;
		m_iDisplayMappingIndex[iPannelIdx] = iSurfaceIdx;
		m_iSurfaceMappingAry.Add(iSurfaceIdx);
		m_pDisplayManagerAry.Add(pObj);
	}

	return bHadExist;
}

void CMediaStream::ReleaseDispManagerAry()
{
	m_pDisplayManagerAry.RemoveAll();
}

IDirect3DSurface9* CMediaStream::GetSurface(int iPannelIdx)
{
	CShapeDispManager *pDispManager = NULL;
	CMixerEffect *pMixerEffect = NULL;
	int iSurfaceIdx;

	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iPannelIdx)
		{
			pDispManager = m_pDisplayManagerAry.GetAt(x);
			if(m_pMixerEffectAry.GetCount() > x)
				pMixerEffect = m_pMixerEffectAry.GetAt(x);
			iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			break;
		}
	}

#ifdef _ENABLE_GPU
	iSurfaceIdx = m_iDisplayMappingIndex[iPannelIdx];
#endif

	if (pDispManager)
	{
//		if (pDispManager->IsSuspend() || m_iFrameCount == 0)
//			return 0;
		/*
		if (m_bLoseVideo)
		{
			m_bLoseVideo = false;
			for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
			{
				DrawPannel(x, m_pYUVBuffer, m_iWidth, m_iHeight, false);
			}
		}
		*/
		D3DSurfaceItem* pD3DSurfaceItem = pDispManager->GetSurface(iSurfaceIdx);
		
		//pD3DSurfaceItem->pOverlaySurface
		sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
		if (pMixerEffect)
		{
			pMixerEffect->SetEffectID(m_pMixerEffect->GetEffectID());
			pMixerEffect->SetEffectParam(effect_param);

			pDispManager->GetMixerEffect()->SetEffectID(m_pMixerEffect->GetEffectID());
			pDispManager->GetMixerEffect()->SetEffectParam(effect_param);

#ifdef _ENABLE_HD_EFFECT
			pDispManager->SetEffectIndex(m_pMixerEffect->GetEffectID());
			pDispManager->SetEffectParam(effect_param.iLevel, effect_param.iParm1, effect_param.iParm2, effect_param.iParm3, effect_param.iParm4, effect_param.iParm5);
#endif
		}

#ifdef _ENABLE_SINGLE_EFFECT
		if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && pDispManager->EffectIsEnabled())
#else
		if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 )
#endif
		{
//			if (!CheckDisplayManagerStatus(RS_NORMAL))
	//			return 0;


			if (!pMixerEffect)
				return 0;

#ifdef _ENABLE_HD_EFFECT
			/*
			pDispManager->Suspend();
			pDispManager->DoMixerEffect_SurfaceHD(pMixerEffect, pD3DSurfaceItem->pD3DSurface, m_iWidth, m_iHeight, iSurfaceIdx);
			pDispManager->Resume();
			*/
#endif

			/*
			pDispManager->Suspend();
#ifdef _ENABLE_GPU
			pDispManager->DoMixerEffect_Surface(pMixerEffect, pD3DSurfaceItem->pOverlaySurface, m_iWidth, m_iHeight, iSurfaceIdx);
#else
			//pDispManager->DoMixerEffect(pMixerEffect, m_pYUVBuffer, m_iWidth, m_iHeight, iSurfaceIdx);
			pDispManager->DoMixerEffect_Surface(pMixerEffect, pD3DSurfaceItem->pOverlaySurface, m_iWidth, m_iHeight, iSurfaceIdx);
#endif
			pDispManager->Resume();
			*/
		}
		else
		{
			if (m_bDoClear)
			{
				m_bDoClear = false;

				if (m_pYUVBuffer)
					pDispManager->RenderFrame(iSurfaceIdx, VPF_I420, m_pYUVBuffer, m_iWidth, m_iHeight);
			}
			else if (m_pMFilePlayerDll->GetPlaybackMode(0) == MFPM_PAUSE)
			{
				if (m_pYUVBuffer)
					pDispManager->RenderFrame(iSurfaceIdx, VPF_I420, m_pYUVBuffer, m_iWidth, m_iHeight);
			}
		}

		if(pD3DSurfaceItem)
			return pD3DSurfaceItem->pOverlaySurface;
	}
	return 0;
}
#endif

int CMediaStream::CreateMixEffect(IDirect3DDevice9* pD3DDev)
{
	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();
	else
		m_pMixerEffect = new CMixerEffect();

	return m_pMixerEffect->CreateSurfaces(pD3DDev, m_iWidth, m_iHeight);
}

int CMediaStream::CreateDispMixer(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h)
{
	CMixerEffect *pMixerEffect = new CMixerEffect();
	pMixerEffect->Create(pD3DDev, foramt, w, h);
	m_pMixerEffectAry.Add(pMixerEffect);
	return 0;
}

int CMediaStream::CreateDispMixer(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h, INT32 iPannelIdx)
{
	bool bFound = false;

	for (int i = 0; i < _MAX_DISPLAY_INDEX; i++)
	{
		if (m_iMixerEffectPannelIdx[i] == iPannelIdx)
		{
			bFound = true;
			return i;
		}
	}

	if (!bFound)
	{
		CMixerEffect *pMixerEffect = new CMixerEffect();
		pMixerEffect->Create(pD3DDev, foramt, w, h);
		m_pMixerEffectAry.Add(pMixerEffect);

		int iIndex = m_pMixerEffectAry.GetCount() - 1;
		m_iMixerEffectPannelIdx[iIndex] = iPannelIdx;
		return iIndex;
	}
}

int CMediaStream::CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt,int w,int h)
{
	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();
	else
		m_pMixerEffect = new CMixerEffect();

	m_pMixerEffect->Create(pD3DDev, foramt, w, h);
	return 0;
}

void CMediaStream::SetSrcArea(CRect rectSrc) 
{ 
//	if (m_pMixerEffect) m_pMixerEffect->SetSrcArea(rectSrc);

	for (int x = 0; x < m_pMixerEffectAry.GetCount(); x++)
		m_pMixerEffectAry.GetAt(x)->SetSrcArea(rectSrc);
}

void CMediaStream::SetDestArea(CRect rectDest) 
{ 
//	if (m_pMixerEffect) m_pMixerEffect->SetDestArea(rectDest);

	for (int x = 0; x < m_pMixerEffectAry.GetCount(); x++)
		m_pMixerEffectAry.GetAt(x)->SetDestArea(rectDest);
}

bool CMediaStream::IsFFFilter()
{
	bool bResult = false;
	int iEffectIndex = m_pMixerEffect->GetEffectID();
	if (strcmp(g_effect_param_items[iEffectIndex].szEffectName, "Negative") == 0)
	{		
		SetNegative(m_pMixerEffect->GetEffectParam(0) > 50);
		bResult = true;
	}
	else if (strcmp(g_effect_param_items[iEffectIndex].szEffectName, "Gray") == 0)
	{
		SetGray((m_pMixerEffect->GetEffectParam(0) -50)* 2);
		bResult = true;
	}
	else if (strcmp(g_effect_param_items[iEffectIndex].szEffectName, "RGB Color") == 0)
	{
		SetR((m_pMixerEffect->GetEffectParam(1) - 50) * 2);
		SetG((m_pMixerEffect->GetEffectParam(2) - 50) * 2);
		SetB((m_pMixerEffect->GetEffectParam(3) - 50) * 2);
		bResult = true;
	}

	return bResult;
}

int CMediaStream::GetTotalOfShapeDisplayManagers()
{
	return m_pDisplayManagerAry.GetCount();
}

CMixerEffect* CMediaStream::GetShapeMixerEffect(int iIndex)
{
	CMixerEffect *pMixerEffect = NULL;

	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iIndex)
		{
			if (m_pMixerEffectAry.GetCount() > x)
			{
				pMixerEffect = m_pMixerEffectAry.GetAt(x);
				break;
			}
		}
	}
	return pMixerEffect;
}

void CMediaStream::Clear()
{
	if (m_iWidth > 0 && m_iHeight > 0)
	{
		int iBufLen = m_iWidth * m_iHeight * 3 / 2;
		if (m_pYUVBuffer)
		{
			memset(m_pYUVBuffer, 16, m_iWidth * m_iHeight);
			memset(m_pYUVBuffer + m_iWidth * m_iHeight, 128, iBufLen - m_iWidth * m_iHeight);
		}

		m_bDoClear = true;
	}


	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		g_PannelAry[i]->EnableDoClear(true);
	}

#ifdef _ENABLE_VIDEO_WALL
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		DrawPannel(x, m_pYUVBuffer, m_iWidth, m_iHeight, false);

		if (m_iSurfaceMappingAry.GetCount() > x)
		{
			int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			if (iSurfaceIdx != -1)
			{
				DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
				D3DSurfaceItem* pD3DSurfaceItem = pCurDisplayManager->GetSurface(iSurfaceIdx);
				g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->DisplayVideo_VW(pD3DSurfaceItem->pOverlaySurface, pD3DSurfaceItem->iWidth, pD3DSurfaceItem->iHeight);
			}
		}
	}
#endif
}

bool CMediaStream::IsAudioFile()
{
	if (IsRTSPUrl(m_szFileName))
	{
	//	if (m_pRTSPPlayerDll && m_pRTSPObj)
		//	return m_pRTSPPlayerDll->_In_RTSP_IsOpened(m_pRTSPObj);
		return false;
	}
	else
	{
		if (m_pMFilePlayerDll)
			return m_pMFilePlayerDll->IsAudioFile(0);
	}
	return false;
}

void CMediaStream::SetMarkMode(int iMode)
{
#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		m_pDisplayManagerAry[x]->SetMarkMode(iMode);
	}
#endif
}

void CMediaStream::DisplayPanelVideo(bool bDoPresent)
{
#if 0
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		//		if (m_pDisplayManagerAry.GetCount() > i)
		{

			int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			if (iSurfaceIdx != -1)
			{
				DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);

//				pCurDisplayManager->UpdateFrameCount(m_iFrameCount);

				D3DSurfaceItem* pD3DSurfaceItem = pCurDisplayManager->GetSurface(iSurfaceIdx);
				g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->DisplayVideo_VW(pD3DSurfaceItem->pOverlaySurface, pD3DSurfaceItem->iWidth, pD3DSurfaceItem->iHeight, bDoPresent);
			}
		}
	}
#else
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	//for (int x = m_pDisplayManagerAry.GetCount() - 1; x >= 0; x--)
	{
		//		if (m_pDisplayManagerAry.GetCount() > i)
		{

			if (m_iSurfaceMappingAry.GetCount() > m_iDisplayOrder[x])
			{
				int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(m_iDisplayOrder[x]);
				if (iSurfaceIdx != -1)
				{
					DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(m_iDisplayOrder[x]);

					//				pCurDisplayManager->UpdateFrameCount(m_iFrameCount);

					D3DSurfaceItem* pD3DSurfaceItem = pCurDisplayManager->GetSurface(iSurfaceIdx);
					if (m_pDisplayManagerAry[m_iDisplayOrder[x]]->GetPannelIdx() != 0)
						g_PannelAry[m_pDisplayManagerAry[m_iDisplayOrder[x]]->GetPannelIdx()]->DisplayVideo_VW(pD3DSurfaceItem->pOverlaySurface, pD3DSurfaceItem->iWidth, pD3DSurfaceItem->iHeight, bDoPresent);
				}
			}
		}
	}
#endif
}

void CMediaStream::Lock()
{
	m_bLocked = true;

	if (m_bPlayerWaitForDisplay)
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableSuspendDisplay(0, m_bLocked);
	}
}

void CMediaStream::Unlock()
{
	m_bLocked = false;

	if (m_bPlayerWaitForDisplay)
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->EnableSuspendDisplay(0, m_bLocked);
	}

	if (m_bLoseVideo)
	{
		m_bLoseVideo = false;
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			DrawPannel(x, m_pYUVBuffer, m_iWidth, m_iHeight, false);
		}
	}
}

bool CMediaStream::CheckFrameCount()
{
	int iCurFrameCount = 0;
	int iCount = 0;

	//g_PannelAry[1]->GetLastFrameCount();

	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		if (x == 0)
			iCurFrameCount = m_pDisplayManagerAry[x]->GetFrameCount();

		if (iCurFrameCount == m_pDisplayManagerAry[x]->GetFrameCount() && g_PannelAry[1]->GetLastFrameCount() < m_pDisplayManagerAry[x]->GetFrameCount())
			iCount++;
	}
	if (iCount == m_pDisplayManagerAry.GetCount())
		return true;
	return false;
}

void CMediaStream::WaitForFrameSyn()
{
	bool bDo = true;
	while (bDo)
	{
		if (CheckFrameCount())
			bDo = false;
		MSG Msg;
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Sleep(1);
	}
}

void CMediaStream::SetDisplayOrder(int iIndex,int iValue)
{
	m_iDisplayOrder[iIndex] = iValue;
}

void CMediaStream::EnablePlayerWaitForDisplay(bool bEnable)
{
	m_bPlayerWaitForDisplay = bEnable;
}

void CMediaStream::SetAudioDelay(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetAudioDelay(0, iValue);
}

void CMediaStream::SetReSampleRateOffset(float fValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetReSampleRateOffset(0, fValue);
}

void CMediaStream::UpdateProcessFPS()
{
	float fCurFPS = 30.0;
	
#if 1
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
		if (iSurfaceIdx != -1)
		{
			if (g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->GetAvgPresentFPS() < fCurFPS && g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->GetAvgPresentFPS() > 0)
				fCurFPS = g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->GetAvgPresentFPS();
		}
	}
#else
	float fPanelFPS = 0.0;
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
		if (pCurDisplayManager)
		{
			int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			if (iSurfaceIdx != -1)
			{
				fPanelFPS = (pCurDisplayManager->GetAvgFPS(iSurfaceIdx) + g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->GetAvgPresentFPS() ) / 2;
				if (fPanelFPS < fCurFPS && fPanelFPS > 0 && g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->GetAvgPresentFPS() > 0 && pCurDisplayManager->GetAvgFPS(iSurfaceIdx) > 0)
					fCurFPS = fPanelFPS;
			}
		}
	}
#endif

//	if(m_AvgPresentFPS.GetAverage() > 0 && m_AvgPresentFPS.GetCount() > 10)
	//	fCurFPS = m_AvgPresentFPS.GetAverage();

//	if (m_pMFilePlayerDll)
	//	m_pMFilePlayerDll->SetProcessFPS(0, fCurFPS);
	/*
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);
		if (pCurDisplayManager)
		{
			int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			if (iSurfaceIdx != -1)
			{
				if (pCurDisplayManager->GetAvgFPS(iSurfaceIdx) < fCurFPS && pCurDisplayManager->GetAvgFPS(iSurfaceIdx) > 0)
					fCurFPS = pCurDisplayManager->GetAvgFPS(iSurfaceIdx);
			}
		}
	}
	*/
#ifdef _ENABLE_GPU
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetProcessFPS(0, fCurFPS + 1.8);

	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		if (m_pMFilePlayerDll)
			m_pMFilePlayerDll->SetProcessFPS(1 + i, fCurFPS + 1.8);
	}
#else
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetProcessFPS(0, fCurFPS + 1.8);
#endif
}

bool CMediaStream::CheckDisplayManagerStatus(int iStatus)
{
	//if (m_pDisplayManager->GetRenderStatus() != RS_NORMAL)

	int iCount = 0;
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		CShapeDispManager* pCurDisplayManager = (CShapeDispManager*)m_pDisplayManagerAry.GetAt(x);
		if (pCurDisplayManager)
		{
			if (pCurDisplayManager->GetRenderStatus() == iStatus)
				iCount++;
		}
	}

	if (iCount == m_pDisplayManagerAry.GetCount())
		return true;
	return false;
}

int CMediaStream::GetSurfaceIndex(int iPanelIdx)
{
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{
		if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iPanelIdx)
			return m_iSurfaceMappingAry.GetAt(x);
	}
		
	return -1;
}

void CMediaStream::SetSurfaceItemIndex(int iIndex)
{
	m_iSurfaceItemIndex = iIndex;
}

/*
void CMediaStream::CalPresentFPS()
{
	char szMsg[256];
	DWORD dwCurTime = timeGetTime();

	if (m_dwPresentFPSLastTime == 0)
		m_dwPresentFPSLastTime = dwCurTime;

	if (m_iPresentCount >= 30)
	{
		DWORD dwSpendTime = dwCurTime - m_dwPresentFPSLastTime;

		m_fPresentFPS = 30.0 / ((float)dwSpendTime / 1000.0);

		m_AvgPresentFPS.Calculate(m_fPresentFPS);

		//		theApp.m_fPresentFPS = m_AvgPresentFPS.GetAverage();

		sprintf(szMsg, "#MS# CMediaStream::CalPresentFPS(): %3.2F (%d)\n", m_AvgPresentFPS.GetAverage(), dwSpendTime);
		OutputDebugStringA(szMsg);

		m_iPresentCount = 0;
		m_dwPresentFPSLastTime = dwCurTime;
	}
	else
		m_iPresentCount++;
}
*/
