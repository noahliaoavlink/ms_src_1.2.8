#ifndef _Server_H_
#define _Server_H_

#include <winsock2.h>

#include "..\..\Include\NetCommon.h"
#include "..\..\Include\QBufferManagerEx.h"
#include "PacketParser.h"
#include "PacketMaker.h"

#define _ENABLE_MSG_Q 1

#ifdef _ENABLE_MSG_Q

#include "..\..\Include\SQQueue.h"

typedef struct
{
	long lCmd;
	long lParm;
}MsgItem;
#endif

class Server
{	
		SOCKET m_hSocket;
		char* m_pRcvBuffer;
		char* m_pSendBuffer;
		char* m_pBuffer;

		//IPCCmdParser* m_pCmdParser;

		PacketParser* m_pPacketParser;

		bool m_bIsPassed;

		bool m_bDoSynPB;
		unsigned long m_ulCurFrameTime;
		unsigned long m_ulLastFrameTime;

#ifdef _ENABLE_MSG_Q
		SQQueue* m_pMsgQueue;
#endif

		QBufferManagerEx* m_pQBuffer;
		HANDLE m_hRecvThread;
		bool m_bIsAlive;
		CRITICAL_SECTION m_CriticalSection;

#ifdef _ENABLE_MSG_Q
		HANDLE m_hSendMsgThread;
		CRITICAL_SECTION m_MsgQCriticalSection;
#endif

   protected:
   public:
		Server();
        ~Server();	

		void Init();
		bool IsAlive();

		void SetSocket(SOCKET hSocket);
		SOCKET GetSocket();
		void EnableDoSynPB(bool bEnable);
		void UpdateCurFrameTime(unsigned long ulTime);

		void Close();
		void Receive();

		void Hello();
		void Login(char* szID, char* szPW);
		void TC_Start();
		void TC_Stop();
		void TC_Pause();
		void TC_Continue();
		void TC_UpdateTimeCode(long lTimeCode);

		void ProcessRecvData();
#ifdef _ENABLE_MSG_Q
		void SendPacket();
#endif

		void Test();
};

#endif
