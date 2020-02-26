#pragma once

#ifndef _FilePlayerObj_H_
#define _FilePlayerObj_H_

#include "MFilePlayerDll.h"
#include "D3DRender.h"

#include "..\\..\\Include\\RenderCommon.h"

enum DisplayMode
{
	DM_NORMAL = 0,
	DM_TVWALL,
};

class FilePlayerObj : public FilePlayerCallback
{
	D3DRender* m_pD3DRender;
	MFilePlayerDll* m_pMFilePlayerDll;

	bool m_bEnableUpdatePos;

	int m_iDisplayMode;
	int m_iLayoutID;
	int m_iLayoutIndex;
protected:
public:
	FilePlayerObj();
	~FilePlayerObj();

	void Init(HWND hWnd);

	void SetDisplayMode(int iMode);
	void SetLayoutInfo(int iID, int iIndex);

	void Open(wchar_t* wzFileName);
	void Close();
	void Play();
	void Stop();
	void Pause();
	void Seek(int iPos);
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

	void UpdateGlobalTimeCode(double dTimeCode);

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
