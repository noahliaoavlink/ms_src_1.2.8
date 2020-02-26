#include "stdafx.h"
#include "AVFilePlayer.h"
#include "../../Include/SCreateWin.h"
#include "../../Include/RenderCommon.h"
#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "rpcrt4.lib")

//#define _DISABLE_LOG 1

#define _LOW_MEM_MODE 1

#define _AUDIO_BUFFER_SIZE 102400
#define _BUFFER_NUM 64//128
#define _RAW_BUFFER_NUM 3


//#define _AUDIO_ONLY 1

//#define _GPU_COPY_BACK 1

/*
extern void gUpdatePosInfo(char* szCurPos,char* szTotalLen);
extern void gSetSliderCtrlRange(int iPos);
extern void gSetSliderCtrlPos(int iPos);
*/

LRESULT WINAPI _DisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
DWORD WINAPI VideoThreadLoop(LPVOID lpParam);
DWORD WINAPI AudioThreadLoop(LPVOID lpParam);
DWORD WINAPI ReOpenThread(LPVOID lpParam);
DWORD WINAPI TimestampThread(LPVOID lpParam);

AVFilePlayer::AVFilePlayer()
{
	m_hDisplayWnd = 0;
	m_iCodecID = 0;
	//m_pFFAVCodecDLL = new FFAVCodecDLL;

#ifdef _ENABLE_MEDIA_FILE_CTRL
	m_bEnableGPU = false;
	m_pMediaFileCtrl = new MediaFileCtrl;
#else
	m_pFFMediaFileDll = new FFMediaFileDll;
#endif

#ifdef _ENABLE_XAUDIO2
	m_pOutXAudio2Dll = new OutXAudio2Dll;
#else
	m_pOutAudioDll = new OutAudioDll;
#endif
//	m_pOutVideoDll = new OutVideoDll;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll = new FFVideoFilterDll;
#else
#endif

	m_bIsOpened = false;
	m_framebuf.pBuffer = 0;
	m_audioframebuf.pBuffer = 0;

	m_pAudioDeBuffer = 0; 
	m_pAudioDeBuffer2 = 0;

	m_bEnableUpdatePos = true;
	m_bUpdatePos = true;
	m_iVolume = 40;
	m_pVolumeCtrl = new VolumeCtrl;

	m_pVideoFrameBuffer = 0;// new FrameBufferManager;
	m_pAudioFrameBuffer = 0;//new FrameBufferManager;
	m_pRawVideoFrame = 0;
	m_pRawAudioFrame = 0;
	m_bIsSuspend = false;
	m_bIsRepeated = false;
	m_bEnableFadeIn = false;
	m_bEnableFadeOut = false;
	m_pCurVideoCodecContext = 0;

	m_hAudioThread = 0;
	m_hVideoThread = 0;
	m_hTimestampThread = 0;

	m_iOutputVideoFormat = VPF_I420;
	m_pConvVideoBuf = 0;
	m_bEnableAudio = true;
	m_bIsAudioFile = false;

	m_dLastPos = 0;
	m_iGOPCount = 0;
	m_iGOP = 0;

	//m_iSpeed = 1;
	m_fSpeed = 1.0;
	m_iPlaybackMode = MFPM_STOP;// MFPM_FORWARD;
	m_bDoBackwardSeek = false;
	m_pRawVideoFrame2 = new RawVideoFrame;
	m_pRawVideoFrame2->pBuffer = 0;
	m_bDoRetry = false;
	m_bDoRetryPushRaw = false;

	m_iBrightness = 0;
	m_iContrast = 0;
	m_iSaturation = 0;
	m_iHue = 0;
	m_iR = 0;
	m_iG = 0;
	m_iB = 0;
	m_iGray = 0;
	m_iNegative = 0;

	m_iCoded_Height = 0;
	m_iCoded_Width = 0;

	m_pCurVideoFrame = 0;
	m_bBeginOfFile = false;
	m_bSuspendDisplay = false;
//	m_bFinished = true;
	m_bOpenIsFinished = true;
	m_bSeekIsFinished = true;

	m_bEOF = false;
	m_bEOF2 = false;

	m_pRawVideoBuf = 0;
	m_pNV12RawVideoBuf = 0;

	m_pD3DDevice = 0;
	//m_ulRawLastTimestamp = 0;
	m_bDoGetNext = true;

	//m_iWaitCount = 100;
	m_ulRawAudioLen = 0;

	m_fScaleW = 1.0;
	m_fScaleH = 1.0;

	m_iChannels = 0;
	m_iSampleRate = 0;
	m_iBits = 0;
	m_iChannelLayout = 0;
	m_iNBSamples = 0;
	m_bRefTimeUpdate = false;

	m_ulLastTime = 0;
	m_ulLastAudioTimestamp = 0;
	m_ulTimestamp = 0;
	m_iDisplayIntervalMode = DIM_NORMAL;
	m_dStartTime = 0.0;

	m_ulLastAudioTime = 0;
	m_dReferenceTime = 0;

	m_iGlobalTimeCodeStatus = GTCS_NORMAL;

#ifdef _ENABLE_AVQUEUE2
	m_pAVPacketQueue2 = new AVPacketQueue2;
#else
	m_pAVPacketQueue = new AVPacketQueue;
#endif

	m_bEnableReSampleRate = true;

	m_bDoSkip = false;
	m_dSeekPos = 0.0;

	m_iAudioDelay = 0;
	m_fReSampleRateOffset = 0.0;

	m_AvgFPS.SetMax(200);
	m_RealAvgFPS.SetMax(1000);
	m_AvgReSampleRate.SetMax(10);
	Reset();

	wchar_t wszEventName[256];
	UUID uuid;

	UuidCreate(&uuid);
	wsprintf(wszEventName,L"thread_event_1_%d", uuid.Data1);
	m_hEvent1 = ::CreateEvent(NULL, TRUE, TRUE, wszEventName);

	UuidCreate(&uuid);
	wsprintf(wszEventName, L"thread_event_2_%d", uuid.Data1);
	m_hEvent2 = ::CreateEvent(NULL, TRUE, TRUE, wszEventName);

	UuidCreate(&uuid);
	wsprintf(wszEventName, L"thread_event_3_%d", uuid.Data1);
	m_hEvent3 = ::CreateEvent(NULL, TRUE, TRUE, wszEventName);

	::SetEvent(m_hEvent1);
	::SetEvent(m_hEvent2);
	::SetEvent(m_hEvent3);

	InitializeCriticalSection(&m_VideoCriticalSection);
	InitializeCriticalSection(&m_AudioCriticalSection);
	InitializeCriticalSection(&m_TimestampCriticalSection);
	InitializeCriticalSection(&m_SeekCriticalSection);
}

AVFilePlayer::~AVFilePlayer()
{
	char szMsg[512];
//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 0\n");
//	OutputDebugStringA(szMsg);

//	Stop();

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 1\n");
//	OutputDebugStringA(szMsg);

	//if(m_pFFAVCodecDLL)
		//delete m_pFFAVCodecDLL;

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 2\n");
//	OutputDebugStringA(szMsg);


#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
	{
		m_pOutXAudio2Dll->Close();
		delete m_pOutXAudio2Dll;
	}
#else
	if(m_pOutAudioDll)
	{
		//if(m_pOutAudioDll->IsReady())
		//{
			//m_pOutAudioDll->Stop();
			m_pOutAudioDll->Close();
		//}

		delete m_pOutAudioDll;
	}
#endif

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 3\n");
//	OutputDebugStringA(szMsg);

//	if(m_pOutVideoDll)
//		delete m_pOutVideoDll;

#ifdef _ENABLE_VIDEO_FILTER
	if(m_pFFVideoFilterDll)
		delete m_pFFVideoFilterDll;
#else
#endif

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 4\n");
//	OutputDebugStringA(szMsg);

	ReleaseBuffers();

#ifdef _ENABLE_MEDIA_FILE_CTRL
	if(m_pMediaFileCtrl)
		delete m_pMediaFileCtrl;
#else
	if (m_pFFMediaFileDll)
		delete m_pFFMediaFileDll;
#endif

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 5\n");
//	OutputDebugStringA(szMsg);

	if(m_hDisplayWnd)
	{
		::SetParent(m_hDisplayWnd,NULL);
		PostMessage(m_hDisplayWnd,WM_CLOSE,0,0);
	}

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 6\n");
//	OutputDebugStringA(szMsg);

	m_hDisplayWnd = 0;
	delete m_pVolumeCtrl;

	if (m_pConvVideoBuf)
		delete m_pConvVideoBuf;

//	sprintf(szMsg,"AVFilePlayer::~AVFilePlayer() 7\n");
//	OutputDebugStringA(szMsg);

	DeleteCriticalSection(&m_VideoCriticalSection);
	DeleteCriticalSection(&m_AudioCriticalSection);
	DeleteCriticalSection(&m_TimestampCriticalSection);
	DeleteCriticalSection(&m_SeekCriticalSection);

	::CloseHandle(m_hEvent1);
	::CloseHandle(m_hEvent2);
	::CloseHandle(m_hEvent3);

#ifdef _ENABLE_AVQUEUE2
	delete m_pAVPacketQueue2;
#else
	delete m_pAVPacketQueue;
#endif
}

void AVFilePlayer::Init(HWND hWnd,FilePlayerCallback* pFilePlayerCallback)
{
	RECT rect;
	char szWinName[256];

	m_pFilePlayerCallback = pFilePlayerCallback;

	if(!m_pFilePlayerCallback)
	{
		sprintf(szWinName,"MFPlayerWnd-%d",GetTickCount());
		m_hDisplayWnd = CreateWin(_DisplayWinProc,szWinName);

		SetParent(m_hDisplayWnd,hWnd);
		GetClientRect(hWnd,&rect);

		SetWinBKColor(m_hDisplayWnd,RGB(0,0,0));
		SetWinSize(m_hDisplayWnd,rect.right,rect.bottom,false);
		SetWinPos(m_hDisplayWnd,0,0,true);
/*
		//out video
		bool bLoadOutVideoRet = m_pOutVideoDll->LoadLib();
		if(bLoadOutVideoRet)
			m_pOutVideoDll->Init(m_hDisplayWnd);
		else
			MessageBoxA(NULL,"Load OutVideo.dll failed!!","Error",MB_OK);
			*/
	}

	//decoder
	/*
	bool bInitDecoderRet = m_pFFAVCodecDLL->Init();
	if(bInitDecoderRet)
		m_iCodecID = m_pFFAVCodecDLL->CreateCodec();
	else
		MessageBoxA(NULL,"Init FFAVCodec.DLL failed!!","Error",MB_OK);
		*/

#ifdef _ENABLE_MEDIA_FILE_CTRL
	//if (m_pMediaFileCtrl)
		//delete m_pMediaFileCtrl;
#else
	bool bLoadDecoderRet = m_pFFMediaFileDll->LoadLib();
	if(!bLoadDecoderRet)
		MessageBoxA(NULL, "Load FFMediaFile.dll failed!!", "Error", MB_OK| MB_TOPMOST);
#endif

#ifdef _ENABLE_VIDEO_FILTER
	//filter
	bool bLoadFilterRet = m_pFFVideoFilterDll->LoadLib();
	if(!bLoadFilterRet)
		MessageBoxA(NULL, "Load FFVideoFilter.dll failed!!", "Error", MB_OK | MB_TOPMOST);
#else
#endif

	m_pVolumeCtrl->Init();

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
	{
		bool bLoadOutAudioRet = m_pOutXAudio2Dll->LoadLib();
		if (bLoadOutAudioRet)
		{
			m_pOutXAudio2Dll->Init();
		}
		else
			MessageBoxA(NULL, "Load OutXAudio2.dll failed!!", "Error", MB_OK | MB_TOPMOST);
	}
#else
	//out audio
	bool bLoadOutAudioRet = m_pOutAudioDll->LoadLib();
	if (bLoadOutAudioRet)
	{
		m_pOutAudioDll->Init(hWnd); //m_hDisplayWnd
		m_pOutAudioDll->SetBufferInfo(1280, 50);
	}
	else
		MessageBoxA(NULL, "Load OutAudio.dll failed!!", "Error", MB_OK);
#endif

}

//failed - 0 , succeed - 1
int AVFilePlayer::Open(char* szFileName)
{
	char szMsg[256];

	if (!m_bOpenIsFinished)
		return -2;

	sprintf(szMsg, "AVFilePlayer::Open() 0 [%s]\n", szFileName);
	OutputDebugStringA(szMsg);

	m_bOpenIsFinished = false;

	strcpy(m_szFileName, szFileName);

#ifdef _ENABLE_MEDIA_FILE_CTRL
	m_bIsOpened = false;
	Stop();
	ReleaseBuffers();

	int iIndex = m_pMediaFileCtrl->SearchFileName(szFileName);
	if (iIndex == -1)
	{
		m_pMediaFileCtrl->CreateNewObj();
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		if (m_bEnableGPU)
		{
			pCurFFMediaFileDll->EnableGPU(m_bEnableGPU);

			if (pCurFFMediaFileDll->GPUIsEnabled())
			{
				pCurFFMediaFileDll->SetupDXVA2(m_pD3DDevice);
			}
		}

		int iFileRet = pCurFFMediaFileDll->Open(szFileName);
		if (iFileRet == 1)
		{
			ResetBuffer();

			m_bIsSuspend = false;
			m_bIsOpened = true;
			m_bOpenIsFinished = true;
			return iFileRet;
	}
		m_bIsOpened = false;
		m_bOpenIsFinished = true;

	}
	else
	{
		int iIndex = m_pMediaFileCtrl->SearchFileName(szFileName);
		m_pMediaFileCtrl->SetTarget(iIndex);

		ResetBuffer();

		m_bIsSuspend = false;
		m_bIsOpened = true;
		m_bOpenIsFinished = true;
		return 1;
	}
#else

	Close();
	//Stop();

	m_pCurAudioCodecContext = 0;

	if (m_pFFMediaFileDll->GPUIsEnabled())
	{
		m_pFFMediaFileDll->SetupDXVA2(m_pD3DDevice);
	}

	//int iFileRet = m_pFFAVCodecDLL->MFR_Open(szFileName);
	int iFileRet = m_pFFMediaFileDll->Open(szFileName);
	if(iFileRet == 1)
	{
		Reset();
		//m_pFFAVCodecDLL->MFR_GetVideoFrameSize(&m_iVideoFrameW,&m_iVideoFrameH);
		//AVCodecContext* pVideoCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_VIDEO);
		m_pFFMediaFileDll->GetVideoFrameSize(&m_iVideoFrameW, &m_iVideoFrameH);
		AVCodecContext* pVideoCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_VIDEO);
		if (pVideoCodecContext)
		{
			m_pCurVideoCodecContext = pVideoCodecContext;
			//m_fVideoFPS = m_pFFAVCodecDLL->MFR_GetVideoFPS();

			m_bIsAudioFile = false;

			//m_fVideoFPS = m_pFFMediaFileDll->GetVideoFPS() * 1.007;
			m_fVideoFPS = m_pFFMediaFileDll->GetVideoFPS();
			m_fVideoInterval = 1000.0 / m_fVideoFPS;

			m_FrameRateCtrl.SetInterval(m_fVideoInterval);

			m_iCoded_Height = pVideoCodecContext->coded_height;
			m_iCoded_Width = pVideoCodecContext->coded_width;

#ifdef _ENABLE_LOADING_CTRL
			m_LoadingCtrl.SetDataFPS(m_fVideoFPS);
#endif
		}
		else
			m_pCurVideoCodecContext = 0;

		if (m_pFFMediaFileDll->GPUIsEnabled())
			m_pSWVideoCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_VIDEO_SW);

		//AVCodecContext* pAudioCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_AUDIO);
		AVCodecContext* pAudioCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_AUDIO);
		if(pAudioCodecContext)
		{
			m_pCurAudioCodecContext = pAudioCodecContext;
			m_iChannels = pAudioCodecContext->channels;
			m_iSampleRate = pAudioCodecContext->sample_rate;
			m_iChannelLayout = pAudioCodecContext->channel_layout;
			m_iNBSamples = pAudioCodecContext->frame_size;

			if(!pVideoCodecContext)
				m_bIsAudioFile = true;

			if(pAudioCodecContext->sample_fmt == AV_SAMPLE_FMT_FLTP)
				m_iBits = 16;
			else
			{
				switch(pAudioCodecContext->sample_fmt)
				{
					case AV_SAMPLE_FMT_U8:
					case AV_SAMPLE_FMT_U8P:
						m_iBits = 8;
						break;
					case AV_SAMPLE_FMT_S16:
					case AV_SAMPLE_FMT_S16P:
						m_iBits = 16;
						break;
			//		case SAMPLE_FMT_S24:
			//			*iBits = 24;
			//			break;
					case AV_SAMPLE_FMT_S32:
					case AV_SAMPLE_FMT_S32P:
						m_iBits = 32;
						break;
					default:
					{
						m_iBits = pAudioCodecContext->bit_rate / 1000;
					}
				}
			}
		}

		//m_pFFAVCodecDLL->SetDePixelFormat(m_iCodecID,AV_PIX_FMT_YUV420P);

		int iBufferLen = 0;
		if(m_iVideoFrameW != -1 && m_iVideoFrameH != -1)
			iBufferLen = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
		else
			iBufferLen = _AUDIO_BUFFER_SIZE;
		m_framebuf.pBuffer = new unsigned char[iBufferLen];
		
//		m_pAudioDeBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
		//m_pAudioDeBuffer2 = new unsigned char[_AUDIO_BUFFER_SIZE * 2];

		if (m_iCoded_Height > 0 && m_iCoded_Width > 0)
		{
			m_pRawVideoBuf = new unsigned char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
			m_pNV12RawVideoBuf = new unsigned char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
		}
		else
		{
			m_pRawVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
			m_pNV12RawVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
		}

		m_pVideoFrameBuffer = new FrameBufferManager;
		//m_pAudioFrameBuffer = new FrameBufferManager;
		m_pRawVideoFrame = new StreamFrame;
		//m_pRawAudioFrame = new StreamFrame;
		m_pRawVideoFrame2 = new RawVideoFrame;

		//m_pRawAudioFrame->pBuffer = new char[_AUDIO_BUFFER_SIZE];
		if (m_iCoded_Height > 0 && m_iCoded_Width > 0)
		{
			m_pRawVideoFrame->pBuffer = new char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
			m_pRawVideoFrame2->pBuffer = new char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
		}
		else
		{
			m_pRawVideoFrame->pBuffer = new char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
			m_pRawVideoFrame2->pBuffer = new char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
		}

		if (m_pCurAudioCodecContext)
		{
			m_pRawAudioFrame = new StreamFrame;
			m_pAudioFrameBuffer = new FrameBufferManager;

			m_pAudioDeBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
			m_pAudioDeBuffer2 = new unsigned char[_AUDIO_BUFFER_SIZE * 2];
			m_pRawAudioFrame->pBuffer = new char[_AUDIO_BUFFER_SIZE];

			m_audioframebuf.pBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
			
			if (!m_pAudioFrameBuffer->IsReady())
				m_pAudioFrameBuffer->Alloc(_AUDIO_BUFFER_SIZE, _BUFFER_NUM*2);
		}

		if (!m_pVideoFrameBuffer->IsReady())
		{
#if _LOW_MEM_MODE
			m_pVideoFrameBuffer->Alloc(iBufferLen + 1000, _RAW_BUFFER_NUM);//32
#else
			m_pVideoFrameBuffer->Alloc(iBufferLen + 1000, _BUFFER_NUM);//32
#endif
		}

		//if(!m_pAudioFrameBuffer->IsReady())
			//m_pAudioFrameBuffer->Alloc(_AUDIO_BUFFER_SIZE, _BUFFER_NUM);

		if (m_pConvVideoBuf)
		{
			delete m_pConvVideoBuf;
			m_pConvVideoBuf = 0;
		}

		switch (m_iOutputVideoFormat)
		{
			case VPF_I420:
				m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3/2];
				break;
			case VPF_YUY2:
				m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 2];
				break;
			case VPF_RGB32:
				m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 4];
				break;
			case VPF_RGB24:
				m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3];
				break;
		}

		//m_ulStartTimestamp = timeGetTime();

		if(m_pFFMediaFileDll->GetAudioLength() > 0)
			m_bDoAVSyn = true;
		else
			m_bDoAVSyn = false;

#ifdef _ENABLE_MS
		UpdatePosInfoMS();
#else
		UpdatePosInfo();
#endif

		if (m_pFilePlayerCallback)
		{
#ifdef _ENABLE_MS
			double dLength = m_pFFMediaFileDll->GetLength();
			m_pFilePlayerCallback->SetSliderCtrlRange(dLength);
#else
			m_pFilePlayerCallback->SetSliderCtrlRange(m_lTotalLen);
#endif
		}
		//gSetSliderCtrlRange(m_lTotalLen);

#ifdef _ENABLE_VIDEO_FILTER
		if (m_pFFMediaFileDll->GPUIsEnabled())
		{
			if (m_pSWVideoCodecContext)
			{
				m_pFFVideoFilterDll->CreateEQFilter(m_pSWVideoCodecContext);
				m_pFFVideoFilterDll->CreateHueFilter(m_pSWVideoCodecContext);
				m_pFFVideoFilterDll->CreateLutYUVFilter(m_pSWVideoCodecContext);
				m_pFFVideoFilterDll->CreateLutRGBFilter(m_pSWVideoCodecContext);
			}
		}
		else
		{
			if (m_pCurVideoCodecContext)
			{
				m_pFFVideoFilterDll->CreateEQFilter(m_pCurVideoCodecContext);
				m_pFFVideoFilterDll->CreateHueFilter(m_pCurVideoCodecContext);
				m_pFFVideoFilterDll->CreateLutYUVFilter(m_pCurVideoCodecContext);
				m_pFFVideoFilterDll->CreateLutRGBFilter(m_pCurVideoCodecContext);
			}
		}
#endif
		m_bIsSuspend = false;
		m_bIsOpened = true;
		m_bOpenIsFinished = true;
		return iFileRet;
	}
	m_bIsOpened = false;
	m_bOpenIsFinished = true;

#endif

	return 0;
}

bool AVFilePlayer::IsOpened()
{
	return m_bIsOpened;
}

void AVFilePlayer::Close()
{
	char szMsg[512];
	/*
#ifdef _ENABLE_MEDIA_FILE_CTRL
	Stop();
#else
*/
	if (!m_bIsOpened)
		return;

	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	//DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);
	DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);  //1000

	sprintf(szMsg,"AVFilePlayer::Close() 0\n");
	OutputDebugStringA(szMsg);

	m_iPlaybackMode = MFPM_STOP;
	m_bEOF2 = false;

	Stop();

//	Reset();
	ReleaseBuffers();

	//m_pFFAVCodecDLL->MFR_Close();
#ifdef _ENABLE_MEDIA_FILE_CTRL
	m_pMediaFileCtrl->Close();
#else
	m_pFFMediaFileDll->Close();
#endif

//	sprintf(szMsg,"AVFilePlayer::Close() 1\n");
//	OutputDebugStringA(szMsg);

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
		m_pOutXAudio2Dll->Close();
#else
	if(m_pOutAudioDll)
		m_pOutAudioDll->Close();	
#endif

//	sprintf(szMsg,"AVFilePlayer::Close() 2\n");
//	OutputDebugStringA(szMsg);

	m_bIsOpened = false;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->CloseFilter();
#endif

//#endif
}

void AVFilePlayer::Play()
{
	if (!m_bIsOpened)
		return;

	if (m_iPlaybackMode == MFPM_BACKWARD)
	{
		m_iPlaybackMode = MFPM_FORWARD;
		Seek2(m_dLastPos - 1);
	}
	else if (m_iPlaybackMode == MFPM_PAUSE)
	{
		m_iPlaybackMode = MFPM_FORWARD;

		m_dwLastTime = 0;
		m_ulLastTime = 0;
		m_ulVideoFrameCount = 0;

#ifdef _ENABLE_XAUDIO2
		if (m_pOutXAudio2Dll)
			m_pOutXAudio2Dll->Continue();
#else
		if (m_pCurAudioCodecContext)
			m_pOutAudioDll->Continue();
#endif
	}
	else
	{
		ThreadBase::Start();
		//Seek(1);

#ifdef _ENABLE_XAUDIO2
		if (m_pOutXAudio2Dll)
			m_pOutXAudio2Dll->Continue();
#endif

		m_iPlaybackMode = MFPM_FORWARD;

		DWORD nThreadID;
		m_hVideoThread = CreateThread(0, 0, VideoThreadLoop, (void *)this, 0, &nThreadID);

		m_hAudioThread = 0;
		if(m_pCurAudioCodecContext)
			m_hAudioThread = CreateThread(0, 0, AudioThreadLoop, (void *)this, 0, &nThreadID);

		m_hTimestampThread = CreateThread(0, 0, TimestampThread, (void *)this, 0, &nThreadID);
	}
	m_bSuspendDisplay = false;
}

void AVFilePlayer::PlayBackward()
{
	m_bDoBackwardSeek = true;
	m_iPlaybackMode = MFPM_BACKWARD;

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dCurPos = pCurFFMediaFileDll->GetCurVideoFramePos();
	pCurFFMediaFileDll->SetLastBackwardPos(dCurPos);
	pCurFFMediaFileDll->SeekBackward();
#else
	double dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
	m_pFFMediaFileDll->SetLastBackwardPos(dCurPos);
	m_pFFMediaFileDll->SeekBackward();
#endif

	Reset();
}

void AVFilePlayer::Pause()
{
	char szMsg[512];
	if (m_iPlaybackMode == MFPM_FORWARD || m_iPlaybackMode == MFPM_BACKWARD)
	{
		sprintf(szMsg,"AVFilePlayer::Pause() 0\n");
		OutputDebugStringA(szMsg);

		m_iPlaybackMode = MFPM_PAUSE;
		m_bSuspendDisplay = true;

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
		m_pOutXAudio2Dll->Pause();
#else
		if (m_pCurAudioCodecContext)
			m_pOutAudioDll->Stop();
#endif
	}
}

void AVFilePlayer::Stop()
{
	char szMsg[512];
	sprintf(szMsg,"AVFilePlayer::Stop() 0\n");
	OutputDebugStringA(szMsg);
/*
	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	WaitForMultipleObjects(2, handles, true, INFINITE);
	*/
	m_iPlaybackMode = MFPM_STOP;

	m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
	m_dwLastTime = 0;
	m_ulTimestamp = 0;

	if (!m_bIsOpened)
		return;

	sprintf(szMsg,"AVFilePlayer::Stop() 1\n");
	OutputDebugStringA(szMsg);

	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	//DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);
	//DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);  //1000
	DWORD dwRet = WaitForMultipleObjects(2, handles, true, 200);  //1000

	m_bSuspendDisplay = true;

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
		m_pOutXAudio2Dll->Stop();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Stop();
#endif

	sprintf(szMsg, "AVFilePlayer::Stop() 2\n");
	OutputDebugStringA(szMsg);

	ThreadBase::Stop();

	if (m_hVideoThread)
		::WaitForSingleObject(m_hVideoThread, 3000);

	if (m_hAudioThread)
		::WaitForSingleObject(m_hAudioThread, 3000);

	if (m_hTimestampThread)
		::WaitForSingleObject(m_hTimestampThread, 3000);

//	Close();
	Reset();

//	ReleaseBuffers();

	sprintf(szMsg,"AVFilePlayer::Stop() 3\n");
	OutputDebugStringA(szMsg);

	m_bSuspendDisplay = false;
}

#if 0
void AVFilePlayer::Stop2()
{
	char szMsg[512];
	sprintf(szMsg, "AVFilePlayer::Stop2() 0\n");
	OutputDebugStringA(szMsg);
	/*
	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	WaitForMultipleObjects(2, handles, true, INFINITE);
	*/
	m_iPlaybackMode = MFPM_STOP;

	m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
	m_dwLastTime = 0;
	m_ulTimestamp = 0;

	if (!m_bIsOpened)
		return;

	sprintf(szMsg, "AVFilePlayer::Stop2() 1\n");
	OutputDebugStringA(szMsg);
/*
	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	//DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);
	DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);  //1000
	*/
//	m_bSuspendDisplay = true;

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
		m_pOutXAudio2Dll->Stop();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Stop();
#endif

	sprintf(szMsg, "AVFilePlayer::Stop2() 2\n");
	OutputDebugStringA(szMsg);

	ThreadBase::Stop();

	if (m_hVideoThread)
		::WaitForSingleObject(m_hVideoThread, 100);

	if (m_hAudioThread)
		::WaitForSingleObject(m_hAudioThread, 100);

	if (m_hTimestampThread)
		::WaitForSingleObject(m_hTimestampThread, 100);

	//	Close();
	Reset();

	//	ReleaseBuffers();

	sprintf(szMsg, "AVFilePlayer::Stop2() 3\n");
	OutputDebugStringA(szMsg);

	m_bSuspendDisplay = false;
}
#endif

void AVFilePlayer::Reset()
{
	m_fVideoInterval = 0.0;
	m_iAudioInterval = 0;
	m_ulVideoFrameCount = 0;
	m_ulAudioFrameCount = 0;
	m_ulLastVideoFrameTime = 0;
	m_ulLastAudioFrameTime = 0;
	m_lLastVideoPos = 0;
	//m_ulTimestamp = 0;
	m_dwLastTime = 0;
//	m_ulAudioTimestamp = 0;
//	m_iAudioTimestampCount = 0;
	m_bDoGetNext = true;

	m_bUpdatePos = true;

	m_bBeginOfFile = false;

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	if (pCurFFMediaFileDll && pCurFFMediaFileDll->GetAudioLength() > 0)
		m_bDoAVSyn = true;
	else
		m_bDoAVSyn = false;
#else
	if (m_pFFMediaFileDll->GetAudioLength() > 0)
		m_bDoAVSyn = true;
	else
		m_bDoAVSyn = false;
#endif

	if(m_pAudioFrameBuffer)
		m_pAudioFrameBuffer->Reset();
	if(m_pVideoFrameBuffer)
		m_pVideoFrameBuffer->Reset();

	//m_pOutAudioDll->Close();

	m_pCurVideoFrame = 0;
	m_pCurAudioFrame = 0;
//	m_iAudioTimestampCount = 0;
	m_ulTimestamp = 0;
//	m_ulAudioTimestamp = 0;
	m_AvgFPS.Reset();
	m_RealAvgFPS.Reset();
	m_AvgReSampleRate.Reset();
	m_ulLastTime = 0;

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
		m_pOutXAudio2Dll->ResetBuffer();
#else
	m_pOutAudioDll->ResetBuffer();
#endif


#ifdef _ENABLE_AVQUEUE2
	m_pAVPacketQueue2->Reset();
#else
	m_pAVPacketQueue->Reset();
#endif

}

void AVFilePlayer::ReleaseBuffers()
{
	if(m_framebuf.pBuffer)
	{
		delete m_framebuf.pBuffer;
		m_framebuf.pBuffer = 0;
	}

	if (m_audioframebuf.pBuffer)
	{
		delete m_audioframebuf.pBuffer;
		m_audioframebuf.pBuffer = 0;
	}
	
	if(m_pAudioDeBuffer)
	{
		delete m_pAudioDeBuffer;
		m_pAudioDeBuffer = 0;
	}

	if (m_pAudioDeBuffer2)
	{
		delete m_pAudioDeBuffer2;
		m_pAudioDeBuffer2 = 0;
	}

	if(m_pVideoFrameBuffer)
	{
		delete m_pVideoFrameBuffer;
		m_pVideoFrameBuffer = 0;
	}
	if(m_pAudioFrameBuffer)
	{
		delete m_pAudioFrameBuffer;
		m_pAudioFrameBuffer = 0;
	}

	if(m_pRawVideoFrame)
	{
		if(m_pRawVideoFrame->pBuffer)
		{
			delete m_pRawVideoFrame->pBuffer;
			m_pRawVideoFrame->pBuffer = 0;
		}

		delete m_pRawVideoFrame;
		m_pRawVideoFrame = 0;
	}

	if(m_pRawAudioFrame)
	{
		if(m_pRawAudioFrame->pBuffer)
		{
			delete m_pRawAudioFrame->pBuffer;
			m_pRawAudioFrame->pBuffer = 0;
		}

		delete m_pRawAudioFrame;
		m_pRawAudioFrame = 0;
	}

	if (m_pRawVideoFrame2)
	{
		if (m_pRawVideoFrame2->pBuffer)
		{
			delete m_pRawVideoFrame2->pBuffer;
			m_pRawVideoFrame2->pBuffer = 0;
		}
		delete m_pRawVideoFrame2;
		m_pRawVideoFrame2 = 0;
	}

	if (m_pRawVideoBuf)
	{
		delete m_pRawVideoBuf;
		m_pRawVideoBuf = 0;
	}

	if (m_pNV12RawVideoBuf)
	{
		delete m_pNV12RawVideoBuf;
		m_pNV12RawVideoBuf = 0;
	}

	if (m_pConvVideoBuf)
	{
		delete m_pConvVideoBuf;
		m_pConvVideoBuf = 0;
	}

#ifdef _ENABLE_AVQUEUE2
	m_pAVPacketQueue2->Reset();
#else
	m_pAVPacketQueue->Reset();
#endif
}

void AVFilePlayer::PlayForwardMode()
{
	AVPacket pkt;
#if _LOW_MEM_MODE
	char szMsg[512];
	bool bIsKeyFrame = false;

	if (!m_bIsSuspend)
	{
		if (m_iPlaybackMode == MFPM_STOP)
			return;


#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();


		int iRet = pCurFFMediaFileDll->GetNextFrame(&pkt, true); //m_pkt
#else
		int iRet = m_pFFMediaFileDll->GetNextFrame(&pkt, true); //m_pkt
#endif
		if (iRet < 0)
		{
			if (iRet == -1)  //eof
			{
				m_bEOF = true;
				int iTotal = m_pVideoFrameBuffer->GetTotal();

				if (!m_pCurVideoCodecContext)
				{
#ifdef _ENABLE_XAUDIO2
					if (m_pAudioFrameBuffer->GetTotal() == 0 && m_bIsRepeated && m_bEOF && m_pOutXAudio2Dll->GetBufferCount() == 0)
#else
					if (m_pAudioFrameBuffer->GetTotal() == 0 && m_pOutAudioDll->GetBufferCount() == 0)
#endif
						DoRepeat();
				}
				else
				{
#ifdef _AUDIO_ONLY
					if (m_pAudioFrameBuffer && m_pAudioFrameBuffer->GetTotal() == 0)
						DoRepeat();
#endif  //_AUDIO_ONLY

#ifdef _ENABLE_AVQUEUE2
					if (m_pAVPacketQueue2->GetCount() == 0 && m_ulVideoFrameCount > 0 && m_bIsRepeated && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#else
					if (m_pAVPacketQueue->GetCount() == 0 && m_ulVideoFrameCount > 0 && m_bIsRepeated && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#endif
						DoRepeat();

#ifdef _ENABLE_AVQUEUE2
					if (m_pAVPacketQueue2->GetCount() == 0 && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#else
					if (m_pAVPacketQueue->GetCount() == 0 && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#endif
						m_bEOF2 = true;
				}
			}

			return;
		}

		if (pkt.flags == AV_PKT_FLAG_KEY)
			bIsKeyFrame = true;

		UpdateGOPCount(bIsKeyFrame);

#ifdef _ENABLE_MEDIA_FILE_CTRL
		if (pCurFFMediaFileDll->IsVideoFrame(&pkt) && !m_bIsAudioFile)
#else
		if (m_pFFMediaFileDll->IsVideoFrame(&pkt) && !m_bIsAudioFile)
#endif
		{

#ifdef _AUDIO_ONLY
#else  //_AUDIO_ONLY


#ifdef _ENABLE_AVQUEUE2
			m_pAVPacketQueue2->Add(pkt);
#else
			m_pAVPacketQueue->Add(pkt);
#endif


#ifdef _ENABLE_AVQUEUE2
			if (m_pAVPacketQueue2->GetCount() >= _BUFFER_NUM - 2)
#else
			if (m_pAVPacketQueue->GetCount() >= _BUFFER_NUM - 2)
#endif
			{
				m_bIsSuspend = true;

				//sprintf(szMsg, "AVFilePlayer::PlayForwardMode() - m_bIsSuspend = true (0)\n");
				//OutputDebugStringA(szMsg);
				//m_pFFMediaFileDll->Suspend();
			}


#endif //_AUDIO_ONLY

		}
#ifdef _ENABLE_MEDIA_FILE_CTRL
		else if (pCurFFMediaFileDll->IsAudioFrame(&pkt) && pkt.size > 0)
#else
		else if (m_pFFMediaFileDll->IsAudioFrame(&pkt) && pkt.size > 0)
#endif
		{

			if (!m_bEnableAudio)
				return;
#ifdef _AUDIO_ONLY
#else  //_AUDIO_ONLY

			//if (m_pAVPacketQueue->GetCount() > 0 || !m_pCurVideoCodecContext)

#endif   //_AUDIO_ONLY
			{

#ifdef _ENABLE_MEDIA_FILE_CTRL
				int iRet = pCurFFMediaFileDll->GetFrameBuffer(&pkt, &m_audioframebuf);
#else
				int iRet = m_pFFMediaFileDll->GetFrameBuffer(&pkt, &m_audioframebuf);
#endif

				/*
				if (m_pCurAudioFrame)
				{
					if (m_pCurAudioFrame && m_audioframebuf.iLen > m_pCurAudioFrame->iLen)
					{
						sprintf(szMsg, "AVFilePlayer::PlayForwardMode() - m_audioframebuf.iLen > m_pCurAudioFrame->iLen [%d , %d]\n", m_audioframebuf.iLen , m_pCurAudioFrame->iLen);
						OutputDebugStringA(szMsg);
					}
				}
				*/
				if (m_audioframebuf.iType != MFT_AUDIO)
				{
					//sprintf(szMsg, "AVFilePlayer::PlayForwardMode() - framebuf.iType != MFT_AUDIO\n");
					//OutputDebugStringA(szMsg);
					return;
				}

				if (iRet >= 0)
				{
					BOOL bRet = IsBadReadPtr(m_audioframebuf.pBuffer, m_audioframebuf.iLen);
					if (bRet)
					{
						sprintf(szMsg, "AVFilePlayer::PlayForwardMode() The 'framebuf.pBuffer' is bad prt!! \n");
						OutputDebugStringA(szMsg);
					}
					else
					{
						if (m_audioframebuf.iLen > _AUDIO_BUFFER_SIZE)
							return;
						m_pRawAudioFrame->ulTimestampSec = m_audioframebuf.lTimestamp;
						memcpy(m_pRawAudioFrame->pBuffer, m_audioframebuf.pBuffer, m_audioframebuf.iLen);
						m_pRawAudioFrame->iLen = m_audioframebuf.iLen;
						m_pAudioFrameBuffer->AddToQueueBuffer(m_pRawAudioFrame);

						if (m_pAudioFrameBuffer->GetTotal() >= m_pAudioFrameBuffer->GetMax() - 2)
						{
							m_bIsSuspend = true;

							//sprintf(szMsg, "AVFilePlayer::PlayForwardMode() - m_bIsSuspend = true (1)\n");
							//OutputDebugStringA(szMsg);
						}
				}
			}
		}
	}
	}
#else
	char szMsg[512];
	if (!m_bIsSuspend)
	{
		//int iRet = m_pFFAVCodecDLL->MFR_GetNextFrameBuffer(&framebuf);
		int iRet = m_pFFMediaFileDll->GetNextFrameBuffer(&m_framebuf);

		if (iRet > 0)
		{
			if (m_framebuf.iType == MFT_VIDEO)
			{
				bool bIsKeyFrame = false;
				int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
				m_pRawVideoFrame->ulTimestampSec = m_framebuf.lTimestamp;
				m_pRawVideoFrame->iLen = iVideoFrameSize;

				AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
				if(pAVFrame->flags == AV_PKT_FLAG_KEY || pAVFrame->key_frame)
					bIsKeyFrame = true;

				//if (pAVFrame->pts < 0)
					//return ;

				//sprintf(szMsg, "PlayForwardMode: Video - pts:%I64d dts:%I64d ts:%d\n", pAVFrame->pkt_pts, pAVFrame->pkt_dts, framebuf.lTimestamp);
				//OutputDebugStringA(szMsg);

				UpdateGOPCount(bIsKeyFrame);

				DoFilter(pAVFrame, m_pRawVideoFrame);

				m_pVideoFrameBuffer->AddToQueueBuffer(m_pRawVideoFrame);

				if (m_pVideoFrameBuffer->GetTotal() >= m_pVideoFrameBuffer->GetMax() - 2)
				{
					m_bIsSuspend = true;
					//m_pFFMediaFileDll->Suspend();
				}
			}
			else if (m_framebuf.iType == MFT_AUDIO)
			{
				if (m_pVideoFrameBuffer->GetTotal() > 0 || m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
				{
					//sprintf(szMsg, "PlayForwardMode: Audio - ts:%d\n", framebuf.lTimestamp);
					//OutputDebugStringA(szMsg);

					m_pRawAudioFrame->ulTimestampSec = m_framebuf.lTimestamp;
					m_pRawAudioFrame->iLen = m_framebuf.iLen;
					memcpy(m_pRawAudioFrame->pBuffer, m_framebuf.pBuffer, m_framebuf.iLen);
					m_pAudioFrameBuffer->AddToQueueBuffer(m_pRawAudioFrame);
				}
			}
		}
		else
		{
			//if (m_pFFMediaFileDll->FormatIsUnSupported())
				//DoReOpen();

			if (iRet == -5)  // end of off.
			{
				m_bEOF = true;

				if(m_bIsRepeated && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
					DoRepeat();
			}
		}
	}
#endif
}


void AVFilePlayer::PlayForwardMode_HW()
{
	char szMsg[512];
	bool bIsKeyFrame = false;
	AVPacket pkt;

	if (!m_bIsSuspend)
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		int iRet = pCurFFMediaFileDll->GetNextFrame(&pkt, true);
#else
		int iRet = m_pFFMediaFileDll->GetNextFrame(&pkt, true);
#endif
		if (iRet < 0)
		{
			if (iRet == -1)  //eof
			{
				m_bEOF = true;

				int iTotal = m_pVideoFrameBuffer->GetTotal();

				if (!m_pCurVideoCodecContext)
				{
#ifdef _ENABLE_XAUDIO2
					if (m_pAudioFrameBuffer->GetTotal() == 0 && m_pOutXAudio2Dll->GetBufferCount() == 0)
#else
					if (m_pAudioFrameBuffer->GetTotal() == 0 && m_pOutAudioDll->GetBufferCount() == 0)
#endif
						DoRepeat();
				}
				else
				{

#ifdef _ENABLE_AVQUEUE2
					if (m_pAVPacketQueue2->GetCount() == 0 && m_ulVideoFrameCount > 0 && m_bIsRepeated && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#else
					if (m_pAVPacketQueue->GetCount() == 0 && m_ulVideoFrameCount > 0 && m_bIsRepeated && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#endif
						DoRepeat();

#ifdef _ENABLE_AVQUEUE2
					if (m_pAVPacketQueue2->GetCount() == 0 && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#else
					if (m_pAVPacketQueue->GetCount() == 0 && m_bEOF && m_pVideoFrameBuffer->GetTotal() == 0)
#endif
						m_bEOF2 = true;
				}

			}

			return;
		}

		if (pkt.flags == AV_PKT_FLAG_KEY)
			bIsKeyFrame = true;

		UpdateGOPCount(bIsKeyFrame);

#ifdef _ENABLE_MEDIA_FILE_CTRL
		if (pCurFFMediaFileDll->IsVideoFrame(&pkt))
#else
		if (m_pFFMediaFileDll->IsVideoFrame(&pkt))
#endif
		{
#ifdef _ENABLE_AVQUEUE2
			m_pAVPacketQueue2->Add(pkt);
#else
			m_pAVPacketQueue->Add(pkt);
#endif

#ifdef _ENABLE_AVQUEUE2
			if (m_pAVPacketQueue2->GetCount() >= _BUFFER_NUM - 2)
#else
			if(m_pAVPacketQueue->GetCount() >= _BUFFER_NUM - 2)
#endif
			{
				m_bIsSuspend = true;

				//sprintf(szMsg, "AVFilePlayer::PlayForwardMode_HW() - m_bIsSuspend = true (0)\n");
				//OutputDebugStringA(szMsg);
				//m_pFFMediaFileDll->Suspend();
			}
		}
#ifdef _ENABLE_MEDIA_FILE_CTRL
		else if (pCurFFMediaFileDll->IsAudioFrame(&pkt))
#else
		else if (m_pFFMediaFileDll->IsAudioFrame(&pkt))
#endif
		{
			if (!m_bEnableAudio)
				return;

			//if (m_VideoFrameQueue.size() > 0)
			{
#ifdef _ENABLE_MEDIA_FILE_CTRL
				int iRet = pCurFFMediaFileDll->GetFrameBuffer(&pkt, &m_audioframebuf);
#else
				int iRet = m_pFFMediaFileDll->GetFrameBuffer(&pkt, &m_audioframebuf);
#endif

				if (iRet >= 0)
				{
					m_pRawAudioFrame->ulTimestampSec = m_audioframebuf.lTimestamp;
					m_pRawAudioFrame->iLen = m_audioframebuf.iLen;
					memcpy(m_pRawAudioFrame->pBuffer, m_audioframebuf.pBuffer, m_audioframebuf.iLen);
					m_pAudioFrameBuffer->AddToQueueBuffer(m_pRawAudioFrame);

					if (m_pAudioFrameBuffer->GetTotal() >= m_pAudioFrameBuffer->GetMax() - 2)
					{
						m_bIsSuspend = true;

						//sprintf(szMsg, "AVFilePlayer::PlayForwardMode_HW() - m_bIsSuspend = true (1)\n");
						//OutputDebugStringA(szMsg);
					}
				}
			}
		}
	}
}

void AVFilePlayer::PlayBackwardMode()
{
	char szMsg[512];
	AVPacket pkt;
//	m_bFinished = false;

	sprintf(szMsg, "AVFilePlayer::PlayBackwardMode() 0\n");
	OutputDebugStringA(szMsg);

	if (!m_BackwardHelper2.IsWritable())
		m_bIsSuspend = true;

//	DecodeBackward();

	if (!m_bIsSuspend && !m_bDoRetry)
	{
		bool bIsKeyFrame = false;
	
		if (m_bBeginOfFile)
		{
//			m_bFinished = true;
			sprintf(szMsg, "AVFilePlayer::PlayBackwardMode() 1\n");
			OutputDebugStringA(szMsg);
			return;
		}

#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		int iRet = pCurFFMediaFileDll->GetNextFrame(&pkt, true);
#else
		int iRet = m_pFFMediaFileDll->GetNextFrame(&pkt,true);
#endif
		if (iRet < 0)
		{
//			m_bFinished = true;
			sprintf(szMsg, "AVFilePlayer::PlayBackwardMode() 2\n");
			OutputDebugStringA(szMsg);
			return;
		}

		sprintf(szMsg, "AVFilePlayer::PlayBackwardMode() 3\n");
		OutputDebugStringA(szMsg);

		if (pkt.flags == AV_PKT_FLAG_KEY)
			bIsKeyFrame = true;

		UpdateGOPCount(bIsKeyFrame);

		if (iRet > 0)
		{
#ifdef _ENABLE_MEDIA_FILE_CTRL
			if (pCurFFMediaFileDll->IsVideoFrame(&pkt))
#else
			if (m_pFFMediaFileDll->IsVideoFrame(&pkt))
#endif
			{
				if (pkt.flags == AV_PKT_FLAG_KEY)
				{
					if (m_bDoBackwardSeek)
					{
						m_bDoBackwardSeek = false;
						
#ifdef _ENABLE_MEDIA_FILE_CTRL
						pCurFFMediaFileDll->SeekBackward();
#else
						m_pFFMediaFileDll->SeekBackward();
#endif
						//double dLastBackwardPos = m_pFFMediaFileDll->GetLastBackwardPos();
					}
					else
					{
						m_bDoBackwardSeek = true;
						int iRet = ProcessAVPacket(&pkt);

						//double dPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, m_pkt.dts);
						//m_pFFMediaFileDll->SetLastBackwardPos(dPos);
						if (iRet != -1)
						{
#ifdef _ENABLE_MEDIA_FILE_CTRL
							double dPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pkt.dts);
							double dLastBackwardPos = pCurFFMediaFileDll->GetLastBackwardPos();

							if (dLastBackwardPos != 0)
								pCurFFMediaFileDll->SetLastBackwardPos(dPos);
#else
							double dPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, pkt.dts);
							double dLastBackwardPos = m_pFFMediaFileDll->GetLastBackwardPos();

							if(dLastBackwardPos != 0)
								m_pFFMediaFileDll->SetLastBackwardPos(dPos);
#endif

							if (dLastBackwardPos == 0.0)
							{
								m_bBeginOfFile = true;//Stop();
								m_bDoBackwardSeek = false;
								//Stop();
							}
						}
						else
							m_bDoRetry = true;
					}
				}
				else
					ProcessAVPacket(&pkt);
			}
		}
	}
	else
	{
		if (m_bDoRetry)
		{
			int iRet = ProcessAVPacket(&pkt);
			if (iRet != -1)
				m_bDoRetry = false;
		}
		/*
		if (m_bDoRetryPushRaw)
		{
			int iRawRet = m_BackwardHelper2.PushToRaw(m_pRawVideoFrame2);
			if (iRawRet >= 0)
				m_bDoRetryPushRaw = false;
			return ;
		}
		*/

		if (m_BackwardHelper2.IsWritable() && m_bIsSuspend && m_BackwardHelper2.FindNextRawStack() != -1)
			m_bIsSuspend = false;
	}
//	m_bFinished = true;
	sprintf(szMsg, "AVFilePlayer::PlayBackwardMode() 4\n");
	OutputDebugStringA(szMsg);
	//::SetEvent(m_hEvent1);
}

void AVFilePlayer::DoFilter(AVFrame* pAVFrame, StreamFrame* pRawVideoFrame)
{
	int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->BeginFilter();

	if (m_bEnableFadeIn || m_bEnableFadeOut)
	{
		//AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
		int frame_w, frame_h;
		m_pFFVideoFilterDll->InputFrame2(FT_FADE, pAVFrame);
		int iRet = m_pFFVideoFilterDll->OutputFrameBuffer2(FT_FADE, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
	}
	else
	{
		std::list<int> FilterList;

		if (m_iBrightness != 0 || m_iContrast != 0 || m_iSaturation != 0 || m_iR != 0 || m_iG != 0 || m_iB != 0 )
			FilterList.push_back(FT_EQ);

		if (m_iHue != 0)
			FilterList.push_back(FT_HUE);

		if (m_iGray != 0)
			FilterList.push_back(FT_LUTYUV);

		if(m_iNegative != 0)
			FilterList.push_back(FT_LUTRGB);

		if (FilterList.size() > 0)
		{
			int frame_w, frame_h;
			if (FilterList.size() >= 2)
			{
				int iCurFilterID;
				AVFrame* pCurAVFrame = av_frame_clone(pAVFrame);
				if (pCurAVFrame)
				{
					for (std::list<int>::iterator it = FilterList.begin(); it != FilterList.end(); ++it)
					{
						iCurFilterID = *it;

						int iInRet = m_pFFVideoFilterDll->InputFrame2(iCurFilterID, pCurAVFrame);
						if (iInRet >= 0)
						{
							AVFrame* pOutAVFrame = av_frame_alloc();
							int iRet = m_pFFVideoFilterDll->OutputFrame2(iCurFilterID, pOutAVFrame);
							av_frame_copy(pCurAVFrame, pOutAVFrame);
							m_pFFVideoFilterDll->UnRefBuffer(iCurFilterID);
							av_frame_free(&pOutAVFrame);
						}
					}

					m_pFFVideoFilterDll->AVFrameToBuffer(iCurFilterID, pCurAVFrame, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
					m_pFFVideoFilterDll->UnRefBuffer(iCurFilterID);
					av_frame_free(&pCurAVFrame);
				}
			}
			else
			{
				std::list<int>::iterator it = FilterList.begin();
				int iFilterID = *it;
				
				int iInRet = m_pFFVideoFilterDll->InputFrame2(iFilterID, pAVFrame);

				if (iInRet >= 0)
						int iRet = m_pFFVideoFilterDll->OutputFrameBuffer2(iFilterID, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
					else
						memcpy(pRawVideoFrame->pBuffer, m_framebuf.pBuffer, iVideoFrameSize);
			}
		}
		else
			memcpy(pRawVideoFrame->pBuffer, m_framebuf.pBuffer, iVideoFrameSize);
	}

	m_pFFVideoFilterDll->EndFilter();
#else
	memcpy(pRawVideoFrame->pBuffer, m_framebuf.pBuffer, iVideoFrameSize);
#endif
}

int AVFilePlayer::DoFilter2(AVFrame* pInAVFrame, StreamFrame* pRawVideoFrame)
{
	int iRet = 0;
	//int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
	int iVideoFrameSize = m_iCoded_Width * m_iCoded_Height * 3 / 2;

#ifdef _ENABLE_VIDEO_FILTER

	m_pFFVideoFilterDll->BeginFilter();

	AVFrame* pAVFrame_Mem = av_frame_alloc();

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	pCurFFMediaFileDll->CopyFrame(pInAVFrame, m_pNV12RawVideoBuf);
#else
	m_pFFMediaFileDll->CopyFrame(pInAVFrame, m_pNV12RawVideoBuf);
#endif

	m_YUVConverter.NV12ToI420(m_pNV12RawVideoBuf, m_pRawVideoBuf, m_iCoded_Width, m_iCoded_Height);
	avpicture_fill((AVPicture *)pAVFrame_Mem, m_pRawVideoBuf, AV_PIX_FMT_YUV420P, m_iCoded_Width, m_iCoded_Height);

	pAVFrame_Mem->pkt_dts = pInAVFrame->pkt_dts;
	pAVFrame_Mem->pkt_pts = pInAVFrame->pkt_pts;
	pAVFrame_Mem->pkt_pos = pInAVFrame->pkt_pos;
	pAVFrame_Mem->pkt_duration = pInAVFrame->pkt_duration;
	pAVFrame_Mem->pts = pInAVFrame->pts;
	pAVFrame_Mem->width = m_iCoded_Width;// pInAVFrame->width;
	pAVFrame_Mem->height = m_iCoded_Height;// pInAVFrame->height;
	pAVFrame_Mem->format = 0;
	pAVFrame_Mem->key_frame = pInAVFrame->key_frame;
	pAVFrame_Mem->pict_type = pInAVFrame->pict_type;
	pAVFrame_Mem->flags = pInAVFrame->flags;
	pAVFrame_Mem->pkt_size = pInAVFrame->pkt_size;
	pAVFrame_Mem->best_effort_timestamp = pInAVFrame->best_effort_timestamp;
	pAVFrame_Mem->coded_picture_number = pInAVFrame->coded_picture_number;

	if (m_bEnableFadeIn || m_bEnableFadeOut)
	{
		//AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
		int frame_w, frame_h;
		m_pFFVideoFilterDll->InputFrame2(FT_FADE, pAVFrame_Mem);
		//int iRet = m_pFFVideoFilterDll->OutputFrame2(FT_FADE, pOutAVFrame);
		int iRet = m_pFFVideoFilterDll->OutputFrameBuffer2(FT_FADE, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
		iRet = 1;
	}
	else
	{
		std::list<int> FilterList;

		if (m_iBrightness != 0 || m_iContrast != 0 || m_iSaturation != 0 || m_iR != 0 || m_iG != 0 || m_iB != 0)
			FilterList.push_back(FT_EQ);

		if (m_iHue != 0)
			FilterList.push_back(FT_HUE);

		if (m_iGray != 0)
			FilterList.push_back(FT_LUTYUV);

		if (m_iNegative != 0)
			FilterList.push_back(FT_LUTRGB);

		if (FilterList.size() > 0)
		{
			int frame_w, frame_h;
			if (FilterList.size() >= 2)
			{
				int iCurFilterID;
				AVFrame* pCurAVFrame = av_frame_clone(pAVFrame_Mem);
				if (pCurAVFrame)
				{
					for (std::list<int>::iterator it = FilterList.begin(); it != FilterList.end(); ++it)
					{
						iCurFilterID = *it;

						int iInRet = m_pFFVideoFilterDll->InputFrame2(iCurFilterID, pCurAVFrame);
						if (iInRet >= 0)
						{
							AVFrame* pOutAVFrame = av_frame_alloc();
							int iRet = m_pFFVideoFilterDll->OutputFrame2(iCurFilterID, pOutAVFrame);
							av_frame_copy(pCurAVFrame, pOutAVFrame);
							m_pFFVideoFilterDll->UnRefBuffer(iCurFilterID);
							av_frame_free(&pOutAVFrame);
						}
					}

					m_pFFVideoFilterDll->AVFrameToBuffer(iCurFilterID, pCurAVFrame, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
					m_pFFVideoFilterDll->UnRefBuffer(iCurFilterID);
					av_frame_free(&pCurAVFrame);
					iRet = 1;
				}
			}
			else
			{
				std::list<int>::iterator it = FilterList.begin();
				int iFilterID = *it;

				int iInRet = m_pFFVideoFilterDll->InputFrame2(iFilterID, pAVFrame_Mem);

				if (iInRet >= 0)
					//int iRet = m_pFFVideoFilterDll->OutputFrame2(iFilterID, pOutAVFrame);
					int iRet = m_pFFVideoFilterDll->OutputFrameBuffer2(iFilterID, (unsigned char*)pRawVideoFrame->pBuffer, &frame_w, &frame_h);
				iRet = 1;
			}
		}
		else
		{
			//int iRet = av_frame_copy(pOutAVFrame, pAVFrame_Mem);
			//if (iRet < 0)
				//int kk = 0;

			//AVFrame* pTmpAVFrame = av_frame_clone(pInAVFrame);
			//int kk = 0;
			memcpy(pRawVideoFrame->pBuffer, m_pRawVideoBuf, iVideoFrameSize);
		}
	}
	m_pFFVideoFilterDll->EndFilter();
	av_frame_free(&pAVFrame_Mem);
#else
	memcpy(pRawVideoFrame->pBuffer, m_pRawVideoBuf, iVideoFrameSize);
#endif
	return iRet;
}

int AVFilePlayer::ProcessAVPacket(AVPacket* pkt)
{
	//int iRet = m_BackwardHelper.PushToRcv(pkt);
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	int iRet = pCurFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);
#else
	int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);
#endif

	if (iRet >= 0)
	{
		//if (framebuf.iPicType == AV_PICTURE_TYPE_I)
			//return -1;

		int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
		m_pRawVideoFrame2->ulTimestampSec = m_framebuf.lTimestamp;
		m_pRawVideoFrame2->iLen = iVideoFrameSize;
		m_pRawVideoFrame2->iWidth = m_iVideoFrameW;
		m_pRawVideoFrame2->iHeight = m_iVideoFrameH;

		m_pRawVideoFrame2->iPicType = m_framebuf.iPicType;

		if (pkt->flags == AV_PKT_FLAG_KEY || m_framebuf.bIsPicIFrame)
			m_pRawVideoFrame2->bIsKeyFrame = true;
		else
			m_pRawVideoFrame2->bIsKeyFrame = false;
		memcpy(m_pRawVideoFrame2->pBuffer, m_framebuf.pBuffer, iVideoFrameSize);

		m_BackwardHelper2.PushToRaw(m_pRawVideoFrame2);
	}

	if (iRet == -1)
		return -1;

#ifdef _ENABLE_MEDIA_FILE_CTRL
	double dPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pkt->dts);
#else
	double dPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO,pkt->dts);
#endif
	
	char szMsg[512];
	if (pkt->flags == AV_PKT_FLAG_KEY)
		sprintf(szMsg, "ProcessAVPacket -  pos=%f (I Frame) \n", dPos);
	else
		sprintf(szMsg, "ProcessAVPacket -  pos=%f  \n", dPos);
	OutputDebugStringA(szMsg);
	
//	if (!m_BackwardHelper.IsWritable())
//		m_bIsSuspend = true;

	
	return 0;
}

void AVFilePlayer::DecodeBackward()
{
	/*
	AVPacket* pCurVideoFrame = m_BackwardHelper.GetNextFromRcv();
	if (pCurVideoFrame)
	{
		int iRawIndex = m_BackwardHelper.FindNextRawStack();
		if (iRawIndex != -1)
		{
			//AVPacket tmp_pkt;
			av_copy_packet(&m_tmp_pkt, pCurVideoFrame);

			int iRet = m_pFFMediaFileDll->GetFrameBuffer(&m_tmp_pkt, &framebuf);

			if (iRet >= 0)
			{
				int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
				m_pRawVideoFrame2->ulTimestampSec = framebuf.lTimestamp;
				m_pRawVideoFrame2->iLen = iVideoFrameSize;
				m_pRawVideoFrame2->iWidth = m_iVideoFrameW;
				m_pRawVideoFrame2->iHeight = m_iVideoFrameH;

				if (pCurVideoFrame->flags == AV_PKT_FLAG_KEY)
					m_pRawVideoFrame2->bIsKeyFrame = true;
				else
					m_pRawVideoFrame2->bIsKeyFrame = false;
				memcpy(m_pRawVideoFrame2->pBuffer, framebuf.pBuffer, iVideoFrameSize);

				m_BackwardHelper.PushToRaw(m_pRawVideoFrame2);
			}
			m_BackwardHelper.PopFromRcv();
		}
	}
	*/
}

void AVFilePlayer::ThreadEvent()
{
	char szMsg[512];
	ThreadBase::Lock();
	if(m_bIsOpened)
	{
		if (!m_bSuspendDisplay && m_iPlaybackMode != MFPM_PAUSE)
		{
			::WaitForSingleObject(m_hEvent2, INFINITE);
			::ResetEvent(m_hEvent1);

			if (m_iPlaybackMode == MFPM_FORWARD)
			{
#ifdef _ENABLE_MEDIA_FILE_CTRL
				FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

				if (pCurFFMediaFileDll->GPUIsEnabled())
#else
				if(m_pFFMediaFileDll->GPUIsEnabled())
#endif
					PlayForwardMode_HW();
				else
				{
					//m_bIsAudioFile;
					PlayForwardMode();
					PlayForwardMode();
					DecodeVideoFrame();
				}
			}
			else if (m_iPlaybackMode == MFPM_BACKWARD)
				PlayBackwardMode();	
		}

		
		::SetEvent(m_hEvent1);
		//DisplayVideoFrame();
		//PlayAudioFrame();
	}
	Sleep(1);
	ThreadBase::Unlock();
}

void AVFilePlayer::VideoThreadEvent()
{
	EnterCriticalSection(&m_VideoCriticalSection);
	
	if (!m_bSuspendDisplay && m_iPlaybackMode != MFPM_PAUSE)
	{
		::WaitForSingleObject(m_hEvent2, INFINITE);
		::ResetEvent(m_hEvent3);

		if (m_iPlaybackMode == MFPM_FORWARD)
		{
#ifdef _ENABLE_MEDIA_FILE_CTRL
			FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

			if (pCurFFMediaFileDll->GPUIsEnabled())
#else
			if (m_pFFMediaFileDll->GPUIsEnabled())
#endif
			{
				//DisplayVideoFrame_HW();
				
				switch (m_iDisplayIntervalMode)
				{
					case DIM_NORMAL:
						DisplayVideoFrame_HW();
						break;
					case DIM_LOCAL:
					case DIM_GOBAL:
						int iRet = DisplayVideoFrame_HW2();
						if (iRet == -1)
						{
							for (int i = 0; i < 100; i++)
							{
								iRet = DisplayVideoFrame_HW2();
								if (iRet == 0)
									break;
							}
						}
						break;
				}
			}
			else
			{
				switch (m_iDisplayIntervalMode)
				{
					case DIM_NORMAL:
						DisplayVideoFrame();
						break;
					case DIM_LOCAL:
					case DIM_GOBAL:
						int iRet = DisplayVideoFrame2();
						if (iRet == -1)
						{
							for (int i = 0; i < 100; i++)
							{
								iRet = DisplayVideoFrame2();
								if (iRet == 0)
									break;
							}
						}
						
						CheckPosInfo();
						break;
				}
			}
		}
		else if (m_iPlaybackMode == MFPM_BACKWARD)
			DisplayBackwardVideoFrame();
	}
	::SetEvent(m_hEvent3);
	Sleep(1);
	LeaveCriticalSection(&m_VideoCriticalSection);
}

void AVFilePlayer::AudioThreadEvent()
{
	EnterCriticalSection(&m_AudioCriticalSection);
	if (!m_bSuspendDisplay && m_iPlaybackMode != MFPM_PAUSE)
	{
		switch (m_iDisplayIntervalMode)
		{
			case DIM_NORMAL:
				PlayAudioFrame();
				break;
			case DIM_LOCAL:
			case DIM_GOBAL:
				PlayAudioFrame2();
				break;
		}	
	}
	Sleep(1);
	LeaveCriticalSection(&m_AudioCriticalSection);
}

void AVFilePlayer::DisplayBackwardVideoFrame()
{
//	m_bFinished = false;

	DWORD dwCurTime = timeGetTime();

	if(m_BackwardHelper2.IsWritable() && m_bIsSuspend && m_BackwardHelper2.FindNextRawStack() != -1)
		m_bIsSuspend = false;

	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_fSpeed;// m_iSpeed;

	if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

		RawVideoFrame* pCurRawVideoFrame = m_BackwardHelper2.GetNextFromRaw();
		if (pCurRawVideoFrame)
		{
			/*
			char szMsg[512];
			sprintf(szMsg, "DisplayBackwardVideoFrame - %d \n", pCurRawVideoFrame->ulTimestampSec);
			OutputDebugStringA(szMsg);
			*/

			if (m_pFilePlayerCallback)
			{
				if (m_iOutputVideoFormat != VPF_I420)
				{
					switch (m_iOutputVideoFormat)
					{
					case VPF_YUY2:
						break;
					case VPF_RGB32:
						m_YUVConverter.I420ToARGB((unsigned char*)pCurRawVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH,32);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					case VPF_RGB24:
						m_YUVConverter.I420ToRGB24((unsigned char*)pCurRawVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH, 24);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					}

				}
				else
				{
					m_pFilePlayerCallback->DisplayVideo((unsigned char*)pCurRawVideoFrame->pBuffer, m_iVideoFrameW, m_iVideoFrameH);
				}
			}
//			else
	//			m_pOutVideoDll->Display((unsigned char*)pCurRawVideoFrame->pBuffer, m_iVideoFrameW, m_iVideoFrameH);

			if(pCurRawVideoFrame->iPicType != AV_PICTURE_TYPE_B)
				UpdatePosInfo(pCurRawVideoFrame->ulTimestampSec);

			m_BackwardHelper2.PopFromRaw();

			m_ulVideoFrameCount++;
			/*
			char szMsg[512];
			sprintf(szMsg, "VideoFrameCount - %d (%d)\n", m_ulVideoFrameCount, pCurRawVideoFrame->ulTimestampSec);
			OutputDebugStringA(szMsg);
			*/
		}
	}
//	m_bFinished = true;
}

void AVFilePlayer::DecodeVideoFrame()
{
#if _LOW_MEM_MODE

	if (!m_pVideoFrameBuffer)
		return;

#ifdef _ENABLE_AVQUEUE2
	if (m_pAVPacketQueue2->GetCount() > 0 && !m_pVideoFrameBuffer->IsFulled())
#else
	if (m_pAVPacketQueue->GetCount() > 0 && !m_pVideoFrameBuffer->IsFulled())
#endif
	{

#ifdef _ENABLE_AVQUEUE2
		AVPacket* pkt = m_pAVPacketQueue2->Get();
#else
		AVPacket* pkt = m_pAVPacketQueue->Get();
#endif

#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		if (!pCurFFMediaFileDll->IsVideoFrame(pkt))
#else
		if (!m_pFFMediaFileDll->IsVideoFrame(pkt))
#endif
		{
			m_pAVPacketQueue2->Remove();
			return;
		}

#ifdef _ENABLE_MEDIA_FILE_CTRL
		int iRet = pCurFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);
#else
		int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);
#endif

		if (iRet >= 0)
		{
			int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
			m_pRawVideoFrame->ulTimestampSec = m_framebuf.lTimestamp;
			m_pRawVideoFrame->iLen = iVideoFrameSize;

#ifdef _ENABLE_MEDIA_FILE_CTRL
			AVFrame* pAVFrame = pCurFFMediaFileDll->GetAVFrame();
#else
			AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
#endif
			DoFilter(pAVFrame, m_pRawVideoFrame);
			m_pVideoFrameBuffer->AddToQueueBuffer(m_pRawVideoFrame);
		}
        else if (iRet == -101)
			{
				m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
				m_dwLastTime = 0;
				m_ulTimestamp = 0;

				char szMsg[512];
				sprintf(szMsg, "#AVF DecodeVideoFrame - iRet == -101\n");
				OutputDebugStringA(szMsg);
			}
#ifdef _ENABLE_AVQUEUE2
		m_pAVPacketQueue2->Remove();
#else
		m_pAVPacketQueue->Remove();
#endif

	}
#endif
}

void AVFilePlayer::DisplayVideoFrame()
{
	DWORD dwCurTime = timeGetTime();
	/*
#if _LOW_MEM_MODE

	if (m_pAVPacketQueue->GetCount() > 0 && !m_pVideoFrameBuffer->IsFulled())
	{

		AVPacket* pkt = m_pAVPacketQueue->Get();
		int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);

		if (iRet >= 0)
		{
			int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
			m_pRawVideoFrame->ulTimestampSec = m_framebuf.lTimestamp;
			m_pRawVideoFrame->iLen = iVideoFrameSize;

			AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
			DoFilter(pAVFrame, m_pRawVideoFrame);
			m_pVideoFrameBuffer->AddToQueueBuffer(m_pRawVideoFrame);
		}
		m_pAVPacketQueue->Remove();

	}
#endif
*/

#if _LOW_MEM_MODE
	//if (m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))

#ifdef _ENABLE_AVQUEUE2
	//m_pAVPacketQueue2
	if (m_bIsSuspend && (m_pAVPacketQueue2->GetCount() < _BUFFER_NUM - 4))
#else
	if (m_bIsSuspend && (m_pAVPacketQueue->GetCount() < _BUFFER_NUM - 4))
#endif

#else
	if (m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))
#endif
	{
		if (!m_pAudioFrameBuffer || (m_pAudioFrameBuffer && (m_pAudioFrameBuffer->GetTotal() < m_pAudioFrameBuffer->GetMax() - 4)))
		{
			m_bIsSuspend = false;
			//m_pFFMediaFileDll->Resume();
		}
	}
	
	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_fSpeed;// m_iSpeed;

	if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

		m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
		if (!m_pCurVideoFrame)
			return;

#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		double dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, m_pCurVideoFrame->ulTimestampSec);
		double dLength = pCurFFMediaFileDll->GetLength();
#else
		double dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, m_pCurVideoFrame->ulTimestampSec);
		double dLength = m_pFFMediaFileDll->GetLength();
#endif
		if (dCurPos - m_dLastPos > 0)
		{
			float fRealFps = 1.0 / (dCurPos - m_dLastPos);

			if (fRealFps > 0)
				m_RealAvgFPS.Calculate(fRealFps);
		}

		m_dLastVideoPos = dCurPos;

//		if(m_pCurVideoFrame->bIsKeyFrame)// && m_pCurVideoFrame->ulTimestampSec > m_ulRawLastTimestamp)
	//		m_ulRawLastTimestamp = m_pCurVideoFrame->ulTimestampSec;

		if (m_bRefTimeUpdate && abs(m_dCurPosMs - m_dReferenceTime) > 3 * m_fVideoInterval)
		{
			if (1)
			{
				::SetEvent(m_hEvent3);
				Seek2(m_dReferenceTime / 1000.0f);
				m_bRefTimeUpdate = false;
				return;
			}
			else if (m_dReferenceTime > m_dCurPosMs)
			{
				while (m_dReferenceTime > m_dCurPosMs + 2 * m_fVideoInterval && m_pCurVideoFrame)
				{
//					if (m_pCurVideoFrame->bIsKeyFrame)// && m_pCurVideoFrame->ulTimestampSec > m_ulRawLastTimestamp)
	//					m_ulRawLastTimestamp = m_pCurVideoFrame->ulTimestampSec;
#ifdef _ENABLE_MS
					if (m_pCurVideoFrame)
						UpdatePosInfoMS(m_pCurVideoFrame->ulTimestampSec);
#else
					UpdatePosInfo();
#endif

					m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
				}

				if (!m_pCurVideoFrame)
					return;
			}
			else
			{
				Sleep(m_dCurPosMs - m_dReferenceTime);
			}

			m_bRefTimeUpdate = false;
		}

		if (m_pFilePlayerCallback)
		{
#ifdef _ENABLE_LOADING_CTRL
			if (m_LoadingCtrl.IsAllowed())
#else
#endif
			{
				if (m_iOutputVideoFormat != VPF_I420)
				{
					switch (m_iOutputVideoFormat)
					{
					case VPF_YUY2:
						break;
					case VPF_RGB32:
						m_YUVConverter.I420ToARGB((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH,32);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					case VPF_RGB24:
						m_YUVConverter.I420ToRGB24((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH, 24);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					}

				}
				else
				{

					if (m_fScaleW != 1.0 || m_fScaleH != 1.0)
					{
						int iDestW = m_iVideoFrameW * m_fScaleW;
						int iDestH = m_iVideoFrameH * m_fScaleW;
						m_YUVConverter.YUV_Scale((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH, iDestW, iDestH);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, iDestW, iDestH);
					}
					else
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pCurVideoFrame->pBuffer, m_iVideoFrameW, m_iVideoFrameH);

				}
			}
		}
//		else
	//		m_pOutVideoDll->Display((unsigned char*)m_pCurVideoFrame->pBuffer,m_iVideoFrameW,m_iVideoFrameH);
		
#ifdef _ENABLE_MS
		if(m_pCurVideoFrame)
			UpdatePosInfoMS(m_pCurVideoFrame->ulTimestampSec);
#else
		UpdatePosInfo();
#endif

//		if(m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))
	//		m_bIsSuspend = false;

		if (m_ulVideoFrameCount % 30 == 0)
		{
			DWORD dwCurTime1 = timeGetTime();

			if (m_ulLastTime != 0)
			{
				unsigned long ulSpendTime = dwCurTime1 - m_ulLastTime;
				float fFPS = 1000.0 / ((float)ulSpendTime / 30.0);

				m_AvgFPS.Calculate(fFPS);

				float fRate = m_RealAvgFPS.GetAverage() / m_AvgFPS.GetAverage();//0.0002
				//float fRate = m_fVideoFPS / m_AvgFPS.GetAverage();
				float fReSampleRate = fRate + m_fReSampleRateOffset;// +0.0002;
				if (m_ulVideoFrameCount > 30 && m_bEnableReSampleRate)
#ifdef _ENABLE_MEDIA_FILE_CTRL
				pCurFFMediaFileDll->SetReSampleRate(fReSampleRate);
#else
					m_pFFMediaFileDll->SetReSampleRate(fReSampleRate);
#endif

				char szMsg[512];
				sprintf(szMsg, "AVFilePlayer::DisplayVideoFrame() - Video display FPS %3.3f [%3.3f , %3.3f]\n", fFPS, m_AvgFPS.GetAverage(),m_fVideoFPS);
				OutputDebugStringA(szMsg);
			}
			
			m_ulLastTime = dwCurTime1;
		}

		m_ulVideoFrameCount++;

		/*
		char szMsg[512];
		sprintf(szMsg, "VideoFrameCount - %d\n",m_ulVideoFrameCount);
		OutputDebugStringA(szMsg);
		*/
	}
}

int AVFilePlayer::DisplayVideoFrame2()
{
	char szMsg[512];
	DWORD dwCurTime = timeGetTime();

	double dCurTimeCode = (float)m_ulTimestamp / 1000.0;
	/*
#if _LOW_MEM_MODE

	if (m_pAVPacketQueue->GetCount() > 0 && !m_pVideoFrameBuffer->IsFulled())
	{
		AVPacket* pkt = m_pAVPacketQueue->Get();

		int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf);

		if (iRet >= 0)
		{
			int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
			m_pRawVideoFrame->ulTimestampSec = m_framebuf.lTimestamp;
			m_pRawVideoFrame->iLen = iVideoFrameSize;

			AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
			DoFilter(pAVFrame, m_pRawVideoFrame);
			m_pVideoFrameBuffer->AddToQueueBuffer(m_pRawVideoFrame);
		}

		m_pAVPacketQueue->Remove();
	}
#endif
*/
#if _LOW_MEM_MODE
	//if (m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))

#ifdef _ENABLE_AVQUEUE2
	if (m_bIsSuspend && (m_pAVPacketQueue2->GetCount() < _BUFFER_NUM - 4))
#else
	if (m_bIsSuspend && (m_pAVPacketQueue->GetCount() < _BUFFER_NUM - 4))
#endif

#else
	if (m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))
#endif
	{
		if (!m_pAudioFrameBuffer || (m_pAudioFrameBuffer && (m_pAudioFrameBuffer->GetTotal() < m_pAudioFrameBuffer->GetMax() - 4)))
		{
			m_bIsSuspend = false;
			//m_pFFMediaFileDll->Resume();
		}
	}

	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_fSpeed;

	//m_bDoGetNextVideo = true;
	if (!m_pCurVideoFrame)
	{
		m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
		if (!m_pCurVideoFrame)
			return 0;
	}

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, m_pCurVideoFrame->ulTimestampSec);
	double dLength = pCurFFMediaFileDll->GetLength();
#else
	double dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, m_pCurVideoFrame->ulTimestampSec);
	double dLength = m_pFFMediaFileDll->GetLength();
#endif

	if (dLength < dCurPos)
	{
		m_pCurVideoFrame = 0;
		return 0;
	}
	
	if (dCurTimeCode >= dCurPos)
		//if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

		if (dCurPos - m_dLastPos > 0)
		{
			float fRealFps = 1.0 / (dCurPos - m_dLastPos);

			if (fRealFps > 0)
				m_RealAvgFPS.Calculate(fRealFps);
		}

		if (m_bDoSkip)
		{
			//if (dCurPos < dCurTimeCode)
			//if (dCurPos < m_dSeekPos)
			if (dCurPos < (dCurTimeCode + m_dSeekPos) / 2.0)
			//if (dCurPos < (dCurTimeCode * 0.1 + m_dSeekPos * 0.9))
			{
				m_pCurVideoFrame = 0;

//				sprintf(szMsg, "AVFilePlayer::DisplayVideoFrame2() - m_bDoSkip!! dCurPos = %3.3f m_dSeekPos = %3.3f dCurTimeCode = %3.3f\n", dCurPos, m_dSeekPos, dCurTimeCode);
//				OutputDebugStringA(szMsg);

				return -1;
			}
			else
			{
				m_bDoSkip = false;
			}
		}

#ifdef _ENABLE_LOADING_CTRL
		//m_LoadingCtrl.SetDataFPS(m_AvgFPS.GetAverage());
		if (m_ulVideoFrameCount % 10 == 0)
			m_LoadingCtrl.Compute();
#endif

		//if (m_pCurVideoFrame->bIsKeyFrame)
			//m_dVideoTimeCode = dCurPos;

		//m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
		//if (!m_pCurVideoFrame)
		//return;

		//if (m_pCurVideoFrame->bIsKeyFrame)// && m_pCurVideoFrame->ulTimestampSec > m_ulRawLastTimestamp)
		//m_ulRawLastTimestamp = m_pCurVideoFrame->ulTimestampSec;

		if (m_pFilePlayerCallback)
		{
#ifdef _ENABLE_LOADING_CTRL
			if (m_LoadingCtrl.IsAllowed())
#else
#endif
			{
				if (m_iOutputVideoFormat != VPF_I420)
				{
					switch (m_iOutputVideoFormat)
					{
					case VPF_YUY2:
						break;
					case VPF_RGB32:
						m_YUVConverter.I420ToARGB((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH,32);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					case VPF_RGB24:
						m_YUVConverter.I420ToRGB24((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						//m_YUVConverter.Flip(m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH, 24);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						break;
					}

				}
				else
				{
					if (m_fScaleW != 1.0 || m_fScaleH != 1.0)
					{
						int iDestW = m_iVideoFrameW * m_fScaleW;
						int iDestH = m_iVideoFrameH * m_fScaleW;
						m_YUVConverter.YUV_Scale((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH, iDestW, iDestH);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, iDestW, iDestH);
					}
					else
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pCurVideoFrame->pBuffer, m_iVideoFrameW, m_iVideoFrameH);
				}

			}
		}
		//else
			//m_pOutVideoDll->Display((unsigned char*)m_pCurVideoFrame->pBuffer, m_iVideoFrameW, m_iVideoFrameH);

#ifdef _ENABLE_MS
		if (m_pCurVideoFrame)
			UpdatePosInfoMS(m_pCurVideoFrame->ulTimestampSec);
#else
		UpdatePosInfo();
#endif

		//		if(m_bIsSuspend && (m_pVideoFrameBuffer->GetTotal() < m_pVideoFrameBuffer->GetMax() - 3))
		//		m_bIsSuspend = false;

		if (m_ulVideoFrameCount % 10 == 0)
		{
			DWORD dwCurTime1 = timeGetTime();

			if (m_ulLastTime != 0)
			{
				unsigned long ulSpendTime = dwCurTime1 - m_ulLastTime;
				float fFPS = 1000.0 / ((float)ulSpendTime / 10.0);

				if (m_AvgFPS.GetCount() < 5 && ((fFPS > m_fVideoFPS + 1.0) || (fFPS < m_fVideoFPS - 2.0)))
				{
					//skip
//					sprintf(szMsg, "AVFilePlayer::DisplayVideoFrame2() - skip!! %3.3f\n", fFPS);
//					OutputDebugStringA(szMsg);

					m_AvgFPS.Calculate(m_fVideoFPS);
				}
				else
					m_AvgFPS.Calculate(fFPS);

#if 1
				//float fRate = m_RealAvgFPS.GetAverage() / m_AvgFPS.GetAverage();//0.0002
				float fRate = m_fVideoFPS / m_AvgFPS.GetAverage();
				float fReSampleRate = fRate + m_fReSampleRateOffset;// +0.0002;
#else
				float fRate = m_AvgFPS.GetAverage() / ((m_fVideoFPS + m_RealAvgFPS.GetAverage()) / 2);
				float fReSampleRate = fRate;// +0.0002;
#endif

				m_AvgReSampleRate.Calculate(fReSampleRate);

				float fRSROffset = 0.0;
#ifdef _ENABLE_XAUDIO2
				if (m_pOutXAudio2Dll->IsOpened())
				{
					int iAudioBufferCount = m_pOutXAudio2Dll->GetBufferCount();
					if (iAudioBufferCount >= 6)
						fRSROffset = (iAudioBufferCount - 5) * 0.008;
				}
#else
#endif

				if (m_ulVideoFrameCount > 10 && m_bEnableReSampleRate)
#ifdef _ENABLE_MEDIA_FILE_CTRL
				pCurFFMediaFileDll->SetReSampleRate(m_AvgReSampleRate.GetAverage() - fRSROffset);
#else
						m_pFFMediaFileDll->SetReSampleRate(m_AvgReSampleRate.GetAverage()- fRSROffset);
#endif

//				sprintf(szMsg, "#MS# AVFilePlayer::DisplayVideoFrame2() - Video display FPS %3.3f [%3.3f , %3.3f , %3.8f(%3.8f)] real:%3.3f\n", fFPS, m_AvgFPS.GetAverage(), m_fVideoFPS, fRate, m_AvgReSampleRate.GetAverage(),m_RealAvgFPS.GetAverage());
//				OutputDebugStringA(szMsg);
			}


			m_ulLastTime = dwCurTime1;
		}

		m_pCurVideoFrame = 0;

		m_ulVideoFrameCount++;
	}
	return 0;
}

void AVFilePlayer::DisplayVideoFrame_HW()
{
	DWORD dwCurTime = timeGetTime();

#if _GPU_COPY_BACK
	if (m_bIsSuspend && (m_VideoFrameQueue.size() < _BUFFER_NUM - 4))
	{
		m_bIsSuspend = false;
		//m_pFFMediaFileDll->Resume();
	}

	if (m_VideoFrameQueue.size() > 0 && !m_pVideoFrameBuffer->IsFulled())
	{
		AVPacket* pkt = &m_VideoFrameQueue.front();
		int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf_hw);

		if (iRet >= 0)
		{
			int iVideoFrameSize = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
			m_pRawVideoFrame->ulTimestampSec = m_framebuf.lTimestamp;
			m_pRawVideoFrame->iLen = iVideoFrameSize;

			AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
			DWORD dwTime1 = timeGetTime();
			int iFilterRet = DoFilter2(pAVFrame, m_pRawVideoFrame);

			DWORD dwTime2 = timeGetTime();

			char szMsg[512];
			sprintf_s(szMsg, "AVFilePlayer::DisplayVideoFrame_HW %d\n", dwTime2 - dwTime1);
			OutputDebugStringA(szMsg);

			if (iFilterRet == 1)
			{
				m_pRawVideoFrame->iCodecID = 1;
				m_pRawVideoFrame->iLen = iVideoFrameSize;
			}
			else
			{
				m_pRawVideoFrame->iCodecID = 0;
				m_pRawVideoFrame->iLen = m_iCoded_Width * m_iCoded_Height * 3 / 2;
			}
			m_pVideoFrameBuffer->AddToQueueBuffer(m_pRawVideoFrame);
		}
		else
		{
			if (m_pFFMediaFileDll->FormatIsUnSupported())
			{
				DWORD nThreadID;
				CreateThread(0, 0, ReOpenThread, (void *)this, 0, &nThreadID);
			}
		}

		m_VideoFrameQueue.pop();
	}

	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_iSpeed;

	if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

		int iVideoW, iVideoH;
		m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
		if (!m_pCurVideoFrame)
			return;

		if (m_pCurVideoFrame->iCodecID == 1)
		{
			iVideoW = m_iVideoFrameW;
			iVideoH = m_iVideoFrameH;
		}
		else
		{
			iVideoW = m_iCoded_Width;
			iVideoH = m_iCoded_Height;
		}

		if (m_pFilePlayerCallback)
		{
			{
				m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pCurVideoFrame->pBuffer, iVideoW, iVideoH);
			}
		}
		else
			m_pOutVideoDll->Display((unsigned char*)m_pCurVideoFrame->pBuffer, iVideoW, iVideoH);

		UpdatePosInfo();

		m_ulVideoFrameCount++;
	}
#else


#ifdef _ENABLE_AVQUEUE2
	if (m_bIsSuspend && (m_pAVPacketQueue2->GetCount() < _BUFFER_NUM - 4))
#else
	if (m_bIsSuspend && (m_pAVPacketQueue->GetCount() < _BUFFER_NUM - 4))
#endif
	{
		if (!m_pAudioFrameBuffer || (m_pAudioFrameBuffer && (m_pAudioFrameBuffer->GetTotal() < m_pAudioFrameBuffer->GetMax() - 4)))
		{
			m_bIsSuspend = false;
			//m_pFFMediaFileDll->Resume();
		}
	}

	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_fSpeed;// m_iSpeed;

	if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

#ifdef _ENABLE_AVQUEUE2
		if (m_pAVPacketQueue2->GetCount() > 0)
#else
		if (m_pAVPacketQueue->GetCount() > 0)
#endif
		{

#ifdef _ENABLE_AVQUEUE2
			AVPacket* pkt = m_pAVPacketQueue2->Get();
#else
			AVPacket* pkt = m_pAVPacketQueue->Get();
#endif

#ifdef _ENABLE_MEDIA_FILE_CTRL
			FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();
			int iRet = pCurFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf_hw);
#else
			int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf_hw);
#endif
			if (iRet >= 0)
			{
#ifdef _ENABLE_MEDIA_FILE_CTRL
				AVFrame* pAVFrame = pCurFFMediaFileDll->GetAVFrame();
#else
				AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
#endif
				if (m_pFilePlayerCallback)
				{
#ifdef _ENABLE_LOADING_CTRL
					if (m_LoadingCtrl.IsAllowed())
#else
#endif
					{
						//m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_framebuf_hw.pBuffer, m_iVideoFrameW, m_iVideoFrameH);
						m_pFilePlayerCallback->DisplayVideo_HW(m_pD3DDevice, (unsigned char*)m_framebuf_hw.pBuffer, m_iVideoFrameW, m_iVideoFrameH);
					}
				}

#ifdef _ENABLE_MS
				UpdatePosInfoMS(m_framebuf_hw.lTimestamp);
#else
				UpdatePosInfo();
#endif
				//av_packet_unref(pkt);
				//av_freep(pkt);
				//av_free_packet(pkt);
				//av_frame_free(&pOutAVFrame);

			}
			else
			{
#ifdef _ENABLE_MEDIA_FILE_CTRL
				if (pCurFFMediaFileDll->FormatIsUnSupported())
#else
				if (m_pFFMediaFileDll->FormatIsUnSupported())
#endif
				{
					DWORD nThreadID;
					CreateThread(0, 0, ReOpenThread, (void *)this, 0, &nThreadID);
				}
			}

#ifdef _ENABLE_AVQUEUE2
			m_pAVPacketQueue2->Remove();
#else
			m_pAVPacketQueue->Remove();
#endif

			m_ulVideoFrameCount++;
		}
	}
#endif
		
}

int AVFilePlayer::DisplayVideoFrame_HW2()
{
	char szMsg[512];
	DWORD dwCurTime = timeGetTime();
	double dCurTimeCode = (float)m_ulTimestamp / 1000.0;

#ifdef _ENABLE_AVQUEUE2
	if (m_bIsSuspend && (m_pAVPacketQueue2->GetCount() < _BUFFER_NUM - 4))
#else
	if (m_bIsSuspend && (m_pAVPacketQueue->GetCount() < _BUFFER_NUM - 4))
#endif
	{
		if (!m_pAudioFrameBuffer || (m_pAudioFrameBuffer && (m_pAudioFrameBuffer->GetTotal() < m_pAudioFrameBuffer->GetMax() - 4)))
		{
			m_bIsSuspend = false;
			//m_pFFMediaFileDll->Resume();
		}
	}

	int iCurInterval = m_FrameRateCtrl.GetInterval(m_ulVideoFrameCount) / m_fSpeed;// m_iSpeed;

	//if (m_ulLastVideoFrameTime == 0 || dwCurTime >= m_ulLastVideoFrameTime + iCurInterval)
	{
		m_ulLastVideoFrameTime = dwCurTime;

#ifdef _ENABLE_AVQUEUE2
		if (m_pAVPacketQueue2->GetCount() > 0)
#else
		if (m_pAVPacketQueue->GetCount() > 0)
#endif
		{

#ifdef _ENABLE_AVQUEUE2
			AVPacket* pkt = m_pAVPacketQueue2->Get();
#else
			AVPacket* pkt = m_pAVPacketQueue->Get();
#endif

			if (!pkt || pkt->size == 0)
				return 0;

#ifdef _ENABLE_MEDIA_FILE_CTRL
			FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

			double dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pkt->dts);
			double dLength = pCurFFMediaFileDll->GetLength();
#else
			double dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, pkt->dts);
			double dLength = m_pFFMediaFileDll->GetLength();
#endif
			if (dCurTimeCode >= dCurPos)
			{
				if (dCurPos - m_dLastPos > 0)
				{
					float fRealFps = 1.0 / (dCurPos - m_dLastPos);

					if (fRealFps > 0)
						m_RealAvgFPS.Calculate(fRealFps);
				}

				if (!pkt || pkt->size == 0)
					return 0;

#ifdef _ENABLE_MEDIA_FILE_CTRL
				int iRet = pCurFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf_hw);
#else
				int iRet = m_pFFMediaFileDll->GetFrameBuffer(pkt, &m_framebuf_hw);
#endif

				if (iRet >= 0)
				{
					if (m_bDoSkip)
					{
						if (dCurPos < (dCurTimeCode + m_dSeekPos) / 2.0)
						{
#ifdef _ENABLE_AVQUEUE2
							m_pAVPacketQueue2->Remove();
#else
							m_pAVPacketQueue->Remove();
#endif
							return -1;
						}
						else
						{
							m_bDoSkip = false;
						}
					}

#ifdef _ENABLE_LOADING_CTRL
					if (m_ulVideoFrameCount % 10 == 0)
						m_LoadingCtrl.Compute();
#endif

#ifdef _ENABLE_MEDIA_FILE_CTRL
					AVFrame* pAVFrame = pCurFFMediaFileDll->GetAVFrame();
#else
					AVFrame* pAVFrame = m_pFFMediaFileDll->GetAVFrame();
#endif
					if (m_pFilePlayerCallback)
					{
#ifdef _ENABLE_LOADING_CTRL
						if (m_LoadingCtrl.IsAllowed())
#else
#endif
						{
							//m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_framebuf_hw.pBuffer, m_iVideoFrameW, m_iVideoFrameH);
							m_pFilePlayerCallback->DisplayVideo_HW(m_pD3DDevice, (unsigned char*)m_framebuf_hw.pBuffer, m_iVideoFrameW, m_iVideoFrameH);
						}
					}
#ifdef _ENABLE_MS
					UpdatePosInfoMS(m_framebuf_hw.lTimestamp);
#else
					UpdatePosInfo();
#endif
				}
				else
				{
#ifdef _ENABLE_MEDIA_FILE_CTRL
					if (pCurFFMediaFileDll->FormatIsUnSupported())
#else
					if (m_pFFMediaFileDll->FormatIsUnSupported())
#endif
					{
						DWORD nThreadID;
						CreateThread(0, 0, ReOpenThread, (void *)this, 0, &nThreadID);
					}
				}

#ifdef _ENABLE_AVQUEUE2
				m_pAVPacketQueue2->Remove();
#else
				m_pAVPacketQueue->Remove();
#endif

				if (m_ulVideoFrameCount % 10 == 0)
				{
					DWORD dwCurTime1 = timeGetTime();

					if (m_ulLastTime != 0)
					{
						unsigned long ulSpendTime = dwCurTime1 - m_ulLastTime;
						float fFPS = 1000.0 / ((float)ulSpendTime / 10.0);

						if (m_AvgFPS.GetCount() < 5 && ((fFPS > m_fVideoFPS + 1.0) || (fFPS < m_fVideoFPS - 2.0)))
						{
							//skip
//							sprintf(szMsg, "AVFilePlayer::DisplayVideoFrame2() - skip!! %3.3f\n", fFPS);
//							OutputDebugStringA(szMsg);

							m_AvgFPS.Calculate(m_fVideoFPS);
						}
						else
							m_AvgFPS.Calculate(fFPS);

#if 1
						//float fRate = m_RealAvgFPS.GetAverage() / m_AvgFPS.GetAverage();//0.0002
						float fRate = m_fVideoFPS / m_AvgFPS.GetAverage();
						float fReSampleRate = fRate + m_fReSampleRateOffset;// +0.0002;
#else
						float fRate = m_AvgFPS.GetAverage() / ((m_fVideoFPS + m_RealAvgFPS.GetAverage()) / 2);
						float fReSampleRate = fRate;// +0.0002;
#endif

						m_AvgReSampleRate.Calculate(fReSampleRate);

						float fRSROffset = 0.0;
#ifdef _ENABLE_XAUDIO2
						if (m_pOutXAudio2Dll->IsOpened())
						{
							int iAudioBufferCount = m_pOutXAudio2Dll->GetBufferCount();
							if (iAudioBufferCount >= 6)
								fRSROffset = (iAudioBufferCount - 5) * 0.008;
						}
#else
#endif

						if (m_ulVideoFrameCount > 10 && m_bEnableReSampleRate)
						{
#ifdef _ENABLE_MEDIA_FILE_CTRL
								pCurFFMediaFileDll->SetReSampleRate(m_AvgReSampleRate.GetAverage() - fRSROffset);
#else
							m_pFFMediaFileDll->SetReSampleRate(m_AvgReSampleRate.GetAverage() - fRSROffset);
#endif
						}

						//sprintf(szMsg, "AVFilePlayer::DisplayVideoFrame_HW2() - Video display FPS %3.3f [%3.3f , %3.3f , %3.8f(%3.8f)] real:%3.3f\n", fFPS, m_AvgFPS.GetAverage(), m_fVideoFPS, fRate, m_AvgReSampleRate.GetAverage(),m_RealAvgFPS.GetAverage());
						//OutputDebugStringA(szMsg);
					}
					m_ulLastTime = dwCurTime1;
				}

				m_ulVideoFrameCount++;
			}
		}
	}
	return 0;
}

void AVFilePlayer::PlayAudioFrame()
{
	if (!m_bEnableAudio)
	{
		m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
		return;
	}

	double dCurTimeCode = m_dLastVideoPos;
	DWORD dwCurTime = timeGetTime();

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
#else
	if(m_pOutAudioDll)
#endif
	{
		bool bDoPlay = false;
		
		char szMsg[512];

#ifdef _ENABLE_XAUDIO2
		if (m_pOutXAudio2Dll->IsOpened() && m_pOutXAudio2Dll->IsFulled())
#else
		if (m_pOutAudioDll->IsOpened() && m_pOutAudioDll->IsFulled() /*&& m_pAVPacketQueue->GetCount() == 0*/)
#endif
			return;
		
		if (!m_pAudioFrameBuffer)
			return;

		if (m_fSpeed > 1) //m_iSpeed
		{
			for(int i = 0;i < m_fSpeed ;i++)  //m_iSpeed
				m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
		}
		else
		{
			if (m_bDoGetNext)
			{
				m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
			}
		}

		if (!m_pCurAudioFrame)
		{
			//sprintf(szMsg, "AVFilePlayer::PlayAudioFrame() - !m_pCurAudioFrame audio:%d video:%d\n", m_pAudioFrameBuffer->GetTotal(), m_pVideoFrameBuffer->GetTotal());
			//OutputDebugStringA(szMsg);
			return;
		}

		if (m_pCurAudioFrame)
		{

#ifdef _ENABLE_MEDIA_FILE_CTRL
			FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

			double dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_AUDIO, m_pCurAudioFrame->ulTimestampSec);
			double dLength = pCurFFMediaFileDll->GetLength();
#else
			double dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_AUDIO, m_pCurAudioFrame->ulTimestampSec);
			double dLength = m_pFFMediaFileDll->GetLength();
#endif

			if (dLength < dCurPos)
			{
				m_pCurAudioFrame = 0;
				return;
			}

			if (m_pCurVideoCodecContext)
			{
				if (dCurTimeCode < dCurPos)
				{
					m_bDoGetNext = false;
					return;
				}
			}
			
#ifdef _AUDIO_ONLY
			m_bDoGetNext = true;
#else  //_AUDIO_ONLY
				m_bDoGetNext = true;
#endif  //_AUDIO_ONLY
		}

#if 0
		if(m_pCurAudioFrame->iLen < m_iSampleRate)
			m_iAudioFrameFactor = (float)(m_iSampleRate) / (float)m_pCurAudioFrame->iLen + 0.1;
		else
			m_iAudioFrameFactor = 1;
#else
		if (m_pCurAudioFrame->iLen < m_iSampleRate)
		{
			m_iAudioFrameFactor = ((float)(m_iSampleRate) / (float)m_pCurAudioFrame->iLen + 0.1) / 4;

			if (m_iAudioFrameFactor < 0)
				m_iAudioFrameFactor = 1;
		}
		else 
			m_iAudioFrameFactor = 1;
#endif
		int iIndex = m_ulAudioFrameCount % m_iAudioFrameFactor;

		memcpy(m_pAudioDeBuffer2 + m_ulRawAudioLen,m_pCurAudioFrame->pBuffer,m_pCurAudioFrame->iLen);
		m_ulRawAudioLen += m_pCurAudioFrame->iLen;
		m_ulLastAudioTimestamp = m_pCurAudioFrame->ulTimestampSec;

		if (m_pFilePlayerCallback)
		{
			memcpy(m_pAudioDeBuffer, m_pCurAudioFrame->pBuffer, m_pCurAudioFrame->iLen);
			m_pFilePlayerCallback->PlayAudio((unsigned char*)m_pAudioDeBuffer, m_pCurAudioFrame->iLen);
		}

		if(m_ulAudioFrameCount % m_iAudioFrameFactor == (m_iAudioFrameFactor - 1))
		{
			m_WaveFrame.iBits = m_iBits;
			m_WaveFrame.iChannels = m_iChannels;
			m_WaveFrame.iSampleRate = m_iSampleRate;
			m_WaveFrame.pBuffer = (char*)m_pAudioDeBuffer2;
			m_WaveFrame.iLen = m_pCurAudioFrame->iLen * m_iAudioFrameFactor * 2;
			m_WaveFrame.ulTimestampSec = 0;
			m_WaveFrame.ulTimestampMSec = 0;
			m_WaveFrame.iTotalOfWOBuffers = 100;

			bDoPlay = true;
		}

		if(bDoPlay)
		{
#ifdef _ENABLE_XAUDIO2
			if (!m_pOutXAudio2Dll->IsOpened())
#else
			if (!m_pOutAudioDll->IsOpened())
#endif
			{
				DWORD dwTime1 = timeGetTime();
#ifdef _ENABLE_XAUDIO2
				m_pOutXAudio2Dll->OpenDevice(m_iChannels, m_iSampleRate, m_iBits, m_WaveFrame.iLen, m_WaveFrame.iTotalOfWOBuffers);
#else
				m_pOutAudioDll->OpenDevice(m_iChannels, m_iSampleRate, m_iBits, m_WaveFrame.iLen, m_WaveFrame.iTotalOfWOBuffers);
#endif
				DWORD dwTime2 = timeGetTime();

				sprintf(szMsg, "OpenDevice - %d\n", dwTime2 - dwTime1);
				OutputDebugStringA(szMsg);
			}

			//if (m_ulRawAudioLen != m_WaveFrame.iLen)
			//{
				//sprintf(szMsg, "AVFilePlayer::PlayAudioFrame() - m_ulRawAudioLen != m_WaveFrame.iLen [%d , %d]\n", m_ulRawAudioLen , m_WaveFrame.iLen);
				//OutputDebugStringA(szMsg);
			//}

#ifdef _ENABLE_XAUDIO2
			m_pOutXAudio2Dll->Output(m_WaveFrame.pBuffer, m_ulRawAudioLen);  //m_WaveFrame.iLen
#else
			m_pOutAudioDll->Output(m_WaveFrame.pBuffer, m_ulRawAudioLen);  //m_WaveFrame.iLen
			m_pOutAudioDll->AddTimestamp(m_pCurAudioFrame->ulTimestampSec);
#endif


#ifdef _AUDIO_ONLY
			if (m_pCurAudioFrame)
				UpdatePosInfoMS(m_pCurAudioFrame->ulTimestampSec);
#else
			if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			{
#ifdef _ENABLE_MS
				if (m_pCurAudioFrame)
					UpdatePosInfoMS(m_pCurAudioFrame->ulTimestampSec);
#else
				UpdatePosInfo();
#endif
			}
#endif

			m_ulRawAudioLen = 0;
			m_pCurAudioFrame = 0;
		}
		else
			m_pCurAudioFrame = 0;

		m_ulAudioFrameCount++;
	}
}

void AVFilePlayer::PlayAudioFrame2()
{
	char szMsg[512];

#ifdef _ENABLE_XAUDIO2
	double dCurTimeCode = (float)(m_ulTimestamp + m_iAudioDelay) / 1000.0;
#else
	double dCurTimeCode = (m_ulTimestamp + 166) / 1000.0;   //improve dsound latency
#endif

	if (!m_bEnableAudio)
	{
		m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
		return;
	}

#ifdef _ENABLE_XAUDIO2
	if (m_pOutXAudio2Dll)
#else
	if (m_pOutAudioDll)
#endif
	{
		bool bDoPlay = false;

#ifdef _ENABLE_XAUDIO2
		if (m_pOutXAudio2Dll->IsOpened() && m_pOutXAudio2Dll->IsFulled())
#else
		if (m_pOutAudioDll->IsOpened() && m_pOutAudioDll->IsFulled() /*&& m_pAVPacketQueue->GetCount() == 0*/)
#endif
			return;
		
		if (!m_pAudioFrameBuffer)
			return;

		if (m_fSpeed > 1)
		{
			for (int i = 0; i < m_fSpeed; i++)
				m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
		}
		else
		{
			if (m_bDoGetNext)
			{
				m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
			}
		}

		if (!m_pCurAudioFrame)
			return;

		if (m_pCurAudioFrame)
		{
#ifdef _ENABLE_MEDIA_FILE_CTRL
			FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

			double dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_AUDIO, m_pCurAudioFrame->ulTimestampSec);
			double dLength = pCurFFMediaFileDll->GetLength();
#else
			double dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_AUDIO, m_pCurAudioFrame->ulTimestampSec);
			double dLength = m_pFFMediaFileDll->GetLength();
#endif

			if (dLength < dCurPos)
			{
				m_pCurAudioFrame = 0;
				return;
			}

			if (dCurTimeCode < dCurPos)
			{
				m_bDoGetNext = false;
				return;
			}

			if (m_bDoSkip)
			{
				//if (dCurPos < dCurTimeCode)
				{
					m_bDoGetNext = true;
					return;
				}
			}

#ifdef _AUDIO_ONLY
			m_bDoGetNext = true;
#else  //_AUDIO_ONLY
			m_bDoGetNext = true;

#endif  //_AUDIO_ONLY
		}

#if 0
		if (m_pCurAudioFrame->iLen < m_iSampleRate)
			m_iAudioFrameFactor = (float)(m_iSampleRate) / (float)m_pCurAudioFrame->iLen + 0.1;
		else
			m_iAudioFrameFactor = 1;
#else
		if (m_pCurAudioFrame->iLen < m_iSampleRate)
		{
#ifdef _ENABLE_XAUDIO2
			m_iAudioFrameFactor = ((float)(m_iSampleRate) / (float)m_pCurAudioFrame->iLen + 0.1) / 4;
#else
			m_iAudioFrameFactor = ((float)(m_iSampleRate) / (float)m_pCurAudioFrame->iLen + 0.1) / 2;
#endif

			if (m_iAudioFrameFactor < 0)
				m_iAudioFrameFactor = 1;
		}
		else
			m_iAudioFrameFactor = 1;
#endif
		if(m_iAudioFrameFactor == 0)
			m_iAudioFrameFactor = 1;

		int iIndex = m_ulAudioFrameCount % m_iAudioFrameFactor;

		memcpy(m_pAudioDeBuffer2 + m_ulRawAudioLen, m_pCurAudioFrame->pBuffer, m_pCurAudioFrame->iLen);
		m_ulRawAudioLen += m_pCurAudioFrame->iLen;
		m_ulLastAudioTimestamp = m_pCurAudioFrame->ulTimestampSec;

		if (m_pFilePlayerCallback)
		{
			if (m_pCurAudioFrame->iLen >= _AUDIO_BUFFER_SIZE)
			{
				sprintf(szMsg, "output - m_pCurAudioFrame->iLen >= _AUDIO_BUFFER_SIZE (%d)\n", m_pCurAudioFrame->iLen);
				OutputDebugStringA(szMsg);
			}

			memcpy(m_pAudioDeBuffer, m_pCurAudioFrame->pBuffer, m_pCurAudioFrame->iLen);
			m_pFilePlayerCallback->PlayAudio((unsigned char*)m_pAudioDeBuffer, m_pCurAudioFrame->iLen);
		}

#ifdef _ENABLE_XAUDIO2
		if (/*m_ulAudioFrameCount % m_iAudioFrameFactor == (m_iAudioFrameFactor - 1) || */m_ulRawAudioLen >= (m_iSampleRate / 3))
#else
		if (/*m_ulAudioFrameCount % m_iAudioFrameFactor == (m_iAudioFrameFactor - 1) || */m_ulRawAudioLen >= (m_iSampleRate/3))
#endif
		{
			m_WaveFrame.iBits = m_iBits;
			m_WaveFrame.iChannels = m_iChannels;
			m_WaveFrame.iSampleRate = m_iSampleRate;
			m_WaveFrame.pBuffer = (char*)m_pAudioDeBuffer2;
#ifdef _ENABLE_XAUDIO2
			m_WaveFrame.iLen = m_iSampleRate / 3;// m_pCurAudioFrame->iLen * m_iAudioFrameFactor * 4;
#else
			m_WaveFrame.iLen = m_iSampleRate/3;// m_pCurAudioFrame->iLen * m_iAudioFrameFactor;
			//m_ulRawAudioLen -= m_iSampleRate;
#endif
			m_WaveFrame.ulTimestampSec = 0;
			m_WaveFrame.ulTimestampMSec = 0;
			m_WaveFrame.iTotalOfWOBuffers = 100;

			m_ulAudioFrameCount = 0;

			bDoPlay = true;
		}

		if (bDoPlay)
		{
#ifdef _ENABLE_XAUDIO2
			if (!m_pOutXAudio2Dll->IsOpened())
#else
			if (!m_pOutAudioDll->IsOpened())
#endif
			{
				DWORD dwTime1 = timeGetTime();
#ifdef _ENABLE_XAUDIO2
				m_pOutXAudio2Dll->OpenDevice(m_iChannels, m_iSampleRate, m_iBits, m_WaveFrame.iLen, m_WaveFrame.iTotalOfWOBuffers);
#else
				m_pOutAudioDll->OpenDevice(m_iChannels, m_iSampleRate, m_iBits, m_WaveFrame.iLen, m_WaveFrame.iTotalOfWOBuffers);
#endif
				DWORD dwTime2 = timeGetTime();

				sprintf(szMsg, "OpenDevice - %d\n", dwTime2 - dwTime1);
				OutputDebugStringA(szMsg);
			}

#ifdef _ENABLE_XAUDIO2
			if (m_pOutXAudio2Dll->IsOpened())
				m_pOutXAudio2Dll->Output(m_WaveFrame.pBuffer, m_ulRawAudioLen);  //m_WaveFrame.iLen
#else
			m_pOutAudioDll->Output(m_WaveFrame.pBuffer, m_ulRawAudioLen);  //m_WaveFrame.iLen
			m_pOutAudioDll->AddTimestamp(m_pCurAudioFrame->ulTimestampSec);
#endif


#if _AUDIO_ONLY
			UpdatePosInfo();
#else
			if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			{
#ifdef _ENABLE_MS
				if (m_pCurAudioFrame)
					UpdatePosInfoMS(m_pCurAudioFrame->ulTimestampSec);
#else
				UpdatePosInfo();
#endif
			}
#endif

			m_ulRawAudioLen = 0;
			m_pCurAudioFrame = 0;
		}
		else
			m_pCurAudioFrame = 0;

		m_ulAudioFrameCount++;
	}
}

void AVFilePlayer::UpdatePosInfo(int64_t pts)
{
	char szCurPos[40];
	char szTotalLen[40];

	//double dLength = m_pFFAVCodecDLL->MFR_GetLength();
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dLength = pCurFFMediaFileDll->GetLength();
#else
	double dLength = m_pFFMediaFileDll->GetLength();
#endif

	double dCurPos = 0.0;

	if (m_iPlaybackMode == MFPM_BACKWARD)
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#else
		dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#endif
		//if(dCurPos < m_dLastPos)
			m_dLastPos = dCurPos;
	}
	else
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			dCurPos = pCurFFMediaFileDll->GetCurAudioFramePos();
		else
			dCurPos = pCurFFMediaFileDll->GetCurVideoFramePos();
#else
		if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			dCurPos = m_pFFMediaFileDll->GetCurAudioFramePos();
		else
			dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
#endif
	}

	int iNewPos = dCurPos;
	
	strcpy(szTotalLen,PosToTimeInfo(dLength, true));
	strcpy(szCurPos,PosToTimeInfo(dCurPos, true));

	if (m_dLastPos != dCurPos)
	{
		m_dLastPos = dCurPos;
	}

	if(iNewPos != m_lLastVideoPos  && dCurPos <= dLength)
	{
		m_bUpdatePos = false;

		if (m_iPlaybackMode == MFPM_BACKWARD && iNewPos < m_lLastVideoPos)
			m_bUpdatePos = true;
		else if(m_iPlaybackMode == MFPM_FORWARD && iNewPos > m_lLastVideoPos)
			m_bUpdatePos = true;

		if(m_lLastVideoPos == 0 && iNewPos != 0)
			m_bUpdatePos = true;

		if(m_bUpdatePos)
		{
			//char szMsg[512];
			//sprintf(szMsg,"%s\n",szCurPos);
			//OutputDebugStringA(szMsg);

			m_bUpdatePos = false;

			if(m_pFilePlayerCallback)
				m_pFilePlayerCallback->UpdatePosInfo(szCurPos,szTotalLen);
			//gUpdatePosInfo(szCurPos,szTotalLen);

			if(m_bEnableUpdatePos)
			{
				if(m_pFilePlayerCallback)
					m_pFilePlayerCallback->SetSliderCtrlPos(dCurPos);
				//gSetSliderCtrlPos(dCurPos);
			}
			m_lLastVideoPos = dCurPos;
		}

	}
	m_lTotalLen = dLength;
}

void AVFilePlayer::UpdatePosInfoMS(int64_t pts)
{
	char szMsg[512];
	char szCurPos[40];
	char szTotalLen[40];

	//double dLength = m_pFFAVCodecDLL->MFR_GetLength();
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dLength = pCurFFMediaFileDll->GetLength();
#else
	double dLength = m_pFFMediaFileDll->GetLength();
#endif

	double dCurPos = 0.0;

	if (m_iPlaybackMode == MFPM_BACKWARD)
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#else
		dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#endif
		//if(dCurPos < m_dLastPos)
		m_dLastPos = dCurPos;
	}
	else
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			dCurPos = pCurFFMediaFileDll->GetCurAudioFramePos();
		else
			//dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
			dCurPos = pCurFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#else
		if (m_iVideoFrameW == -1 || m_iVideoFrameH == -1)
			dCurPos = m_pFFMediaFileDll->GetCurAudioFramePos();
		else
			//dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
			dCurPos = m_pFFMediaFileDll->GetFramePos(MFT_VIDEO, pts);
#endif
	}

	strcpy(szTotalLen, PosToTimeInfo(dLength, false));
	m_dCurPosMs = dCurPos * 1000;
	strcpy(szCurPos, PosToTimeInfo(dCurPos, true));

	int iNewPos = dCurPos;

	if (m_dLastPos != dCurPos && dCurPos <= dLength)
	{
		m_bUpdatePos = false;

		if (m_iPlaybackMode == MFPM_BACKWARD && dCurPos < m_dLastPos)
			m_bUpdatePos = true;
		else if (m_iPlaybackMode == MFPM_FORWARD && dCurPos > m_dLastPos)
			m_bUpdatePos = true;

		if (m_lLastVideoPos == 0 && iNewPos != 0)
			m_bUpdatePos = true;

		if (m_bUpdatePos)
		{
			//sprintf(szMsg, "UpdatePos - %s\n", szCurPos);
			//OutputDebugStringA(szMsg);

			m_bUpdatePos = false;

			if (m_pFilePlayerCallback)
				m_pFilePlayerCallback->UpdatePosInfo(szCurPos, szTotalLen);
			//gUpdatePosInfo(szCurPos,szTotalLen);

			if (m_bEnableUpdatePos)
			{
				if (m_pFilePlayerCallback)
					m_pFilePlayerCallback->SetSliderCtrlPos(dCurPos);
				//gSetSliderCtrlPos(dCurPos);
			}
			m_lLastVideoPos = dCurPos;
			m_dLastPos = dCurPos;
		}
	}
}

char* AVFilePlayer::PosToTimeInfo(double dPos,bool bEnableMS)
{
	int iHour = dPos / (60 * 60);

	double dNum = dPos - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;

	if (bEnableMS)
	{
		double dNum2 = dNum1 - iSecond;
		int iMSec = dNum2 * 1000;

		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d:%03d", iHour, iMinute, iSecond, iMSec);
		else
			sprintf(m_szTime, "%02d:%02d:%03d", iMinute, iSecond, iMSec);
	}
	else
	{
		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d", iHour, iMinute, iSecond);
		else
			sprintf(m_szTime, "%02d:%02d", iMinute, iSecond);
	}
	return m_szTime;
}

void AVFilePlayer::SetVolume(int iValue)
{
	m_iVolume = iValue;
#ifdef _ENABLE_XAUDIO2
	m_pOutXAudio2Dll->SetVolume(iValue);
#else
	m_pOutAudioDll->SetVolume(iValue);
#endif
//	m_pVolumeCtrl->SetVolume(iValue);
}

void AVFilePlayer::SetSystemVolume(int iValue)
{
	m_iVolume = iValue;
	m_pVolumeCtrl->SetVolume(iValue);
}

void AVFilePlayer::UpdateGOPCount(bool bIsKeyFrame)
{
	if (bIsKeyFrame)
	{
		//if(m_iGOPCount > 0)
		//{
		//m_iGOPCount++;
		m_iGOP = m_iGOPCount;
		//}

		m_iGOPCount = 0;
	}
	m_iGOPCount++;
}

void AVFilePlayer::Seek(double dPos)
{
	/*
	char szMsg[512];

	EnterCriticalSection(&m_SeekCriticalSection);
	//m_SeekCriticalSection

	if (!m_bSeekIsFinished)
	{
		LeaveCriticalSection(&m_SeekCriticalSection);
		return;
	}
	m_bSeekIsFinished = false;

	double dLength = m_pFFMediaFileDll->GetLength();

	if (dPos > dLength)
	{
		m_bSeekIsFinished = true;
		LeaveCriticalSection(&m_SeekCriticalSection);
		return;
	}

	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	WaitForMultipleObjects(2, handles,true, INFINITE);
	::ResetEvent(m_hEvent2);

#ifdef _ENABLE_XAUDIO2
	if (m_pCurAudioCodecContext)
		m_pOutXAudio2Dll->Pause();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Stop();
#endif

	sprintf(szMsg, "AVFilePlayer::Seek 0 \n");
	OutputDebugStringA(szMsg);

	if (m_bSuspendDisplay)
	{
		sprintf(szMsg, "AVFilePlayer::Seek 1\n");
		OutputDebugStringA(szMsg);
		::SetEvent(m_hEvent2);
		m_bSeekIsFinished = true;
		LeaveCriticalSection(&m_SeekCriticalSection);
		return;
	}

	m_bSuspendDisplay = true;

	DWORD dwTime1 = timeGetTime();

	if (m_iPlaybackMode == MFPM_BACKWARD)
	{
		m_BackwardHelper2.Reset();

		m_pFFMediaFileDll->SetLastBackwardPos(dPos);
		m_pFFMediaFileDll->SeekBackward();

		m_lLastVideoPos = dPos;
		m_dLastPos = dPos;
		m_dLastPos = dPos;
	}
	else
	{
		m_pFFMediaFileDll->SeekTo(dPos);

		m_dLastPos = 0;
		m_lLastVideoPos = 0;
	}

	DWORD dwTime2 = timeGetTime();

	sprintf(szMsg, "Seek Spend time:%d\n", dwTime2 - dwTime1);
	OutputDebugStringA(szMsg);
	
	if(m_pAudioFrameBuffer)
		m_pAudioFrameBuffer->Reset();
	if(m_pVideoFrameBuffer)
		m_pVideoFrameBuffer->Reset();
	
	m_bUpdatePos = true;
	m_bDoAVSyn = true;
	m_ulAudioFrameCount = 0;
	m_ulVideoFrameCount = 0;

	m_ulLastVideoFrameTime = 0;
	m_ulLastAudioFrameTime = 0;

	m_pCurVideoFrame = 0;
	m_ulTimestamp = dPos * 1000;
	m_pCurAudioFrame = 0;
	m_bDoGetNext = true;
	m_ulLastTime = 0;

	m_dwLastTime = 0;

	m_AvgFPS.Reset();
	m_RealAvgFPS.Reset();
	m_AvgReSampleRate.Reset();

	m_pFFMediaFileDll->SetReSampleRate(1.0);

#ifdef _ENABLE_XAUDIO2
	m_pOutXAudio2Dll->ResetBuffer();
#else
	//m_pOutAudioDll->Close();
	m_pOutAudioDll->ResetBuffer();
#endif

	m_pAVPacketQueue->Reset();

	m_bSuspendDisplay = false;

	sprintf(szMsg, "AVFilePlayer::Seek 2\n");
	OutputDebugStringA(szMsg);
	::SetEvent(m_hEvent2);

#ifdef _ENABLE_XAUDIO2
	if (m_pCurAudioCodecContext)
		m_pOutXAudio2Dll->Continue();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Continue();
#endif

	m_bSeekIsFinished = true;
	LeaveCriticalSection(&m_SeekCriticalSection);
	*/
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dLength = pCurFFMediaFileDll->GetLength();
#else

	double dLength = m_pFFMediaFileDll->GetLength();
#endif

	if (dPos >= dLength)
	{
		unsigned long ulLength = dLength * 1000;
		//unsigned long ulStartTime = m_dStartTime * 1000;

		unsigned long ulPos = dPos * 1000;
		unsigned long ulNewPos = ulPos % ulLength;
		double dNewPos = ulNewPos / 1000.0;
		Seek2(dNewPos);
	}
	else
	{
		if (m_bIsAudioFile)
			Seek2(dPos + 0.01);
		else
			Seek2(dPos + 0.01);
	}
}

void AVFilePlayer::Seek2(double dPos)
{
	char szMsg[512];

//	EnterCriticalSection(&m_SeekCriticalSection);
	//m_SeekCriticalSection
	
	if (!m_bSeekIsFinished)
	{
		//LeaveCriticalSection(&m_SeekCriticalSection);
		return;
	}
	m_bSeekIsFinished = false;
	

#if 1
	HANDLE handles[2];
	handles[0] = m_hEvent1;
	handles[1] = m_hEvent3;
	//DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);
	DWORD dwRet = WaitForMultipleObjects(2, handles, true, INFINITE);  //1000

#else
	DWORD dwRet = ::WaitForSingleObject(m_hEvent3, 1000);
#endif

	::ResetEvent(m_hEvent2);
	switch (dwRet)
	{
		case WAIT_TIMEOUT:
			sprintf(szMsg, "AVFilePlayer::Seek - WaitForMultipleObjects timeout!! \n");
			OutputDebugStringA(szMsg);
			
			//LeaveCriticalSection(&m_SeekCriticalSection);
			::SetEvent(m_hEvent2);
			m_bSeekIsFinished = true;
			return;
			break;
		case WAIT_FAILED:
			DWORD dwError = GetLastError();
			sprintf(szMsg, "AVFilePlayer::Seek - WaitForMultipleObjects failed!! [%d]\n", dwError);
			OutputDebugStringA(szMsg);
			//LeaveCriticalSection(&m_SeekCriticalSection);
			::SetEvent(m_hEvent2);
			m_bSeekIsFinished = true;
			return;
			break;
	}

	if(!m_bIsAudioFile)
		m_bDoSkip = true;
	m_dSeekPos = dPos;

#ifdef _ENABLE_XAUDIO2
	if (m_pCurAudioCodecContext)
		m_pOutXAudio2Dll->Pause();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Stop();
#endif

	sprintf(szMsg, "AVFilePlayer::Seek 0 [%f]\n", dPos);
	OutputDebugStringA(szMsg);

	if (m_bSuspendDisplay)
	{
		sprintf(szMsg, "AVFilePlayer::Seek 1\n");
		OutputDebugStringA(szMsg);
		::SetEvent(m_hEvent2);
//		m_bSeekIsFinished = true;
//		LeaveCriticalSection(&m_SeekCriticalSection);
		m_bSeekIsFinished = true;
		return;
	}

	m_bSuspendDisplay = true;

	DWORD dwTime1 = timeGetTime();

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();
#endif

	if (m_iPlaybackMode == MFPM_BACKWARD)
	{
		m_BackwardHelper2.Reset();

#ifdef _ENABLE_MEDIA_FILE_CTRL
		pCurFFMediaFileDll->SetLastBackwardPos(dPos);
		pCurFFMediaFileDll->SeekBackward();
#else
		m_pFFMediaFileDll->SetLastBackwardPos(dPos);
		m_pFFMediaFileDll->SeekBackward();
#endif

		m_lLastVideoPos = dPos;
		m_dLastPos = dPos;
		m_dLastPos = dPos;
	}
	else
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		pCurFFMediaFileDll->SeekTo(dPos);
#else
		m_pFFMediaFileDll->SeekTo(dPos);
#endif

		m_dLastPos = 0;
		m_lLastVideoPos = 0;
	}

	DWORD dwTime2 = timeGetTime();

	sprintf(szMsg, "Seek Spend time:%d\n", dwTime2 - dwTime1);
	OutputDebugStringA(szMsg);

	if (m_pAudioFrameBuffer)
		m_pAudioFrameBuffer->Reset();
	if (m_pVideoFrameBuffer)
		m_pVideoFrameBuffer->Reset();

	m_bUpdatePos = true;
	m_bDoAVSyn = true;
	m_ulAudioFrameCount = 0;
	m_ulVideoFrameCount = 0;

	m_ulLastVideoFrameTime = 0;
	m_ulLastAudioFrameTime = 0;

	m_pCurVideoFrame = 0;
	m_ulTimestamp = dPos * 1000;
	m_pCurAudioFrame = 0;
	m_bDoGetNext = true;
	m_ulLastTime = 0;

	m_dwLastTime = 0;

	m_AvgFPS.Reset();
	m_RealAvgFPS.Reset();
	m_AvgReSampleRate.Reset();

#ifdef _ENABLE_MEDIA_FILE_CTRL
	pCurFFMediaFileDll->SetReSampleRate(1.0);
#else
	m_pFFMediaFileDll->SetReSampleRate(1.0);
#endif

#ifdef _ENABLE_XAUDIO2
	m_pOutXAudio2Dll->ResetBuffer();
#else
	//m_pOutAudioDll->Close();
	m_pOutAudioDll->ResetBuffer();
#endif


#ifdef _ENABLE_AVQUEUE2
	m_pAVPacketQueue2->Reset();
#else
	m_pAVPacketQueue->Reset();
#endif

	m_bSuspendDisplay = false;

#ifdef _ENABLE_XAUDIO2
	if (m_pCurAudioCodecContext)
		m_pOutXAudio2Dll->Continue();
#else
	if (m_pCurAudioCodecContext)
		m_pOutAudioDll->Continue();
#endif

	m_bSeekIsFinished = true;
	sprintf(szMsg, "AVFilePlayer::Seek 2\n");
	OutputDebugStringA(szMsg);
	::SetEvent(m_hEvent2);
//	LeaveCriticalSection(&m_SeekCriticalSection);
}

void AVFilePlayer::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

int AVFilePlayer::GetVolume()
{
#ifdef _ENABLE_XAUDIO2
	return m_pOutXAudio2Dll->GetVolume();
#else
	return m_pOutAudioDll->GetVolume();
#endif
	//return m_pVolumeCtrl->GetVolume();
}

int AVFilePlayer::GetSystemVolume()
{
	return m_pVolumeCtrl->GetVolume();
}

void AVFilePlayer::SetRepeat(bool bEnable)
{
	m_bIsRepeated = bEnable;
}

//fade in
void AVFilePlayer::EnableFadeIn(bool bEnable)
{
	m_bEnableFadeIn = bEnable;

	//m_pFFVideoFilterDll->CreateFadeInFilter(m_pCurVideoCodecContext);
/*
	if (m_pFFMediaFileDll->GPUIsEnabled())
		m_pFFVideoFilterDll->RestartFadeIn(m_pSWVideoCodecContext);
	else */

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->RestartFadeIn(m_pCurVideoCodecContext);
#endif

	if(m_bEnableFadeIn)
		m_bEnableFadeOut = false;
}

//fade out
void AVFilePlayer::EnableFadeOut(bool bEnable)
{
	m_bEnableFadeOut = bEnable;

	//m_pFFVideoFilterDll->CreateFadeOutFilter(m_pCurVideoCodecContext);
	/*
	if (m_pFFMediaFileDll->GPUIsEnabled())
		m_pFFVideoFilterDll->RestartFadeOut(m_pSWVideoCodecContext);
	else */

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->RestartFadeOut(m_pCurVideoCodecContext);
#endif

	if(m_bEnableFadeOut)
		m_bEnableFadeIn = false;
}

void AVFilePlayer::SetOutputVideoFormat(int iFormat)
{
	m_iOutputVideoFormat = iFormat;
}

void AVFilePlayer::EnableAudio(bool bEnable)
{
	m_bEnableAudio = bEnable;
}

float AVFilePlayer::GetVideoFPS()
{
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();
	return pCurFFMediaFileDll->GetVideoFPS();
#else
	return m_pFFMediaFileDll->GetVideoFPS();
#endif
}

int AVFilePlayer::GetTotalOfFrames()
{
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();
	return pCurFFMediaFileDll->GetTotalOfFrames();
#else
	return m_pFFMediaFileDll->GetTotalOfFrames();
#endif
}

int AVFilePlayer::GetVideoWidth()
{
	return m_iVideoFrameW;
}

int AVFilePlayer::GetVideoHeight()
{
	return m_iVideoFrameH;
}

int AVFilePlayer::GetVideoFrame(unsigned char* pBuffer, int* w, int* h,double* pos,char* szTime)
{
	bool bDo = true;

	int iRet = 0;
	while (bDo)
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		iRet = pCurFFMediaFileDll->GetNextFrameBuffer(&m_framebuf);
#else
		iRet = m_pFFMediaFileDll->GetNextFrameBuffer(&m_framebuf);
#endif

		if (iRet > 0)
		{
			if (m_framebuf.iType == MFT_VIDEO)
			{
				bDo = false;

				if (m_iOutputVideoFormat != VPF_I420)
				{
					switch (m_iOutputVideoFormat)
					{
					case VPF_YUY2:
						break;
					case VPF_RGB32:
						m_YUVConverter.I420ToARGB((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						memcpy(pBuffer, m_pConvVideoBuf, m_iVideoFrameW * m_iVideoFrameH * 4);
						break;
					case VPF_RGB24:
						m_YUVConverter.I420ToRGB24((unsigned char*)m_pCurVideoFrame->pBuffer, m_pConvVideoBuf, m_iVideoFrameW, m_iVideoFrameH);
						memcpy(pBuffer, m_pConvVideoBuf, m_iVideoFrameW * m_iVideoFrameH * 3);
						break;
					}
				}
				else
					memcpy(pBuffer, m_framebuf.pBuffer, m_framebuf.iLen);

#ifdef _ENABLE_MEDIA_FILE_CTRL
				double dCurPos = pCurFFMediaFileDll->GetCurVideoFramePos();
#else
				double dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
#endif
				*pos = dCurPos;

				strcpy(szTime,PosToTimeInfo(dCurPos,true));
			}
		}
		else
			bDo = false;
	}
	return iRet;
}

void AVFilePlayer::SpeedUp()
{
	_MAX_SPEED;

	/*
	if (m_iSpeed < _MAX_SPEED) //m_iSpeed
	{
		m_iSpeed *= 2;
		
		if (m_iSpeed > _MAX_SPEED)
			m_iSpeed = _MAX_SPEED;
	}
	*/
	if (m_fSpeed < _MAX_SPEED)
	{
		m_fSpeed *= 2;

		if (m_fSpeed > _MAX_SPEED)
			m_fSpeed = _MAX_SPEED;
	}
}

void AVFilePlayer::SpeedDown()
{
	/*
	if (m_iSpeed > 1)
	{
		m_iSpeed /=  2;

		if (m_iSpeed < 1)
			m_iSpeed = 1;
	}
	*/
	if (m_fSpeed > 0.25)
	{
		m_fSpeed /= 2;

		if (m_fSpeed < 0.25)
			m_fSpeed = 0.25;
	}
}

void AVFilePlayer::SetBrightness(int iValue)
{
	m_iBrightness = iValue;

	if (m_iBrightness > 100)
		m_iBrightness = 100;
	if(m_iBrightness < -100)
		m_iBrightness = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetBrightness(m_iBrightness);
#endif
}

void AVFilePlayer::SetContrast(int iValue)
{
	m_iContrast = iValue;

	if (m_iContrast > 100)
		m_iContrast = 100;
	if (m_iContrast < -100)
		m_iContrast = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetContrast(m_iContrast);
#endif
}

void AVFilePlayer::SetSaturation(int iValue)
{
	m_iSaturation = iValue;

	if (m_iSaturation > 100)
		m_iSaturation = 100;
	if (m_iSaturation < -100)
		m_iSaturation = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetSaturation(m_iSaturation);
#endif
}

void AVFilePlayer::SetHue(int iValue)
{
	m_iHue = iValue;

	if (m_iHue > 100)
		m_iHue = 100;
	if (m_iHue < -100)
		m_iHue = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetHue(m_iHue);
#endif
}

void AVFilePlayer::SetR(int iValue)
{
	m_iR = iValue;

	if (m_iR > 100)
		m_iR = 100;
	if (m_iR < -100)
		m_iR = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetGammaR(m_iR);
#endif
}

void AVFilePlayer::SetG(int iValue)
{
	m_iG = iValue;

	if (m_iG > 100)
		m_iG = 100;
	if (m_iG < -100)
		m_iG = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetGammaG(m_iG);
#endif
}

void AVFilePlayer::SetB(int iValue)
{
	m_iB = iValue;

	if (m_iB > 100)
		m_iB = 100;
	if (m_iB < -100)
		m_iB = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetGammaB(m_iB);
#endif
}

void AVFilePlayer::SetGray(int iValue)
{
	m_iGray = iValue;

	if (m_iGray > 100)
		m_iGray = 100;
	if (m_iGray < -100)
		m_iGray = -100;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetGray(m_iGray);
#endif
}

void AVFilePlayer::SetNegative(int iValue)
{
	m_iNegative = iValue;

	if (m_iNegative > 1)
		m_iNegative = 1;
	if (m_iNegative < 0)
		m_iNegative = 0;

#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetNegative(m_iNegative);
#endif
}

void AVFilePlayer::SetPlaybackMode(int iMode)
{
	m_iPlaybackMode = iMode;
}

int AVFilePlayer::GetPlaybackMode()
{
	return m_iPlaybackMode;
}

void AVFilePlayer::SetFadeDuration(float fDuration)
{
#ifdef _ENABLE_VIDEO_FILTER
	m_pFFVideoFilterDll->SetFadeDuration(fDuration);
#endif
}

void AVFilePlayer::SetupDXVA2(void* pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();
	if(pCurFFMediaFileDll)
		pCurFFMediaFileDll->SetupDXVA2(pD3DDevice);
#else
	m_pFFMediaFileDll->SetupDXVA2(pD3DDevice);
#endif
}

void AVFilePlayer::EnableGPU(bool bEnable)
{
#ifdef _ENABLE_MEDIA_FILE_CTRL
	m_bEnableGPU = bEnable;
#else
	m_pFFMediaFileDll->EnableGPU(bEnable);
#endif
}

bool AVFilePlayer::GPUIsEnabled()
{
#if _GPU_COPY_BACK
	return false;
#else

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	return pCurFFMediaFileDll->GPUIsEnabled();
#else
	return m_pFFMediaFileDll->GPUIsEnabled();
#endif

#endif
}

void AVFilePlayer::DoRepeat()
{
	m_bEOF = false;

	//do repeat
	if (m_bIsRepeated)
	{
		char szMsg[512];
		sprintf(szMsg, "AVFilePlayer::DoRepeat() - Do repeat - Seek(0) !!\n");
		OutputDebugStringA(szMsg);
		::SetEvent(m_hEvent1);
		Seek2(0);
		ResetFilter();

		if (m_pFilePlayerCallback)
			m_pFilePlayerCallback->Event(FPE_DO_REPEAT);
	}
	
}

void AVFilePlayer::DoReOpen()
{
	char szMsg[512];
	sprintf(szMsg, "AVFilePlayer::DoReOpen()!!\n");
	OutputDebugStringA(szMsg);

	Stop();
	Close();
	int iRet = Open(m_szFileName);
	Play();
}

void AVFilePlayer::ResetFilter()
{
	SetBrightness(0);
	SetContrast(0);
	SetSaturation(0);
	SetHue(0);
	SetR(0);
	SetG(0);
	SetB(0);
	SetNegative(0);
	SetGray(0);
}

bool AVFilePlayer::IsEOF(bool bEOF2)
{
	if(bEOF2)
		return m_bEOF2;
	return m_bEOF;
}

void AVFilePlayer::SetScale(float fW, float fH)
{
	m_fScaleW = fW;
	m_fScaleH = fH;
}

void AVFilePlayer::EnableReSampleRate(bool bEnable)
{
	m_bEnableReSampleRate = bEnable;
}

int AVFilePlayer::GetAudioInfo(int& iChannelLayout, int& iSampleRate, int& iNBSamples)
{
	if (m_pCurAudioCodecContext)
	{
		iChannelLayout = m_iChannelLayout;
		iNBSamples = m_iNBSamples;
		//iChannels = m_iChannels;
		iSampleRate = m_iSampleRate;
		//iBits = m_iBits;
		return 1;
	}
	return 0;
}

void AVFilePlayer::UpdateTimestamp()
{
	EnterCriticalSection(&m_TimestampCriticalSection);

	if(m_iDisplayIntervalMode == DIM_LOCAL)
	{
		
		bool bUpdate = false;
#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		if (pCurFFMediaFileDll->GPUIsEnabled())
#else
		if (m_pFFMediaFileDll->GPUIsEnabled())
#endif
		{
			if (!m_bSuspendDisplay)
				bUpdate = true;
		}
		else
		{
			if (m_pCurVideoCodecContext)
			{
				if (m_pVideoFrameBuffer && m_pVideoFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
					bUpdate = true;
			}
			else if (m_pAudioFrameBuffer && m_pAudioFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
				bUpdate = true;
		}

		

		if(bUpdate)
		{
			DWORD dwCurTime = timeGetTime();

			//if (m_dwLastTime != 0 && dwCurTime >= m_dwLastTime + 30)
			if (m_dwLastTime != 0 && dwCurTime - m_dwLastTime >= 20)
			{
				m_ulTimestamp += (dwCurTime - m_dwLastTime);
				m_dwLastTime = dwCurTime;
			}
			else if (m_dwLastTime == 0)
				m_dwLastTime = dwCurTime;
		}
	}
	LeaveCriticalSection(&m_TimestampCriticalSection);
}

void AVFilePlayer::UpdateGlobalTimeCode(double dTimeCode)
{
	if (dTimeCode < m_dStartTime && dTimeCode != 0)
		return;

	double dCurTimeCode = dTimeCode - m_dStartTime;

#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	double dLength = pCurFFMediaFileDll->GetLength();
#else
	double dLength = m_pFFMediaFileDll->GetLength();
#endif

	unsigned long ulCurTimestamp = dCurTimeCode * 1000;
	unsigned long ulLength = dLength * 1000;
	unsigned long ulStartTime = m_dStartTime * 1000;
	//m_dGlobalTimeCode = dTimeCode;
	
	/*
	if (m_iDisplayIntervalMode == DIM_GOBAL)
	{
		if (ulCurTimestamp > 0)
		{
			if (ulCurTimestamp > ulLength)
				m_ulTimestamp = ulCurTimestamp % ulLength;
			else
				m_ulTimestamp = ulCurTimestamp;
		}
		else
		{
			m_ulTimestamp = 0;
			Seek(0);
		}
	}
	*/

	bool bUpdate = false;
	switch (m_iGlobalTimeCodeStatus)
		{
			case GTCS_NORMAL:
			{
				if (m_pCurVideoCodecContext)
				{
					if (m_iDisplayIntervalMode == DIM_GOBAL)
					{
						if (ulCurTimestamp > 0)
						{
							if (ulCurTimestamp > ulLength)
							{
								unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
								double dNextTimeCode = ulNextTimestamp / 1000.0;

								long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
								if (lOffset > 300)//1000
									m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
							}
							else
							{
								long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
								if (lOffset > 300)//1000
									m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
								/*
								else
								{
									if(m_ulTimestamp > ulCurTimestamp && lOffset > 300)
										m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
								}
								*/
							}
						}
					}

					//if (m_pFFMediaFileDll->GPUIsEnabled())
					{
						if (!m_bSuspendDisplay)
							bUpdate = true;
					}
					
					//else
					//{
						//if (m_pVideoFrameBuffer /*&& m_pVideoFrameBuffer->GetTotal() > 0 */&& !m_bSuspendDisplay)
							//bUpdate = true;
					//}
				}
				else if (m_bIsAudioFile /*&& m_pAudioFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay*/)
				{
					if (m_iDisplayIntervalMode == DIM_GOBAL)
					{
						if (ulCurTimestamp > 0)
						{
							if (ulCurTimestamp > ulLength)
							{
								unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
								double dNextTimeCode = ulNextTimestamp / 1000.0;

								long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
								if (lOffset > 300)//1000
									m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
							}
							else
							{
								long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
								if (lOffset > 300)//1000
									m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
								/*
								else
								{
								if(m_ulTimestamp > ulCurTimestamp && lOffset > 300)
								m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
								}
								*/
							}
						}
						else
						{
							m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
							m_dwLastTime = 0;
							m_ulTimestamp = 0;
						}

						if (!m_bSuspendDisplay)
							bUpdate = true;
					}

					bUpdate = true;
				}

				if (m_iGlobalTimeCodeStatus == GTCS_NORMAL)
				{
					if (m_iDisplayIntervalMode == DIM_GOBAL && bUpdate)
					{
						if (ulCurTimestamp > 0)
						{
							if (ulCurTimestamp > ulLength)
							{
								unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
								double dNextTimeCode = ulNextTimestamp / 1000.0;
								m_ulTimestamp = ulNextTimestamp;
							}
							else
								m_ulTimestamp = ulCurTimestamp;
						}
						else
						{
							m_ulTimestamp = 0;
							//Seek(0);
							m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
						}
					}
				}
			}
			break;
		case GTCS_DO_SEEK:
			{
				m_iGlobalTimeCodeStatus = GTCS_SEEKING;

				if (ulCurTimestamp > ulLength)
				{
					unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
					double dNextTimeCode = ulNextTimestamp / 1000.0;

					long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
					if (lOffset > 300)//1000
					{
						Seek2(dNextTimeCode);
						m_ulTimestamp = ulNextTimestamp;
					}
				}
				else
				{
					long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
					if (lOffset > 300)//1000
					{
						Seek2(dCurTimeCode);
						m_ulTimestamp = ulCurTimestamp;
					}
					else
					{
						if (m_bIsAudioFile)
							Seek2(dCurTimeCode + 0.01);
						else
							Seek2(dCurTimeCode);
					}
				}
				m_iGlobalTimeCodeStatus = GTCS_NORMAL;
			}
			break;
		case GTCS_SEEKING:
			break;
	}
	

/*
	int iRet = CheckGlobalTimeCodeStatus(dTimeCode);
	if (iRet == GTCS_DO_SEEK)
		UpdateGlobalTimeCode2(dTimeCode);
		*/
}
/*
void AVFilePlayer::UpdateGlobalTimeCode2(double dTimeCode)
{
	if (dTimeCode < m_dStartTime)
		return;

	double dCurTimeCode = dTimeCode - m_dStartTime;
	double dLength = m_pFFMediaFileDll->GetLength();

	unsigned long ulCurTimestamp = dCurTimeCode * 1000;
	unsigned long ulLength = dLength * 1000;
	unsigned long ulStartTime = m_dStartTime * 1000;
	//m_dGlobalTimeCode = dTimeCode;

	bool bUpdate = false;
	switch (m_iGlobalTimeCodeStatus)
	{
		case GTCS_NORMAL:
		{
			if (m_pCurVideoCodecContext)
			{
				if (m_iDisplayIntervalMode == DIM_GOBAL)
				{
					if (ulCurTimestamp > 0)
					{
						if (ulCurTimestamp > ulLength)
						{

							unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
							double dNextTimeCode = ulNextTimestamp / 1000.0;

							long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
							if (lOffset > 1000)
								m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
						}
						else
						{
							long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
							if (lOffset > 1000)
								m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
						}
					}
				}

				if (m_pFFMediaFileDll->GPUIsEnabled())
				{
					if (!m_bSuspendDisplay)
						bUpdate = true;
				}
				else
				{
					if (m_pVideoFrameBuffer && m_pVideoFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
						bUpdate = true;
				}
			}
			else if (m_pAudioFrameBuffer && m_pAudioFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
				bUpdate = true;

			if (m_iGlobalTimeCodeStatus == GTCS_NORMAL)
			{
				if (m_iDisplayIntervalMode == DIM_GOBAL && bUpdate)
				{
					if (ulCurTimestamp > 0)
					{
						if (ulCurTimestamp > ulLength)
						{
							unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
							double dNextTimeCode = ulNextTimestamp / 1000.0;
							m_ulTimestamp = ulNextTimestamp;
						}
						else
							m_ulTimestamp = ulCurTimestamp;
					}
					else
					{
						m_ulTimestamp = 0;
						//Seek(0);
						m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
					}
				}
			}
		}
		break;
		case GTCS_DO_SEEK:
		{
			m_iGlobalTimeCodeStatus = GTCS_SEEKING;

			if (ulCurTimestamp > ulLength)
			{
				unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
				double dNextTimeCode = ulNextTimestamp / 1000.0;

				long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
				if (lOffset > 1000)
				{
					Seek(dNextTimeCode);
					m_ulTimestamp = ulNextTimestamp;
				}
			}
			else
			{
				long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
				if (lOffset > 1000)
				{
					Seek(dCurTimeCode);
					m_ulTimestamp = ulCurTimestamp;
				}
				else
					Seek(dCurTimeCode);
			}
			m_iGlobalTimeCodeStatus = GTCS_NORMAL;
		}
		break;
		case GTCS_SEEKING:
			break;
	}
}

int AVFilePlayer::CheckGlobalTimeCodeStatus(double dTimeCode)
{
	if (dTimeCode < m_dStartTime)
		return -1;

	double dCurTimeCode = dTimeCode - m_dStartTime;
	double dLength = m_pFFMediaFileDll->GetLength();

	unsigned long ulCurTimestamp = dCurTimeCode * 1000;
	unsigned long ulLength = dLength * 1000;
	unsigned long ulStartTime = m_dStartTime * 1000;

	bool bUpdate = false;
	switch (m_iGlobalTimeCodeStatus)
	{
		case GTCS_NORMAL:
		{
			if (m_pCurVideoCodecContext)
			{
				if (m_iDisplayIntervalMode == DIM_GOBAL)
				{
					if (ulCurTimestamp > 0)
					{
						if (ulCurTimestamp > ulLength)
						{

							unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
							double dNextTimeCode = ulNextTimestamp / 1000.0;

							long lOffset = abs((long)ulNextTimestamp - (long)m_ulTimestamp);
							if (lOffset > 1000)
								m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
						}
						else
						{
							long lOffset = abs((long)ulCurTimestamp - (long)m_ulTimestamp);
							if (lOffset > 1000)
								m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
						}
					}
				}

				if (m_pFFMediaFileDll->GPUIsEnabled())
				{
					if (!m_bSuspendDisplay)
						bUpdate = true;
				}
				else
				{
					if (m_pVideoFrameBuffer && m_pVideoFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
						bUpdate = true;
				}
			}
			else if (m_pAudioFrameBuffer && m_pAudioFrameBuffer->GetTotal() > 0 && !m_bSuspendDisplay)
				bUpdate = true;

			if (m_iGlobalTimeCodeStatus == GTCS_NORMAL)
			{
				if (m_iDisplayIntervalMode == DIM_GOBAL && bUpdate)
				{
					if (ulCurTimestamp > 0)
					{
						if (ulCurTimestamp > ulLength)
						{
							unsigned long ulNextTimestamp = ulCurTimestamp % ulLength;
							double dNextTimeCode = ulNextTimestamp / 1000.0;
							m_ulTimestamp = ulNextTimestamp;
						}
						else
							m_ulTimestamp = ulCurTimestamp;
					}
					else
					{
						m_ulTimestamp = 0;
						//Seek(0);
						m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
					}
				}
			}
		}
		break;
		case GTCS_DO_SEEK:
			break;
		case GTCS_SEEKING:
			break;
	}

	return m_iGlobalTimeCodeStatus;
}
*/

void AVFilePlayer::CheckPosInfo()
{
	if (m_iPlaybackMode == MFPM_FORWARD && DIM_GOBAL == m_iDisplayIntervalMode && m_iGlobalTimeCodeStatus == GTCS_NORMAL && m_dSeekPos == 0.0)
	{
#ifdef _ENABLE_MEDIA_FILE_CTRL
		FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

		double dCurPos = pCurFFMediaFileDll->GetCurVideoFramePos();
#else
		double dCurPos = m_pFFMediaFileDll->GetCurVideoFramePos();
#endif

		if (m_ulTimestamp > 100 && m_ulVideoFrameCount == 0 && dCurPos > 1.0)
		{
			char szMsg[512];
//			sprintf(szMsg, "#AVF%d AVFilePlayer::CheckPosInfo() - m_ulTimestamp > 100 && m_ulVideoFrameCount == 0 [%d , %d , %5.3f]\n", m_iFileNameNum, m_ulTimestamp, m_ulVideoFrameCount, dCurPos);
//			OutputDebugStringA(szMsg);

			m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
			m_dwLastTime = 0;
			m_ulTimestamp = 0;
		}
		/*
		if (m_ulTimestamp > 300 && m_ulVideoFrameCount == 0 && dCurPos > 1.0)
		{
			char szMsg[512];
			sprintf(szMsg, "#AVF%d AVFilePlayer::CheckPosInfo() - m_ulTimestamp > 300 && m_ulVideoFrameCount == 0 [%d , %d , %5.3f]\n", m_iFileNameNum, m_ulTimestamp, m_ulVideoFrameCount, dCurPos);
			OutputDebugStringA(szMsg);

			m_iGlobalTimeCodeStatus = GTCS_DO_SEEK;
			m_dwLastTime = 0;
			m_ulTimestamp = 0;
		}
		*/
	}
}

void AVFilePlayer::SetDisplayIntervalMode(int iMode)
{
	m_iDisplayIntervalMode = iMode;
}

void AVFilePlayer::SetStartTime(double dTime)
{
	m_dStartTime = dTime;
}

void* AVFilePlayer::GetD3DDevice()
{
	return m_pD3DDevice;
}

bool AVFilePlayer::IsAudioFile()
{
	return m_bIsAudioFile;
}

void AVFilePlayer::SetProcessFPS(float fFPS)
{
#ifdef _ENABLE_LOADING_CTRL
	m_LoadingCtrl.SetProcessFPS(fFPS);
#endif	
}

void AVFilePlayer::ResetBuffer()
{
#ifdef _ENABLE_MEDIA_FILE_CTRL
	FFMediaFileDll* pCurFFMediaFileDll = m_pMediaFileCtrl->GetCurSelObj();

	Reset();
	//m_pFFAVCodecDLL->MFR_GetVideoFrameSize(&m_iVideoFrameW,&m_iVideoFrameH);
	//AVCodecContext* pVideoCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_VIDEO);
	pCurFFMediaFileDll->GetVideoFrameSize(&m_iVideoFrameW, &m_iVideoFrameH);
	AVCodecContext* pVideoCodecContext = pCurFFMediaFileDll->GetAVCodecContext(MFT_VIDEO);
	if (pVideoCodecContext)
	{
		m_pCurVideoCodecContext = pVideoCodecContext;
		//m_fVideoFPS = m_pFFAVCodecDLL->MFR_GetVideoFPS();

		m_bIsAudioFile = false;

		//m_fVideoFPS = m_pFFMediaFileDll->GetVideoFPS() * 1.007;
		m_fVideoFPS = pCurFFMediaFileDll->GetVideoFPS();
		m_fVideoInterval = 1000.0 / m_fVideoFPS;

		m_FrameRateCtrl.SetInterval(m_fVideoInterval);

		m_iCoded_Height = pVideoCodecContext->coded_height;
		m_iCoded_Width = pVideoCodecContext->coded_width;

#ifdef _ENABLE_LOADING_CTRL
		m_LoadingCtrl.SetDataFPS(m_fVideoFPS);
#endif
	}
	else
		m_pCurVideoCodecContext = 0;

	if (pCurFFMediaFileDll->GPUIsEnabled())
		m_pSWVideoCodecContext = pCurFFMediaFileDll->GetAVCodecContext(MFT_VIDEO_SW);

	//AVCodecContext* pAudioCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_AUDIO);
	AVCodecContext* pAudioCodecContext = pCurFFMediaFileDll->GetAVCodecContext(MFT_AUDIO);
	if (pAudioCodecContext)
	{
		m_pCurAudioCodecContext = pAudioCodecContext;
		m_iChannels = pAudioCodecContext->channels;
		m_iSampleRate = pAudioCodecContext->sample_rate;
		m_iChannelLayout = pAudioCodecContext->channel_layout;
		m_iNBSamples = pAudioCodecContext->frame_size;

		if (!pVideoCodecContext)
			m_bIsAudioFile = true;

		if (pAudioCodecContext->sample_fmt == AV_SAMPLE_FMT_FLTP)
			m_iBits = 16;
		else
		{
			switch (pAudioCodecContext->sample_fmt)
			{
			case AV_SAMPLE_FMT_U8:
			case AV_SAMPLE_FMT_U8P:
				m_iBits = 8;
				break;
			case AV_SAMPLE_FMT_S16:
			case AV_SAMPLE_FMT_S16P:
				m_iBits = 16;
				break;
				//		case SAMPLE_FMT_S24:
				//			*iBits = 24;
				//			break;
			case AV_SAMPLE_FMT_S32:
			case AV_SAMPLE_FMT_S32P:
				m_iBits = 32;
				break;
			default:
			{
				m_iBits = pAudioCodecContext->bit_rate / 1000;
			}
			}
		}
	}

	//m_pFFAVCodecDLL->SetDePixelFormat(m_iCodecID,AV_PIX_FMT_YUV420P);

	int iBufferLen = 0;
	if (m_iVideoFrameW != -1 && m_iVideoFrameH != -1)
		iBufferLen = m_iVideoFrameW * m_iVideoFrameH * 3 / 2;
	else
		iBufferLen = _AUDIO_BUFFER_SIZE;
	m_framebuf.pBuffer = new unsigned char[iBufferLen];

	//		m_pAudioDeBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
	//m_pAudioDeBuffer2 = new unsigned char[_AUDIO_BUFFER_SIZE * 2];

	if (m_iCoded_Height > 0 && m_iCoded_Width > 0)
	{
		m_pRawVideoBuf = new unsigned char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
		m_pNV12RawVideoBuf = new unsigned char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
	}
	else
	{
		m_pRawVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
		m_pNV12RawVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
	}

	m_pVideoFrameBuffer = new FrameBufferManager;
	//m_pAudioFrameBuffer = new FrameBufferManager;
	m_pRawVideoFrame = new StreamFrame;
	//m_pRawAudioFrame = new StreamFrame;
	m_pRawVideoFrame2 = new RawVideoFrame;

	//m_pRawAudioFrame->pBuffer = new char[_AUDIO_BUFFER_SIZE];
	if (m_iCoded_Height > 0 && m_iCoded_Width > 0)
	{
		m_pRawVideoFrame->pBuffer = new char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
		m_pRawVideoFrame2->pBuffer = new char[m_iCoded_Height * m_iCoded_Width * 3 / 2];
	}
	else
	{
		m_pRawVideoFrame->pBuffer = new char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
		m_pRawVideoFrame2->pBuffer = new char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
	}

	if (m_pCurAudioCodecContext)
	{
		m_pRawAudioFrame = new StreamFrame;
		m_pAudioFrameBuffer = new FrameBufferManager;

		m_pAudioDeBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
		m_pAudioDeBuffer2 = new unsigned char[_AUDIO_BUFFER_SIZE * 2];
		m_pRawAudioFrame->pBuffer = new char[_AUDIO_BUFFER_SIZE];

		m_audioframebuf.pBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];

		if (!m_pAudioFrameBuffer->IsReady())
			m_pAudioFrameBuffer->Alloc(_AUDIO_BUFFER_SIZE, _BUFFER_NUM * 2);
	}

	if (!m_pVideoFrameBuffer->IsReady())
	{
#if _LOW_MEM_MODE
		m_pVideoFrameBuffer->Alloc(iBufferLen + 1000, _RAW_BUFFER_NUM);//32
#else
		m_pVideoFrameBuffer->Alloc(iBufferLen + 1000, _BUFFER_NUM);//32
#endif
	}

	//if(!m_pAudioFrameBuffer->IsReady())
	//m_pAudioFrameBuffer->Alloc(_AUDIO_BUFFER_SIZE, _BUFFER_NUM);

	if (m_pConvVideoBuf)
	{
		delete m_pConvVideoBuf;
		m_pConvVideoBuf = 0;
	}

	switch (m_iOutputVideoFormat)
	{
	case VPF_I420:
		m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3 / 2];
		break;
	case VPF_YUY2:
		m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 2];
		break;
	case VPF_RGB32:
		m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 4];
		break;
	case VPF_RGB24:
		m_pConvVideoBuf = new unsigned char[m_iVideoFrameW * m_iVideoFrameH * 3];
		break;
	}

	//m_ulStartTimestamp = timeGetTime();

	if (pCurFFMediaFileDll->GetAudioLength() > 0)
		m_bDoAVSyn = true;
	else
		m_bDoAVSyn = false;

#ifdef _ENABLE_MS
	UpdatePosInfoMS();
#else
	UpdatePosInfo();
#endif

	if (m_pFilePlayerCallback)
	{
#ifdef _ENABLE_MS
		double dLength = pCurFFMediaFileDll->GetLength();
		m_pFilePlayerCallback->SetSliderCtrlRange(dLength);
#else
		m_pFilePlayerCallback->SetSliderCtrlRange(m_lTotalLen);
#endif
	}
	//gSetSliderCtrlRange(m_lTotalLen);

#ifdef _ENABLE_VIDEO_FILTER
	if (pCurFFMediaFileDll->GPUIsEnabled())
	{
		if (m_pSWVideoCodecContext)
		{
			m_pFFVideoFilterDll->CreateEQFilter(m_pSWVideoCodecContext);
			m_pFFVideoFilterDll->CreateHueFilter(m_pSWVideoCodecContext);
			m_pFFVideoFilterDll->CreateLutYUVFilter(m_pSWVideoCodecContext);
			m_pFFVideoFilterDll->CreateLutRGBFilter(m_pSWVideoCodecContext);
		}
	}
	else
	{
		if (m_pCurVideoCodecContext)
		{
			m_pFFVideoFilterDll->CreateEQFilter(m_pCurVideoCodecContext);
			m_pFFVideoFilterDll->CreateHueFilter(m_pCurVideoCodecContext);
			m_pFFVideoFilterDll->CreateLutYUVFilter(m_pCurVideoCodecContext);
			m_pFFVideoFilterDll->CreateLutRGBFilter(m_pCurVideoCodecContext);
		}
	}
#endif

#endif
}

DWORD WINAPI VideoThreadLoop(LPVOID lpParam)
{
	AVFilePlayer* pObj = (AVFilePlayer*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->VideoThreadEvent();
	}
	return 0;
}

DWORD WINAPI AudioThreadLoop(LPVOID lpParam)
{
	AVFilePlayer* pObj = (AVFilePlayer*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->AudioThreadEvent();
	}
	return 0;
}

DWORD WINAPI ReOpenThread(LPVOID lpParam)
{
	AVFilePlayer* pObj = (AVFilePlayer*)lpParam;
	pObj->DoReOpen();
	return 0;
}

DWORD WINAPI TimestampThread(LPVOID lpParam)
{
	AVFilePlayer* pObj = (AVFilePlayer*)lpParam;
	while (pObj->IsAlive())
	{
		pObj->UpdateTimestamp();
		Sleep(1);
	}
	return 0;
}

LRESULT WINAPI _DisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	LRESULT result = 0;
	switch (msg) 
	{
		case WM_CLOSE:
			{
			   DestroyWindow(hwnd);
			}
		  	break;
		case WM_PAINT:
			{
				PAINTSTRUCT	ps;
	            HDC hdc = BeginPaint(hwnd, &ps);
		        EndPaint(hwnd, &ps);
			}
			break;
		case WM_CREATE:
			DragAcceptFiles(hwnd,TRUE);
			break;
		case WM_WINDOWPOSCHANGING:
			break;
		case WM_LBUTTONDOWN:
		   break;
	    case WM_LBUTTONUP:
		   break;
	    case WM_MOUSEMOVE:
		   break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_KEYDOWN:
			{
			}
			break;
		case WM_COPYDATA:
			{
			}
			break;
		case WM_DROPFILES:
			{
				/*
				char szFile[MAX_PATH];
				TCHAR lpszFile[MAX_PATH] = {0};
				UINT uFile = 0;
				HDROP hDrop = (HDROP)wParam;

				uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);

				lpszFile[0] = '\0';
				if (DragQueryFile(hDrop, 0, lpszFile, MAX_PATH))
					;
				DragFinish(hDrop);

				if(g_pUIController)
					g_pUIController->OpenFileW(lpszFile);
					*/
			}
			break;
        default:
			{
			    result = DefWindowProc( hwnd, msg, wParam, lParam );
			    break;
			}
	}

	return result;
}

void AVFilePlayer::SaveLog(CString strLog)
{
#ifdef _DISABLE_LOG
#else
	CStdioFile sfLog;
	CFileFind fFind;
	CString strLogPath = L"C:\\ProgramData\\Log.csv";
	//CString strLogPath = L"C:\\ProgramData\\Log.txt";

	if (!fFind.FindFile(strLogPath))
		sfLog.Open(strLogPath, CFile::modeCreate | CFile::modeReadWrite);
	else
	{
		sfLog.Open(strLogPath, CFile::modeReadWrite);
		sfLog.SeekToEnd();
	}
	sfLog.WriteString(strLog + L"\n");
	sfLog.Close();
#endif
}

void AVFilePlayer::SetReferenceTimeCode(double dNewRefTime)
{
	m_dReferenceTime = dNewRefTime;
	m_bRefTimeUpdate = true;

}

void AVFilePlayer::EnableSuspendDisplay(bool bEnable)
{
	m_bSuspendDisplay = bEnable;
}

void AVFilePlayer::SetAudioDelay(int iValue)
{
	m_iAudioDelay = iValue;
}

void AVFilePlayer::SetReSampleRateOffset(float fOffset)
{
	m_fReSampleRateOffset = fOffset;
}
