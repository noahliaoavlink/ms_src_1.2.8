#ifndef _ThreadBase_H
#define _ThreadBase_H

class ThreadBase
{
		HANDLE m_hWorkerThread;
		bool m_bIsAlive;
		CRITICAL_SECTION m_CriticalSection;

   public:
	    ThreadBase();
        ~ThreadBase();

		//void Start(LPTHREAD_START_ROUTINE pFunctionPtr,void* lpParameter);
		void Start();
		void Stop();
		bool IsAlive();
		void Lock();
		void Unlock();

		virtual void ThreadEvent() = 0;
};

#endif