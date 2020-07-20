#include "stdafx.h"
#include "Client.h"
#include "Mmsystem.h"
//#include "..\..\..\Include\screatewin.h"

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "rpcrt4.lib")

#define _NS_CLIENT_ID 10102     //¥DID

#define _MAX_RCV_BUFFER 65536

#define TIMEOUT 3000

Client* g_pTCClient;

//LRESULT WINAPI SocketWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
unsigned int __stdcall RcvThreadProc(void* lpvThreadParm);
unsigned int __stdcall ProcessThreadProc(void* lpvThreadParm);
unsigned int __stdcall ReconnectThreadProc(void* lpvThreadParm);

unsigned long getAddrFromIP(char *ip)
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

Client::Client()
{
	Startup();
	m_pEventCallback = 0;

	m_hRcvThread = NULL;
	m_hProcessThread = NULL; 

	m_pUDPClient = new UDPClient;

	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(200);
	m_pQBuffer->SetBufferSize(_MAX_RCV_BUFFER);

	m_pRcvBuffer = new char[_MAX_RCV_BUFFER];
	m_pBuffer = new char[_MAX_RCV_BUFFER];

	strcpy(m_szID, "");
	strcpy(m_szPW, "");

	m_pPacketParser = new PacketParser;

	ResetRcvStatus();

	m_bIsPassed = false;
	m_dwIoctlLastTime = 0;
	m_iSkipCount = 0;
	m_bEnableReconnect = true;
	m_bDoReconnect = false;
	m_iReconnectStatus = RS_NONE;
	strcpy(m_szIP, "127.0.0.1");
	m_iPort = 0;
	m_iSelectTimeOutCount = 0; 

#ifdef _ENABLE_PACKET_LIST
	//m_pPacketList = new SQList;
	//m_pPacketList->EnableRemoveData(false);

	m_pPacketQueue = new SQQueue;
	m_pPacketQueue->Alloc(sizeof(PacketItem),200);
#endif

	//m_SimpleIMCP.CreateSocket();

	InitializeCriticalSection(&m_RcvCriticalSection);
	InitializeCriticalSection(&m_CriticalSection);
}

Client::~Client()
{
	//m_SimpleIMCP.Close();
	Cleanup();

	delete m_pQBuffer;

	if (m_pRcvBuffer)
		delete m_pRcvBuffer;

	if (m_pBuffer)
		delete m_pBuffer;

	if (m_pUDPClient)
		delete m_pUDPClient;

	if (m_pPacketParser)
		delete m_pPacketParser;

#ifdef _ENABLE_PACKET_LIST
	/*
	if (m_pPacketList)
	{
		int iTotal = m_pPacketList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			PacketItem* pCurPacketItem = (PacketItem*)m_pPacketList->Get(i);
			if (pCurPacketItem)
				delete pCurPacketItem;
		}

		delete m_pPacketList;
	}
	*/
	delete m_pPacketQueue;
#endif

	DeleteCriticalSection(&m_RcvCriticalSection);
	DeleteCriticalSection(&m_CriticalSection);
}

bool Client::Startup()
{
	//create win
#ifdef _WINSOCKET
	char szWinName[256];

	UUID uuid;
	UuidCreate(&uuid);
//	pInfo->ulID = uuid.Data1;

	sprintf(szWinName, "MSClientWin_%d", uuid.Data1);
	//m_hWnd = CreateWin(SocketWinProc, szWinName);

	m_pClientWin = new CNWin(szWinName, WT_EXT); //0
	m_pClientWin->SetCallback(this);
#endif
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		m_iErrorCode = WSAGetLastError();
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{
		m_iErrorCode = 20000;
		return false;
	}
	return true;
}

void Client::Cleanup()
{
	//Ãö³¬ÂÂªºsocket
	if (m_hSocket)
		Close();

	//if (m_hWnd)
		//DestroyWindow(m_hWnd);
#ifdef _WINSOCKET
	if (m_pClientWin)
	{
		m_pClientWin->DestroyWindow();
		delete m_pClientWin;
	}
#endif
	WSACleanup();
}

void Client::SetEventCallback(EventCallback* pObj)
{
	m_pEventCallback = pObj;
}

bool Client::Connect(char* szIP, int iPort)
{
	m_bIsAlive = true;

	strcpy(m_szIP, szIP);
	m_iPort = iPort;

	if ((m_hSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}

	//int size = 65535;
	//setsockopt(m_Socket,SOL_SOCKET,SO_RCVBUF,(const char *)&size,sizeof(int));
	int iSize = 65535;
	setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&iSize, sizeof(int));
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&iSize, sizeof(int));

	struct sockaddr_in dst_addr;
	dst_addr.sin_family = PF_INET;
	dst_addr.sin_addr.s_addr = getAddrFromIP((char*)szIP);//inet_addr(szIPAddr);
	dst_addr.sin_port = htons(iPort);

#ifdef _WINSOCKET
	if (connect(m_hSocket, (struct sockaddr FAR *)&dst_addr, sizeof(dst_addr)) != 0)
	{
		m_iErrorCode = WSAGetLastError();
		if (m_iErrorCode != WSAEWOULDBLOCK)
		{
			Close();
			return false;
		}
		else
		{
			char szMsg[512];
			// nonblocking connect
			struct timeval oTV;
			oTV.tv_sec = TIMEOUT / 1000;
			oTV.tv_usec = TIMEOUT;
			fd_set oRead, oWrite;
			FD_ZERO(&oRead);
			FD_ZERO(&oWrite);
			int nResult, nError;

			FD_SET(m_hSocket, &oRead);
			oWrite = oRead;
			nResult = select(m_hSocket + 1, &oRead, &oWrite, 0, &oTV);
			if (nResult == 0)
			{
				sprintf(szMsg, "Connection timeout\n");
				OutputDebugStringA(szMsg);
				closesocket(m_hSocket);
				return false;
			}
			if (FD_ISSET(m_hSocket, &oRead) || FD_ISSET(m_hSocket, &oWrite))
			{
				int nLen = sizeof(nError);
				if (getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen) < 0)
				{
					sprintf(szMsg, "Connect error %d\n", nError);
					OutputDebugStringA(szMsg);
					closesocket(m_hSocket);
					return false;
				}
			}
			else
			{
				sprintf(szMsg, "Unknown err in connect\n");
				OutputDebugStringA(szMsg);
				closesocket(m_hSocket);
				return false;
			}
		}
	}
#else
	unsigned long ulArgp = 1;  //Non-blocking
	int iRet = ioctlsocket(m_hSocket, FIONBIO, &ulArgp);
	if (!iRet)
	{
		if (connect(m_hSocket, (struct sockaddr FAR *)&dst_addr, sizeof(dst_addr)) != 0)
		{
			m_iErrorCode = WSAGetLastError();
			if (m_iErrorCode != WSAEWOULDBLOCK)
			{
				Close();
				return false;
			}
			else
			{
				char szMsg[512];
				// nonblocking connect
				struct timeval oTV;
				oTV.tv_sec = TIMEOUT / 1000;
				oTV.tv_usec = TIMEOUT;
				fd_set oRead, oWrite;
				FD_ZERO(&oRead);
				FD_ZERO(&oWrite);
				int nResult, nError;

				FD_SET(m_hSocket, &oRead);
				oWrite = oRead;
				nResult = select(m_hSocket + 1, &oRead, &oWrite, 0, &oTV);
				if (nResult == 0)
				{
					//printf("Connection timeout\n");
					sprintf(szMsg, "Connection timeout\n");
					OutputDebugStringA(szMsg);
					closesocket(m_hSocket);
					return false;
				}
				if (FD_ISSET(m_hSocket, &oRead) || FD_ISSET(m_hSocket, &oWrite))
				{
					int nLen = sizeof(nError);
					if (getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen) < 0)
					{
						//printf("Connect error %d\n", nError);
						sprintf(szMsg, "Connect error %d\n", nError);
						OutputDebugStringA(szMsg);
						closesocket(m_hSocket);
						return false;
					}
				}
				else
				{
					//printf("Unknown err in connect\n");
					sprintf(szMsg, "Unknown err in connect\n");
					OutputDebugStringA(szMsg);
					closesocket(m_hSocket);
					return false;
				}
			}
		}
	}
#endif
	/*
	linger oLinger;
	oLinger.l_onoff = 1;
	oLinger.l_linger = 3; // wait 3 seconds for TCP handshake
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (char*)&oLinger, sizeof(oLinger)) == SOCKET_ERROR)
	{
	}
	*/
//	SetSocket((void*)m_hSocket, iPort);

	unsigned threadID;
#ifdef _WINSOCKET
	WSAAsyncSelect(m_hSocket, m_pClientWin->GetSafeHwnd(), m_hSocket + 30000, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE | FD_ACCEPT);
#else
	m_hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RcvThreadProc, this, 0, &threadID);
#endif
	m_hProcessThread = (HANDLE)_beginthreadex(NULL, 0, ProcessThreadProc, this, 0, &threadID);

	return true;
}

void Client::Close()
{
	m_bIsAlive = false;

	ResetRcvStatus();

#ifdef _WINSOCKET
#else
	::WaitForSingleObject(m_hRcvThread, 2000);
#endif
	::WaitForSingleObject(m_hProcessThread, 2000);

	if (m_hSocket)
	{
		closesocket(m_hSocket);
		shutdown(m_hSocket, 2);
	}

	m_hSocket = 0;
	m_bConnected = false;
	m_bIsPassed = false;

#ifdef _ENABLE_PACKET_LIST
	m_pPacketQueue->Reset();
#endif

	ResetRcvStatus();

	LeaveCriticalSection(&m_CriticalSection);
//	LeaveCriticalSection(&m_RcvCriticalSection);
}

void Client::DoLogin()
{
	Login("","");
}

void Client::Login(char* szID,char* szPW)
{
	char szMsg[512];
	char szID2[64]; //= "bruvis";
	char szPW2[64];// = "avlink";

	if (strcmp(szID, "") == 0 && strcmp(szPW, "") == 0)
	{
		strcpy(szID2, "bruvis");
		strcpy(szPW2, "avlink");
	}
	else
	{
		strcpy(szID2, szID);
		strcpy(szPW2, szPW);
	}

	strcpy(m_szID, szID2);
	strcpy(m_szPW, szPW2);

	PacketMaker packet_maker;
	packet_maker.Make(CID_LOGIN, (unsigned char*)szID2, strlen(szID2), (unsigned char*)szPW2, strlen(szPW2));

	m_dwLoginStartTime = timeGetTime();

	sprintf(szMsg, "TC Client::Login() - [%s , %s]\n",szID2, szPW2);
	OutputDebugStringA(szMsg);

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}

void Client::ReportTimeCode(unsigned long ulTimeCode)
{
	PacketMaker packet_maker;
	packet_maker.Make(CID_REPORT, ulTimeCode);
	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}
/*
void Client::SetSocket(void* pSocket, int iPort)
{
	m_hSocket = (SOCKET)pSocket;
	m_iPort = iPort;

	//WSAAsyncSelect(m_Socket,m_hWnd,_NS_CLIENT_ID,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE|FD_ACCEPT);
//	HWND hWnd = m_pClientWin->GetSafeHwnd();
//	WSAAsyncSelect(m_hSocket, m_pClientWin->GetSafeHwnd(), _NS_CLIENT_ID, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE | FD_ACCEPT);
}
*/
//#define TIMEOUT 1000
void Client::Checkioctl()
{
	char szMsg[512];
	int nResult, nError;
	struct timeval oTV;
	
	if (TIMEOUT >= 1000)
	{
		oTV.tv_sec = TIMEOUT / 1000;
		oTV.tv_usec = TIMEOUT - (oTV.tv_sec * 1000);
	}
	else
	{
		oTV.tv_sec = 0;
		oTV.tv_usec = TIMEOUT;
	}
	
	fd_set oRead;
	FD_ZERO(&oRead);
	FD_SET(m_hSocket, &oRead);

	m_dwIoctlLastTime = timeGetTime();

	nResult = select(m_hSocket+1, &oRead, 0, 0, &oTV);
	if (nResult == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		sprintf(szMsg, "TC Client::Checkioctl() - select Error : %d\n", iError);
		OutputDebugStringA(szMsg);
	}

	if (nResult == 0)
	{
		m_iSelectTimeOutCount++;
		sprintf(szMsg, "TC Client::Checkioctl() - select timeout!!(%d)\n", m_iSelectTimeOutCount);
		OutputDebugStringA(szMsg);

		if (m_iSelectTimeOutCount > 60)
		{
			Close();

			if (m_bEnableReconnect)
			{
				m_bDoReconnect = true;
				m_iReconnectStatus = RS_PING;// RS_RECONNECT;

				unsigned threadID;
				HANDLE hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, ReconnectThreadProc, this, 0, &threadID);
			}
		}
	}

	if (FD_ISSET(m_hSocket, &oRead))
		Receive();
}

void Client::Receive()
{
#ifdef _WINSOCKET
	EnterCriticalSection(&m_RcvCriticalSection);
	int iLen = recv(m_hSocket, m_pRcvBuffer, _MAX_RCV_BUFFER, 0);
	if (iLen > 0)
		bool bRet = m_pQBuffer->Add(m_pRcvBuffer, iLen);
	LeaveCriticalSection(&m_RcvCriticalSection);
#else
	int iLen = recv(m_hSocket, m_pRcvBuffer, _MAX_RCV_BUFFER, 0);

	if (iLen <= 0)
	{
		if (m_pEventCallback)
			m_pEventCallback->Event(CID_DISCONNECT, 0, 0);

		Close();

		if (m_bEnableReconnect)
		{
			m_bDoReconnect = true;
			m_iReconnectStatus = RS_PING;// RS_RECONNECT;

			unsigned threadID;
			HANDLE hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, ReconnectThreadProc, this, 0, &threadID);
		}

		return;
	}

	if (iLen > 0)
		bool bRet = m_pQBuffer->Add(m_pRcvBuffer, iLen);
#endif
}

void Client::ReceiveLoop()
{
//	if (TryEnterCriticalSection(&m_RcvCriticalSection) == FALSE)
	//	return;
	EnterCriticalSection(&m_RcvCriticalSection);
	Checkioctl();
	LeaveCriticalSection(&m_RcvCriticalSection);
}

bool Client::IsAlive()
{
	return m_bIsAlive;
}
/*
void Client::SetStatus(bool bEnable)
{
	m_bConnected = bEnable;
	m_bConnectRet = true;
}
*/

#pragma optimize( "", off )
void Client::ProcessRecvData()
{
	char szMsg[1024];
	EnterCriticalSection(&m_CriticalSection);

	//if (TryEnterCriticalSection(&m_CriticalSection) == FALSE)
		//return;

	if (m_pQBuffer->GetTotal() > 0)
	{
		int iRet;
		ZeroMemory(m_pBuffer, _MAX_RCV_BUFFER);
		int iLen = m_pQBuffer->GetNext((unsigned char *)m_pBuffer);

		m_pPacketParser->Input((unsigned char *)m_pBuffer, iLen);

		bool bDo = true;
		while (bDo)
		{
			iRet = m_pPacketParser->Parse();
			if (iRet == -1 || iRet == 0)
				bDo = false;
			else
			{
#ifdef _ENABLE_PACKET_LIST
				PacketItem* pCurPacketItem = m_pPacketParser->GetPacketItem();
				/*
				if (pCurPacketItem)
				{
					PacketItem* pNewPacketItem = new PacketItem;
					memcpy(pNewPacketItem, pCurPacketItem,sizeof(PacketItem));
					m_pPacketList->Add(pNewPacketItem);
				}
				*/

				if (pCurPacketItem)
					m_pPacketQueue->Add((unsigned char*)pCurPacketItem,sizeof(PacketItem));
#else
				PacketItem* pCurPacketItem = m_pPacketParser->GetPacketItem();
				if (pCurPacketItem)
				{
					switch (pCurPacketItem->lID)
					{
						case CID_HELLO:
							DoLogin();
							break;
						case CID_LOGIN_RET:
						{
							ResetRcvStatus();
							DWORD dwLoginEndTime = timeGetTime();

							sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET spend_time:%d\n", dwLoginEndTime - m_dwLoginStartTime);
							OutputDebugStringA(szMsg);

							/*
							if (pCurPacketItem->lData == LR_SUCESSED)
							{
								sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_SUCESSED\n");
							}
							else if (pCurPacketItem->lData == LR_FAILED)
							{
								sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_FAILED\n");
							}
							OutputDebugStringA(szMsg);
							*/

							if (pCurPacketItem->lData == LR_SUCESSED)
								m_bIsPassed = true;
							else if (pCurPacketItem->lData == LR_FAILED)
								m_bIsPassed = false;

							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData,0);
						}
						break;
						case CID_START:
							//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_START \n");
							//OutputDebugStringA(szMsg);
							ResetRcvStatus();
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
							break;
						case CID_STOP:
							//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_STOP \n");
							//OutputDebugStringA(szMsg);
							ResetRcvStatus();
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
							break;
						case CID_PAUSE:
							//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_PAUSE \n");
							//OutputDebugStringA(szMsg);
							ResetRcvStatus();
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
							break;
						case CID_UPDATE_TIME_CODE:
							//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_UPDATE_TIME_CODE %d\n", pCurPacketItem->lData);
							//OutputDebugStringA(szMsg);
							if (m_iReceiveStatus == CRS_HUNGER)
							{
								m_iSkipCount = 2;
							}

							m_iReceiveStatus = CRS_NORMAL;

							if (m_iSkipCount > 0)
							{
								m_iSkipCount--;
								//LeaveCriticalSection(&m_CriticalSection);
								//return;
							}
							else
							{
								if (pCurPacketItem->ucPacketType == PT_SIMPLE)
								{
									if (m_pEventCallback)
										m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
								}
								else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
								{
									if (m_pEventCallback)
										m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, (void*)pCurPacketItem->lData2);
								}
							}
							break;
						case CID_DO_SYN_PB:
							sprintf(szMsg, "TC Client::ProcessRecvData() - CID_DO_SYN_PB %u\n", pCurPacketItem->lData);
							OutputDebugStringA(szMsg);
							ResetRcvStatus();
							if (pCurPacketItem->ucPacketType == PT_SIMPLE)
							{
								if (m_pEventCallback)
									m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
							}
							else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
							{
							}
							break;
						case CID_CONTINUE:
							ResetRcvStatus();
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
							break;
						default:
							{
								sprintf(szMsg, "TC Client::ProcessRecvData() - unknow packet!! [%d]\n", pCurPacketItem->lID);
								OutputDebugStringA(szMsg);
							}
							break;
					}
				}
#endif
			}
//			Sleep(1);
		}
	}


#ifdef _ENABLE_PACKET_LIST
	/*
	if (m_pPacketList && m_pPacketList->GetTotal() > 0)
	{
		PacketItem* pCurPacketItem = (PacketItem*)m_pPacketList->Get(0);
		if (pCurPacketItem)
		{
			switch (pCurPacketItem->lID)
			{
				case CID_HELLO:
					DoLogin();
					break;
				case CID_LOGIN_RET:
				{
					ResetRcvStatus();
					DWORD dwLoginEndTime = timeGetTime();

					sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET spend_time:%d\n", dwLoginEndTime - m_dwLoginStartTime);
					OutputDebugStringA(szMsg);

					
					//if (pCurPacketItem->lData == LR_SUCESSED)
					//{
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_SUCESSED\n");
					//}
					//else if (pCurPacketItem->lData == LR_FAILED)
					//{
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_FAILED\n");
					//}
					//OutputDebugStringA(szMsg);
					

					if (pCurPacketItem->lData == LR_SUCESSED)
						m_bIsPassed = true;
					else if (pCurPacketItem->lData == LR_FAILED)
						m_bIsPassed = false;

					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
				}
				break;
				case CID_START:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_START \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_STOP:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_STOP \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_PAUSE:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_PAUSE \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_UPDATE_TIME_CODE:
					ResetRcvStatus();
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_UPDATE_TIME_CODE %d\n", pCurPacketItem->lData);
					//OutputDebugStringA(szMsg);
					if (m_iReceiveStatus == CRS_HUNGER)
					{
						m_iSkipCount = 2;
					}

					m_iReceiveStatus = CRS_NORMAL;

					if (m_iSkipCount > 0)
					{
						m_iSkipCount--;
						//LeaveCriticalSection(&m_CriticalSection);
						//return;
					}
					else
					{
						if (pCurPacketItem->ucPacketType == PT_SIMPLE)
						{
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
						}
						else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
						{
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, (void*)pCurPacketItem->lData2);
						}
					}
					break;
				case CID_DO_SYN_PB:
					sprintf(szMsg, "TC Client::ProcessRecvData() - CID_DO_SYN_PB %u\n", pCurPacketItem->lData);
					OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (pCurPacketItem->ucPacketType == PT_SIMPLE)
					{
						if (m_pEventCallback)
							m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
					}
					else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
					{
					}
					break;
				case CID_CONTINUE:
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
					break;
				default:
				{
					sprintf(szMsg, "TC Client::ProcessRecvData() - unknow packet!! [%d]\n", pCurPacketItem->lID);
					OutputDebugStringA(szMsg);
				}
				break;
			}
			delete pCurPacketItem;
		}
		m_pPacketList->Delete(0);
	}
	*/

	if (m_pPacketQueue->GetTotal() > 0)
	{
		PacketItem* pCurPacketItem = (PacketItem*)m_pPacketQueue->GetNext();
		if (pCurPacketItem)
		{
			switch (pCurPacketItem->lID)
			{
				case CID_HELLO:
					DoLogin();
					break;
				case CID_LOGIN_RET:
				{
					ResetRcvStatus();
					DWORD dwLoginEndTime = timeGetTime();

					sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET spend_time:%d\n", dwLoginEndTime - m_dwLoginStartTime);
					OutputDebugStringA(szMsg);

					/*
					if (pCurPacketItem->lData == LR_SUCESSED)
					{
					sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_SUCESSED\n");
					}
					else if (pCurPacketItem->lData == LR_FAILED)
					{
					sprintf(szMsg, "TCClient::ProcessRecvData() - CID_LOGIN_RET : LR_FAILED\n");
					}
					OutputDebugStringA(szMsg);
					*/

					if (pCurPacketItem->lData == LR_SUCESSED)
						m_bIsPassed = true;
					else if (pCurPacketItem->lData == LR_FAILED)
						m_bIsPassed = false;

					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
				}
				break;
				case CID_START:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_START \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_STOP:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_STOP \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_PAUSE:
					//sprintf(szMsg, "TCClient::ProcessRecvData() - CID_PAUSE \n");
					//OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, 0, 0);
					break;
				case CID_UPDATE_TIME_CODE:
					ResetRcvStatus();
					sprintf(szMsg, "TC Client::ProcessRecvData() - CID_UPDATE_TIME_CODE %d\n", pCurPacketItem->lData);
					OutputDebugStringA(szMsg);
					if (m_iReceiveStatus == CRS_HUNGER)
					{
						m_iSkipCount = 2;
					}

					m_iReceiveStatus = CRS_NORMAL;

					if (m_iSkipCount > 0)
					{
						m_iSkipCount--;
						//LeaveCriticalSection(&m_CriticalSection);
						//return;
					}
					else
					{
						if (pCurPacketItem->ucPacketType == PT_SIMPLE)
						{
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
						}
						else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
						{
							if (m_pEventCallback)
								m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, (void*)pCurPacketItem->lData2);
						}
					}
					break;
				case CID_DO_SYN_PB:
					sprintf(szMsg, "TC Client::ProcessRecvData() - CID_DO_SYN_PB %u\n", pCurPacketItem->lData);
					OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (pCurPacketItem->ucPacketType == PT_SIMPLE)
					{
						if (m_pEventCallback)
							m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
					}
					else if (pCurPacketItem->ucPacketType == PT_DUAL_LONG)
					{
					}
					break;
				case CID_CONTINUE:
					sprintf(szMsg, "TC Client::ProcessRecvData() - CID_CONTINUE %u\n", pCurPacketItem->lData);
					OutputDebugStringA(szMsg);
					ResetRcvStatus();
					if (m_pEventCallback)
						m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
					break;
				case CID_SWITCH_TO:
					{
						sprintf(szMsg, "#SwitchTo# TC Client::ProcessRecvData() - CID_SWITCH_TO %u\n", pCurPacketItem->lData);
						OutputDebugStringA(szMsg);

						if (m_pEventCallback)
							m_pEventCallback->Event(pCurPacketItem->lID, (void*)pCurPacketItem->lData, 0);
					}
					break;
				default:
				{
					sprintf(szMsg, "TC Client::ProcessRecvData() - unknow packet!! [%d]\n", pCurPacketItem->lID);
					OutputDebugStringA(szMsg);
				}
				break;
			}
		}
		Sleep(1);
	}
#endif

//	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}
#pragma optimize( "", on )

void Client::SearchServer()
{
	int iRet = m_pUDPClient->Open();
	if(iRet != 1)
	{
		for (int i = 0; i < 10; i++)
		{
			iRet = m_pUDPClient->Open();
			if (iRet == 1)
				break;
			Sleep(100);
		}
	}

	if (iRet == 1)
		m_pUDPClient->WaitForFinished();
}

void* Client::GetServerList()
{
	return m_pUDPClient->GetRecieveList();
}

bool Client::IsConnected()
{
	return m_bIsPassed;
}

void Client::CheckReceiveStatus()
{
	char szMsg[512];
	DWORD dwCurTime = timeGetTime();

	if (CRS_NORMAL == m_iReceiveStatus)
	{
		if (m_dwRcvLastTime == 0)
			m_dwRcvLastTime = dwCurTime;
		else
		{
			if (dwCurTime >= m_dwRcvLastTime + 50)
			{
				m_iReceiveStatus = CRS_HUNGER;

				sprintf(szMsg, "TC Client::CheckReceiveStatus() - m_iReceiveStatus = CRS_HUNGER\n");
				OutputDebugStringA(szMsg);
			}
			m_dwRcvLastTime = dwCurTime;
		}

		if (m_dwIoctlLastTime > 0)
		{
			int iTmpNum = dwCurTime - m_dwIoctlLastTime;

			if (iTmpNum > 100)
			{
				m_iReceiveStatus = CRS_HUNGER;
				sprintf(szMsg, "TC Client::CheckReceiveStatus() - dwCurTime - m_dwIoctlLastTime > 400 : %d\n", iTmpNum);
				OutputDebugStringA(szMsg);
			}
		}
	}
}

void Client::ResetRcvStatus()
{
	m_iReceiveStatus = CRS_NONE;
	m_dwRcvLastTime = 0;
	m_dwIoctlLastTime = 0;
	m_iSelectTimeOutCount = 0;
}

int Client::GetReceiveStatus()
{
	return m_iReceiveStatus;
}

void Client::DoReconnect()
{
	char szMsg[512];
	bool bDo = true;
	while (bDo)
	{
		if (m_bDoReconnect)
		{
			switch (m_iReconnectStatus)
			{
				case RS_PING:
					{
#if 0
						int iRet = m_SimpleIMCP.Ping(m_szIP);
						if (iRet == 1)
							m_iReconnectStatus = RS_RECONNECT;
						else
							Sleep(10);

						sprintf(szMsg, "TC Client::DoReconnect() - RS_PING (%d)\n", iRet);
						OutputDebugStringA(szMsg);
#else
					m_IPDetector.SetTargetIP(m_szIP);
					m_IPDetector.Open();
					m_IPDetector.WaitForFinished();
					if(m_IPDetector.IsConnected())
						m_iReconnectStatus = RS_RECONNECT;
					else
						Sleep(10);

					sprintf(szMsg, "TC Client::DoReconnect() - RS_PING (%d)\n", m_IPDetector.IsConnected());
					OutputDebugStringA(szMsg);
					sprintf(szMsg, "TC Client::DoReconnect() - RS_PING (%d)", m_IPDetector.IsConnected());
					Add2LogFile(LL_INFO, szMsg);
#endif
					}
					break;
				case RS_RECONNECT:
					{
						bool bRet = Connect(m_szIP, m_iPort);
						if (bRet)
						{
							m_iReconnectStatus = RS_LOGIN;
							Sleep(10);
						}
						else
							Sleep(1000);

						sprintf(szMsg, "TC Client::DoReconnect() - RS_RECONNECT (%d)\n", bRet);
						OutputDebugStringA(szMsg);
						sprintf(szMsg, "TC Client::DoReconnect() - RS_RECONNECT (%d)", bRet);
						Add2LogFile(LL_INFO, szMsg);
					}
					break;
				case RS_LOGIN:
					{
						Login(m_szID, m_szPW);
						m_iReconnectStatus = RS_LOGIN_RET;

						sprintf(szMsg, "TC Client::DoReconnect() - RS_LOGIN\n");
						OutputDebugStringA(szMsg);
						sprintf(szMsg, "TC Client::DoReconnect() - RS_LOGIN");
						Add2LogFile(LL_INFO, szMsg);
					}
					break;
				case RS_LOGIN_RET:
					{
						if (m_bIsPassed)
							m_iReconnectStatus = RS_FINISHED;
						else
							Sleep(100);

						sprintf(szMsg, "TC Client::DoReconnect() - RS_LOGIN_RET (%d)\n", m_bIsPassed);
						OutputDebugStringA(szMsg);
						sprintf(szMsg, "TC Client::DoReconnect() - RS_LOGIN_RET (%d)", m_bIsPassed);
						Add2LogFile(LL_INFO, szMsg);
					}
					break;
				case RS_FINISHED:
					m_iReconnectStatus = RS_NONE;
					m_bDoReconnect = false;

					sprintf(szMsg, "TC Client::DoReconnect() - RS_FINISHED\n");
					OutputDebugStringA(szMsg);
					sprintf(szMsg, "TC Client::DoReconnect() - RS_FINISHED");
					Add2LogFile(LL_INFO,szMsg);
					break;
			}
		}
		else
			bDo = false;
		Sleep(1);
	}
}

void Client::Add2LogFile(int iLevel,char* szMsg)
{
	if (m_pEventCallback)
		m_pEventCallback->Event(CID_CLIENT_EVENT, (void*)iLevel, szMsg);
}

#ifdef _WINSOCKET
void* Client::WinMsg(HWND hWnd, int iMsg, void* pParameter1, void* pParameter2)
{
	if (hWnd == m_pClientWin->GetSafeHwnd())
	{
		switch (WSAGETSELECTEVENT(pParameter2))
		{
			case FD_CONNECT:
			{
				bool bConnected = true;
				if (WSAGETSELECTERROR(pParameter2))//lParam //cannot connect to..
					bConnected = false;
			}
			break;
			case FD_READ:
			{
				//OutputDebugString("FD_READ..\n");
				bool bBreak = false;
				if (WSAGETSELECTERROR(pParameter2))
					bBreak = true;
				//ReceiveData(iMsg);
				Receive();
			}
			break;
			case FD_WRITE:
				break;
			case FD_CLOSE:
			{
				OutputDebugStringA("FD_CLOSE..\n");
				//CloseClient(iMsg);
				//DeleteObj(iMsg);

				if (m_pEventCallback)
					m_pEventCallback->Event(CID_DISCONNECT, 0, 0);

				Close();

				if (m_bEnableReconnect)
				{
					m_bDoReconnect = true;
					m_iReconnectStatus = RS_PING;// RS_RECONNECT;

					unsigned threadID;
					HANDLE hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, ReconnectThreadProc, this, 0, &threadID);
				}
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
#endif


unsigned int __stdcall RcvThreadProc(void* lpvThreadParm)
{
	Client* pNetObj = (Client*)lpvThreadParm;
	while (pNetObj->IsAlive())
	{
		pNetObj->ReceiveLoop();
		Sleep(1);
	}

	return 0;
}

unsigned int __stdcall ProcessThreadProc(void* lpvThreadParm)
{
	Client* pNetObj = (Client*)lpvThreadParm;
	while (pNetObj->IsAlive())
	{
		pNetObj->CheckReceiveStatus();
		pNetObj->ProcessRecvData();
		pNetObj->ProcessRecvData();
		Sleep(1);
	}
	return 0;
}

unsigned int __stdcall ReconnectThreadProc(void* lpvThreadParm)
{
	Client* pNetObj = (Client*)lpvThreadParm;
	pNetObj->DoReconnect();
	return 0;
}
