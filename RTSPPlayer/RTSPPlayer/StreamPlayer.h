#pragma once

#ifndef _StreamPlayer_H
#define _StreamPlayer_H

#include "RtspClientDll.h"
#include "FFAVCodecDLL.h"
//#include "../../../Include/OutVideoDll.h"
#include "../../../Include/OutXAudio2Dll.h"
#include "FrameBufferManager.h"
#include "../../../Include/SmoothCtrl.h"
#include "../../../Include/MathCommon.h"
#include "../../../Include/FilePlayerCommon.h"
#include "YUVConverter.h"

class StreamPlayer : public RtspCallbackObj
{
	RtspClientDll* m_pRtspClientDll;
	FFAVCodecDLL* m_pFFAVCodecDLL;
//	OutVideoDll* m_pOutVideoDll;
	OutXAudio2Dll* m_pOutXAudio2Dll;

	//HWND m_hDisplayWnd;
	unsigned long m_ulCodecID;
	void* m_pRtspObj;
	unsigned char m_szVideoExtraData[1024];
	int m_iVideoExtraDataLen;

	FrameBufferManager* m_pVideoFrameBuffer;
	FrameBufferManager* m_pAudioFrameBuffer;
	RtspFrameInfo* m_pCurVideoFrame;
	RtspFrameInfo* m_pCurAudioFrame;
	SmoothCtrl* m_pSmoothCtrl;
	unsigned char* m_pDeBuffer;
	unsigned char* m_pAudioDeBuffer;
	unsigned long m_ulAudioFrameCount;
	int m_iAudioFrameFactor;
	unsigned long m_ulRawAudioLen;

	bool m_bIsAlive;
	HANDLE m_hThread;

	//	int m_iGOPCount;
	//	int m_iGOP;
	float m_fFPS;
	float m_fCurFPS;
	unsigned long m_ulFrameCount;
	DWORD m_dwFPSStartTime;
	DWORD m_dwUpdateStartTime;
	Average m_AvgFps;
	Average m_AvgRCVFps;
	unsigned long m_ulLastTimestampSec;
	unsigned long m_ulLastTimestampMSec;

	FilePlayerCallback* m_pFilePlayerCallback;
	int m_iOutputVideoFormat;
	YUVConverter m_YUVConverter;
	unsigned char* m_pConvVideoBuf;
	int m_iConvBufWidth;
	int m_iConvBufHeight;
	bool m_bEnableAudio;

	bool m_bPresent;
	bool m_bIsOpened;

	int m_iTmpFFVideoCodecID;
	int m_iTmpFFAudioCodecID;
	char m_szTmpVideoCodecName[128];
	char m_szTmpAudioCodecName[128];
	int m_iVideoWidth;
	int m_iVideoHeight;
public:
	StreamPlayer();
	~StreamPlayer();

	void Init(FilePlayerCallback* pFilePlayerCallback);
	void Open(char* szUrl);
	void Close();
	void Play();
	void Stop();
	bool IsAlive();
	void DoPlayVideo();
	void DoPlayAudio();
	void DoDecodeVideoFrame();
	void DoDecodeAudioFrame();
	void DecodeVideoFrame(RtspFrameInfo* pInfo);
	void DecodeAudioFrame(RtspFrameInfo* pInfo);
	void CalFPS();
	void SetOutputVideoFormat(int iFormat);
	void CheckConvVideoBuf(int w, int h);
	void EnableAudio(bool bEnable);
	int GetVolume();
	void SetVolume(int iValue);
	bool IsOpened();

	int GetVideoCodecID();
	int GetAudioCodecID();
	char* GetVideoCodecName();
	char* GetAudioCodecName();
	void CalRCVFPS(unsigned long ulTimestampSec, unsigned long ulTimestampMSec);
	void Reset();
	float GetFPS();
	int GetVideoWidth();
	int GetVideoHeight();
	unsigned char* GetFrameBuffer();

	int CheckNetworkStatus(char* szUrl);

	virtual void* DoEvent(int iEventID, void* pParameter1, void* pParameter2);
};

#endif
