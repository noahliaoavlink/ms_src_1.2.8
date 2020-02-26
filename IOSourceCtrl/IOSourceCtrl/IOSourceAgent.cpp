#include "stdafx.h"
#include "IOSourceAgent.h"

IOSourceAgent::IOSourceAgent()
{
	m_pMediaFileManager = new MediaFileManager;
#ifdef _DISABLE_TASK_MANAGER
#else
	m_pTaskManager = new TaskManager;
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
	m_pVWallPlayer = new VWallPlayer;
	m_pVWallPlayer->Init("IOSourceAgent");
#endif
}

IOSourceAgent::~IOSourceAgent()
{
#ifdef _DISABLE_WALL_PLAYER
#else
	if (m_pVWallPlayer)
		delete m_pVWallPlayer;
#endif

#ifdef _DISABLE_TASK_MANAGER
#else
	if (m_pTaskManager)
		delete m_pTaskManager;
#endif

	if (m_pMediaFileManager)
		delete m_pMediaFileManager;
}

void IOSourceAgent::Init()
{
	m_pMediaFileManager->Init();
#ifdef _DISABLE_TASK_MANAGER
#else
	m_pTaskManager->Init(m_pMediaFileManager);
#endif
}

MediaFileManager* IOSourceAgent::GetMediaFileManager()
{
	return m_pMediaFileManager;
}

#ifdef _DISABLE_TASK_MANAGER
#else
TaskManager* IOSourceAgent::GetTaskManager()
{
	return m_pTaskManager;
}
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
VWallPlayer* IOSourceAgent::GetVWallPlayer()
{
#ifdef _DISABLE_WALL_PLAYER
	return 0;
#else
	return m_pVWallPlayer;
#endif
}
#endif
