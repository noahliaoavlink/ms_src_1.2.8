#pragma once

#ifndef _PBSchedule_H
#define _PBSchedule_H

#include "../../Include/SQList.h"
#include "MidiCtrlSender.h"
//#include "ThreadBase.h"

typedef struct
{
	int iHour;
	int iMinute;
	int iSecond;
	int iCmdID;
	int iParameter;
}PBItem;

class PBSchedule
{
	bool m_bEnable;

	SQList* m_pPlayList;
	MidiCtrlSender m_MidiCtrlSender;
	bool m_bDoCheckReciever;
public:
	PBSchedule();
	~PBSchedule();

	void LoadConfig();
	void LoadPlaylist();

	void CheckPlaylist();
};

#endif