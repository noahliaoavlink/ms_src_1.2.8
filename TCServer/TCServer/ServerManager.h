#ifndef _ServerManager_H_
#define _ServerManager_H_

#include "Server.h"
#include "NWin.h"
#include <list>
#include "UDPServer.h"
#include "../../Include/SQQueue.h"

/*
enum TCStatus
{
	TCS_STOP = 0,
	TCS_RUNNING,
	TCS_PAUSE,
};
*/

class ServerManager: public WinMsgCallback2
{	
		std::list<Server*> m_SocketList;

		int m_iErrorCode;
		
		CNWin* m_pServerWin;
		CNWin* m_pDispatchWin;

		SOCKET m_Socket;
		int m_iPort;

		//PlayerAgent* m_pPlayer;

		int m_iTCStatus;
		unsigned long m_ulCurTimeCode;
		unsigned long m_ulLastTimeCode;
		SQQueue* m_pTimeCodeQueue;
		unsigned long m_ulTotalOfTimeCode;
		DWORD m_dwLastTime;
		int m_iWaitCount;
		unsigned long m_ulCurFrameTime;

		long m_lTimeCodeCount;

		UDPServer* m_pUDPServer;

		HANDLE m_hTCThread;
		bool m_bIsAlive;
		CRITICAL_SECTION m_CriticalSection;
   protected:
   public:
		ServerManager();
        ~ServerManager();

		bool Startup();
		void Cleanup();

		bool Accept();

		bool Init();
		int Open(int iPort);
		void Close();

		void DeleteObj(unsigned long ulID);
		void CloseClient(unsigned long ulID);
		void ReceiveData(unsigned long ulID);

		void SetSocket(void* pSocket,int iPort);

		void UpdateTimeCode(unsigned long ulTimeCode);
		void SetTotalOfTimeCode(unsigned long ulTimeCode);
		void UpdateCurFrameTime(unsigned long ulTime);

		void TC_Start();
		void TC_Stop();
		void TC_Pause();
		bool IsAlive();
		void TimeCodeLoop();
		int GetTCStatus();
		unsigned long GetTotalOfTimeCode();
		bool IsWorking();
		void RemoveAll();
		int GetTotalOfClients();
		void TC_SwitchTo(long lIndex);

		void* WinMsg(HWND hWnd,int iMsg,void* pParameter1,void* pParameter2);
};

#endif
