#include "stdafx.h"
#include "OutAudioDevice.h"

OutAudioDevice::OutAudioDevice()
{
	m_pDSound = new DSound;

	m_bIsOpened = false;

	m_nChannels = 2;
	m_nFrequency = 8000;
	m_nBits = 16;
	m_iBufferSize = 320;
	m_iTotalOfBuffer = 120;
}

OutAudioDevice::~OutAudioDevice()
{
	delete m_pDSound;
}

void OutAudioDevice::Init(HWND hWnd)
{
	char szEventName[256];
	m_hWnd = hWnd;
	sprintf(szEventName,"WaveManager-%d",GetTickCount());
	m_pDSound->Init(hWnd,szEventName);
	m_pDSound->SetPrimaryBufferFormat(2, 22050, 16);
}


void OutAudioDevice::Open(bool bReset)
{
//	WAVEFORMATEX WFE;

	if(bReset)
	{
		char szEventName[256];
		sprintf(szEventName,"WaveManager-%d",GetTickCount());
		m_pDSound->Init(m_hWnd,szEventName);
		//m_pDSound->SetPrimaryBufferFormat(2, 22050, 16);
		m_pDSound->SetPrimaryBufferFormat(m_nChannels, m_nFrequency, m_nBits);
	}

	m_pDSound->SetBufferInfo(m_iBufferSize,m_iTotalOfBuffer);

#if 0
	WAVEFORMATEX WFE;
	WFE.cbSize = sizeof(WAVEFORMATEX);
	WFE.wFormatTag = WAVE_FORMAT_PCM;
	WFE.nChannels = m_nChannels;
	WFE.nSamplesPerSec = m_nFrequency;
	WFE.nAvgBytesPerSec = m_nFrequency * m_nChannels * m_nBits / 8;
	WFE.nBlockAlign = m_nChannels * m_nBits / 8;
	WFE.wBitsPerSample = m_nBits;

	HRESULT hr = m_pDSound->CreateSurface(&WFE);

#else
	if (m_nChannels > 2 || m_nBits > 16 || m_nFrequency > 48000)
	{
		WAVEFORMATEXTENSIBLE wfxe;
		ZeroMemory( &wfxe, sizeof(WAVEFORMATEXTENSIBLE) ); 
		wfxe.Format.wFormatTag      = (WORD) WAVE_FORMAT_EXTENSIBLE; 
		wfxe.Format.nChannels       = (WORD) m_nChannels; 
		wfxe.Format.nSamplesPerSec  = (DWORD) m_nFrequency; 
		wfxe.Format.wBitsPerSample  = (WORD) m_nBits; 
		wfxe.Format.nBlockAlign     = (WORD) (wfxe.Format.wBitsPerSample / 8 * wfxe.Format.nChannels);
		wfxe.Format.nAvgBytesPerSec = (DWORD) (wfxe.Format.nSamplesPerSec * wfxe.Format.nBlockAlign);
		wfxe.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		wfxe.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		wfxe.Samples.wValidBitsPerSample = wfxe.Format.wBitsPerSample;
		if (m_nChannels == 8)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
		else if (m_nChannels == 6)
		{
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
	//		wfxe.Samples.wValidBitsPerSample = 20;
		}
		else if (m_nChannels == 4)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
		else if (m_nChannels == 2)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
		else
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;

		HRESULT hr = m_pDSound->CreateSurface((WAVEFORMATEX*)&wfxe);
	}
	else
	{
		WAVEFORMATEX WFE;
		WFE.cbSize = sizeof(WAVEFORMATEX);
		WFE.wFormatTag = WAVE_FORMAT_PCM;
		WFE.nChannels = m_nChannels;
		WFE.nSamplesPerSec = m_nFrequency;
		WFE.nAvgBytesPerSec = m_nFrequency * m_nChannels * m_nBits / 8;
		WFE.nBlockAlign = m_nChannels * m_nBits / 8;
		WFE.wBitsPerSample = m_nBits;

		HRESULT hr = m_pDSound->CreateSurface(&WFE);
	}
#endif
	m_pDSound->Play();
	m_bIsOpened = true;
}

void OutAudioDevice::Close()
{
//	m_pWaveOut->Stop();
//	m_pWaveOut->Close();
	if(m_bIsOpened)
	{
		m_pDSound->Stop();
		m_pDSound->Close();
		m_pDSound->Reset();
	}
	m_bIsOpened = false;
}

void OutAudioDevice::Output(char* pBuffer,int iLen)
{
	//m_pWaveOut->OutputWavBuffer(pBuffer,iLen);
	m_pDSound->AddBuffer((unsigned char *)pBuffer,iLen);
}

void OutAudioDevice::Stop()
{
	__try
	{
		//m_pWaveOut->Stop();
		m_pDSound->Stop();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		char szMsg[512];
		sprintf(szMsg,"HEError - Catching an exception in OutAudioDevice::Stop() !!\n");
		OutputDebugStringA(szMsg);
	}
}

void OutAudioDevice::Continue()
{
	m_pDSound->Continue();
}

void OutAudioDevice::SetWaveFormat(int nChannels,int nFrequency,int nBits)
{
	m_nChannels = nChannels;
	m_nFrequency = nFrequency;
	m_nBits = nBits;
}

void OutAudioDevice::SetBufferInfo(int iBufferSize,int iTotalOfBuffers)
{
	m_iBufferSize = iBufferSize;
	m_iTotalOfBuffer = iTotalOfBuffers;
}

bool OutAudioDevice::IsReady()
{
	return m_pDSound->IsReady();
}

void OutAudioDevice::SetVolume(int iValue)
{
	m_pDSound->SetVolume(iValue);
}

long OutAudioDevice::GetVolume()
{
	return m_pDSound->GetVolume();
}

bool OutAudioDevice::IsOpened()
{
	return m_bIsOpened;
}

void OutAudioDevice::ResetBuffer()
{
	m_pDSound->ResetBuffer();
}

bool OutAudioDevice::IsFulled()
{
	return m_pDSound->IsFulled();
}
int OutAudioDevice::GetBufferCount()
{
	return m_pDSound->GetBufferCount();
}

void OutAudioDevice::AddTimestamp(unsigned long ulTimestamp)
{
	m_pDSound->AddTimestamp(ulTimestamp);
}

unsigned long OutAudioDevice::GetLastTimestamp()
{
	return m_pDSound->GetLastTimestamp();
}