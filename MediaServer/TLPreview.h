#pragma once

#ifndef _TLPreview_H_
#define _TLPreview_H_

#include "MFilePlayerDll.h"
#include "D3DRender.h"
#include "TL_Out_Device.h"

#include "MixerEffect.h"
//#include "MediaStream.h"
#include "MediaStreamEx.h"

#ifdef _ENABLE_DISPLAY_MANAGER
#include "DisplayManager.h"
#endif


class TLPreview : public VideoRawDataCallback, public OutDevice
//class TLPreview : public OutDevice
{
	//CMediaStreamEx* m_pMedisStream;
	CMediaStream* m_pMedisStream;
//	CMixerEffect m_MixerEffect;

#ifdef _ENABLE_DISPLAY_MANAGER
	DisplayManager* m_pDisplayManager;
	int m_iSurfaceItemIndex;
#else
	D3DRender* m_pD3DRender;
#endif

	bool m_bEnableUpdatePos;

	int m_iIndex;

	int m_iStreamAryIndex;

	float m_fScaleW;
	float m_fScaleH;

	sEffectParam m_effect_param;

	int m_iFrameCount;
	
protected:
public:
	
	TLPreview(HWND hWnd);
	~TLPreview();

	//void SetMFilePlayerDll(MFilePlayerDll* pObj);
	void SetDisplayManager(DisplayManager* pObj);
	void Init(HWND hWnd, int iIndex = 0);

	void Open(char* szFileName);
	void Close();
	void Play();
	void Stop();
	void Stop2();
	void Pause();
	void Seek(float fPos);
	void EnableUpdatePos(bool bEnable);
	void EnableAudio(bool bEnable);
	void SpeedUp();
	void SpeedDown();
	void SetVolume(int iValue);
//	int GetSystemVoluem();
//	void SetSystemVolume(int iValue);
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

	int GetVoluem();
	bool IsOpened();
	void ResetFilter();
	void SetStartTime(double dTime);
	void UpdateGlobalTimeCode(double dTimeCode);
	double GetCurPos();

	void SetEffectIndex(int iID);
	int GetEffectIndex();
	//void SetEffectParam(int iID, int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectLevel(int iLevel);
	void SetEffectParam1(int iParm);
	void SetEffectParam2(int iParm);
	void SetEffectParam3(int iParm);
	void SetEffectParam4(int iParm);
	void SetEffectParam5(int iParm);

	void LoadSourceScaleFile();
	void ResetMixerEffectD3DDevice();

	//fade in / out
	void SetFadeDuration(float fValue);
	void EnableFadeIn(bool bEnable);
	void EnableFadeOut(bool bEnable);

	//callback functions
	void DisplayVideoRawData(unsigned char* pBuffer, int w, int h);
	void DisplayVideoSurfaceData(void* pSurface, int w, int h);
	void DisplayNV12SurfaceData(void* pSurface, int w, int h);

};

#endif
