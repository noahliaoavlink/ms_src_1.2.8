#pragma once

#ifndef _XA2Stream_H
#define _XA2Stream_H

#define INITGUID

//#include "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.15063.0\\um\\xaudio2.h"
#include "C:\\Program Files (x86)\\Windows Kits\\8.1\\Include\\um\\xaudio2.h"
//#include <XAudio2.h>
#include "../../Include/QBufferManagerEx.h"
#include "ThreadBase.h"

//#include <ks.h>
//#include <ksmedia.h>

#define MAX_BUFFER_COUNT 3

struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32)
	{
	}
	STDMETHOD_(void, OnVoiceProcessingPassEnd)()
	{
	}
	STDMETHOD_(void, OnStreamEnd)()
	{
	}
	STDMETHOD_(void, OnBufferStart)(void*)
	{
	}
	STDMETHOD_(void, OnBufferEnd)(void*)
	{
		SetEvent(hBufferEndEvent);
	}
	STDMETHOD_(void, OnLoopEnd)(void*)
	{
	}
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT)
	{
	}

	HANDLE hBufferEndEvent;

	StreamingVoiceContext() : hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL))
	{
	}
	virtual ~StreamingVoiceContext()
	{
		CloseHandle(hBufferEndEvent);
	}
};

class XA2Stream : public ThreadBase
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	IXAudio2SourceVoice* pSourceVoice;
	StreamingVoiceContext voiceContext;

	//for wave out
	int m_nChannels;
	int m_nSampleRate;
	int m_nBits;

	//float m_fVolume;
	int m_iVolume;

	QBufferManagerEx* m_pQBuffer;
	unsigned char* m_pCurFrameBuffer;
	unsigned char* m_pFrameBuffer[MAX_BUFFER_COUNT];
	int m_iDSBufferSize;
	int m_iDSTotalOfBuffers;

	int m_iBufIndex;

	int m_iStatus;
	bool m_bIsOpened;

	int m_iBufferSize;

	bool m_bDoInit;

	unsigned char* m_pTmpFrameBuffer;
	unsigned char* m_pTmpFrameBuffer2;
	int m_iTmpBufOffset;

	CRITICAL_SECTION m_CriticalSection;
public:
	XA2Stream();
	~XA2Stream();

	int Init();
	int Init2();
	void Destroy();

	void OpenDevice(int nChannels, int nSampleRate, int iBits, int iBufferLen, int iTotalOfBuffers);
	void Close();
	bool IsOpened();
	void SetWaveFormat(int nChannels, int nFrequency, int nBits);
	void CreateSource();
	void SetVolume(int iVolume);
	int GetVolume();
	void Play();
	void Pause();
	void Stop();
	void Continue();

	void SetBufferInfo(int iBufferSize, int iTotalOfBuffers);
	void AddBuffer(LPBYTE pBuffer, int iLen);
	int GetNextBuffer();
	void ResetBuffer();
	int GetBufferCount();
	bool IsFulled();
	void UpdateBuffer();
	void Output(LPBYTE pBuffer, int iLen);

	void SetSourceVoiceVolume();

	void ThreadEvent();
};

#endif
