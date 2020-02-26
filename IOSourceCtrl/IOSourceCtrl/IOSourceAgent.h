#pragma once

#ifndef _IOSourceAgent_H_
#define _IOSourceAgent_H_

#define _DISABLE_WALL_PLAYER 1
#define _DISABLE_TASK_MANAGER 1

#include "MediaFileManager.h"

#ifdef _DISABLE_TASK_MANAGER
#else
#include "TaskManager.h"
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
#include "VWallPlayer.h"
#endif

class IOSourceAgent
{
	MediaFileManager* m_pMediaFileManager;

#ifdef _DISABLE_TASK_MANAGER
#else
	TaskManager* m_pTaskManager;
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
	VWallPlayer* m_pVWallPlayer;
#endif
public:
	IOSourceAgent();
	~IOSourceAgent();

	void Init();
	MediaFileManager* GetMediaFileManager();
#ifdef _DISABLE_TASK_MANAGER
#else
	TaskManager* GetTaskManager();
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
	VWallPlayer* GetVWallPlayer();
#endif
};

#endif