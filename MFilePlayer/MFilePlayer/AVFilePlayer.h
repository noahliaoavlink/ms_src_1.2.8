#ifndef _AVFilePlayer_H_
#define _AVFilePlayer_H_

//#include "../../../Include/FFAVCodecDLL.h"
#include "../../Include/IDStreamCommon.h"
#include "../../Include/FrameBufferManager.h"
//#include "../../../Include/BackwardHelper.h"
#include "BackwardHelper2.h"
//#include "OutVideoDll.h"
#include "ThreadBase.h"
#include "FrameRateCtrl.h"
#include "VolumeCtrl.h"
#include "../../Include/FilePlayerCommon.h"
#include "../../Include/FFVideoFilterDll.h"
#include "../../Include/mediafilecommon.h"
#include "../../Include/MathCommon.h"
#include "YUVConverter.h"
#include "FFMediaFileDll.h"

#include "AVPacketQueue.h"

#define _ENABLE_XAUDIO2 1
#define _ENABLE_LOADING_CTRL 1
//#define _ENABLE_MEDIA_FILE_CTRL 1
//#define _ENABLE_VIDEO_FILTER 1

#ifdef _ENABLE_XAUDIO2
#include "../../../Include/OutXAudio2Dll.h"
#else
#include "../../../Include/OutAudioDll.h"
#endif

#ifdef _ENABLE_LOADING_CTRL
#include "LoadingCtrl.h"
#endif

#ifdef _ENABLE_MEDIA_FILE_CTRL
#include "MediaFileCtrl.h"
#endif

#define _ENABLE_AVQUEUE2 1

#define _ENABLE_MS 1
//#define _ENABLE_BUF_MODE 1

#define _MAX_SPEED 16

enum GlobalTimeCodeStatus
{
	GTCS_NORMAL = 0,
	GTCS_DO_SEEK,
	GTCS_SEEKING,
};

class AVFilePlayer : public ThreadBase
{
		//FFAVCodecDLL* m_pFFAVCodecDLL;		
#ifdef _ENABLE_MEDIA_FILE_CTRL
	MediaFileCtrl* m_pMediaFileCtrl;
	bool m_bEnableGPU;
#else
	FFMediaFileDll* m_pFFMediaFileDll;
#endif

#ifdef _ENABLE_XAUDIO2
		OutXAudio2Dll* m_pOutXAudio2Dll;
#else
		OutAudioDll* m_pOutAudioDll;
#endif
//		OutVideoDll* m_pOutVideoDll;

#ifdef _ENABLE_VIDEO_FILTER
		FFVideoFilterDll* m_pFFVideoFilterDll;
#else
#endif
		HWND m_hDisplayWnd;
		int m_iCodecID;

		int m_iVideoFrameW;
		int m_iVideoFrameH;
		int m_iCoded_Height;
		int m_iCoded_Width;
		bool m_bIsOpened;
		bool m_bRefTimeUpdate;
		double m_dReferenceTime;
		double m_dCurPosMs;

		MediaFrameBuffer m_framebuf;
		MediaFrameBuffer m_audioframebuf;
		float m_fVideoFPS;
		float m_fAudioFPS;
		float m_fVideoInterval;
		int m_iAudioInterval;
		unsigned long m_ulVideoFrameCount;
		unsigned long m_ulAudioFrameCount;
		unsigned long m_ulLastVideoFrameTime;
		unsigned long m_ulLastAudioFrameTime;
		bool m_bDoGetNext;

		unsigned long m_ulTimestamp;
		//unsigned long m_ulStartTimestamp;
		DWORD m_dwLastTime;

		int m_iChannels;
		int m_iSampleRate;
		int m_iBits;
		int m_iAudioFrameFactor;
		int m_iChannelLayout;
		int m_iNBSamples;

		unsigned char* m_pAudioDeBuffer;
		unsigned char* m_pAudioDeBuffer2;
		RawAudioFrame m_WaveFrame;
		char m_szTime[40];

		long m_lLastVideoPos;
		long m_lTotalLen;
		double m_dLastPos;
		double m_dLastVideoPos;

		int m_iVolume;
		VolumeCtrl* m_pVolumeCtrl;

		FrameRateCtrl m_FrameRateCtrl;
		bool m_bEnableUpdatePos;
		bool m_bUpdatePos;
		bool m_bDoAVSyn;

		FrameBufferManager* m_pVideoFrameBuffer;
		FrameBufferManager* m_pAudioFrameBuffer;
		StreamFrame* m_pRawVideoFrame;
		StreamFrame* m_pRawAudioFrame;
		StreamFrame* m_pCurVideoFrame;
		StreamFrame* m_pCurAudioFrame;
		bool m_bIsSuspend;
		bool m_bSuspendDisplay;

		bool m_bIsRepeated;
		bool m_bEnableFadeIn;
		bool m_bEnableFadeOut;
		FilePlayerCallback* m_pFilePlayerCallback;
		AVCodecContext* m_pCurVideoCodecContext;
		AVCodecContext* m_pSWVideoCodecContext;
		AVCodecContext* m_pCurAudioCodecContext;

		int m_iOutputVideoFormat;
		YUVConverter m_YUVConverter;
		unsigned char* m_pConvVideoBuf;
		bool m_bEnableAudio;

		HANDLE m_hVideoThread;
		HANDLE m_hAudioThread;
		HANDLE m_hTimestampThread;
		CRITICAL_SECTION m_VideoCriticalSection;
		CRITICAL_SECTION m_AudioCriticalSection;
		CRITICAL_SECTION m_SeekCriticalSection;
		CRITICAL_SECTION m_TimestampCriticalSection;

		//int m_iSpeed;
		float m_fSpeed;
		int m_iPlaybackMode;
		bool m_bDoBackwardSeek;
		//BackwardHelper m_BackwardHelper;
		BackwardHelper2 m_BackwardHelper2;
		RawVideoFrame* m_pRawVideoFrame2;

		//AVPacket m_pkt;
		bool m_bDoRetry;
		bool m_bDoRetryPushRaw;

		int m_iGOPCount;
		int m_iGOP;

		bool m_bBeginOfFile;

//		double m_dLastPos;
		bool m_bOpenIsFinished;
		bool m_bSeekIsFinished;
		AVPacket m_tmp_pkt;
		//for debug
		//unsigned long m_ulRawLastTimestamp;
		unsigned long m_ulLastAudioTimestamp;

		HANDLE m_hEvent1;
		HANDLE m_hEvent2;
		HANDLE m_hEvent3;

		int m_iBrightness;
		int m_iContrast;
		int m_iSaturation;
		int m_iHue;
		int m_iR;
		int m_iG;
		int m_iB;
		int m_iGray;
		int m_iNegative;

		
#ifdef _ENABLE_AVQUEUE2
		AVPacketQueue2* m_pAVPacketQueue2;
#else
		AVPacketQueue* m_pAVPacketQueue;
#endif

		MediaFrameBuffer m_framebuf_hw;
		bool m_bEOF;
		bool m_bEOF2;
		unsigned char* m_pRawVideoBuf;
		unsigned char* m_pNV12RawVideoBuf;
		void* m_pD3DDevice;

		char m_szFileName[512];

		unsigned long m_ulRawAudioLen;

		float m_fScaleW;
		float m_fScaleH;
		Average m_AvgFPS;
		Average m_RealAvgFPS;
		Average m_AvgReSampleRate;
		unsigned long m_ulLastTime;
		unsigned long m_ulLastAudioTime;
		//float m_fCurAvgAudioBuffers;
		//int m_iWaitCount;

		//bool m_bGlobalMode;
		//double m_dGlobalTimeCode;
		int m_iDisplayIntervalMode;
		double m_dStartTime;

		int m_iGlobalTimeCodeStatus;
		bool m_bEnableReSampleRate;

		bool m_bIsAudioFile;

		bool m_bDoSkip;
		double m_dSeekPos;

		int m_iAudioDelay;
		float m_fReSampleRateOffset;

#ifdef _ENABLE_LOADING_CTRL
		LoadingCtrl m_LoadingCtrl;
#endif

	public:
		AVFilePlayer();
        ~AVFilePlayer();

		void Init(HWND hWnd,FilePlayerCallback* pFilePlayerCallback = 0);
		int Open(char* szFileName);
		void Close();
		void Play();
		void PlayBackward();
		void Pause();
		void Stop();
		void Stop2();
		void Reset();
		void ReleaseBuffers();
		void ResetBuffer();

		void DisplayVideoFrame();
		void PlayAudioFrame();
		int DisplayVideoFrame2();
		void PlayAudioFrame2();

		void UpdatePosInfo(int64_t pts = 0);
		void UpdatePosInfoMS(int64_t pts = 0);
		char* PosToTimeInfo(double dPos, bool bEnableMS = false);
		void SetVolume(int iValue);
		void SetSystemVolume(int iValue);
		void Seek(double dPos);
		void Seek2(double dPos);
		void EnableUpdatePos(bool bEnable);
		int GetVolume();
		int GetSystemVolume();
		void SetRepeat(bool bEnable);
		void EnableFadeIn(bool bEnable);
		void EnableFadeOut(bool bEnable);
		void SetOutputVideoFormat(int iFormat);
		void EnableAudio(bool bEnable);
		float GetVideoFPS();
		int GetTotalOfFrames();
		int GetVideoFrame(unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime);
		int GetVideoWidth();
		int GetVideoHeight();
		void SpeedUp();
		void SpeedDown();

		//for backward playback
		void SetPlaybackMode(int iMode);
		int GetPlaybackMode();
		void PlayForwardMode();
		void PlayBackwardMode();
		//void PlayBackwardMode2();
		int ProcessAVPacket(AVPacket* pkt);
		void DisplayBackwardVideoFrame();
		void UpdateGOPCount(bool bIsKeyFrame);
		void DecodeBackward();
		void DecodeVideoFrame();

		//filter
		void SetBrightness(int iValue);
		void SetContrast(int iValue);
		void SetSaturation(int iValue);
		void SetHue(int iValue);
		void SetR(int iValue);
		void SetG(int iValue);
		void SetB(int iValue);
		void SetGray(int iValue);
		void SetNegative(int iValue);
		void DoFilter(AVFrame* pAVFrame, StreamFrame* pRawVideoFrame);
		void SetFadeDuration(float fDuration);
		//void WaitForFinished();

		void SetupDXVA2(void* pD3DDevice);
		void EnableGPU(bool bEnable);
		bool GPUIsEnabled();
		void PlayForwardMode_HW();
		void DisplayVideoFrame_HW();
		int DisplayVideoFrame_HW2();
		void* GetD3DDevice();

		void DoRepeat();
		void DoReOpen();
		int DoFilter2(AVFrame* pInAVFrame, StreamFrame* pRawVideoFrame);
		bool IsOpened();
		void ResetFilter();

		bool IsEOF(bool bEOF2);
		void SetScale(float fW, float fH);
		void EnableReSampleRate(bool bEnable);
		int GetAudioInfo(int& iChannelLayout, int& iSampleRate, int& iNBSamples);

		void UpdateTimestamp();
		void CheckPosInfo();
		bool IsAudioFile();
		void EnableSuspendDisplay(bool bEnable);
		void SetAudioDelay(int iValue);
		void SetReSampleRateOffset(float fOffset);
		void SetProcessFPS(float fFPS);

		//GlobalMode
		//void EnableGlobalMode(bool bEnable);
		void UpdateGlobalTimeCode(double dTimeCode);
		void SetDisplayIntervalMode(int iMode);
		void SetStartTime(double dTime);
//		int CheckGlobalTimeCodeStatus(double dTimeCode);
//		void UpdateGlobalTimeCode2(double dTimeCode);

		virtual void ThreadEvent();
		void VideoThreadEvent();
		void AudioThreadEvent();
		void SaveLog(CString strLog);
		void SetReferenceTimeCode(double dNewRefTime);
};

#endif
