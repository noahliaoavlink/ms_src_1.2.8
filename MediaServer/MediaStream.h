#pragma once
#include "MFilePlayerDll.h"
#include "RenderCommon.h"
#include "D3DRender.h"
#include "YUVConverter.h"
#include "RTSPPlayerDll.h"
#include "..\\..\\..\\Include\\MathCommon.h"

#define _ENABLE_DISPLAY_MANAGER 1

#ifdef _ENABLE_DISPLAY_MANAGER
//#include "DisplayManager.h"
#include "ShapeDispManager.h"
#endif

#define _MAX_DISPLAY_INDEX 16

#ifdef _ENABLE_GPU
enum SourceMode
{
	SM_TIMELINE = 0,
	SM_SHAPE,
};

#define _ENABLE_SOURCE_MODE 1

#endif

class CMediaStream : public FilePlayerCallback
{
//	friend class CMixerEffect;
public:
	CMediaStream(HWND hWnd);
	~CMediaStream();
	double GetCurPos() { return m_dCurPos; }
	double GetTotalPos() { return m_dTotalPos; };
	INT32 GetVideoWidth() { return m_iWidth; };
	INT32 GetVideoHeight() { return m_iHeight; };

	void Init(HWND hWnd);
	void EnableGPU(bool bEnable, void* pD3DDevice);
	void Open(char* szFileName, BOOL bFromShape = FALSE);
	void OpenFromFile(char* szFileName, BOOL bFromShape = FALSE);
	void OpenFromRTSP(char* szUrl, BOOL bFromShape = FALSE);
	void Close(BOOL bCloseFromShape = FALSE);
	void Play();
	void Stop();
	void Stop2();
	void Pause();
	void Seek(int iPos);
	void EnableUpdatePos(bool bEnable);
	void EnableAudio(bool bEnable);
	void SpeedUp();
	void SpeedDown();
	int  GetVolume();
	int  GetSystemVolume();
	void SetVolume(int iValue);
	void SetSystemVolume(int iValue);
	void SetBrightness(int iValue);
	void SetContrast(int iValue);
	void SetSaturation(int iValue);
	void SetHue(int iValue);
	void SetR(int iValue);
	void SetG(int iValue);
	void SetB(int iValue);
	void SetGray(int iValue);
	void SetNegative(int iValue);
	void PlayBackward();
	int GetPlaybackMode();
	ULONG AddRef();
	ULONG Release();
	void SetReferenceTimeCode(double dNewRefTime);
	void UpdateGlobalTimeCode(double dTimeCode);
	void SetStartTime(double dTime);
	void EnableReSampleRate(bool bEnable);
	bool GPUIsEnabled();
	bool IsAudioFile();

	BOOL IsD3DUsing(INT32 iPannel)
	{ 
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iPannel)
			{
				return m_pDisplayManagerAry.GetAt(x)->IsD3DUsing();
			}
		}
		return 0; 
	};
	void UsingD3D(INT32 iPannel, int iUsingD3D) 
	{
		for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
		{
			if (m_pDisplayManagerAry.GetAt(x)->GetPannelIdx() == iPannel)
			{
				return m_pDisplayManagerAry.GetAt(x)->UsingD3D(iUsingD3D);
			}
		}
	};
	int GetIndex() { return m_iIndex; };
	void SetSrcArea(CRect rectSrc);
	void SetDestArea(CRect rectDest);
	CString FilePath() { return m_strFilePath; }
	CString FileName() { return m_strFileName; }
	CString& TotalLen() { return m_sTotalLen; }
	CString& CurPos() { return m_sCurPos; }
	BOOL IsExtraVideo() { return m_bExtraVideo; }
	void ExtraVideo(bool bExtraVideo) { m_bExtraVideo = bExtraVideo; }

	void SetVideoRawDataCallback(void* pObj);
#ifdef _ENABLE_GPU
	void SetSourceMode(int iMode);
	void EnableGPU2(bool bEnable, int iIndex, void* pD3DDevice);
#endif

	CMixerEffect* GetMixerEffect();
	void SetEffectIndex(int iID);
	int GetEffectIndex();
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectParam(int iIndex, int iValue);
	INT32 GetEffectParam(int iIndex);
#ifdef _ENABLE_MEDIA_STREAM_EX
	
	void InitRender(HWND hWnd, void* pVoid = NULL);
	D3DRender* GetD3DRender() { return m_pD3DRender; };
	BOOL HasRender() { return (m_pD3DRender != NULL); }

	void SetMixerEffect(CMixerEffect* pObj);
#endif
	void SetFadeDuration(float fValue);
	void EnableFadeIn(bool bEnable);
	void EnableFadeOut(bool bEnable);
	bool IsOpened();
	void SetScale(float fW, float fH);
	void ResetFilter();
	BYTE* GetVideoBuffer(BOOL bReleaseBusy = true);
	int CreateMixEffect(IDirect3DDevice9* pD3DDev);
	int CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h);
	int CreateDispMixer(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h);
	int CreateDispMixer(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h, INT32 iPannelIdx);
	void ReleaseBusy() 
	{ 
		if (m_bBusyFlag == 1) 
			m_bBusyFlag = 0; 
	};
	INT32 GetPannelIndex() { return m_iPannelIdx; };

#ifdef _ENABLE_DISPLAY_MANAGER
	BOOL SetDisplayManager(CShapeDispManager* pObj, INT32 iPannelIdx);
	void ReleaseDispManagerAry();
	IDirect3DSurface9* GetSurface(int iPannelIdx);
	int GetTotalOfShapeDisplayManagers();
	CMixerEffect* GetShapeMixerEffect(int iIndex);
#endif

	bool IsFFFilter();
	float GetFPS();
	int GetAudioInfo(int& iChannels, int& iSampleRate, int& iBits);
	void Clear();
	void DrawPannel(int iPannelIndex, unsigned char* pBuffer, int w, int h, bool bDoEffect);
	void DrawPannel_HW(int iPannelIndex, unsigned char* pBuffer, int w, int h, bool bDoEffect);
	void SetMarkMode(int iMode);
	void DisplayPanelVideo(bool bDoPresent = true);
	void Lock();
	void Unlock();
	bool CheckFrameCount();
	void WaitForFrameSyn();
	void SetDisplayOrder(int iIndex, int iValue);
	void EnablePlayerWaitForDisplay(bool bEnable);
	void SetAudioDelay(int iValue);
	void SetReSampleRateOffset(float fValue);
	void UpdateProcessFPS();
//	void CalPresentFPS();

	bool CheckDisplayManagerStatus(int iStatus);
	int GetSurfaceIndex(int iPanelIdx);
	void SetSurfaceItemIndex(int iIndex);

protected:

	INT32 m_iEffectIdx;
	INT32 m_bBusyFlag;
	CRITICAL_SECTION m_csInUse;
	BOOL m_bFromShape;

	CMixerEffect* m_pMixerEffect;
	CArray<CMixerEffect*, CMixerEffect*> m_pMixerEffectAry;
#ifdef _ENABLE_MEDIA_STREAM_EX
	D3DRender* m_pD3DRender;
#endif

#ifdef _ENABLE_DISPLAY_MANAGER
	CArray<CShapeDispManager*, CShapeDispManager*> m_pDisplayManagerAry;
	CArray<int, int> m_iSurfaceMappingAry;

	int m_iDisplayMappingIndex[_MAX_DISPLAY_INDEX];
#endif

	VideoRawDataCallback* m_pVideoRawDataCallback;

	unsigned char* m_pBuffer;
	unsigned char* m_pYUVBuffer;
	MFilePlayerDll* m_pMFilePlayerDll;
	//LONGLONG m_lCurPos;
	//LONGLONG m_lTotalPos;
	double m_dCurPos;
	double m_dTotalPos;
	INT32 m_iWidth;
	INT32 m_iHeight;
	INT32 m_iPannelIdx;
	bool m_bEnableUpdatePos;
	int m_iIndex;
	CString m_strFilePath;
	CString m_strFileName;
	CString	m_sTotalLen;
	CString	m_sCurPos;

	int m_iScreenWidth;
	int m_iScreenHeight;
	BOOL m_bExtraVideo;

//	int m_iSurfaceItemIndex;
	YUVConverter m_YUVConverter;

	RTSPPlayerDll* m_pRTSPPlayerDll;
	void* m_pRTSPObj;
	char m_szFileName[512];
	bool m_bDoClear;

	int m_iFrameCount;
	bool m_bLocked;
	bool m_bLoseVideo;
	bool m_bPlayerWaitForDisplay;

	int m_iDisplayOrder[4];
	float m_fDropFrameRate;
	int m_iDropFrameCount;

	int m_iSurfaceItemIndex;

	/*
	int m_iPresentCount;
	DWORD m_dwPresentFPSLastTime;
	float m_fPresentFPS;
	Average m_AvgPresentFPS;
	*/

	int m_iMixerEffectPannelIdx[_MAX_DISPLAY_INDEX];
#ifdef _ENABLE_GPU
	int m_iSourceMode;
#endif

	void DisplayVideoWithEffect(unsigned char* pBuffer, int w, int h);
	void DisplayVideoWithoutEffect(unsigned char* pBuffer, int w, int h);
	void DisplayVideo_HW_WithEffect(void* pD3DDevice, unsigned char* pBuffer, int w, int h);
	void DisplayVideo_HW_WithoutEffect(void* pD3DDevice, unsigned char* pBuffer, int w, int h);

		//callback functions
	void UpdatePosInfo(char* szCurPos, char* szTotalLen);
	void SetSliderCtrlRange(double dPos);
	void SetSliderCtrlPos(double dPos);
	void DisplayVideo(unsigned char* pBuffer, int w, int h);
	void PlayAudio(unsigned char* pBuffer, int iLen);
	void Event(int iType);
	void DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h);

private:
	long m_refCount;
};

