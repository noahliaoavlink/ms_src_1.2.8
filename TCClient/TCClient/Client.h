#pragma once

#ifndef _Client_H_
#define _Client_H_


#include <Winsock.h>
#include "..\..\Include\QBufferManagerEx.h"
#include "PacketParser.h"
#include "PacketMaker.h"
#include "..\..\Include\NetCommon.h"
#include "..\..\Include\MediaFileCommon.h"
#include "UDPClient.h"
#include "SimpleIMCP.h"
#include "IPDetector.h"
#include "..\..\Include\LogCommon.h"

#define _ENABLE_PACKET_LIST 1

#ifdef _ENABLE_PACKET_LIST
//#include "..\..\Include\SQList.h"
#include "..\..\Include\SQQueue.h"
#endif

#define _WINSOCKET 1   //Winsock

#ifdef _WINSOCKET
#include "NWin.h"
#endif

#ifdef _WINSOCKET
class Client : public WinMsgCallback2
#else
class Client
#endif
{
	SOCKET m_hSocket;
	int m_iErrorCode;
	int m_iPort;
	bool m_bConnected;
	bool m_bConnectRet;

	char m_szID[64];
	char m_szPW[64];

	char* m_pRcvBuffer;
	char* m_pBuffer;
	QBufferManagerEx* m_pQBuffer;
	PacketParser* m_pPacketParser;
	EventCallback* m_pEventCallback;

#ifdef _ENABLE_PACKET_LIST
	//SQList* m_pPacketList;
	SQQueue* m_pPacketQueue;
#endif

#ifdef _WINSOCKET
	CNWin* m_pClientWin;
#endif

	UDPClient* m_pUDPClient;

	bool m_bIsPassed;
	bool m_bEnableReconnect;
	bool m_bDoReconnect;
	int m_iReconnectStatus;
	int m_iSelectTimeOutCount;
	char m_szIP[64];

	int m_iReceiveStatus;
	DWORD m_dwRcvLastTime;
	DWORD m_dwIoctlLastTime;
	DWORD m_dwLoginStartTime;
	int m_iSkipCount;

	//SimpleIMCP m_SimpleIMCP;
	IPDetector m_IPDetector;

	bool m_bIsAlive;
	HANDLE m_hRcvThread;
	HANDLE m_hProcessThread;
	CRITICAL_SECTION m_CriticalSection;
	CRITICAL_SECTION m_RcvCriticalSection;
	
protected:
public:
	Client();
	~Client();

	
	bool Startup();
	void Cleanup();

	void SetEventCallback(EventCallback* pObj);
	
	bool Connect(char* szIP, int iPort);
	void Close();
	void DoLogin();
	void Login(char* szID, char* szPW);
	//void SetSocket(void* pSocket, int iPort);
	void ReportTimeCode(unsigned long ulTimeCode);

	void Receive();
	bool IsAlive();
	//void SetStatus(bool bEnable);
	void ProcessRecvData();
	void Checkioctl();

	void SearchServer();
	void* GetServerList();
	bool IsConnected();
	void CheckReceiveStatus();
	void ResetRcvStatus();
	int GetReceiveStatus();
	void DoReconnect();
	void ReceiveLoop();

	void Add2LogFile(int iLevel, char* szMsg);

#ifdef _WINSOCKET
	void* WinMsg(HWND hWnd, int iMsg, void* pParameter1, void* pParameter2);
#endif
};

#endif
