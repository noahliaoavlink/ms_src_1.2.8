#ifndef _DSound_H
#define _DSound_H

#define INITGUID
//#include <dmusici.h>

//#ifdef _WIN64
#include "dxerr.h"
//#else
//#include "dxerr8.h"
//#endif
#include <Mmreg.h>
#include <Mmsystem.h>
#include <dsound.h>
#include "CallbackObj.h"

#include <queue>          // std::queue

#include <ks.h>
#include <ksmedia.h>

#include "../../Include/SQQueue.h"

#define _ENABLE_QBUFFER 1

#if _ENABLE_QBUFFER
#include "../../Include/QBufferManagerEx.h"
#endif

#define _MAX_DS_BUFFER 500

#define EVENT_MUM 4

#define _MAX_ITEMS 5

class Average
{
	int m_iTotalOfItems;
	float m_fValue[_MAX_ITEMS];
	float m_fAvgValue;
	unsigned long m_ulCount;
protected:
public:
	Average();
	~Average();

	void Reset();
	void Calculate(float fValue);
	float GetAverage();
};

class DSound //: public CallbackObj
{
        LPDIRECTSOUND8 m_pDS;
//		WAVEFORMATEX m_WFE;
		LPDIRECTSOUNDBUFFER m_lpDSB;
		HANDLE m_pHEvent[EVENT_MUM];

		LPBYTE m_lpAudioBuf;

		bool m_bDSBufferIsFinished[EVENT_MUM];
		MMRESULT m_timerID;

#if _ENABLE_QBUFFER
		QBufferManagerEx* m_pQBuffer;
		unsigned char* m_pTempFrameBuffer;
#else
		int m_iAudioBufCount;
		LPBYTE m_lpAudioBuf1[_MAX_DS_BUFFER];
		int m_iAudioBufLen[_MAX_DS_BUFFER];
#endif
		int m_iPlayIndex;
		int m_iNextIndex;

		int m_iDSBufferSize;
		int m_iDSTotalOfBuffers;

		bool m_bFinished;
		HANDLE m_hThread;
		bool m_bBufferIsEmpty;
		int m_iStatus;
		CRITICAL_SECTION m_CriticalSection;

		BYTE* m_pMixBuffer;

		long m_lTmpVolume;
		long m_lVolume;

		Average m_AvgBuffer;
		unsigned long m_ulLastTime;

		//std::queue<unsigned long> m_TimestampQueue;
		unsigned long m_ulLastTimestamp;

		SQQueue* m_pTimestampQ;
   public:
	    DSound();
        ~DSound();

		HRESULT Init(HWND hWnd,char* szEventName);
		HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate );
		HRESULT CreateSurface(WAVEFORMATEX* pWFE);
		//HRESULT CreateSurface1(DWORD dwChannels, DWORD dwFreq, DWORD dwBits);
		//HRESULT CreateSurface2(DWORD dwChannels, DWORD dwFreq, DWORD dwBits);
		void SetBufferInfo(int iBufferSize,int iTotalOfBuffers);

		void Play();
		void Stop();
		void Continue();
		void Close();

		void Reset();
		bool IsEmpty();
		bool IsFulled();
		int GetDSBufferSize();
		bool IsReady();
		void AddBuffer(LPBYTE pBuffer,int iLen);
		int GetNextBuffer(LPBYTE pBuffer);
		HRESULT FillBuffer(int iIndex);
		void TimerCallback();

		bool IsFinished();
		void SetVolume(long lValue);
		long GetVolume();
		void ResetBuffer();
		int GetBufferCount();
		void AddTimestamp(unsigned long ulTimestamp);
		void UpdateTimestamp();
		void ResetTimestamp();
		unsigned long GetLastTimestamp();

		//void* DoEvent(int iEvent,void* pParameter1,void* pParameter2);
};

#endif