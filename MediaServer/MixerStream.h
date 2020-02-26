#pragma once
#include "MixerPlayerDll.h"
#include "RenderCommon.h"
#include "D3DRender.h"

class CMixerStream : public FilePlayerCallback
{
	D3DRender* m_pD3DRender;
	MixerPlayerDll* m_pMFilePlayerDll;
	bool m_bEnableUpdatePos;
	int m_iIndex;

protected:

public:
	CMixerStream();
	~CMixerStream();

	void SetMFilePlayerDll(MixerPlayerDll* pObj);
	void Init(HWND hWnd, int iIndex = 0);

	void Open(char* szFileName);
	void Close();
	void Play();
	void Stop();
	void Seek(int iPos);
	void EnableUpdatePos(bool bEnable);
	void EnableAudio(bool bEnable);
	void SpeedUp();
	void SpeedDown();
	int  GetVolume();
	void SetVolume(int iValue);
	void SetBrightness(int iValue);
	void SetContrast(int iValue);
	void SetSaturation(int iValue);

	//callback functions
	void UpdatePosInfo(char* szCurPos, char* szTotalLen);
	void SetSliderCtrlRange(int iPos);
	void SetSliderCtrlPos(int iPos);
	void DisplayVideo(unsigned char* pBuffer, int w, int h);
};
