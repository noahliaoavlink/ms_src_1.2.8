#include "stdafx.h"
#include "UDPServer.h"
#include <stdio.h>
#include <process.h>

#define BROADCAST_PORT 1013
#define BROADCAST_MSG "BRUVIS_UDP_MSG_V1"
#define BROADCAST_INTERVAL 200

#pragma comment(lib, "wsock32.lib")

unsigned int __stdcall UDPServerThreadProc(void* lpvThreadParm);

UDPServer::UDPServer()
{
	m_ulLastTime = 0;

	strcpy(m_Server_Info.szServerVersion,"1.0");
	strcpy(m_Server_Info.szProtocolVersion,"1.0");
	strcpy(m_Server_Info.szIP,"127.0.0.1");
	m_Server_Info.iPort = 8080;
	InitializeCriticalSection(&m_CriticalSection);
}

UDPServer::~UDPServer()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void UDPServer::SetInfo(char* szServerVersion,char* szProtocolVersion,char* szIP,int iPort)
{
	strcpy(m_Server_Info.szServerVersion,szServerVersion);
	strcpy(m_Server_Info.szProtocolVersion,szProtocolVersion);
	strcpy(m_Server_Info.szIP,szIP);
	m_Server_Info.iPort = iPort;
}

bool UDPServer::Startup()
{
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested,&wsaData) != 0)
	{
       m_iErrorCode = WSAGetLastError();
       return false;
	}

	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{
       m_iErrorCode = 20000;
       return false;
	}
	return true;
}

void UDPServer::Cleanup()
{
	WSACleanup();
}

void UDPServer::Open()
{
	m_bIsAlive = true;
	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, UDPServerThreadProc, this, 0, &threadID );
}

void UDPServer::Close()
{
	m_bIsAlive = false;
	if(m_hThread)
	{
		::WaitForSingleObject(m_hThread,1000);
		m_hThread = 0;
	}
}

int UDPServer::SendToClient()
{
	EnterCriticalSection(&m_CriticalSection);
	SOCKET s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == -1)
    {
		OutputDebugStringA("Error in creating socket. \n");
		LeaveCriticalSection(&m_CriticalSection);
        return 0;
    }
    char opt = 1; 
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(char));
    SOCKADDR_IN brdcastaddr;
    memset(&brdcastaddr,0, sizeof(brdcastaddr));
    brdcastaddr.sin_family = AF_INET;
    brdcastaddr.sin_port = htons(BROADCAST_PORT);
    brdcastaddr.sin_addr.s_addr = INADDR_BROADCAST;
    int len = sizeof(brdcastaddr);
#if 0
    char sbuf[2048];
    sprintf(sbuf, "%s\r\n", BROADCAST_MSG);
    int ret = sendto(s, sbuf, strlen(sbuf), 0, (sockaddr*)&brdcastaddr, len);
#else
	sprintf(m_szSendBuf, "%s %s %s %s %d\r\n", BROADCAST_MSG,
		m_Server_Info.szServerVersion,
		m_Server_Info.szProtocolVersion,
		m_Server_Info.szIP,m_Server_Info.iPort);
    int ret = sendto(s, m_szSendBuf, strlen(m_szSendBuf), 0, (sockaddr*)&brdcastaddr, len);
#endif
    if(ret < 0)
    {
		OutputDebugStringA("Error broadcasting to the clients. \n");
    }
#if 0
    else if(ret < strlen(sbuf))
#else
	else if(ret < strlen(m_szSendBuf))
#endif
    {
		OutputDebugStringA("Not all data broadcasted to the clients. \n");
    }
    else
    {
		//OutputDebugStringA("Broadcasting is done. \n");
    }
    ::closesocket(s);
	LeaveCriticalSection(&m_CriticalSection);
}

bool UDPServer::IsAlive()
{
	return m_bIsAlive;
}

void UDPServer::TimeEvent()
{
	unsigned long ulCurTime = GetTickCount();
	if(ulCurTime >= m_ulLastTime + BROADCAST_INTERVAL)
	{
		m_ulLastTime = ulCurTime;
		SendToClient();
	}
}

unsigned int __stdcall UDPServerThreadProc(void* lpvThreadParm)
{

	UDPServer* pNetObj = (UDPServer*)lpvThreadParm;
    while(pNetObj->IsAlive())
	{
		pNetObj->TimeEvent();
		Sleep(1);
	}

	return 0;
}