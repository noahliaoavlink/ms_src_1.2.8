#include "stdafx.h"
#include "StreamPlayer.h"

#include "../../../Include/SCreateWin.h"
#include "../../../Include/RenderCommon.h"

#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")

#define _VIDEO_BUFFER_SIZE 1024*1024*2 //2048*2048 //4096 * 4096
//#define _AUDIO_BUFFER_SIZE 	409600
#define _AUDIO_BUFFER_SIZE  102400
#define _MAX_VIDEO_WIDTH 2048
#define _MAX_VIDEO_HEIGHT 2048

LRESULT WINAPI _DisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
unsigned int __stdcall _DecodeThreadProc(void* lpvThreadParm);

StreamPlayer::StreamPlayer()
{
	m_pRtspClientDll = 0;
	m_pFFAVCodecDLL = 0;
//	m_pOutVideoDll = 0;
	m_pOutXAudio2Dll = 0;
//	m_hDisplayWnd = 0;
	m_pRtspObj = 0;
	m_pVideoFrameBuffer = 0;
	m_pAudioFrameBuffer = 0;
	m_bIsAlive = false;
	m_pDeBuffer = 0;
	m_pAudioDeBuffer = 0;
	//m_ulAudioFrameCount = 0;
	m_iAudioFrameFactor = 0;
	m_ulRawAudioLen = 0;

	m_pSmoothCtrl = new SmoothCtrl;
	m_pSmoothCtrl->SetAvgFPS(30);

	m_AvgFps.SetMax(100);

	m_AvgRCVFps.SetMax(100);

	Reset();

	m_pFilePlayerCallback = 0;
	m_iOutputVideoFormat = VPF_I420;
	m_bEnableAudio = true;
	m_pConvVideoBuf = 0;
	m_iConvBufWidth = 0;
	m_iConvBufHeight = 0;

	m_bPresent = false;
	m_bIsOpened = false;
}

StreamPlayer::~StreamPlayer()
{
	if (m_pRtspObj)
	{
		m_pRtspClientDll->Close(m_pRtspObj);
		m_pRtspClientDll->Destroy(m_pRtspObj);
	}

	if (m_pOutXAudio2Dll)
	{
		m_pOutXAudio2Dll->Close();
		delete m_pOutXAudio2Dll;
	}

//	if (m_pOutVideoDll)
//		delete m_pOutVideoDll;

	if (m_pFFAVCodecDLL)
	{
		m_pFFAVCodecDLL->CloseVideoDecoder(m_ulCodecID);
		//m_pFFAVCodecDLL->CloseConverter(m_ulCodecID);
		delete m_pFFAVCodecDLL;
	}

	if (m_pVideoFrameBuffer)
		delete m_pVideoFrameBuffer;

	if (m_pAudioFrameBuffer)
		delete m_pAudioFrameBuffer;

	if (m_pSmoothCtrl)
		delete m_pSmoothCtrl;

	if (m_pDeBuffer)
		delete m_pDeBuffer;

	if (m_pAudioDeBuffer)
		delete m_pAudioDeBuffer;

	if (m_pConvVideoBuf)
		delete m_pConvVideoBuf;
}

void StreamPlayer::Init(FilePlayerCallback* pFilePlayerCallback)
{
	RECT rect;
	char szWinName[256];

	m_pFilePlayerCallback = pFilePlayerCallback;

	m_pOutXAudio2Dll = new OutXAudio2Dll;

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
	/*
	m_pOutVideoDll = new OutVideoDll;

	if (m_pOutVideoDll)
	{
		sprintf(szWinName, "RTSPPlayerWnd-%d", GetTickCount());
		m_hDisplayWnd = CreateWin(_DisplayWinProc, szWinName);

		SetParent(m_hDisplayWnd, hWnd);
		GetClientRect(hWnd, &rect);

		SetWinBKColor(m_hDisplayWnd, RGB(0, 0, 0));
		SetWinSize(m_hDisplayWnd, rect.right, rect.bottom, false);
		SetWinPos(m_hDisplayWnd, 0, 0, true);

		//out video
		bool bLoadOutVideoRet = m_pOutVideoDll->LoadLib();
		if (bLoadOutVideoRet)
			m_pOutVideoDll->Init(0, m_hDisplayWnd);
		else
			MessageBoxA(NULL, "Load OutVideo.dll failed!!", "Error", MB_OK);
	}
	*/
	m_pFFAVCodecDLL = new FFAVCodecDLL;

	if (m_pFFAVCodecDLL)
	{
		m_pFFAVCodecDLL->Init();
		m_ulCodecID = m_pFFAVCodecDLL->CreateCodec();

		m_pFFAVCodecDLL->SetEnPixelFormat(m_ulCodecID, PIX_FMT_YUV420P);
		m_pFFAVCodecDLL->SetDePixelFormat(m_ulCodecID, PIX_FMT_YUV420P);

		//	m_pFFAVCodecDLL->OpenConverter(m_ulCodecID, PIX_FMT_YUV420P, PIX_FMT_YUYV422, 2592, 1592);
		m_pFFAVCodecDLL->EnableMT(m_ulCodecID, true);
	}

	m_pRtspClientDll = new RtspClientDll;

	if (m_pRtspClientDll)
		m_pRtspObj = m_pRtspClientDll->Create(this);

	m_pVideoFrameBuffer = new FrameBufferManager;
	m_pAudioFrameBuffer = new FrameBufferManager;

	m_pDeBuffer = new unsigned char[_MAX_VIDEO_WIDTH * _MAX_VIDEO_HEIGHT * 3 / 2];
	m_pAudioDeBuffer = new unsigned char[_AUDIO_BUFFER_SIZE];
}

void StreamPlayer::Open(char* szUrl)
{
	/*
	m_iGOPCount = 0;
	m_iGOP = 0;
	m_fFPS = 0.0;
	m_ulFrameCount = 0;
	*/
	Reset();

	if (m_pRtspObj)
	{
		char szPrefix[64];
		char szSeparator[128];
		char szSuffix[64];
		char szUserName[64];
		char szPW[64];
		char szRtspUrl[512];
		int iPort = 0;
		bool bRet = ParseUrl(szUrl, szPrefix, szSeparator, szSuffix, &iPort, szUserName, szPW);
		if (bRet)
		{
			sprintf(szRtspUrl, "%s%s%s", szPrefix, szSeparator, szSuffix);

			m_pRtspClientDll->SetUserInfo(m_pRtspObj, szUserName, szPW);
			m_pRtspClientDll->EnableOverTCP(m_pRtspObj, true);
			m_pRtspClientDll->Open(m_pRtspObj, szUrl);
			m_pRtspClientDll->EnableReconnect(m_pRtspObj, true);
			m_pRtspClientDll->EnableAudio(m_pRtspObj, true);

			m_bIsOpened = true;
		}
	}

	unsigned threadID1;
	m_bIsAlive = true;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _DecodeThreadProc, this, 0, &threadID1);
}

int StreamPlayer::CheckNetworkStatus(char* szUrl)
{
	int iRet = -1;
	Reset();

	if (m_pRtspObj)
	{
		char szPrefix[64];
		char szSeparator[128];
		char szSuffix[64];
		char szUserName[64];
		char szPW[64];
		char szRtspUrl[512];
		int iPort = 0;
		bool bRet = ParseUrl(szUrl, szPrefix, szSeparator, szSuffix, &iPort, szUserName, szPW);
		if (bRet)
		{
			sprintf(szRtspUrl, "%s%s%s", szPrefix, szSeparator, szSuffix);

			m_pRtspClientDll->SetUserInfo(m_pRtspObj, szUserName, szPW);
			m_pRtspClientDll->EnableOverTCP(m_pRtspObj, true);
			//m_pRtspClientDll->Open(m_pRtspObj, szUrl);
			iRet = m_pRtspClientDll->CheckNetworkStatus(m_pRtspObj, szUrl);
			//m_pRtspClientDll->EnableReconnect(m_pRtspObj, true);
			//m_pRtspClientDll->EnableAudio(m_pRtspObj, true);

//			m_bIsOpened = true;
		}
	}
	return iRet;
}

void StreamPlayer::Close()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hThread, 1000);

	if (!m_bIsOpened)
		return;

	if (m_pRtspObj)
		m_pRtspClientDll->Close(m_pRtspObj);

	m_pFFAVCodecDLL->CloseVideoDecoder(m_ulCodecID);

	m_bPresent = false;
	m_bIsOpened = false;

	//	g_ClearList();
}

void StreamPlayer::Play()
{
	m_bPresent = true; 
}

void StreamPlayer::Stop()
{
	m_bPresent = false;
}

void StreamPlayer::DoPlayVideo()
{
#if 0
	if (m_pVideoFrameBuffer->GetTotal() > (m_pVideoFrameBuffer->GetMax() / 2))
		DoDecodeVideoFrame();

	if (m_VideoFrameSpeedCtrl.IsExpired())
		DoDecodeVideoFrame();
#else
	if (m_pSmoothCtrl->IsAllowed() || m_pVideoFrameBuffer->GetTotal() > (m_pVideoFrameBuffer->GetMax() / 2))
		DoDecodeVideoFrame();
#endif
}

void StreamPlayer::DoDecodeVideoFrame()
{
	/*
	if (m_bWaitBuffer)
	{
	if (m_pVideoFrameBuffer->IsReady() && m_pVideoFrameBuffer->GetTotal() > (m_pVideoFrameBuffer->GetMax() / 2))
	m_bWaitBuffer = false;
	else
	return;
	}
	*/
	m_pCurVideoFrame = m_pVideoFrameBuffer->GetNext();
	if (m_pCurVideoFrame)
	{
		DecodeVideoFrame(m_pCurVideoFrame);

		//		m_VideoFrameSpeedCtrl.UpdateTimestamp(m_pCurVideoFrame->ulTimestampSec, m_pCurVideoFrame->ulTimestampMSec);
	}
}

void StreamPlayer::DecodeVideoFrame(RtspFrameInfo* pInfo)
{
	char szMsg[512];
	//RtspFrameInfo* pInfo = (RtspFrameInfo*)pParameter1;
	//pInfo->ulTimestamp;

	//sprintf(szMsg,"Timestamp = %d %d\n",pInfo->ulTimestampSec,pInfo->ulTimestampMSec);
	//OutputDebugString(szMsg);

	int iFFCodecID;
	if (!m_pFFAVCodecDLL->VideoDecoderIsOpened(m_ulCodecID))
	{
		switch (pInfo->iFormat)
		{
		case RTSP_DF_MPEG4:
			iFFCodecID = CODEC_ID_MPEG4;
			break;
		case RTSP_DF_H264:
			iFFCodecID = CODEC_ID_H264;
			break;
		case RTSP_DF_MJPEG:
			iFFCodecID = CODEC_ID_MJPEG;
			break;
		case RTSP_DF_H265:
			iFFCodecID = AV_CODEC_ID_HEVC;
			break;
		}

		if (pInfo->iFormat == RTSP_DF_H265)
		{
			if (m_iVideoExtraDataLen > 0)
				m_pFFAVCodecDLL->SetVideoExtraData(m_ulCodecID, (unsigned char*)m_szVideoExtraData, m_iVideoExtraDataLen);

			int iDeRet = m_pFFAVCodecDLL->OpenVideoDecoder(m_ulCodecID, iFFCodecID, -1, -1);
			int iDecodeRet = m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID, m_szVideoExtraData, m_pDeBuffer, m_iVideoExtraDataLen);
		}
		else
		{
			//int iDeRet = m_pFFAVCodecDLL->OpenVideoDecoder(m_ulCodecID,iFFCodecID,pInfo->iWidth,pInfo->iHeight);
			int iDeRet = m_pFFAVCodecDLL->OpenVideoDecoder(m_ulCodecID, iFFCodecID, -1, -1);
		}
	}

	bool bSkipError = false;
	//skip VPS/SPS/PPS
	if (pInfo->iFormat == RTSP_DF_H265)
	{
		//skip
		if (pInfo->pBuffer[0] != 0)
			return;

		int nal_type = ((pInfo->pBuffer[4]) & 0x7E) >> 1;
		switch (nal_type)
		{
			/* video parameter set (VPS) */
		case 32:
			/* sequence parameter set (SPS) */
		case 33:
			/* picture parameter set (PPS) */
		case 34:
		{
			bSkipError = true;
			//						return ;//0;
		}
		break;
		}
	}

	int iDecodeRet = m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID, pInfo->pBuffer, m_pDeBuffer, pInfo->iLen);
	if (pInfo->iFormat == RTSP_DF_H265 && iDecodeRet < 0 && pInfo->iLen > m_iVideoExtraDataLen)
	{
		int iCount = 0;

		//if(pkt.size > iLen)
		//{
		for (int i = 0; i < m_iVideoExtraDataLen; i++)
		{
			if (pInfo->pBuffer[i] == m_szVideoExtraData[i])
				iCount++;
		}

		if (iCount == m_iVideoExtraDataLen)
		{
			iDecodeRet = m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID, pInfo->pBuffer + m_iVideoExtraDataLen, m_pDeBuffer, pInfo->iLen - m_iVideoExtraDataLen);
		}
		else
		{
			m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID, m_szVideoExtraData, m_pDeBuffer, m_iVideoExtraDataLen);
			iDecodeRet = m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID, pInfo->pBuffer, m_pDeBuffer, pInfo->iLen);
		}
		//}
	}
	/*
	if (pInfo->iIPCount == 0)
	{
	m_iGOP = m_iGOPCount;
	m_iGOPCount = 1;
	}
	else
	m_iGOPCount++;
	*/

	if (iDecodeRet >= 0)
	{
		int w, h, key;
		m_pFFAVCodecDLL->GetRealSize(m_ulCodecID, &w, &h, &key);

		m_iVideoWidth = w;
		m_iVideoHeight = h;

		if (m_ulFrameCount == 0)
		{
			sprintf(szMsg, "StreamPlayer::DecodeVideoFrame w:%d h:%d\n", m_iVideoWidth, m_iVideoHeight);
			OutputDebugStringA(szMsg);
		}

		CheckConvVideoBuf(w,h);

		if (m_bPresent)
		{
			if (m_pFilePlayerCallback)
			{
				if (m_iOutputVideoFormat != VPF_I420)
				{
					switch (m_iOutputVideoFormat)
					{
					case VPF_YUY2:
						break;
					case VPF_RGB32:
						m_YUVConverter.I420ToARGB((unsigned char*)m_pDeBuffer, m_pConvVideoBuf, w, h);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, w, h);
						break;
					case VPF_RGB24:
						m_YUVConverter.I420ToRGB24((unsigned char*)m_pDeBuffer, m_pConvVideoBuf, w, h);
						m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pConvVideoBuf, w, h);
						break;
					}

				}
				else
				{
					m_pFilePlayerCallback->DisplayVideo((unsigned char*)m_pDeBuffer, w, h);
				}
			}
		}

//		m_pOutVideoDll->RenderFrame2(0, VPF_I420, m_pDeBuffer, w, h);
//		m_pOutVideoDll->Flip(0);
	}
	else
	{
		//int iDecodeRet = m_pFFAVCodecDLL->DecodeFrame(m_ulCodecID,m_szVideoExtraData,m_pDeBuffer,m_iVideoExtraDataLen);
		if (!bSkipError)
		{
			//skip SSP,PPS decode error
			if (iDecodeRet == -1 && pInfo->iFormat == RTSP_DF_H264 && pInfo->iIPCount == -1)
				;
			else
			{
				sprintf(szMsg, "Decode Failed!! Ret = %d [len:%d] [%d %d %d %d %d %d %d %d %d %d]!!!\n", iDecodeRet, pInfo->iLen
					, pInfo->pBuffer[0], pInfo->pBuffer[1], pInfo->pBuffer[2], pInfo->pBuffer[3], pInfo->pBuffer[4]
					, pInfo->pBuffer[5], pInfo->pBuffer[6], pInfo->pBuffer[7], pInfo->pBuffer[8], pInfo->pBuffer[9]);
				OutputDebugStringA(szMsg);
			}

			//			void g_AddToList(char* szText);
			//			g_AddToList(szMsg);
		}

	}

	CalFPS();

	m_ulFrameCount++;
}

void StreamPlayer::DoPlayAudio()
{
	DoDecodeAudioFrame();
}

void StreamPlayer::DoDecodeAudioFrame()
{
	if (m_pOutXAudio2Dll->IsOpened() && m_pOutXAudio2Dll->IsFulled())
		return;

	if (!m_pAudioFrameBuffer)
		return;

	m_pCurAudioFrame = m_pAudioFrameBuffer->GetNext();
	if (m_pCurAudioFrame)
	{
		DecodeAudioFrame(m_pCurAudioFrame);
	}
}

void StreamPlayer::DecodeAudioFrame(RtspFrameInfo* pInfo)
{
	/*
	pInfo->iFormat;
	pInfo->iSample;
	pInfo->iBits;
	pInfo->iLen;
	pInfo->pBuffer;
	//pInfo->iChannels;
	*/

	int iSamples = 8000;
	int iBits = 64;
	int iBitRate;
	int iChannels = 1;

	bool bDoPlay = false;

	if (!m_bEnableAudio)
		return;

	int iFFCodecID;
	if (!m_pFFAVCodecDLL->AudioDecoderIsOpened(m_ulCodecID))
	{
		switch (pInfo->iFormat)
		{
		case RTSP_DF_AAC:
		{
			iFFCodecID = AV_CODEC_ID_AAC;//aac-hbr
			iSamples = pInfo->iSample;
			iBits = pInfo->iBits;
			iChannels = pInfo->iChannels;

			iBitRate = iSamples * iChannels;
		}
		break;
		case RTSP_DF_G726:
		{
			iFFCodecID = AV_CODEC_ID_ADPCM_G726;

			iSamples = pInfo->iSample;
			iBits = pInfo->iBits;

			iBitRate = iBits * 1000;
		}
		break;
		case RTSP_DF_MULAW:
		{
			iFFCodecID = AV_CODEC_ID_PCM_MULAW;
			iSamples = 8000;
			iBits = 64;
			iBitRate = iBits * 1000;
		}
		break;
		case RTSP_DF_ALAW:
		{
			iFFCodecID = AV_CODEC_ID_PCM_ALAW;
			iSamples = 8000;
			iBits = 64;
			iBitRate = iBits * 1000;
		}
		break;
		}

		m_pFFAVCodecDLL->OpenAudioDecoder(m_ulCodecID, iFFCodecID, iChannels, iSamples, iBitRate);
	}

	long lRet = m_pFFAVCodecDLL->DecodeAudioFrame(m_ulCodecID, pInfo->pBuffer, m_pAudioDeBuffer, pInfo->iLen);
	if (lRet > 0)
	{
		if (m_bPresent)
		{
			int iOutChannel, iOutSampleRate, iOutBits;
			m_pFFAVCodecDLL->GetWaveFormat(m_ulCodecID, &iOutChannel, &iOutSampleRate, &iOutBits);

			if (!m_pOutXAudio2Dll->IsOpened())
				m_pOutXAudio2Dll->OpenDevice(iOutChannel, iOutSampleRate, iOutBits, lRet, 200);
			m_pOutXAudio2Dll->Output((char*)m_pAudioDeBuffer, lRet);
		}

		m_pCurAudioFrame = 0;

		m_ulAudioFrameCount++;
	}
}

void StreamPlayer::CalFPS()
{
	DWORD dwCutTime = timeGetTime();
	if (m_ulFrameCount % 11 == 0)
		m_dwFPSStartTime = dwCutTime;
	else if (m_ulFrameCount % 11 == 10)
	{
		m_fCurFPS = 1000.0 * 10.0 / (float)(dwCutTime - m_dwFPSStartTime);

		m_AvgFps.Calculate(m_fCurFPS);
		m_fFPS = m_AvgFps.GetAverage();

		m_pSmoothCtrl->SetAvgFPS(m_fFPS + (m_fFPS * 0.0275));
	}
}

int StreamPlayer::GetVolume()
{
	return m_pOutXAudio2Dll->GetVolume();
}

void StreamPlayer::SetVolume(int iValue)
{
	m_pOutXAudio2Dll->SetVolume(iValue);
}

bool StreamPlayer::IsOpened()
{
	return m_bIsOpened;
}

void StreamPlayer::CalRCVFPS(unsigned long ulTimestampSec,unsigned long ulTimestampMSec)
{
	char szMsg[512];
	if (m_ulLastTimestampSec == 0 && m_ulLastTimestampMSec == 0)
	{
		m_ulLastTimestampSec = ulTimestampSec;
		m_ulLastTimestampMSec = ulTimestampMSec;
	}
	else
	{
		long lMSOffset = ulTimestampMSec / 1000 - m_ulLastTimestampMSec / 1000;

		long lSecOffset = (float)(ulTimestampSec - m_ulLastTimestampSec) * 1000;
		long lTimeOffset = lSecOffset + lMSOffset;

		if (lTimeOffset > 0)
		{
			float fFPS = 0.0;
			if (lTimeOffset > 1000)
				fFPS = 1000.0 / (float)(lTimeOffset % 1000);
			else
				fFPS = 1000.0 / (float)lTimeOffset;
			m_AvgRCVFps.Calculate(fFPS);

//			sprintf(szMsg, "StreamPlayer::CalRCVFPS %3.2f %3.2f (%d %d %d %d %d)\n", m_AvgRCVFps.GetAverage(), fFPS, ulTimestampSec, ulTimestampMSec, lTimeOffset, lSecOffset,lMSOffset);
//			OutputDebugStringA(szMsg);
		}

		m_ulLastTimestampSec = ulTimestampSec;
		m_ulLastTimestampMSec = ulTimestampMSec;
	}
}

float StreamPlayer::GetFPS()
{
	return m_AvgRCVFps.GetAverage();
}

unsigned char* StreamPlayer::GetFrameBuffer()
{
	return m_pDeBuffer;
}

void* StreamPlayer::DoEvent(int iEventID, void* pParameter1, void* pParameter2)
{
	char szMsg[512];

	switch (iEventID)
	{
		case RCE_RECEIVE_VIDEO_EXTRA_DATA:
			{
				RtspFrameInfo* pInfo = (RtspFrameInfo*)pParameter1;

				m_iVideoExtraDataLen = pInfo->iLen;
				memset(m_szVideoExtraData, 0, 1024);
				memcpy(m_szVideoExtraData, (unsigned char*)pInfo->pBuffer, pInfo->iLen);
			}
			break;
		case RCE_RECEIVE_AUDIO:
			{
				RtspFrameInfo* pInfo = (RtspFrameInfo*)pParameter1;

				if (!m_pAudioFrameBuffer->IsReady())
					m_pAudioFrameBuffer->Alloc(_AUDIO_BUFFER_SIZE, 32);

				m_pAudioFrameBuffer->AddToQueueBuffer(pInfo);
			}
			break;
		case RCE_RECEIVE_VIDEO:
			{
				RtspFrameInfo* pInfo = (RtspFrameInfo*)pParameter1;
				if (!m_pVideoFrameBuffer->IsReady())
				{
					//m_pVideoFrameBuffer->Alloc(_VIDEO_BUFFER_SIZE / 2,64);
					m_pVideoFrameBuffer->Alloc(_VIDEO_BUFFER_SIZE / 2, 16);
				}

				CalRCVFPS(pInfo->ulTimestampSec, pInfo->ulTimestampMSec);
				m_pVideoFrameBuffer->AddToQueueBuffer(pInfo);
			}
			break;
	}

	return 0;
}


bool StreamPlayer::IsAlive()
{
	return m_bIsAlive;
}

void StreamPlayer::SetOutputVideoFormat(int iFormat)
{
	m_iOutputVideoFormat = iFormat;
}

void StreamPlayer::EnableAudio(bool bEnable)
{
	m_bEnableAudio = bEnable;
}

void StreamPlayer::CheckConvVideoBuf(int w,int h)
{
	if (m_iConvBufWidth != w || m_iConvBufHeight != h)
	{
		if (m_pConvVideoBuf)
		{
			delete m_pConvVideoBuf;
			m_pConvVideoBuf = 0;
		}

		switch (m_iOutputVideoFormat)
		{
		case VPF_I420:
			m_pConvVideoBuf = new unsigned char[w * h * 3 / 2];
			break;
		case VPF_YUY2:
			m_pConvVideoBuf = new unsigned char[w * h * 2];
			break;
		case VPF_RGB32:
			m_pConvVideoBuf = new unsigned char[w * h * 4];
			break;
		case VPF_RGB24:
			m_pConvVideoBuf = new unsigned char[w * h * 3];
			break;
		}

		m_iConvBufWidth = w;
		m_iConvBufHeight = h;
	}
}

int StreamPlayer::GetVideoCodecID()
{
	if (m_pRtspClientDll && m_pRtspObj)
	{
		int iVideoCodecID = m_pRtspClientDll->GetVideoCodecID(m_pRtspObj);
		switch (iVideoCodecID)
		{
			case RTSP_DF_MPEG4:
				m_iTmpFFVideoCodecID = CODEC_ID_MPEG4;
				break;
			case RTSP_DF_H264:
				m_iTmpFFVideoCodecID = CODEC_ID_H264;
				break;
			case RTSP_DF_MJPEG:
				m_iTmpFFVideoCodecID = CODEC_ID_MJPEG;
				break;
			case RTSP_DF_H265:
				m_iTmpFFVideoCodecID = AV_CODEC_ID_HEVC;
				break;
		}

		return m_iTmpFFVideoCodecID;
	}
	return -1;
}

int StreamPlayer::GetAudioCodecID()
{
	if (m_pRtspClientDll && m_pRtspObj)
	{
		int iAudioCodecID = m_pRtspClientDll->GetAudioCodecID(m_pRtspObj);
		switch (iAudioCodecID)
		{
			case RTSP_DF_AAC:
				m_iTmpFFAudioCodecID = AV_CODEC_ID_AAC;//aac-hbr
				break;
			case RTSP_DF_G726:
				m_iTmpFFAudioCodecID = AV_CODEC_ID_ADPCM_G726;
				break;
			case RTSP_DF_MULAW:
				m_iTmpFFAudioCodecID = AV_CODEC_ID_PCM_MULAW;
				break;
			case RTSP_DF_ALAW:
				m_iTmpFFAudioCodecID = AV_CODEC_ID_PCM_ALAW;
				break;
		}
		return m_iTmpFFAudioCodecID;
	}
	return -1;
}

char* StreamPlayer::GetVideoCodecName()
{
	if (m_pRtspClientDll && m_pRtspObj)
	{
		int iVideoCodecID = m_pRtspClientDll->GetVideoCodecID(m_pRtspObj);
		switch (iVideoCodecID)
		{
		case RTSP_DF_MPEG4:
			strcpy(m_szTmpVideoCodecName,"mpeg4");
			break;
		case RTSP_DF_H264:
			strcpy(m_szTmpVideoCodecName, "h264");
			break;
		case RTSP_DF_MJPEG:
			strcpy(m_szTmpVideoCodecName, "mjpeg");
			break;
		case RTSP_DF_H265:
			strcpy(m_szTmpVideoCodecName, "hevc");
			break;
		}

		return m_szTmpVideoCodecName;
	}
	return "";
}

char* StreamPlayer::GetAudioCodecName()
{
	if (m_pRtspClientDll && m_pRtspObj)
	{
		int iAudioCodecID = m_pRtspClientDll->GetAudioCodecID(m_pRtspObj);
		switch (iAudioCodecID)
		{
		case RTSP_DF_AAC:
			strcpy(m_szTmpAudioCodecName,"aac");
			break;
		case RTSP_DF_G726:
			strcpy(m_szTmpAudioCodecName, "g726");
			break;
		case RTSP_DF_MULAW:
			strcpy(m_szTmpAudioCodecName, "ulaw");
			break;
		case RTSP_DF_ALAW:
			strcpy(m_szTmpAudioCodecName, "alaw");
			break;
		}
		return m_szTmpAudioCodecName;
	}
	return "";
}

int StreamPlayer::GetVideoWidth()
{
	return m_iVideoWidth;
}

int StreamPlayer::GetVideoHeight()
{
	return m_iVideoHeight;
}

void StreamPlayer::Reset()
{
	m_ulLastTimestampSec = 0;
	m_ulLastTimestampMSec = 0;
	m_ulFrameCount = 0;
	m_ulAudioFrameCount = 0;

	m_iVideoWidth = 0;
	m_iVideoHeight = 0;
}

unsigned int __stdcall _DecodeThreadProc(void* lpvThreadParm)
{
	StreamPlayer* pObj = (StreamPlayer *)lpvThreadParm;

	while (pObj->IsAlive())
	{
		pObj->DoPlayVideo();
		pObj->DoPlayAudio();
		//		pObj->UpdateInfo();
		Sleep(1);
	}

	return 0;
}

LRESULT WINAPI _DisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		DragAcceptFiles(hwnd, TRUE);
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
		result = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	}

	return result;
}
