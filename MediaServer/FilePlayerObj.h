#pragma once

#ifndef _FilePlayerObj_H_
#define _FilePlayerObj_H_

#include "MFilePlayerDll.h"
#include "D3DRender.h"
#include "YUVConverter.h"

#include "../../../Include/RenderCommon.h"
#include "../../../Include/MediaFileCommon.h"
#include "../../../Include/EffectParam.h"

#ifdef _ENABLE_GPU
#include "ProcessorHD.h"
#endif

enum SkinMode
{
	SM_1080P = 0,
	SM_1366,
	SM_1280,
};

class FilePlayerObj : public FilePlayerCallback
{
	D3DRender* m_pD3DRender;
	MFilePlayerDll* m_pMFilePlayerDll;
	CMixerEffect* m_pMixerEffect;
	unsigned char* m_pRGBBuffer;
	YUVConverter m_YUVConverter;
	sEffectParam m_effect_param;

	bool m_bEnableUpdatePos;

	EventCallback* m_pEventCallback;

#ifdef _ENABLE_GPU
	ProcessorHD* m_pProcessorHD;
#endif

	int m_iFrameCount;

	Average m_AvgEffect;
protected:
public:
	FilePlayerObj();
	~FilePlayerObj();

	void Init(HWND hWnd);

	void SetEventCallback(void* pObj);

	//void SetDisplayMode(int iMode);
	//void SetLayoutInfo(int iID, int iIndex);

	void ResetD3D();

	void Open(wchar_t* wzFileName);
	void Close();
	void Play();
	void Stop();
	void Pause();
	void Seek(double dPos);
	void EnableUpdatePos(bool bEnable);
	void SpeedUp();
	void SpeedDown();
	int GetVoluem();
	void SetVolume(int iValue);
	int GetSystemVoluem();
	void SetSystemVolume(int iValue);
	void SetBrightness(int iValue);
	void SetContrast(int iValue);
	void SetSaturation(int iValue);
	void SetHue(int iValue);
	void SetR(int iValue);
	void SetG(int iValue);
	void SetB(int iValue);
	void PlayBackward();
	int GetPlaybackMode();

	void SetBGColor_R(int iValue);
	void SetBGColor_G(int iValue);
	void SetBGColor_B(int iValue);
	void SetPlanarAlpha(int iValue);

	void UpdateGlobalTimeCode(double dTimeCode);

	int CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h);
	void SetEffectIndex(int iID);
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectLevel(int iLevel);
	void SetEffectParam1(int iParm);
	void SetEffectParam2(int iParm);
	void SetEffectParam3(int iParm);
	void SetEffectParam4(int iParm);
	void SetEffectParam5(int iParm);


	//callback functions
	void UpdatePosInfo(char* szCurPos, char* szTotalLen);
	void SetSliderCtrlRange(double dPos);
	void SetSliderCtrlPos(double dPos);
	void DisplayVideo(unsigned char* pBuffer, int w, int h);
	virtual void PlayAudio(unsigned char* pBuffer, int iLen);
	virtual void Event(int iType);
	void DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h);
	
};

#endif
