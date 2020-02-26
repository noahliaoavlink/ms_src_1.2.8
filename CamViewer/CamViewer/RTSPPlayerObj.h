#pragma once

#ifndef _RTSPPlayerObj_H
#define _RTSPPlayerObj_H

#include "RTSPPlayerDll.h"
#include "../../Include/OutVideoDll.h"

class RTSPPlayerObj : public FilePlayerCallback
{
	RTSPPlayerDll* m_pRTSPPlayerDll;
	void* m_pRTSPObj;

	OutVideoDll* m_pOutVideoDll;
	HWND m_hDisplayWnd;
public:
	RTSPPlayerObj();
	~RTSPPlayerObj();

	void Init(HWND hWnd);
	void Open(char* szUrl);
	void Close();

	int CheckNetworkStatus(char* szUrl);

	//callback functions
	void UpdatePosInfo(char* szCurPos, char* szTotalLen);
	void SetSliderCtrlRange(double dPos);
	void SetSliderCtrlPos(double dPos);
	void DisplayVideo(unsigned char* pBuffer, int w, int h);
	virtual void PlayAudio(unsigned char* pBuffer, int iLen);
	virtual void Event(int iType);
	virtual void DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h);
};

#endif