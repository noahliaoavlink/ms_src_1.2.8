#include "stdafx.h"
#include "ServerManager.h"

#include <Ws2tcpip.h>

#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "wsock32.lib")

#define WM_SERVER_SOCKET_EVENT  WM_USER + 1
#define WM_CLIENT_SOCKET_EVENT  WM_USER + 2

DWORD WINAPI TCThread(LPVOID lpParam);

ServerManager::ServerManager()
{
	//m_pPlayer = new PlayerAgent;
	m_Socket = 0;
	m_pServerWin = 0;
	m_pDispatchWin = 0;

	m_bIsAlive = false;
	m_hTCThread = NULL;
	m_iTCStatus = TCS_STOP;
	m_dwLastTime = 0;
	m_ulCurTimeCode = 0;
	m_ulTotalOfTimeCode = 1000 * 60 * 2 + 15 * 1000;
	m_ulCurFrameTime = 0;

	m_lTimeCodeCount = 0;

	Startup();

	m_pUDPServer = new UDPServer;

	m_pTimeCodeQueue = new SQQueue;
	m_pTimeCodeQueue->Alloc(sizeof(long),200);
	m_iWaitCount = 0;

	InitializeCriticalSection(&m_CriticalSection);
}

ServerManager::~ServerManager()
{
	if (m_pUDPServer)
	{
		m_pUDPServer->Close();
		delete m_pUDPServer;
	}

	Cleanup();

	RemoveAll();

//	m_TimeCodeList.clear();
	if (m_pTimeCodeQueue)
	{
		m_pTimeCodeQueue->Reset();
		delete m_pTimeCodeQueue;
	}

	/*
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
				delete pCurItem;
		}
	}
	*/

	//delete m_pPlayer;
	DeleteCriticalSection(&m_CriticalSection);
}

bool ServerManager::Startup()
{
	UUID uuid;
	UuidCreate(&uuid);
	char szServerWinName[256];
	char szDispatchWinName[256];

	sprintf(szServerWinName, "MSMServerWin_%d", uuid.Data1);
	sprintf(szDispatchWinName, "MSDispatchWin_%d", uuid.Data1);

	m_pServerWin = new CNWin(szServerWinName,0);
	m_pDispatchWin = new CNWin(szDispatchWinName,0);

	m_pServerWin->SetCallback(this);
	m_pDispatchWin->SetCallback(this);

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

void ServerManager::Cleanup()
{
	if(m_pServerWin)
	{
		m_pServerWin->DestroyWindow();
		delete m_pServerWin;
	}

	if(m_pDispatchWin)
	{
		m_pDispatchWin->DestroyWindow();
		delete m_pDispatchWin;
	}

    WSACleanup();
}

bool ServerManager::Init()
{
///	if (m_pUDPServer)
	//	m_pUDPServer->SetInfo("1.0", "1.0",,);
	return true;
}

int ServerManager::Open(int iPort)
{
	int rVal;
	hostent* localHost;
	char* szLocalIP;

	//create socket
	m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_Socket == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
        return false;
	}

	//fill in sockaddr_in struct 
	SOCKADDR_IN sin;
	sin.sin_family = PF_INET;
	sin.sin_port = htons(iPort);
	sin.sin_addr.s_addr = INADDR_ANY;

	//bind the socket
	rVal = bind(m_Socket, (LPSOCKADDR)&sin, sizeof(sin));
	if(rVal == SOCKET_ERROR)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}

	localHost = gethostbyname("");
	szLocalIP = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);

	//get socket to listen 
	rVal = listen(m_Socket, SOMAXCONN);
	if(rVal == SOCKET_ERROR)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}
	SetSocket((void*)m_Socket,iPort);

	if (m_pUDPServer)
	{
		m_pUDPServer->SetInfo("1.0", "1.0", szLocalIP, iPort);
		m_pUDPServer->Open();
	}

//	SetTimer(m_ServerWin->GetSafeHwnd(),123,1000,NULL);
	return 0;
}

void ServerManager::Close()
{
	char szMsg[512];
//	KillTimer(m_ServerWin->GetSafeHwnd(),123);

	sprintf(szMsg, "ServerManager::Close() %d 0\n", m_Socket);
	OutputDebugStringA(szMsg);

	TC_Stop();

	RemoveAll();

	if(m_Socket)
		closesocket(m_Socket);
	m_Socket = 0;

	if (m_pUDPServer)
		m_pUDPServer->Close();

//	Sleep(100);

	sprintf(szMsg, "ServerManager::Close() %d 1\n", m_Socket);
	OutputDebugStringA(szMsg);
}

//callback functions
bool ServerManager::Accept()
{
	char szMsg[512];
	SOCKET hSocket = accept(m_Socket, NULL, NULL);

	sprintf(szMsg,"ServerManager::Accept() %d\n",hSocket);
	OutputDebugStringA(szMsg);

	//extern CTest_IPC_ServerDlg* g_pDlg;
	//g_pDlg->AddToList(szMsg);

	if(hSocket == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}
	else
	{
		int iSize = 65535;
		setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(const char *)&iSize,sizeof(int));
		setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)&iSize,sizeof(int));

		Server* pObj = new Server;

		/*
		//pObj->SetPlayer(m_pPlayer);
		pObj->SetSocket(hSocket);
		pObj->Init();
		if (m_iTCStatus == TCS_RUNNING)
		{
			pObj->EnableDoSynPB(true);
		}
		*/

		m_SocketList.push_back(pObj);

		Server* pCurItem = m_SocketList.back();
		pCurItem->SetSocket(hSocket);
		pCurItem->Init();
		if (m_iTCStatus == TCS_RUNNING)
		{
			pCurItem->EnableDoSynPB(true);
		}

		WSAAsyncSelect(hSocket,m_pDispatchWin->GetSafeHwnd(),hSocket+30000,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE|FD_ACCEPT);

		pCurItem->Hello();
	}
	return true;
}

void ServerManager::DeleteObj(unsigned long ulID)
{

	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					delete pCurItem;
					it = m_SocketList.erase(it);
					//break;
				}
			}
		}
	}
}

void ServerManager::CloseClient(unsigned long ulID)
{
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					pCurItem->Close();
					break;
				}
			}
		}
	}
}

void ServerManager::ReceiveData(unsigned long ulID)
{
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					pCurItem->Receive();
					break;
				}
			}
		}
	}
}

void ServerManager::SetSocket(void* pSocket,int iPort)
{
	m_Socket = (SOCKET)pSocket;
	m_iPort = iPort;
	WSAAsyncSelect(m_Socket,m_pServerWin->GetSafeHwnd(),WM_SERVER_SOCKET_EVENT,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE|FD_ACCEPT);
}

void ServerManager::TC_Start()
{
	bool bDoContinue = false;
	if (m_iTCStatus == TCS_PAUSE)
	{
		bDoContinue = true;
	}

	else if (m_iTCStatus == TCS_STOP)
	{
		m_ulCurTimeCode = 0;
		m_dwLastTime = 0;

		m_bIsAlive = true;
		DWORD nThreadID;
		m_hTCThread = CreateThread(0, 0, TCThread, (void *)this, 0, &nThreadID);
	}

	if (m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		{
			Server* pCurItem = *it;
			if (pCurItem && !bDoContinue)
				pCurItem->TC_Start();
			else
				pCurItem->TC_Continue();
		}
	}

	m_iTCStatus = TCS_RUNNING;
}

void ServerManager::TC_Stop()
{
	m_iTCStatus = TCS_STOP;

	m_bIsAlive = false;
	::WaitForSingleObject(m_hTCThread, 10000);

	if (m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		{
			Server* pCurItem = *it;
			if (pCurItem)
				pCurItem->TC_Stop();
		}
	}

#if 1
	m_pTimeCodeQueue->Reset();
#else
	m_TimeCodeList.clear();
#endif
}

void ServerManager::TC_Pause()
{
	m_iTCStatus = TCS_PAUSE;
	m_dwLastTime = 0;

	if (m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		{
			Server* pCurItem = *it;
			if (pCurItem)
				pCurItem->TC_Pause();
		}
	}
}

int ServerManager::GetTCStatus()
{
	return m_iTCStatus;
}

unsigned long ServerManager::GetTotalOfTimeCode()
{
	return m_ulTotalOfTimeCode;
}

bool ServerManager::IsAlive()
{
	return m_bIsAlive;
}

void ServerManager::TimeCodeLoop()
{
	EnterCriticalSection(&m_CriticalSection);
#if 0
	if (m_iTCStatus == TCS_RUNNING)
	{
		if (m_ulLastTimeCode != m_ulCurTimeCode)
		{
			if (m_SocketList.size() > 0)
			{
				std::list<Server*>::iterator it;
				for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
				{
					Server* pCurItem = *it;
					if (pCurItem)
						pCurItem->TC_UpdateTimeCode(m_ulCurTimeCode);
				}
			}

			m_ulLastTimeCode = m_ulCurTimeCode;
		}
	}
#else
	if (m_iTCStatus == TCS_RUNNING && m_pTimeCodeQueue->GetTotal() > 0 && m_iWaitCount == 0)
	{
		unsigned char* p = m_pTimeCodeQueue->GetNext();
		memcpy(&m_ulCurTimeCode, p,sizeof(long));

		if (m_ulLastTimeCode != m_ulCurTimeCode)
		{
			if (m_SocketList.size() > 0)
			{
				std::list<Server*>::iterator it;
				for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
				{
					Server* pCurItem = *it;
					if (pCurItem)
					{
						pCurItem->UpdateCurFrameTime(m_ulCurFrameTime);
						pCurItem->TC_UpdateTimeCode(m_ulCurTimeCode);
					}
				}
			}

			m_ulLastTimeCode = m_ulCurTimeCode;
		}
	}

	if (m_iWaitCount > 0)
		m_iWaitCount--;
#endif
	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

void ServerManager::UpdateTimeCode(unsigned long ulTimeCode)
{
#if 0
	m_ulCurTimeCode = ulTimeCode;
#else
	if (m_iTCStatus == TCS_RUNNING)
	{
		if (ulTimeCode > m_ulTotalOfTimeCode)
			return;

		if(ulTimeCode <= 40 || ulTimeCode == m_ulTotalOfTimeCode)
			m_pTimeCodeQueue->Add((unsigned char*)&ulTimeCode, sizeof(long));
		else
		{
			if(m_lTimeCodeCount % 2 == 0)
				m_pTimeCodeQueue->Add((unsigned char*)&ulTimeCode, sizeof(long));
		}

		m_lTimeCodeCount++;
	}
#endif
}

void ServerManager::SetTotalOfTimeCode(unsigned long ulTimeCode)
{
	m_ulTotalOfTimeCode = ulTimeCode;
	m_iWaitCount = 4;
	m_pTimeCodeQueue->Reset();
}

bool ServerManager::IsWorking()
{
	if (m_Socket)
		return true;
	return false;
}

void ServerManager::UpdateCurFrameTime(unsigned long ulTime)
{
	m_ulCurFrameTime = ulTime;
}

void ServerManager::RemoveAll()
{
	if (m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		//for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it)
		{
			Server* pCurItem = *it;
			if (pCurItem)
			{
				pCurItem->Close();
				delete pCurItem;
			}
			it = m_SocketList.erase(it);
		}
	}

	m_SocketList.clear();
}

int ServerManager::GetTotalOfClients()
{
	return m_SocketList.size();
}

void* ServerManager::WinMsg(HWND hWnd,int iMsg,void* pParameter1,void* pParameter2)
{
	if(hWnd == m_pServerWin->GetSafeHwnd())
	{
		switch(WSAGETSELECTEVENT(pParameter2))  //lParam
		{
		   case FD_CONNECT:
			   {
				   bool bConnected = true;
				   if(WSAGETSELECTERROR(pParameter2)) //lParam //cannot connect to..
					   bConnected = false;
			   }
			   break;
		   case FD_READ:
			  break;
		   case FD_WRITE:
			  break;
		   case FD_CLOSE:
			  break;
		   case FD_ACCEPT:
			   Accept();
			 break;
		   default:
			   ;
		}
	}
	else if (hWnd == m_pDispatchWin->GetSafeHwnd())
	{
		switch(WSAGETSELECTEVENT(pParameter2))
		{
		   case FD_CONNECT:
			   {
				   bool bConnected = true;
				   if(WSAGETSELECTERROR(pParameter2))//lParam //cannot connect to..
					   bConnected = false;
			   }
			   break;
		   case FD_READ:
			   {
				   //OutputDebugString("FD_READ..\n");
				   bool bBreak = false;
				   if(WSAGETSELECTERROR(pParameter2))
					   bBreak = true;
				   ReceiveData(iMsg);
			   }
			  break;
		   case FD_WRITE:
			  break;
		   case FD_CLOSE:
			   {
				   OutputDebugStringA("FD_CLOSE..\n");
				   CloseClient(iMsg);
				   DeleteObj(iMsg);
			   }
			  break;
		   case FD_ACCEPT:
			 break;
		   default:
			   ;
		}
	}
	
	return 0;
}

DWORD WINAPI TCThread(LPVOID lpParam)
{
	ServerManager* pObj = (ServerManager*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->TimeCodeLoop();
	}
	return 0;
}
