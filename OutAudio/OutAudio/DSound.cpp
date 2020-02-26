#include "stdafx.h"
#include "DSound.h"
#include <process.h>
#include "..\\..\\Include\\Common.h"
#include "VolumeMixer.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "Winmm.lib")

//#if defined(_WIN64)
#pragma comment(lib, "dxerr.lib")
//#else
//#pragma comment(lib, "dxerr8.lib")
//#endif

#pragma comment(lib, "dxguid.lib")
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define _ENABLE_THREAD 1

int(WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

unsigned int __stdcall _PlaybackProc(void* lpvThreadParm);

#ifdef _WIN64
void CALLBACK TimerProcess(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
#else
void CALLBACK TimerProcess(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
#endif
{
	DSound *pDDS = (DSound *)dwUser;
	pDDS->TimerCallback();	
}

Average::Average()
{
	Reset();
}

Average::~Average()
{
}

void Average::Reset()
{
	m_iTotalOfItems = 0;
	for (int i = 0; i < _MAX_ITEMS; i++)
		m_fValue[i] = 0;
	m_fAvgValue = 0.0;
	m_ulCount = 0;
}

void Average::Calculate(float fValue)
{
	m_fValue[m_ulCount % _MAX_ITEMS] = fValue;

	if (m_iTotalOfItems < _MAX_ITEMS)
		m_iTotalOfItems++;

	unsigned long ulTotalOfValues = 0;
	for (int i = 0; i < m_iTotalOfItems; i++)
		ulTotalOfValues += m_fValue[i];

	m_fAvgValue = (float)ulTotalOfValues / (float)m_iTotalOfItems;

	m_ulCount++;
}

float Average::GetAverage()
{
	return m_fAvgValue;
}

DSound::DSound()
{
	m_timerID = 0;
	m_pDS = NULL;
	m_hThread = 0;
	m_iStatus = PS_STOP;

//	ZeroMemory(&m_WFE, sizeof(m_WFE));
	m_lpDSB = NULL;
//	m_pHEvent[0] = CreateEvent(NULL, FALSE, FALSE, _T("Direct_Sound_Buffer_Notify_0"));
//	m_pHEvent[1] = CreateEvent(NULL, FALSE, FALSE, _T("Direct_Sound_Buffer_Notify_1"));	

#if 0
	m_bDSBufferIsFinished[0] = true;
	m_bDSBufferIsFinished[1] = true;
#else
	for (int i = 0; i < EVENT_MUM; i++)
		m_bDSBufferIsFinished[i] = true;
#endif

	m_iPlayIndex = 0;
	m_iNextIndex = 0;
	m_lpAudioBuf = 0;
	m_iDSBufferSize = 4000;
	m_bBufferIsEmpty = true;

#if _ENABLE_QBUFFER
	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(0);

	m_pTempFrameBuffer = 0;
#else
	m_iAudioBufCount = 0;
	for(int i = 0;i < _MAX_DS_BUFFER;i++)
	{
		m_lpAudioBuf1[i] = 0;
		m_iAudioBufLen[i] = 0;
	}
#endif

	m_lVolume = 50.0 * 1.28;
	m_pMixBuffer = new BYTE[102400];

	m_ulLastTimestamp = 0;
	m_pTimestampQ = new SQQueue;
	m_pTimestampQ->Alloc(sizeof(long),100);

	InitializeCriticalSection(&m_CriticalSection);
}

DSound::~DSound()
{
	if(m_timerID)
		Close();

	SAFE_RELEASE( m_pDS ); 
//	SAFE_RELEASE( m_lpDSB );

	if (NULL != m_lpAudioBuf) 
	{
		delete []m_lpAudioBuf;
		m_lpAudioBuf = NULL;
	}

	if (m_pMixBuffer)
		delete m_pMixBuffer;

#if _ENABLE_QBUFFER
	if (m_pQBuffer)
		delete m_pQBuffer;
	if (m_pTempFrameBuffer)
		delete m_pTempFrameBuffer;
#else
	for(int i = 0;i < _MAX_DS_BUFFER;i++)
	{
		if (NULL != m_lpAudioBuf1[i]) 
		{
			delete []m_lpAudioBuf1[i];
			m_lpAudioBuf1[i] = NULL;
		}
	}
#endif

	for (int i = 0; i < EVENT_MUM; i++)
	{
		if (m_pHEvent[i])
			CloseHandle(m_pHEvent[i]);
	}

	ResetTimestamp();

	if (m_pTimestampQ)
		delete m_pTimestampQ;

	DeleteCriticalSection(&m_CriticalSection);
}

HRESULT DSound::Init(HWND hWnd,char* szEventName)
{
	HRESULT             hr;
	char szObjName[256];
	DWORD dwCoopLevel = DSSCL_PRIORITY;// DSSCL_EXCLUSIVE;// DSSCL_PRIORITY;

    SAFE_RELEASE( m_pDS );

#if 0
	/*
	sprintf(szObjName,"%s%d",szEventName,0);
	m_pHEvent[0] = CreateEvent(NULL, FALSE, FALSE, szObjName);
	sprintf(szObjName,"%s%d",szEventName,1);
	m_pHEvent[1] = CreateEvent(NULL, FALSE, FALSE,szObjName);	
	*/
#else
	for (int i = 0; i < EVENT_MUM; i++)
	{
		sprintf(szObjName, "%s%d", szEventName, i);
		m_pHEvent[i] = CreateEvent(NULL, FALSE, FALSE, szObjName);
	}
#endif

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );   

    return S_OK;
}

HRESULT DSound::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

#if 0
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );
#else
	if (dwPrimaryChannels > 2 || dwPrimaryBitRate > 16 || dwPrimaryFreq > 48000)
	{
		WAVEFORMATEXTENSIBLE wfxe;
		ZeroMemory( &wfxe, sizeof(WAVEFORMATEXTENSIBLE) ); 
		wfxe.Format.wFormatTag      = (WORD) WAVE_FORMAT_EXTENSIBLE; 
		wfxe.Format.nChannels       = (WORD) dwPrimaryChannels; 
		wfxe.Format.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
		wfxe.Format.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
		wfxe.Format.nBlockAlign     = (WORD) (wfxe.Format.wBitsPerSample / 8 * wfxe.Format.nChannels);
		wfxe.Format.nAvgBytesPerSec = (DWORD) (wfxe.Format.nSamplesPerSec * wfxe.Format.nBlockAlign);
		wfxe.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		wfxe.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		wfxe.Samples.wValidBitsPerSample = wfxe.Format.wBitsPerSample;
		if (dwPrimaryChannels == 8)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
		else if (dwPrimaryChannels == 6)
		{
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
	//		wfxe.Samples.wValidBitsPerSample = 20;
		}
		else if (dwPrimaryChannels == 4)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
		else if (dwPrimaryChannels == 2)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
		else
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;

		if( FAILED( hr = pDSBPrimary->SetFormat((LPCWAVEFORMATEX)&wfxe) ) )
			return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}
	else
	{
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}
#endif

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}

void DSound::SetBufferInfo(int iBufferSize,int iTotalOfBuffers)
{
	m_iDSBufferSize = iBufferSize;
	m_iDSTotalOfBuffers = iTotalOfBuffers;

	//New audio buffer
	if (NULL != m_lpAudioBuf) 
	{
		delete []m_lpAudioBuf;
		m_lpAudioBuf = NULL;		
	}
	m_lpAudioBuf = new BYTE[m_iDSBufferSize];

	//Init Audio Buffer
	memset(m_lpAudioBuf, 0, m_iDSBufferSize);

#if _ENABLE_QBUFFER
	if (m_pQBuffer)
	{
		m_pQBuffer->SetMax(iTotalOfBuffers);
		m_pQBuffer->SetBufferSize(iBufferSize);
	}

	if (m_pTempFrameBuffer)
		delete m_pTempFrameBuffer;

	m_pTempFrameBuffer = new unsigned char[iBufferSize];
#else
	for(int i = 0;i < _MAX_DS_BUFFER;i++)
	{
		if (NULL != m_lpAudioBuf1[i]) 
		{
			delete []m_lpAudioBuf1[i];
			m_lpAudioBuf1[i] = NULL;		
		}

		if(i < m_iDSTotalOfBuffers)
		{
			m_lpAudioBuf1[i] = new BYTE[m_iDSBufferSize];
			memset(m_lpAudioBuf1[i], 0, m_iDSBufferSize);
		}
	}
#endif
}

HRESULT DSound::CreateSurface(WAVEFORMATEX* pWFE)
{
	HRESULT             hr;
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

	//m_WFE = WFE;
//	memcpy(&m_WFE,pWFE,sizeof(WAVEFORMATEX));
//	m_iDSBufferSize = iSize;

	//Create Second Sound Buffer
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
	dsbd.dwBufferBytes = m_iDSBufferSize*EVENT_MUM;//2*m_WFE.nAvgBytesPerSec; //2 Seconds Buffer
	dsbd.lpwfxFormat = pWFE;//&m_WFE;

	if ( FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL)) ) {
		OutputDebugString(_T("Create Second Sound Buffer Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

	//Query DirectSoundNotify
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	if ( FAILED(hr = m_lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSBNotify)) ) {
		OutputDebugString(_T("QueryInterface DirectSoundNotify Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

#if 0
	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[2];

	pPosNotify[0].dwOffset = m_iDSBufferSize / 2 - 1;
	pPosNotify[1].dwOffset = m_iDSBufferSize - 1;		
	pPosNotify[0].hEventNotify = m_pHEvent[0];
	pPosNotify[1].hEventNotify = m_pHEvent[1];	

	if ( FAILED(hr = lpDSBNotify->SetNotificationPositions(2, pPosNotify)) ) 
	{
		OutputDebugString(_T("Set NotificationPosition Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}	
	return hr;
#else
	int iPos = m_iDSBufferSize;
	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[EVENT_MUM];
	for (int i = 0; i < EVENT_MUM; i++)
	{
		pPosNotify[i].dwOffset = iPos * (i + 1) - 1;
		pPosNotify[i].hEventNotify = m_pHEvent[i];
	}

	if (FAILED(hr = lpDSBNotify->SetNotificationPositions(EVENT_MUM, pPosNotify)))
	{
		OutputDebugString(_T("Set NotificationPosition Failed!"));
		//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR(TEXT("SetFormat"), hr);
	}
	return hr;
#endif

/*
	//New audio buffer
	if (NULL != m_lpAudioBuf) 
	{
		delete []m_lpAudioBuf;
		m_lpAudioBuf = NULL;		
	}
	m_lpAudioBuf = new BYTE[m_iDSBufferSize];

	//Init Audio Buffer
	memset(m_lpAudioBuf, 0, m_iDSBufferSize);

	for(int i = 0;i < _MAX_DS_BUFFER;i++)
	{
		if (NULL != m_lpAudioBuf1[i]) 
		{
			delete []m_lpAudioBuf1[i];
			m_lpAudioBuf1[i] = NULL;		
		}
		m_lpAudioBuf1[i] = new BYTE[m_iDSBufferSize];

		memset(m_lpAudioBuf1[i], 0, m_iDSBufferSize);
	}
	*/
}
/*
HRESULT DSound::CreateSurface1(DWORD dwChannels, DWORD dwFreq, DWORD dwBits)
{
	HRESULT             hr;
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    //dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;

#if 0
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 // Better position accuracy
	                | DSBCAPS_GLOBALFOCUS         // Allows background playing
	                | DSBCAPS_CTRLVOLUME
					| DSBCAPS_CTRLPOSITIONNOTIFY;         // volume control enabled
#else
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
#endif

    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

	dsbd.dwBufferBytes = m_iDSBufferSize * 2;//2*m_WFE.nAvgBytesPerSec; //2 Seconds Buffer

	if (dwChannels > 2 || dwBits > 16 || dwFreq > 48000)
	{
		WAVEFORMATEXTENSIBLE wfxe;
		ZeroMemory( &wfxe, sizeof(WAVEFORMATEXTENSIBLE) ); 
		wfxe.Format.wFormatTag      = (WORD) WAVE_FORMAT_EXTENSIBLE; 
		wfxe.Format.nChannels       = (WORD) dwChannels; 
		wfxe.Format.nSamplesPerSec  = (DWORD) dwFreq; 
		wfxe.Format.wBitsPerSample  = (WORD) dwBits; 
		wfxe.Format.nBlockAlign     = (WORD) (wfxe.Format.wBitsPerSample / 8 * wfxe.Format.nChannels);
		wfxe.Format.nAvgBytesPerSec = (DWORD) (wfxe.Format.nSamplesPerSec * wfxe.Format.nBlockAlign);
		wfxe.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		wfxe.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		wfxe.Samples.wValidBitsPerSample = wfxe.Format.wBitsPerSample;
		if (dwChannels == 8)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
		else if (dwChannels == 6)
		{
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
	//		wfxe.Samples.wValidBitsPerSample = 20;
		}
		else if (dwChannels == 4)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
		else if (dwChannels == 2)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
		else
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;

		//if( FAILED( hr = pDSBPrimary->SetFormat((LPCWAVEFORMATEX)&wfxe) ) )
			//return DXTRACE_ERR( TEXT("SetFormat"), hr );

		//Create Second Sound Buffer
	//	if (dwPrimaryChannels > 2)
		//	dsbd.dwFlags |= DSBCAPS_LOCHARDWARE;

		//dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
		dsbd.lpwfxFormat = (WAVEFORMATEX*)&wfxe;//&m_WFE;
	}
	else
	{
		WAVEFORMATEX wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
		wfx.nChannels       = (WORD) dwChannels; 
		wfx.nSamplesPerSec  = (DWORD) dwFreq; 
		wfx.nBlockAlign     = (WORD) (dwChannels * dwBits / 8);
		wfx.nAvgBytesPerSec = (DWORD) (dwFreq * dwChannels * dwBits / 8);
		wfx.wBitsPerSample  = (WORD) dwBits; 

		//if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
		//	return DXTRACE_ERR( TEXT("SetFormat"), hr );

		dsbd.lpwfxFormat = (WAVEFORMATEX*)&wfx;//&m_WFE;
	}

	if ( FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL)) ) {
		OutputDebugString(_T("Create Second Sound Buffer Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

	//Query DirectSoundNotify
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	if ( FAILED(hr = m_lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSBNotify)) ) {
		OutputDebugString(_T("QueryInterface DirectSoundNotify Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[2];
#if 0
	pPosNotify[0].dwOffset = m_WFE.nAvgBytesPerSec/2 - 1;
	pPosNotify[1].dwOffset = 3*m_WFE.nAvgBytesPerSec/2 - 1;		
	//pPosNotify[0].dwOffset = m_iDSBufferSize / 3 - 1;
	//pPosNotify[1].dwOffset = m_iDSBufferSize + m_iDSBufferSize / 3 - 1;		
	pPosNotify[0].hEventNotify = m_pHEvent[0];
	pPosNotify[1].hEventNotify = m_pHEvent[1];	
#else
	//pPosNotify[0].dwOffset = m_iDSBufferSize * 0.5 - 1;
	//pPosNotify[1].dwOffset = m_iDSBufferSize + m_iDSBufferSize * 0.5 - 1;		
	pPosNotify[0].dwOffset = m_iDSBufferSize / 2 - 1;
	pPosNotify[1].dwOffset = m_iDSBufferSize - 1;		
	pPosNotify[0].hEventNotify = m_pHEvent[0];
	pPosNotify[1].hEventNotify = m_pHEvent[1];	
#endif

	if ( FAILED(hr = lpDSBNotify->SetNotificationPositions(2, pPosNotify)) ) 
	{
		OutputDebugString(_T("Set NotificationPosition Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}	
	return hr;
}

HRESULT DSound::CreateSurface2(DWORD dwChannels, DWORD dwFreq, DWORD dwBits)
{
	HRESULT hr;
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

	//m_WFE = WFE;
//	memcpy(&m_WFE,pWFE,sizeof(WAVEFORMATEX));
//	m_iDSBufferSize = iSize;

	//Create Second Sound Buffer
	dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
	dsbd.dwBufferBytes = m_iDSBufferSize * 2;//2*m_WFE.nAvgBytesPerSec; //2 Seconds Buffer

	if (dwChannels > 2 || dwBits > 16 || dwFreq > 48000)
	{
		WAVEFORMATEXTENSIBLE wfxe;
		ZeroMemory( &wfxe, sizeof(WAVEFORMATEXTENSIBLE) ); 
		wfxe.Format.wFormatTag      = (WORD) WAVE_FORMAT_EXTENSIBLE; 
		wfxe.Format.nChannels       = (WORD) dwChannels; 
		wfxe.Format.nSamplesPerSec  = (DWORD) dwFreq; 
		wfxe.Format.wBitsPerSample  = (WORD) dwBits; 
		wfxe.Format.nBlockAlign     = (WORD) (wfxe.Format.wBitsPerSample / 8 * wfxe.Format.nChannels);
		wfxe.Format.nAvgBytesPerSec = (DWORD) (wfxe.Format.nSamplesPerSec * wfxe.Format.nBlockAlign);
		wfxe.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		wfxe.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		wfxe.Samples.wValidBitsPerSample = wfxe.Format.wBitsPerSample;
		if (dwChannels == 8)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
		else if (dwChannels == 6)
		{
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
	//		wfxe.Samples.wValidBitsPerSample = 20;
		}
		else if (dwChannels == 4)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
		else if (dwChannels == 2)
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
		else
			wfxe.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;

		//if( FAILED( hr = pDSBPrimary->SetFormat((LPCWAVEFORMATEX)&wfxe) ) )
			//return DXTRACE_ERR( TEXT("SetFormat"), hr );

		//Create Second Sound Buffer
	//	if (dwPrimaryChannels > 2)
		//	dsbd.dwFlags |= DSBCAPS_LOCHARDWARE;

		//dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
		dsbd.lpwfxFormat = (WAVEFORMATEX*)&wfxe;//&m_WFE;
	}
	else
	{
		WAVEFORMATEX WFE;
		WFE.cbSize = sizeof(WAVEFORMATEX);
		WFE.wFormatTag = WAVE_FORMAT_PCM;
		WFE.nChannels = dwChannels;
		WFE.nSamplesPerSec = dwFreq;
		WFE.nAvgBytesPerSec = dwFreq * dwChannels * dwBits / 8;
		WFE.nBlockAlign = dwChannels * dwBits / 8;
		WFE.wBitsPerSample = dwBits;

		//hr = CreateSurface(&WFE);
		dsbd.lpwfxFormat = &WFE;//&m_WFE;
	}

	if ( FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL)) ) {
		OutputDebugString(_T("Create Second Sound Buffer Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

	//Query DirectSoundNotify
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	if ( FAILED(hr = m_lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSBNotify)) ) {
		OutputDebugString(_T("QueryInterface DirectSoundNotify Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}

	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[2];

	//pPosNotify[0].dwOffset = m_iDSBufferSize * 0.5 - 1;
	//pPosNotify[1].dwOffset = m_iDSBufferSize + m_iDSBufferSize * 0.5 - 1;		
	pPosNotify[0].dwOffset = m_iDSBufferSize / 2 - 1;
	pPosNotify[1].dwOffset = m_iDSBufferSize - 1;		
	pPosNotify[0].hEventNotify = m_pHEvent[0];
	pPosNotify[1].hEventNotify = m_pHEvent[1];	

	if ( FAILED(hr = lpDSBNotify->SetNotificationPositions(2, pPosNotify)) ) 
	{
		OutputDebugString(_T("Set NotificationPosition Failed!"));
//		m_strLastError = _T("MyDirectSound SetFormat Failed!");
		return DXTRACE_ERR( TEXT("SetFormat"), hr );
	}	

//	return hr;
}
*/

void DSound::Play()
{
//	if(!m_bFinished)
//		return ;
    m_iStatus = PS_PLAY;
	//TRACE("DSound::Play()\n");
	OutputDebugString("DSound::Play()\n");

//	if(m_timerID || m_hThread)
//		Close();

	m_bFinished = false;
//	SetEventLoop(1);
	m_lpDSB->Play(0, 0, DSBPLAY_LOOPING);

	
#if _ENABLE_THREAD
	if(!m_hThread)
	{
		unsigned threadID;
		m_hThread = (HANDLE)_beginthreadex( NULL, 0, _PlaybackProc, this, 0, &threadID );
	}
#else
	//timeSetEvent
	if(!m_timerID)
		m_timerID = timeSetEvent(5, 1, TimerProcess, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
#endif
}

void DSound::Stop()
{
	m_iStatus = PS_STOP;
	if(m_lpDSB)
		m_lpDSB->Stop();
//	timeKillEvent(m_timerID);
	OutputDebugString("DSound::Stop() \n");
}

void DSound::Continue()
{
	m_iStatus = PS_PLAY;
	if (m_lpDSB)
	m_lpDSB->Play(0, 0, DSBPLAY_LOOPING);
	OutputDebugString("DSound::Continue()\n");
}

void DSound::Close()
{
	m_iStatus = PS_STOP;
//	EnterCriticalSection(&m_CriticalSection);
	//TRACE("DSound::Close() begin..\n");
	OutputDebugString("DSound::Close() begin..\n");
	m_bFinished = true;
//	SetEventLoop(0);
//	m_lpDSB->Stop();
	
#if _ENABLE_THREAD
	if(m_hThread)
	{
		for (int i = 0; i < EVENT_MUM; i++)
		{
			if(m_pHEvent[i])
				SetEvent(m_pHEvent[i]);
		}

		::WaitForSingleObject(m_hThread, 1000); //1000
		CloseHandle(m_hThread);
		m_hThread = 0;
	}
#else
	if(m_timerID)
		timeKillEvent(m_timerID);
	m_timerID = 0;
#endif
	//TRACE("DSound::Close() end..\n");
	OutputDebugString("DSound::Close() end..\n");
//	LeaveCriticalSection(&m_CriticalSection);
}

void DSound::Reset()
{
	m_iPlayIndex = 0;
	m_iNextIndex = 0;
	m_iDSTotalOfBuffers = 0;

#if _ENABLE_QBUFFER
	if (m_pQBuffer)
		m_pQBuffer->Reset();
#else
	m_iAudioBufCount = 0;
	for(int i = 0;i < _MAX_DS_BUFFER;i++)
	{
		m_iAudioBufLen[i] = 0;
		if (NULL != m_lpAudioBuf1[i]) 
		{
			delete []m_lpAudioBuf1[i];
			m_lpAudioBuf1[i] = NULL;		
		}
	}
#endif

	m_lpDSB->Release();
	ResetTimestamp();

	OutputDebugString("DSound::Reset()\n");
}

void DSound::ResetBuffer()
{
#if _ENABLE_QBUFFER
	m_pQBuffer->Reset();
#else
	m_iAudioBufCount = 0;
#endif
	m_iPlayIndex = 0;
	m_iNextIndex = 0;
	m_bBufferIsEmpty = true;

	ResetTimestamp();

	OutputDebugString("DSound::ResetBuffer()\n");
}

bool DSound::IsReady()
{
#if _ENABLE_QBUFFER
	if(!m_pQBuffer->IsFull()) //!m_pQBuffer->IsFull()
#else
	if(m_iAudioBufCount < m_iDSTotalOfBuffers)
#endif
		return true;
	return false;
}

bool DSound::IsEmpty()
{
#if _ENABLE_QBUFFER
	int iFinishedCount = 0;
	for (int i = 0; i < EVENT_MUM; i++)
		if (m_bDSBufferIsFinished[i])
			iFinishedCount++;

	if (m_pQBuffer->IsEmpty() && iFinishedCount == EVENT_MUM && m_bBufferIsEmpty)
#else
	if(m_iAudioBufCount == 0 && m_bDSBufferIsFinished[0] && m_bDSBufferIsFinished[1] && m_bBufferIsEmpty)
#endif
		return true;
	return false;
}

int DSound::GetDSBufferSize()
{
	return m_iDSBufferSize;
}

void DSound::AddBuffer(LPBYTE pBuffer,int iLen)
{
#if _ENABLE_QBUFFER
	if (!m_pQBuffer->IsFull())
	{
		//memset(m_pMixBuffer,0,102400);
		//MixAudioFormat(m_pMixBuffer, pBuffer, AUDIO_S16, iLen, m_lVolume);
		//m_pQBuffer->Add(m_pMixBuffer, iLen);
		
		m_pQBuffer->Add(pBuffer, iLen);

		if (m_iStatus == PS_WAIT)
			Continue();
	}
	else
		//TRACE("AddBuffer - isfull!!\n");
		OutputDebugString("DSound::AddBuffer - isfull!!\n");
#else
	if(m_iAudioBufCount < m_iDSTotalOfBuffers && m_iDSBufferSize >= iLen)
	{
		memset(m_lpAudioBuf1[m_iNextIndex], 0, m_iDSBufferSize);
		memcpy(m_lpAudioBuf1[m_iNextIndex],pBuffer, iLen);
		m_iAudioBufLen[m_iNextIndex] = iLen;
		m_iNextIndex = (m_iNextIndex + 1) % m_iDSTotalOfBuffers;
//		TRACE("m_iAudioBufCount %d\n",m_iAudioBufCount);
		m_iAudioBufCount++;
		m_bBufferIsEmpty = false;
	}
#endif
}

int DSound::GetNextBuffer(LPBYTE pBuffer)
{
#if _ENABLE_QBUFFER

	m_AvgBuffer.Calculate(m_pQBuffer->GetTotal());
	float fAvg = m_AvgBuffer.GetAverage();

	char szMsg[512];
	if (m_pQBuffer->GetTotal() <= 3 || m_pQBuffer->GetTotal() > 180)
	{
	sprintf(szMsg, "DSound::GetNextBuffer - avg_buffer:%3.2f [%d]\n", fAvg, m_pQBuffer->GetTotal());
	OutputDebugString(szMsg);
	}

	memset(pBuffer, 0, m_iDSBufferSize);
	if (m_pQBuffer->IsEmpty())
	{
		//TRACE("GetNextBuffer - empty!!\n");
		OutputDebugString("DSound::GetNextBuffer - empty!!\n");
		return 0;
	}
	else
	{
		int iLen = m_pQBuffer->GetNext(m_pTempFrameBuffer);
		if (iLen > 0)
		{
			memcpy(pBuffer, m_pTempFrameBuffer, iLen);
			return iLen;
		}
		return 0;
	}
#else
	int iLen = 0;
	memset(pBuffer, 0, m_iDSBufferSize);
	if(m_iAudioBufCount > 0)
	{
		memcpy(pBuffer, m_lpAudioBuf1[m_iPlayIndex], m_iAudioBufLen[m_iPlayIndex]);
		iLen = m_iAudioBufLen[m_iPlayIndex];
		m_iAudioBufCount--;
//        TRACE("m_iAudioBufCount %d m_iPlayIndex %d\n",m_iAudioBufCount,m_iPlayIndex);
		m_iPlayIndex = (m_iPlayIndex + 1) % m_iDSTotalOfBuffers;

        return iLen;
	}
	else
	{
		TRACE("GetNextBuffer - empty!!\n");
		return 0;
	}
#endif
}

bool DSound::IsFulled()
{
	if (m_pQBuffer->GetTotal() > (m_pQBuffer->GetMax() - 4))
		return true;
	return false;
}

HRESULT DSound::FillBuffer(int iIndex)
{
#if 0
	LPVOID lpvAudio1 = NULL, lpvAudio2 = NULL;
	DWORD dwBytesAudio1 = 0, dwBytesAudio2 = 0;

	int iRet = GetNextBuffer(m_lpAudioBuf);
	if(iRet == 0)
		return -1;

	if (iRet < m_iDSBufferSize) 
	{
		//DWORD dwRetBytes = dwRetSamples*m_WFE.nBlockAlign;
		memset(m_lpAudioBuf + iRet, 0, m_iDSBufferSize - iRet);				
	}

	HRESULT hr = m_lpDSB->Lock(m_iDSBufferSize * iIndex, m_iDSBufferSize, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0);
	if ( FAILED(hr) ) 
	{
		//m_strLastError = _T("Lock DirectSoundBuffer Failed!");
		//OutputDebugString(m_strLastError);
		return DXTRACE_ERR( TEXT("m_lpDSB->Lock"), hr );
	}		

	//Copy AudioBuffer to DirectSoundBuffer
	if (NULL == lpvAudio2) 
	{
		memcpy(lpvAudio1, m_lpAudioBuf, dwBytesAudio1);
	}
	else 
	{
		memcpy(lpvAudio1, m_lpAudioBuf, dwBytesAudio1);
		memcpy(lpvAudio2, m_lpAudioBuf + dwBytesAudio1, dwBytesAudio2);
		//memcpy(lpvAudio2, m_lpAudioBuf, dwBytesAudio2);
	}
	
	//Unlock DirectSoundBuffer
	m_lpDSB->Unlock(lpvAudio1, dwBytesAudio1, lpvAudio2, dwBytesAudio2);
#else
	if(	m_iStatus == PS_STOP)
		return 0;

//	EnterCriticalSection(&m_CriticalSection);
	LPVOID lpvAudio1 = NULL, lpvAudio2 = NULL;
	DWORD dwBytesAudio1 = 0, dwBytesAudio2 = 0;

	int iRet = GetNextBuffer(m_lpAudioBuf);
	if(iRet == 0)
	{
		m_bBufferIsEmpty = true;
		m_iStatus = PS_WAIT;
		if (m_lpDSB)
			m_lpDSB->Stop();
//		Sleep(1);
		return -1;
	}
    else
	{
		if (iRet < m_iDSBufferSize) 
		{
			//DWORD dwRetBytes = dwRetSamples*m_WFE.nBlockAlign;
			memset(m_lpAudioBuf + iRet, 0, m_iDSBufferSize - iRet);				
		}

		UpdateTimestamp();
	}

	//HRESULT hr = m_lpDSB->Lock(m_iDSBufferSize * iIndex, m_iDSBufferSize, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0);
	int iBufLen = iRet;

	if (iBufLen != m_iDSBufferSize)
	{
		char szMsg[512];
		sprintf(szMsg,"DSound::FillBuffer iBufLen != m_iDSBufferSize [%d , %d]\n", iBufLen , m_iDSBufferSize);
		OutputDebugString(szMsg);
	}

	//if (iBufLen == 0)
		//return -1;

	HRESULT hr;

	if (iBufLen == 0)
		hr = m_lpDSB->Lock(m_iDSBufferSize * iIndex, m_iDSBufferSize, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0);
	else
		hr = m_lpDSB->Lock(m_iDSBufferSize * iIndex, iBufLen, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0);
	if ( FAILED(hr) ) 
	{
		//m_strLastError = _T("Lock DirectSoundBuffer Failed!");
		//OutputDebugString(m_strLastError);
		//TRACE("Lock DirectSoundBuffer Failed!\n");
		OutputDebugString("DSound::FillBuffer Lock DirectSoundBuffer Failed!\n");
//		LeaveCriticalSection(&m_CriticalSection);
		return DXTRACE_ERR( TEXT("m_lpDSB->Lock"), hr );
	}		

	//Copy AudioBuffer to DirectSoundBuffer
	if (NULL == lpvAudio2) 
	{
		memcpy(lpvAudio1, m_lpAudioBuf, dwBytesAudio1);

		//OutputDebugString("DSound::FillBuffer (0)\n");
	}
	else 
	{
		memcpy(lpvAudio1, m_lpAudioBuf, dwBytesAudio1);
		memcpy(lpvAudio2, m_lpAudioBuf + dwBytesAudio1, dwBytesAudio2);
		//memcpy(lpvAudio2, m_lpAudioBuf, dwBytesAudio2);

		OutputDebugString("DSound::FillBuffer (1)\n");
	}
	
	//Unlock DirectSoundBuffer
	m_lpDSB->Unlock(lpvAudio1, dwBytesAudio1, lpvAudio2, dwBytesAudio2);
//	LeaveCriticalSection(&m_CriticalSection);
	return 0;
#endif
}

int DSound::GetBufferCount()
{
	return m_pQBuffer->GetTotal();
}

void DSound::AddTimestamp(unsigned long ulTimestamp)
{
	m_pTimestampQ->Add((unsigned char*)&ulTimestamp,sizeof(unsigned long));

	//m_TimestampQueue.push(ulTimestamp);
}

void DSound::UpdateTimestamp()
{
//	m_ulLastTimestamp = m_TimestampQueue.front();
//	m_TimestampQueue.pop();

	unsigned char* p = m_pTimestampQ->GetNext();
	memcpy(&m_ulLastTimestamp, p,sizeof(unsigned long));
}

void DSound::ResetTimestamp()
{
	m_pTimestampQ->Reset();
	/*
	int iTotal = m_TimestampQueue.size();
	for (int j = 0; j < iTotal; j++)
		m_TimestampQueue.pop();
		*/
	m_ulLastTimestamp = 0;
}

unsigned long DSound::GetLastTimestamp()
{
	return m_ulLastTimestamp;
}

void DSound::TimerCallback()
{

#if 0
	HRESULT hr = WaitForMultipleObjects(EVENT_MUM, m_pHEvent, FALSE, INFINITE);
	if(WAIT_OBJECT_0 == hr)
	{
		m_bDSBufferIsFinished[1] = true;
		FillBuffer(1);
	}
	else if (WAIT_OBJECT_0 + 1 == hr)
	{
		m_bDSBufferIsFinished[0] = true;
		FillBuffer(0);
	}
	else
		return;
	
#else
	HRESULT hr = WaitForMultipleObjects(EVENT_MUM, m_pHEvent, FALSE, INFINITE);
	DWORD thread_index = hr - WAIT_OBJECT_0;

	DWORD dwCurTime = timeGetTime();
	/*
	char szMsg[512];

	if(m_ulLastTime > 0)
		sprintf(szMsg, "DSound::TimerCallback() - [%d] %d (%d)\n", thread_index, dwCurTime, dwCurTime - m_ulLastTime);
	else
		sprintf(szMsg, "DSound::TimerCallback() - [%d] %d \n", thread_index, dwCurTime);
	OutputDebugStringA(szMsg);
	*/
	EnterCriticalSection(&m_CriticalSection);
	m_bDSBufferIsFinished[thread_index] = true;
	FillBuffer(thread_index);
	LeaveCriticalSection(&m_CriticalSection);

	m_ulLastTime = dwCurTime;


	/*
	for (int i = 0; i < EVENT_MUM; i++)
	{
		if (WAIT_OBJECT_0 + i == hr)
		{
			int iIndex = (i + 1) % EVENT_MUM;
			m_bDSBufferIsFinished[iIndex] = true;
			FillBuffer(iIndex);
			break;
		}
	}
	*/
#endif
	
}


bool DSound::IsFinished()
{
	return m_bFinished;
}

int vol_to_db(int vol)
{
	if (vol)
		return (int)(20.0 * 100.0 * log10((double)vol / 127.0));
	return -10000;
}

void DSound::SetVolume(long lValue)
{
#if 0
	int nVolume;
    if (lValue <= 0)   
        nVolume = DSBVOLUME_MIN;   
    else if (lValue>=100)   
        nVolume = DSBVOLUME_MAX;   
    else   
        nVolume = DSBVOLUME_MIN + (DSBVOLUME_MAX - DSBVOLUME_MIN) * lValue / 100; 
		//nVolume = DSBVOLUME_MIN + (DSBVOLUME_MAX - DSBVOLUME_MIN) / lValue;

	int iMax = DSBVOLUME_MAX;
	int iMin = DSBVOLUME_MIN;

	int iErrorCode = 0;

	if (!m_lpDSB)
		return;

	//HRESULT hr = m_lpDSB->SetVolume(nVolume);
	HRESULT hr = m_lpDSB->SetVolume(vol_to_db(lValue));
	switch(hr)
	{
		case DS_OK:
			iErrorCode = 0;
			break;
		case DSERR_CONTROLUNAVAIL:
			iErrorCode = -1;
			break;
		case DSERR_GENERIC:
			iErrorCode = -2;
			break;
		case DSERR_INVALIDPARAM:
			iErrorCode = -3;
			break;
		case DSERR_PRIOLEVELNEEDED:
			iErrorCode = -4;
			break;
	}
#else
	m_lVolume = lValue * 1.28;
#endif
}

long DSound::GetVolume()
{
#if 0
	int nVolume;
	/*
	if (lValue <= 0)
		nVolume = DSBVOLUME_MIN;
	else if (lValue >= 100)
		nVolume = DSBVOLUME_MAX;
	else
		nVolume = DSBVOLUME_MIN + (DSBVOLUME_MAX - DSBVOLUME_MIN) * lValue / 100;
	//nVolume = DSBVOLUME_MIN + (DSBVOLUME_MAX - DSBVOLUME_MIN) / lValue;
	*/

	int iMax = DSBVOLUME_MAX;
	int iMin = DSBVOLUME_MIN;

	int iErrorCode = 0;

	if (!m_lpDSB)
		return 0;

	long lValue;

	HRESULT hr = m_lpDSB->GetVolume(&lValue);

	float fNum = (DSBVOLUME_MAX - DSBVOLUME_MIN) / 100.0;

	m_lTmpVolume = (lValue - DSBVOLUME_MIN) / fNum;

	switch (hr)
	{
	case DS_OK:
		iErrorCode = 0;
		break;
	case DSERR_CONTROLUNAVAIL:
		iErrorCode = -1;
		break;
	case DSERR_GENERIC:
		iErrorCode = -2;
		break;
	case DSERR_INVALIDPARAM:
		iErrorCode = -3;
		break;
	case DSERR_PRIOLEVELNEEDED:
		iErrorCode = -4;
		break;
	}
	return m_lTmpVolume;
#else
	return m_lVolume / 1.28;
#endif
}

/*
void* DSound::DoEvent(int iEvent,void* pParameter1,void* pParameter2)
{
	switch(iEvent)
	{
		case CE_IS_ALIVE:
			return (void*)!m_bFinished;
			break;
		case CE_TIMERCALLBACK:
			TimerCallback();
			break;
	}
	return 0;
}
*/
unsigned int __stdcall _PlaybackProc(void* lpvThreadParm)
{
#if 1
	DSound* pObj = (DSound *)lpvThreadParm;
	
	while(!pObj->IsFinished())
	{
		pObj->TimerCallback();
		Sleep(1);
	}
	OutputDebugString("DSound Exit _PlaybackProc()..\n");
	_endthread();
	return 0;
#else
	CallbackObj* pObj = (CallbackObj *)lpvThreadParm;
	
	while(pObj->GetEventLoop() == 1)
	{
		//pObj->TimerCallback();
		pObj->DoEvent(CE_TIMERCALLBACK,0,0);
		Sleep(1);
	}
	TRACE("Exit _PlaybackProc()..\n");
	return 0;
#endif
}
