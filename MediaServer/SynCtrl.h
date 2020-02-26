#pragma once
#ifndef _SynCtrl_H_
#define _SynCtrl_H_

#include "ThreadBase.h"
#include "..\\..\\..\\Include\\MathCommon.h"

#define _MAX_ORDER_ITEMS 16

//#define _MT_MODE 1

#ifdef _MT_MODE
#define _MAX_PANELS 4

typedef struct
{
	void* pObj;
	int iIndex;
}ThreadItem;
#endif

typedef struct
{
	int iFrameNum;
	int iDisplayOrder[4];
}OrderItem;

class SynCtrl : public ThreadBase
{
	DWORD m_dwLastTime;
	float m_fFPS;
	int m_iInterval;
	int m_iDelayTime;
	HANDLE m_hEvent;
	bool m_bFinished;

	int m_iDisplayOrder[4];
	int m_iTotalOfOrderItems;
	OrderItem m_OrderItems[_MAX_ORDER_ITEMS];
	int m_iCurOrderIndex;

	bool m_bPlayerWaitForDisplay;
	int m_iAudioDelay;
	float m_fReSampleRateOffset;

	Average m_AvgPresentSpendTime;
	Average m_AvgPresentFPS;

	//Present fps
	float m_fPresentFPS;
	int m_iPresentCount;
	DWORD m_dwPresentFPSLastTime;

#ifdef _MT_MODE
	HANDLE m_hEvent_MT[_MAX_PANELS];
	HANDLE m_hThread[_MAX_PANELS];

	ThreadItem m_ThreadItems[_MAX_PANELS];
	CRITICAL_SECTION m_CriticalSection;
#endif

public:
	SynCtrl();
	~SynCtrl();

	void Start();
	void Stop();

	void DisplayPanelVideo();

#ifdef _MT_MODE
	void UpdateVideo();
	void ShowVideo(int iIndex);
#endif

	void LoadDisplayOrder();
	void CalPresentFPS();
	int CheckOrderTable(int iFrameCount);
	void UpdateDisplayOrder();

	virtual void ThreadEvent();
};

#endif
