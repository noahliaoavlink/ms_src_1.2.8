#include "stdafx.h"
#include "NetTester.h"
#include <process.h>
#include "../../../Include/RtspCommon.h"

#define WAIT_TIMEOUT_INTERVAL 100
unsigned int __stdcall ThreadProc(void* lpvThreadParm);

NetTester::NetTester()
{
//	Startup();
	m_bIsFinished = true;
}

NetTester::~NetTester()
{
//	Cleanup();
}

bool NetTester::Startup()
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

void NetTester::Cleanup()
{
    WSACleanup();
}

bool NetTester::Connect(char* szUrl)
{
	char szPrefix[10];
	char szSeparator[256];
	char szSuffix[80];
	char szUserName[64];
	char szPW[64];
	char szRtspUrl[512];
	int iPort = 0;
	bool bConnected;

	OutputDebugString("Connect 0 \n");

	m_bIsFinished = false;

	bool bRet = ParseUrl(szUrl,szPrefix,szSeparator,szSuffix,&iPort, szUserName, szPW);

	OutputDebugString("Connect 1 \n");

	if(bRet)
	{
		if(iPort == -1)
			bConnected = Connect(szSeparator,554);
		else
			bConnected = Connect(szSeparator,iPort);
		bConnected = WaitForConnection(10000);
	}
	m_bIsFinished = true;
	return bConnected;
}

bool NetTester::Connect(char* szIP,int iPort)
{
//	Startup();

	OutputDebugString("NetTester::Connect 0 \n");

	Close();

	if((m_Socket = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)
	{
        m_iErrorCode = WSAGetLastError();
//		WSACleanup();
        return false;
	}

	OutputDebugString("NetTester::Connect 1 \n");

	int size = 65535;
	setsockopt(m_Socket,SOL_SOCKET,SO_RCVBUF,(const char *)&size,sizeof(int));

	BOOL optval = FALSE;
	setsockopt(m_Socket, SOL_SOCKET, SO_OOBINLINE, (char *)&optval, sizeof(optval));

	struct sockaddr_in dst_addr;
	dst_addr.sin_family      = PF_INET;
	dst_addr.sin_addr.s_addr = getAddrFromIP((char*)szIP);//inet_addr(szIPAddr);
    dst_addr.sin_port        = htons(iPort);

	unsigned long ulArgp = 1;  //Non-blocking
    int iRet = ioctlsocket(m_Socket ,FIONBIO,&ulArgp);
	if(!iRet)
	{
		m_bConnectRet = false;
		if(connect(m_Socket, (struct sockaddr FAR *)&dst_addr, sizeof(dst_addr)) != 0)
		{
		   m_iErrorCode = WSAGetLastError();
		   if(m_iErrorCode != WSAEWOULDBLOCK)
		   {
			  Close();
			  return false;
		   }
		}
	}

	OutputDebugString("NetTester::Connect 2 \n");

	m_hConnectEvent = WSACreateEvent();
	if (WSA_INVALID_EVENT == m_hConnectEvent)
	{
		//goto error;
		return false;
	}

	OutputDebugString("NetTester::Connect 3 \n");

	if (SOCKET_ERROR == WSAEventSelect(m_Socket, m_hConnectEvent, FD_CONNECT))
	{
		WSACloseEvent(m_hConnectEvent);
		//goto error;
		return false;
	}

	OutputDebugString("NetTester::Connect 4 \n");

	m_bIsAlive = true;
	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, ThreadProc, this, 0, &threadID );

//	SetSocket((void*) m_Socket,iPort);
	return true;
}

void NetTester::Close()
{
	m_bIsAlive = false;

	if(m_hThread)
	{
		::WaitForSingleObject(m_hThread,1000);
		m_hThread = 0;
	}

	if(m_Socket)
	{
		shutdown(m_Socket,2);
		closesocket(m_Socket);
	}
	m_Socket = 0;
	m_bConnected = false;
}

bool NetTester::IsAlive()
{
	return m_bIsAlive;
}

bool NetTester::IsFinished()
{
	return m_bIsFinished;
}

void NetTester::DoEvent()
{
	WSANETWORKEVENTS WSAEvents;
	if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &m_hConnectEvent, FALSE, WAIT_TIMEOUT_INTERVAL, FALSE))
	{
		WSAEnumNetworkEvents(m_Socket, m_hConnectEvent, &WSAEvents);
		//if ((WSAEvents.lNetworkEvents & FD_CONNECT) && (0 == WSAEvents.iErrorCode[FD_CONNECT_BIT]))
		//	m_bConnectRet = true;
		if(WSAEvents.lNetworkEvents & FD_CONNECT)
		{
			switch (WSAEvents.iErrorCode[FD_CONNECT_BIT])
			{
				case WSANOTINITIALISED:
				case WSAENETDOWN:
				case WSAEADDRNOTAVAIL:
				case WSAEAFNOSUPPORT:
				case WSAECONNREFUSED:
				case WSAENETUNREACH:
				case WSAEHOSTUNREACH:
				case WSAENOBUFS:
				case WSAETIMEDOUT:
					m_bConnectRet = false;
					break;
				default:
					m_bConnectRet = true;
					break;
			}
		}
	}
}

bool NetTester::WaitForResult(bool* pValue,int iWaitTime)
{
	DWORD dwStart = 0;
	DWORD dwEnd = 0;
    bool bDo = true;
	bool bRet = false;

	dwStart = GetTickCount();

    while(bDo)
    {
		dwEnd = GetTickCount();

		if(*pValue)
		{
			bRet = true;
	        bDo = false;
		}
		else if(dwEnd - dwStart > iWaitTime)  //µ¥«Ý®É¶¡
			bDo = false;
        MSG Msg;
        if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE | PM_NOYIELD))
		{
            TranslateMessage(&Msg);			
            DispatchMessage(&Msg);				
		}
		Sleep(1);
    }
	return bRet;
}

bool NetTester::WaitForConnection(int iWaitTime)
{
	bool bRet = WaitForResult(&m_bConnectRet,iWaitTime);
	if(bRet)
	{
		return m_bConnectRet;//m_bConnected;
	}
	return false;
}

unsigned long NetTester::getAddrFromIP(char *ip) 
{
	if (ip) 
	{
		if (isalpha(*ip)) 
		{
			HOSTENT *host;
			host = gethostbyname(ip);
			if (host) 
				return ((LPIN_ADDR)host->h_addr)->s_addr;
		}
		return inet_addr(ip);
	}
	return htonl(INADDR_ANY);
}


unsigned int __stdcall ThreadProc(void* lpvThreadParm)
{
	NetTester* pNetObj = (NetTester*)lpvThreadParm;
    while(pNetObj->IsAlive())
	{
		pNetObj->DoEvent();
	}
	
	return 0;
}