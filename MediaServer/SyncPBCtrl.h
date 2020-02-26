#pragma once

#ifndef _SyncPBCtrl_H
#define _SyncPBCtrl_H

#include "ThreadBase.h"

#define _MAX_PANELS 5

enum PBCtrlStatus
{
	PBCS_IDLE = 0,
	PBCS_NORMAL = 1,
	PBCS_UPDATING,
	PBCS_UPDATED,
	PBCS_PRESENTING,
	PBCS_PRESENTED,
};

enum PanelSourceStatus
{
	PSS_IDLE = 0,
	PSS_NORMAL = 1,
	PSS_UPDATING,
	PSS_UPDATED,
	PSS_PRESENTING,
	PSS_PRESENTED,
};

typedef struct
{
//	int iIndex;
	int iSourceCount;
	double dPos[10];
	int iStatus;
}PanelPBInfo;

class SyncPBCtrl : public ThreadBase
{
	PanelPBInfo m_Panels[_MAX_PANELS];
	int m_iStatus;

	bool m_bIsWorking[_MAX_PANELS];
	int m_iJoinCount;
public:
	SyncPBCtrl();
	~SyncPBCtrl();

	void Init();
	void Start();
	void Stop();

	int GetStatus();
	void SetPanelStatus(int iPanelIndex, int iStatus);
	int GetPanelStatus(int iPanelIndex);
	void SetSourceCount(int iPanelIndex, int iCount);
	void SetPanelSourcePos(int iPanelIndex, int iSourceIndex, double dPos);

	void CheckSourceCount();
	void UpdateStatus();
	void Normal();
	void Updating();
	void Updated();
	void Presenting();
	void Reset();

	void ThreadEvent();
};

#endif
