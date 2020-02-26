#include "stdafx.h"
#include "UDPClient.h"
#include <process.h>
#include <stdio.h>

#include "../../Include/sstring.h"

#define BROADCAST_PORT 1013
#define BROADCAST_MSG "BRUVIS_UDP_MSG_V1"
#define DURATION 10 * 100  //1 Sec //30 Sec

#pragma comment(lib, "wsock32.lib")

unsigned int __stdcall UDPClientThreadProc(void* lpvThreadParm);

UDPClient::UDPClient()
{
	m_ClientSocket = 0;
	m_hThread = 0;
	m_bFinished = false;
	m_pRecieveList = new SQList;
	m_pRecieveList->EnableRemoveData(false);
	InitializeCriticalSection(&m_CriticalSection);
}

UDPClient::~UDPClient()
{
	if (m_pRecieveList)
	{
		ResetRecieveList();
		delete m_pRecieveList;
	}

	//delete m_pRecieveList;
	DeleteCriticalSection(&m_CriticalSection);
}

bool UDPClient::Startup()
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

void UDPClient::Cleanup()
{
	WSACleanup();
}

int UDPClient::Open()
{
	m_bFinished = false;

	ResetRecieveList();

	m_ClientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_ClientSocket == -1)
    {
        MessageBoxA(NULL,"Error in creating socket","",MB_OK | MB_TOPMOST);
        return -1;
    }

    SOCKADDR_IN UDPserveraddr;
    memset(&UDPserveraddr,0, sizeof(UDPserveraddr));
    UDPserveraddr.sin_family = AF_INET;
    UDPserveraddr.sin_port = htons(BROADCAST_PORT);
    UDPserveraddr.sin_addr.s_addr = INADDR_ANY;
	//UDPserveraddr.sin_addr.s_addr = inet_addr("192.168.0.102");
    
    int len = sizeof(UDPserveraddr);

	bool bDoRetry = false;
    if(bind(m_ClientSocket, (SOCKADDR*)&UDPserveraddr,sizeof(SOCKADDR_IN)) < 0) 
    {
        //MessageBoxA(NULL,"ERROR binding in the server socket","",MB_OK);
        return - 2;
    }
	/*
	if (bDoRetry)
	{
		Sleep(100);
		for (int i = 0; i < 3; i++)
		{
			if (bind(m_ClientSocket, (SOCKADDR*)&UDPserveraddr, sizeof(SOCKADDR_IN)) < 0)
				;
			else
			{
				bDoRetry = false;
				break;
			}
			Sleep(100);
		}
	}

	if (bDoRetry)
	{
		MessageBoxA(NULL, "ERROR binding in the server socket", "", MB_OK);
		return -2;
	}
	*/

	m_bIsAlive = true;
	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, UDPClientThreadProc, this, 0, &threadID );

	m_ulStartTime = GetTickCount();

	CheckDefaultServerInfo();

	return 1;
}

void UDPClient::Close()
{
	m_bIsAlive = false;
	if(m_hThread)
	{
		::WaitForSingleObject(m_hThread,1000);
		m_hThread = 0;
	}

	if(m_ClientSocket)
	{
		::closesocket(m_ClientSocket);
		m_ClientSocket = 0;
	}
	m_bFinished = true;
}

void UDPClient::Recieve()
{
	char szMsg[2048];

	EnterCriticalSection(&m_CriticalSection);
	if(m_ClientSocket <= 0)
	{
		LeaveCriticalSection(&m_CriticalSection);
		return ;
	}

	fd_set fds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 600;

    FD_ZERO(&fds);
    FD_SET(m_ClientSocket, &fds);

    //int rc = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);
	int rc = select(m_ClientSocket + 1, &fds, NULL, NULL, &timeout);
    if(rc > 0)
    {
#if 0
        char rbuf[1024];
        SOCKADDR_IN clientaddr;
        int iAddrSize = sizeof(clientaddr);
		int iRet = recvfrom(m_ClientSocket,rbuf, 1024, 0, (sockaddr*)&clientaddr, &iAddrSize);
        if( iRet > 0)
        {   
            for(int i = 1024; i >= 1; i--)
            {
                if(rbuf[i] == '\n' && rbuf[i - 1] == '\r')
                {
                    rbuf[i-1] = '\0';
                    break;
                }
            }
            char *sz_ServerIP = inet_ntoa(clientaddr.sin_addr);
            int serverportno = ntohs(clientaddr.sin_port);
//            CString rData;
            
			if(strcmp(rbuf,BROADCAST_MSG) == 0)
			{
//				rData.Format("%s\r\nBroadcast Server: %s \r\n%s\r\n\r\n", (const char*)CTime::GetCurrentTime().Format("%B %d, %Y %H:%M:%S"), sz_ServerIP, rbuf);

				AddToRecieveList(sz_ServerIP);
/*
				char szMsg[1024];
				sprintf(szMsg,"%s \n",rData);
				OutputDebugString(szMsg);
				*/
			}
        }	
#else
		OutputDebugStringA("UDPClient::Recieve().. \n");

	//m_RecieveBuf
		SOCKADDR_IN clientaddr;
        int iAddrSize = sizeof(clientaddr);
		int iRet = recvfrom(m_ClientSocket,m_RecieveBuf, _BUF_SIZE, 0, (sockaddr*)&clientaddr, &iAddrSize);
        if( iRet > 0)
        {
			for(int i = _BUF_SIZE; i >= 1; i--)
            {
                if(m_RecieveBuf[i] == '\n' && m_RecieveBuf[i - 1] == '\r')
                {
                    m_RecieveBuf[i-1] = '\0';
                    break;
                }
            }	

			char *sz_ServerIP = inet_ntoa(clientaddr.sin_addr);
            int serverportno = ntohs(clientaddr.sin_port);

			/*
			if(strcmp(rbuf,BROADCAST_MSG) == 0)
			{
			}
			*/

			sprintf(szMsg,"%s \n",m_RecieveBuf);
			OutputDebugStringA(szMsg);

			char* pch = strstr(m_RecieveBuf,BROADCAST_MSG);
			if(pch)
			{
				int iMsgLen = strlen(BROADCAST_MSG);
				int iBufLen = strlen(m_RecieveBuf);

				if(iBufLen > iMsgLen + 2)
				{
					BCUDPServerInfo server_info;
					char szBroadcastMsg[100];
					sscanf(m_RecieveBuf,"%s %s %s %s %d",szBroadcastMsg,
						server_info.szServerVersion,
						server_info.szProtocolVersion,
						server_info.szIP,
						&server_info.iPort);

					
					sprintf(szMsg,"[Rev] s_ver:%s p_ver:%s ip:%s port:%d \n",server_info.szServerVersion,server_info.szProtocolVersion,server_info.szIP,server_info.iPort);
					OutputDebugStringA(szMsg);

					AddToRecieveList(&server_info);
				}
			}

		}
#endif
    }

	unsigned long ulCurTime = GetTickCount();
	if(ulCurTime >= m_ulStartTime + DURATION)
	{
		Close();
	}

	LeaveCriticalSection(&m_CriticalSection);
}
/*
void UDPClient::AddToRecieveList(char* szServerIP)
{
	for(int i = 0;i < m_pRecieveList->GetTotal();i++)
	{
		char* szCurItem = (char*)m_pRecieveList->Get(i);
		if(szCurItem)
		{
			if(strcmp(szCurItem,szServerIP) == 0)
			{
				return ;
			}
		}
	}

	char* szNewItem = new char[strlen(szServerIP) + 1];
	strcpy(szNewItem,szServerIP);
	m_pRecieveList->Add(szNewItem);
}
*/

void UDPClient::AddToRecieveList(BCUDPServerInfo* pServerInfo)
{
	char szMsg[1024];

	for(int i = 0;i < m_pRecieveList->GetTotal();i++)
	{
		BCUDPServerInfo* pCurItem = (BCUDPServerInfo*)m_pRecieveList->Get(i);
		if(pCurItem)
		{
			sprintf(szMsg,"pCurItem->szIP:%s pServerInfo->szIP:%s \n",pCurItem->szIP,pServerInfo->szIP);
			OutputDebugStringA(szMsg);

			if(strcmp(pCurItem->szIP,pServerInfo->szIP) == 0)
			{
				return ;
			}
		}
	}

	sprintf(szMsg,"[Add] s_ver:%s p_ver:%s ip:%s port:%d \n",pServerInfo->szServerVersion,pServerInfo->szProtocolVersion,pServerInfo->szIP,pServerInfo->iPort);
	OutputDebugStringA(szMsg);

	BCUDPServerInfo* szNewItem = new BCUDPServerInfo;
	memcpy(szNewItem,pServerInfo,sizeof(BCUDPServerInfo));
	m_pRecieveList->Add(szNewItem);
}

bool UDPClient::IsAlive()
{
	return m_bIsAlive;
}

void UDPClient::WaitForFinished()
{
	DWORD dwStart = 0;
	DWORD dwEnd = 0;
    bool bDo = true;

    while(bDo)
    {
		if(m_bFinished)
	        bDo = false;
		
        MSG Msg;
        if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE | PM_NOYIELD))
		{
            TranslateMessage(&Msg);			
            DispatchMessage(&Msg);				
		}
		Sleep(1);
    }
}

void* UDPClient::GetRecieveList()
{
	return m_pRecieveList;
}

void UDPClient::ResetRecieveList()
{
	if (m_pRecieveList)
	{
		int iTotal = m_pRecieveList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			BCUDPServerInfo* pCurItem = (BCUDPServerInfo*)m_pRecieveList->Get(i);
			if (pCurItem)
				delete pCurItem;
		}
//		delete m_pRecieveList;
		m_pRecieveList->Reset();
	}
}

void UDPClient::CheckDefaultServerInfo()
{
	char szAPPath[256];
	char szIniFileName[256];
	char szServerIP[100] = "";
	int iServerPort = 0;
	GetExecutionPath(NULL, szAPPath);

	sprintf(szIniFileName,"%s\\Setting.ini", szAPPath);

	GetPrivateProfileStringA("Network_Setting", "ServerIP", "", szServerIP, 100, szIniFileName);
	iServerPort = GetPrivateProfileIntA("Network_Setting", "ServerPort", 0, szIniFileName);

	if (strcmp(szServerIP, "") != 0)
	{
		BCUDPServerInfo server_info;
		server_info.iPort = iServerPort;
		strcpy(server_info.szIP, szServerIP);
		strcpy(server_info.szProtocolVersion,"1.0");
		strcpy(server_info.szServerVersion, "1.0");

		AddToRecieveList(&server_info);
	}

}

unsigned int __stdcall UDPClientThreadProc(void* lpvThreadParm)
{

	UDPClient* pNetObj = (UDPClient*)lpvThreadParm;
    while(pNetObj->IsAlive())
	{
		pNetObj->Recieve();
		Sleep(1);
	}

	return 0;
}
