#include "stdafx.h"
#include "XA2Stream.h"
#include "..\\..\\Include\\Common.h"

//#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//#pragma comment(lib,"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x64/xaudio2_8.lib")
//#pragma comment(lib,"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x64/xaudio2.lib")
#pragma comment(lib,"C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64/xaudio2.lib")


// Speaker Positions:
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

// Bit mask locations reserved for future use
#define SPEAKER_RESERVED                0x7FFC0000

// Used to specify that any possible permutation of speaker configurations
#define SPEAKER_ALL                     0x80000000

// DirectSound Speaker Config
#if (NTDDI_VERSION >= NTDDI_WINXP)
#define KSAUDIO_SPEAKER_DIRECTOUT       0
#endif
#define KSAUDIO_SPEAKER_MONO            (SPEAKER_FRONT_CENTER)
#define KSAUDIO_SPEAKER_STEREO          (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
#define KSAUDIO_SPEAKER_QUAD            (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_SURROUND        (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER)
#define KSAUDIO_SPEAKER_5POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_7POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER)

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

XA2Stream::XA2Stream()
{
	pXAudio2 = NULL;
	pMasteringVoice = NULL;
	pSourceVoice = NULL;

	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(0);

	m_pCurFrameBuffer = 0;
	m_pTmpFrameBuffer2 = 0;
	m_iBufIndex = 0;

	m_pTmpFrameBuffer = 0;
	m_iTmpBufOffset = 0;

	m_iVolume = 50;

	for(int i = 0;i < MAX_BUFFER_COUNT;i++)
		m_pFrameBuffer[i] = 0;

	m_iStatus = PS_STOP;
	m_bIsOpened = false;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_bDoInit = false;

//	InitializeCriticalSection(&m_CriticalSection);
}

XA2Stream::~XA2Stream()
{
	if (m_pQBuffer)
		delete m_pQBuffer;

	for (int i = 0; i < MAX_BUFFER_COUNT; i++)
	{
		if (m_pFrameBuffer[i])
			delete m_pFrameBuffer[i];
	}

//	if (m_pCurFrameBuffer)
	//	delete m_pCurFrameBuffer;

	Close();
	Destroy();
	ThreadBase::Stop();
	CoUninitialize();
//	DeleteCriticalSection(&m_CriticalSection);
}

int XA2Stream::Init()
{
	/*
	HRESULT hr;
	UINT32 flags = 0;

#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		wprintf(L"Failed to init XAudio2 engine: %#X\n", hr);
		CoUninitialize();
		return 0;
	}

	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		return 0;
	}
	*/
	return 1;
}

int XA2Stream::Init2()
{
	HRESULT hr;
	char szMsg[512];
	UINT32 flags = 0;

	if (m_bDoInit)
	{
		m_bDoInit = false;
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

		if (S_OK != hr)
		{
			sprintf(szMsg, "XA2Stream::Init2() - CoInitializeEx() failed!! \n");
			OutputDebugStringA(szMsg);
		}
	}

#ifdef _DEBUG
	//flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		wprintf(L"Failed to init XAudio2 engine: %#X\n", hr);

		sprintf(szMsg, "XA2Stream::Init2() - Failed to init XAudio2 engine!!\n");
		OutputDebugStringA(szMsg);

		CoUninitialize();
		m_bDoInit = true;
		return 0;
	}

	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);

		sprintf(szMsg, "XA2Stream::Init2() - Failed creating mastering voice!! %#X\n", hr);
		OutputDebugStringA(szMsg);

		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		m_bDoInit = true;
		return 0;
	}

	return 1;
}

void XA2Stream::Destroy()
{
//	ThreadBase::Stop();

	if (pSourceVoice)
	{
		pSourceVoice->DestroyVoice();
		pSourceVoice = NULL;
	}

	if (pMasteringVoice != NULL)
	{
		pMasteringVoice->DestroyVoice();
		pMasteringVoice = NULL;
	}

	if (pXAudio2 != NULL)
	{
		pXAudio2->Release();
		pXAudio2 = NULL;
	}

	if (m_pTmpFrameBuffer)
	{
		delete m_pTmpFrameBuffer;
		m_pTmpFrameBuffer = 0;
	}

	if (m_pTmpFrameBuffer2)
	{
		delete m_pTmpFrameBuffer2;
		m_pTmpFrameBuffer2 = 0;
	}

	if (m_pCurFrameBuffer)
	{
		delete m_pCurFrameBuffer;
		m_pCurFrameBuffer = 0;
	}
}

void XA2Stream::OpenDevice(int nChannels, int nSampleRate, int iBits, int iBufferLen, int iTotalOfBuffers)
{
	char szMsg[512];

//	Close();

//	sprintf(szMsg, "Output - XA2Stream::OpenDevice - 0\n");
//	OutputDebugStringA(szMsg);
	while (1)
	{
		int iRet = Init2();

//		sprintf(szMsg, "Output - XA2Stream::OpenDevice - 1\n");
//		OutputDebugStringA(szMsg);
		if (iRet == 1)
		{
//			sprintf(szMsg, "Output - XA2Stream::OpenDevice - 2\n");
//			OutputDebugStringA(szMsg);

			SetWaveFormat(nChannels, nSampleRate, iBits);

//			sprintf(szMsg, "Output - XA2Stream::OpenDevice - 3\n");
//			OutputDebugStringA(szMsg);

			SetBufferInfo(iBufferLen, iTotalOfBuffers);

//			sprintf(szMsg, "Output - XA2Stream::OpenDevice - 4\n");
//			OutputDebugStringA(szMsg);

			CreateSource();

//			sprintf(szMsg, "Output - XA2Stream::OpenDevice - 5\n");
//			OutputDebugStringA(szMsg);

			Play();

//			sprintf(szMsg, "Output - XA2Stream::OpenDevice - 6\n");
//			OutputDebugStringA(szMsg);

			//Sleep(5);
			m_bIsOpened = true;
		}
		else
		{
			sprintf(szMsg, "XA2Stream::OpenDevice - Init2() failed!!\n");
			OutputDebugStringA(szMsg);
		}
		break;
	}

//	sprintf(szMsg, "Output - XA2Stream::OpenDevice - 7\n");
//	OutputDebugStringA(szMsg);
}

void XA2Stream::Close()
{
	m_bIsOpened = false;
	Stop();
//	ThreadBase::Stop();
	Destroy();
}

bool XA2Stream::IsOpened()
{
	return m_bIsOpened;
}

void XA2Stream::SetWaveFormat(int nChannels, int nSampleRate, int nBits)
{
	m_nChannels = nChannels;
	m_nSampleRate = nSampleRate;
	m_nBits = nBits;
}

void XA2Stream::CreateSource()
{
	HRESULT hr;

	char szMsg[512];

	WAVEFORMATEXTENSIBLE wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
	wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wfx.Format.nChannels = m_nChannels;
	wfx.Format.nSamplesPerSec = m_nSampleRate;
	wfx.Samples.wValidBitsPerSample = m_nBits;
	wfx.Format.wBitsPerSample = m_nBits;
	if (wfx.Samples.wValidBitsPerSample == 0)
		wfx.Format.wBitsPerSample = wfx.Samples.wValidBitsPerSample = 16;
	else if (wfx.Samples.wValidBitsPerSample == 24)
		wfx.Format.wBitsPerSample = 32;
	//wfx.Format.cbSize = 22;
	wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	wfx.dwChannelMask = 0;
	wfx.Format.nBlockAlign = (wfx.Format.nChannels * wfx.Format.wBitsPerSample) / 8;
	wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
	wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	/*
	if (m_nChannels == 8)
		wfx.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
	else if (m_nChannels == 6)
	{
		wfx.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
		//		wfxe.Samples.wValidBitsPerSample = 20;
	}
	else if (m_nChannels == 4)
		wfx.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
	else if (m_nChannels == 2)
		wfx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
	else
		wfx.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;
	*/
	//int bufferSize = wfx.Format.nAvgBytesPerSec * XAUDIO2_BUFFER_SECONDS;

	//if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx, XAUDIO2_VOICE_NOPITCH)))
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx, XAUDIO2_VOICE_NOPITCH, 1.0f, &voiceContext)))
	{
//		CleanUpFFmpegResource();
//		throw gcnew Exception(String::Format(ResourceStrings::GetString("CannotUseAudioEngine"), "IXAudio2::CreateSourceVoice", hr));
		sprintf(szMsg, "XA2Stream::CreateSource() - CreateSourceVoice\n");
		OutputDebugStringA(szMsg);
	}
}

//0.0~1.0
void XA2Stream::SetVolume(int iVolume)
{
	float fUnit = 1.0 / 100.0;
	float fVolume = iVolume * fUnit;
	m_iVolume = iVolume;
	//pMasteringVoice->SetVolume(fVolume);
	if (pSourceVoice)
		pSourceVoice->SetVolume(fVolume);
}

int XA2Stream::GetVolume()
{
	float fVolume = 0.f;
	//pMasteringVoice->GetVolume(&m_fVolume);
	if (pSourceVoice)
		pSourceVoice->GetVolume(&fVolume);
	m_iVolume = fVolume * 100.0;
	return m_iVolume;
}

void XA2Stream::Play()
{
	m_iStatus = PS_PLAY;
	if (pSourceVoice)
		pSourceVoice->Start();

	SetSourceVoiceVolume();

//	ThreadBase::Start();
	if (!ThreadBase::IsAlive())
		ThreadBase::Start();
}

void XA2Stream::Pause()
{
	m_iStatus = PS_PAUSE;
	if (pSourceVoice)
		pSourceVoice->Stop();
}

void XA2Stream::Stop()
{
	bool bDoStop = true;
	if (m_iStatus == PS_PAUSE)
	{
		SetEvent(voiceContext.hBufferEndEvent);
		bDoStop = false;
	}

	if (m_iStatus == PS_STOP)
		return;

//	ThreadBase::Stop();
	m_iStatus = PS_STOP;

	if (bDoStop)
	{
		if (pSourceVoice)
			pSourceVoice->Stop();
	}
}

void XA2Stream::Continue()
{
	m_iStatus = PS_PLAY;
	if (m_bIsOpened)
	{
		if (pSourceVoice)
			pSourceVoice->Start();

		SetSourceVoiceVolume();

		if(!ThreadBase::IsAlive())
			ThreadBase::Start();
	}
	SetEvent(voiceContext.hBufferEndEvent);
}

void XA2Stream::SetSourceVoiceVolume()
{
	float fUnit = 1.0 / 100.0;
	float fVolume = m_iVolume * fUnit;

	if (pSourceVoice)
		pSourceVoice->SetVolume(fVolume);
}

void XA2Stream::SetBufferInfo(int iBufferSize, int iTotalOfBuffers)
{
//	char szMsg[512];
//	sprintf(szMsg, "XA2Stream::SetBufferInfo() - 0\n");
//	OutputDebugStringA(szMsg);

	m_iDSBufferSize = iBufferSize;
	m_iDSTotalOfBuffers = iTotalOfBuffers;

	if (m_pQBuffer)
	{
		m_pQBuffer->SetMax(iTotalOfBuffers);
		m_pQBuffer->SetBufferSize(iBufferSize);
	}

	if (m_pCurFrameBuffer)
		delete m_pCurFrameBuffer;

	if (m_pTmpFrameBuffer)
		delete m_pTmpFrameBuffer;

	if (m_pTmpFrameBuffer2)
		delete m_pTmpFrameBuffer2;

	m_pCurFrameBuffer = new unsigned char[iBufferSize];
	m_pTmpFrameBuffer = new unsigned char[iBufferSize * 4];
	m_pTmpFrameBuffer2 = new unsigned char[iBufferSize];
	m_iTmpBufOffset = 0;

	for (int i = 0; i < MAX_BUFFER_COUNT; i++)
	{
		if (m_pFrameBuffer[i])
			delete m_pFrameBuffer[i];

		m_pFrameBuffer[i] = new unsigned char[iBufferSize];
	}

//	sprintf(szMsg, "XA2Stream::SetBufferInfo() - 1\n");
//	OutputDebugStringA(szMsg);
}

void XA2Stream::Output(LPBYTE pBuffer, int iLen)
{
	if (m_bIsOpened)
	{
		EnterCriticalSection(&m_CriticalSection);

		AddBuffer(pBuffer, iLen);
		//Sleep(5);
		LeaveCriticalSection(&m_CriticalSection);
	}
}

void XA2Stream::AddBuffer(LPBYTE pBuffer, int iLen)
{
#if 0
	char szMsg[512];
//	sprintf(szMsg, "XA2Stream::AddBuffer() - 0 \n");
//	OutputDebugStringA(szMsg);

	if (m_pQBuffer && !m_pQBuffer->IsFull() && m_pQBuffer->GetUnitSize() > 0)
	{
//		sprintf(szMsg, "XA2Stream::AddBuffer() - 1 [%d , %d]\n", iLen, m_pQBuffer->GetUnitSize());
//		OutputDebugStringA(szMsg);

		if (iLen > m_pQBuffer->GetUnitSize())
		{
			sprintf(szMsg, "XA2Stream::AddBuffer() - iLen > m_pQBuffer->GetUnitSize() [%d , %d]\n", iLen, m_pQBuffer->GetUnitSize());
			OutputDebugStringA(szMsg);

			bool bDo = true;
			int iOffset = 0;
			int iBufLen = 0;
			while (bDo)
			{
				if (iLen - iOffset > m_pQBuffer->GetUnitSize())
				{
					iBufLen = m_pQBuffer->GetUnitSize();
					m_pQBuffer->Add(pBuffer + iOffset, iBufLen);
					iOffset += m_pQBuffer->GetUnitSize();
				}
				else
				{
					iBufLen = iLen - iOffset;
					m_pQBuffer->Add(pBuffer + iOffset, iBufLen);
					bDo = false;
				}
			}
		}
		else
		{
			BOOL bRet = IsBadReadPtr(pBuffer, iLen);
			if (bRet)
			{
				sprintf(szMsg, "XA2Stream::AddBuffer() The 'pBuffer' is bad prt!! \n");
				OutputDebugStringA(szMsg);
			}
			else
			{
				m_pQBuffer->Add(pBuffer, iLen);
			}
		}
	}
	
//	sprintf(szMsg, "XA2Stream::AddBuffer() - 2\n");
//	OutputDebugStringA(szMsg);
#else
	/*
	if (iLen > m_iDSBufferSize)
	{
		m_pQBuffer->Add(pBuffer, m_iDSBufferSize);
		int iOffset = iLen - m_iDSBufferSize;
		memcpy(m_pTmpFrameBuffer + m_iTmpBufOffset, pBuffer + m_iDSBufferSize, iOffset);
		m_iTmpBufOffset = iOffset;
	}
	else if (m_iTmpBufOffset > 0)
	{
		int iOffset = 0;
		int iTmpBufLen = m_iTmpBufOffset;
		memcpy(m_pTmpFrameBuffer + m_iTmpBufOffset, pBuffer, iLen);
		iTmpBufLen += iLen;
		bool bDo = true;
		while (bDo)
		{
			if (iTmpBufLen > m_iDSBufferSize)
			{
				memcpy(m_pTmpFrameBuffer2, m_pTmpFrameBuffer + iOffset, m_iDSBufferSize);
				m_pQBuffer->Add(m_pTmpFrameBuffer2, m_iDSBufferSize);
				iTmpBufLen -= m_iDSBufferSize;
				iOffset += m_iDSBufferSize;
			}
			else
			{
				memcpy(m_pTmpFrameBuffer2, m_pTmpFrameBuffer + iOffset, iTmpBufLen);
				memcpy(m_pTmpFrameBuffer, m_pTmpFrameBuffer2, iTmpBufLen);

				m_iTmpBufOffset = iTmpBufLen;
				bDo = false;
			}
		}
	}
	else
		m_pQBuffer->Add(pBuffer, iLen);
		*/

	int iOffset = 0;
	int iTmpBufLen = m_iTmpBufOffset;
	memcpy(m_pTmpFrameBuffer + m_iTmpBufOffset, pBuffer, iLen);
	iTmpBufLen += iLen;

	if (iTmpBufLen >= m_iDSBufferSize)
	{
		bool bDo = true;
		while (bDo)
		{
			if (iTmpBufLen > m_iDSBufferSize)
			{
				memcpy(m_pTmpFrameBuffer2, m_pTmpFrameBuffer + iOffset, m_iDSBufferSize);
				m_pQBuffer->Add(m_pTmpFrameBuffer2, m_iDSBufferSize);
				iTmpBufLen -= m_iDSBufferSize;
				iOffset += m_iDSBufferSize;
			}
			else if (iTmpBufLen == m_iDSBufferSize)
			{
				memcpy(m_pTmpFrameBuffer2, m_pTmpFrameBuffer + iOffset, m_iDSBufferSize);
				m_pQBuffer->Add(m_pTmpFrameBuffer2, m_iDSBufferSize);
				bDo = false;
				m_iTmpBufOffset = 0;
			}
			else
			{
				memcpy(m_pTmpFrameBuffer2, m_pTmpFrameBuffer + iOffset, iTmpBufLen);
				memcpy(m_pTmpFrameBuffer, m_pTmpFrameBuffer2, iTmpBufLen);

				m_iTmpBufOffset = iTmpBufLen;
				bDo = false;
			}
		}
	}
#endif
}

int XA2Stream::GetNextBuffer()
{
	if (!m_pQBuffer || m_pQBuffer->IsEmpty())
	{
		return 0; 
	}
	else
	{
		int iLen = m_pQBuffer->GetNext(m_pCurFrameBuffer);
		if (iLen > 0)
		{
			return iLen;
		}
		return 0;
	}
}

void XA2Stream::ResetBuffer()
{
	char szMsg[512];
//	sprintf(szMsg, "XA2Stream::ResetBuffer() - 0\n");
//	OutputDebugStringA(szMsg);

	if (m_pQBuffer)
		m_pQBuffer->Reset();

//	sprintf(szMsg, "XA2Stream::ResetBuffer() - 1\n");
//	OutputDebugStringA(szMsg);
}

int XA2Stream::GetBufferCount()
{
	if(m_pQBuffer)
		return m_pQBuffer->GetTotal();
	return 0;
}

bool XA2Stream::IsFulled()
{
	if (m_pQBuffer && m_pQBuffer->GetTotal() > (m_pQBuffer->GetMax() - 4))
		return true;
	return false;
}

void XA2Stream::UpdateBuffer()
{
	char szMsg[512];
//	sprintf(szMsg, "XA2Stream::UpdateBuffer() - 0\n");
//	OutputDebugStringA(szMsg);

	int iLen = GetNextBuffer();

	if (iLen > 0)
	{
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = iLen;

		memcpy(m_pFrameBuffer[m_iBufIndex], m_pCurFrameBuffer, iLen);
		buf.pAudioData = m_pFrameBuffer[m_iBufIndex];
		buf.Flags = XAUDIO2_END_OF_STREAM;

		if (pSourceVoice)
			pSourceVoice->SubmitSourceBuffer(&buf);

		m_iBufIndex++;

		if (m_iBufIndex >= MAX_BUFFER_COUNT)
			m_iBufIndex = 0;
	}
	else
	{

	}
//	sprintf(szMsg, "XA2Stream::UpdateBuffer() - 1\n");
//	OutputDebugStringA(szMsg);
}

void XA2Stream::ThreadEvent()
{
	ThreadBase::Lock();

	if (m_iStatus == PS_PLAY && m_bIsOpened)
	{
		XAUDIO2_VOICE_STATE state;
		if (pSourceVoice)
			pSourceVoice->GetState(&state);
		if (state.BuffersQueued < MAX_BUFFER_COUNT - 1)
			SetEvent(voiceContext.hBufferEndEvent);

		if (IsAlive())
		{
			WaitForSingleObject(voiceContext.hBufferEndEvent, INFINITE);
			UpdateBuffer();
		}
	}
	Sleep(1);
	ThreadBase::Unlock();
}
