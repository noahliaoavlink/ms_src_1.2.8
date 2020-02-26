#include "stdafx.h"
#include "ThreadBase.h"

DWORD WINAPI ThreadLoop(LPVOID lpParam);

ThreadBase::ThreadBase()
{
	m_bIsAlive = false;
	m_hWorkerThread = 0;
	InitializeCriticalSection(&m_CriticalSection);
}

ThreadBase::~ThreadBase()
{
	if(m_hWorkerThread)
	{
		Stop();
		m_hWorkerThread = 0;
	}
	DeleteCriticalSection(&m_CriticalSection);
}
/*
void ThreadBase::Start(LPTHREAD_START_ROUTINE pFunctionPtr,void* lpParameter)
{
	m_bIsAlive = true;
	DWORD nThreadID;
	//m_hWorkerThread = CreateThread(0, 0, pFunctionPtr , (void *)lpParameter, 0, &nThreadID);
	m_hWorkerThread = CreateThread(0, 0, ThreadLoop , (void *)this, 0, &nThreadID);
}
*/
void ThreadBase::Start()
{
	m_bIsAlive = true;
	DWORD nThreadID;
	//m_hWorkerThread = CreateThread(0, 0, pFunctionPtr , (void *)lpParameter, 0, &nThreadID);
	m_hWorkerThread = CreateThread(0, 0, ThreadLoop , (void *)this, 0, &nThreadID);
}

void ThreadBase::Stop()
{
	if(m_hWorkerThread)
	{
		m_bIsAlive = false;
		::WaitForSingleObject(m_hWorkerThread,30000);
		m_hWorkerThread = 0;
	}
}

bool ThreadBase::IsAlive()
{
	return m_bIsAlive;
}

void ThreadBase::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void ThreadBase::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}

DWORD WINAPI ThreadLoop(LPVOID lpParam)
{    
	ThreadBase* pObj = (ThreadBase*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->ThreadEvent();
	}
	return 0;
}