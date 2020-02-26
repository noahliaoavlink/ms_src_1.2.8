#include "stdafx.h"
#include "Server.h"

DWORD WINAPI RecvThread(LPVOID lpParam);

#ifdef _ENABLE_MSG_Q
DWORD WINAPI SendMsgThread(LPVOID lpParam);
#endif

Server::Server()
{
	m_hSocket = 0;

	m_pRcvBuffer = new char[_MAX_RCV_BUFFER];
	m_pSendBuffer = new char[_MAX_SEND_BUFFER];
	m_pBuffer = new char[_MAX_RCV_BUFFER];

	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(5);
	m_pQBuffer->SetBufferSize(_MAX_RCV_BUFFER);

	m_pPacketParser = new PacketParser;
	m_bIsPassed = false;

	m_bDoSynPB = false;
	m_ulCurFrameTime = 0;
	m_ulLastFrameTime = 0;

#ifdef _ENABLE_MSG_Q
	m_pMsgQueue = new SQQueue;
	m_pMsgQueue->Alloc(sizeof(MsgItem), 200);

	InitializeCriticalSection(&m_MsgQCriticalSection);
#endif

	m_hRecvThread = 0;
	InitializeCriticalSection(&m_CriticalSection);
}

Server::~Server()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hRecvThread,10000);

#ifdef _ENABLE_MSG_Q
	::WaitForSingleObject(m_hSendMsgThread, 10000);
#endif

	delete m_pRcvBuffer;
	delete m_pSendBuffer;
	delete m_pBuffer;
//	delete m_pCmdParser;
	delete m_pQBuffer;

	if (m_pPacketParser)
		delete m_pPacketParser;

#ifdef _ENABLE_MSG_Q
	if(m_pMsgQueue)
		delete m_pMsgQueue;
	DeleteCriticalSection(&m_MsgQCriticalSection);
#endif

	DeleteCriticalSection(&m_CriticalSection);
}

void Server::Init()
{
	m_bIsAlive = true;
	DWORD nThreadID;
	m_hRecvThread = CreateThread(0, 0, RecvThread, (void *)this, 0, &nThreadID);

#ifdef _ENABLE_MSG_Q
	m_hSendMsgThread = CreateThread(0, 0, SendMsgThread, (void *)this, 0, &nThreadID);
#endif
}

bool Server::IsAlive()
{
	return m_bIsAlive;
}

void Server::SetSocket(SOCKET hSocket)
{
	m_hSocket = hSocket;
}

SOCKET Server::GetSocket()
{
	return m_hSocket;
}

void Server::Close()
{
	char szMsg[1024];
	sprintf(szMsg,"Server::Close()\n");
	OutputDebugStringA(szMsg);

	//extern CTest_IPC_ServerDlg* g_pDlg;
	//g_pDlg->AddToList(szMsg);

	if(m_hSocket)
		closesocket(m_hSocket);
	m_hSocket = 0;
}

void Server::Receive()
{
	ZeroMemory(m_pRcvBuffer,_MAX_RCV_BUFFER);
	int iLen = recv(m_hSocket,m_pRcvBuffer,_MAX_RCV_BUFFER,0);

	if(iLen > 0)
		bool bRet = m_pQBuffer->Add(m_pRcvBuffer,iLen);
}

void Server::ProcessRecvData()
{
	EnterCriticalSection(&m_CriticalSection);
	if(m_pQBuffer->GetTotal() > 0)
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
				PacketItem* pCurPacketItem = m_pPacketParser->GetPacketItem();
				if (pCurPacketItem)
				{
					switch (pCurPacketItem->lID)
					{
						case CID_LOGIN:
							//Login((char*)pCurPacketItem->pData, (char*)pCurPacketItem->pData2);
							Login((char*)pCurPacketItem->szString, (char*)pCurPacketItem->szString2);
							break;
						case CID_LOGOUT:
							break;
					}
				}
			}
		}
	}

	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

#ifdef _ENABLE_MSG_Q
void Server::SendPacket()
{
	EnterCriticalSection(&m_MsgQCriticalSection);
	if (m_pMsgQueue->GetTotal() > 0)
	{
		MsgItem* pCurPacketItem = (MsgItem*)m_pMsgQueue->GetNext();
		if (pCurPacketItem)
		{
			PacketMaker packet_maker;
			switch (pCurPacketItem->lCmd)
			{
				case CID_LOGIN_RET:
				case CID_DO_SYN_PB:
					{
						packet_maker.Make(pCurPacketItem->lCmd, pCurPacketItem->lParm);
						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
					}
					break;
				case CID_UPDATE_TIME_CODE:
					{
						packet_maker.Make(pCurPacketItem->lCmd, pCurPacketItem->lParm);
						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
					}
					break;
				default:
					{
						packet_maker.Make(pCurPacketItem->lCmd);
						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
					}
					break;
			}
		}
	}
	Sleep(1);
	LeaveCriticalSection(&m_MsgQCriticalSection);
}
#endif

void Server::Hello()
{
#ifdef _ENABLE_MSG_Q
	MsgItem msg_item;

	msg_item.lCmd = CID_HELLO;
	msg_item.lParm = -1;

	m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
	char szMsg[512];
	PacketMaker packet_maker;
	packet_maker.Make(CID_HELLO);
	int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

	sprintf(szMsg, "TCS Server::Hello:  %d\n", iRet);
	OutputDebugStringA(szMsg);
#endif
}

//201
void Server::Login(char* szID, char* szPW)
{
#ifdef _ENABLE_MSG_Q
	MsgItem msg_item;

	if (strcmp(szID, "bruvis") == 0 && strcmp(szPW, "avlink") == 0)
	{
//		packet_maker.Make(CID_LOGIN_RET, LR_SUCESSED);

		msg_item.lCmd = CID_LOGIN_RET;
		msg_item.lParm = LR_SUCESSED;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));

		m_bIsPassed = true;
	}
	else
	{
//		packet_maker.Make(CID_LOGIN_RET, LR_FAILED);

		msg_item.lCmd = CID_LOGIN_RET;
		msg_item.lParm = LR_FAILED;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));

		m_bIsPassed = false;
	}
#else
	PacketMaker packet_maker;

	if (strcmp(szID, "bruvis") == 0 && strcmp(szPW, "avlink") == 0)
	{
		packet_maker.Make(CID_LOGIN_RET, LR_SUCESSED);
		m_bIsPassed = true;
	}
	else
	{
		packet_maker.Make(CID_LOGIN_RET, LR_FAILED);
		m_bIsPassed = false;
	}

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
}

void Server::TC_Start()
{
	if (m_bIsPassed)
	{
#ifdef _ENABLE_MSG_Q
		MsgItem msg_item;
		msg_item.lCmd = CID_START;
		msg_item.lParm = -1;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
		PacketMaker packet_maker;
		packet_maker.Make(CID_START);
		send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
	}
}

void Server::TC_Continue()
{
	if (m_bIsPassed)
	{
#ifdef _ENABLE_MSG_Q
		MsgItem msg_item;
		msg_item.lCmd = CID_CONTINUE;
		msg_item.lParm = -1;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
		PacketMaker packet_maker;
		packet_maker.Make(CID_CONTINUE);
		send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
	}
}

void Server::TC_Stop()
{
	if (m_bIsPassed)
	{
#ifdef _ENABLE_MSG_Q
		MsgItem msg_item;
		msg_item.lCmd = CID_STOP;
		msg_item.lParm = -1;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
		PacketMaker packet_maker;
		packet_maker.Make(CID_STOP);
		send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
	}
}

void Server::TC_Pause()
{
	if (m_bIsPassed)
	{
#ifdef _ENABLE_MSG_Q
		MsgItem msg_item;
		msg_item.lCmd = CID_PAUSE;
		msg_item.lParm = -1;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
		PacketMaker packet_maker;
		packet_maker.Make(CID_PAUSE);
		send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
	}
}

void Server::TC_UpdateTimeCode(long lTimeCode)
{
	char szMsg[512];
	if (m_bIsPassed)
	{
		PacketMaker packet_maker;

		if (m_bDoSynPB)
		{
			m_bDoSynPB = false;
#ifdef _ENABLE_MSG_Q
			MsgItem msg_item;
			msg_item.lCmd = CID_DO_SYN_PB;
			msg_item.lParm = lTimeCode;

			//m_pMsgQueue->Reset();

			m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
			packet_maker.Make(CID_DO_SYN_PB, lTimeCode);
			send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

			sprintf(szMsg, "TCS Server::TC_UpdateTimeCode: CID_DO_SYN_PB %d\n", lTimeCode);
			OutputDebugStringA(szMsg);
#endif
		}
		else
		{
			bool bDoSynPB = false;
			if (m_ulLastFrameTime > lTimeCode)
			{
				bDoSynPB = true;
				m_ulLastFrameTime = lTimeCode;
			}
			
			if(bDoSynPB)
			{
#ifdef _ENABLE_MSG_Q
				MsgItem msg_item;
				msg_item.lCmd = CID_DO_SYN_PB;
				msg_item.lParm = lTimeCode;

				//m_pMsgQueue->Reset();

				m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
				PacketMaker packet_maker1;

				packet_maker1.Make(CID_DO_SYN_PB, lTimeCode);
				send(m_hSocket, (char*)packet_maker1.GetBuffer(), packet_maker1.GetBufLen(), 0);

				sprintf(szMsg, "TCS Server::TC_UpdateTimeCode: CID_DO_SYN_PB %d\n", lTimeCode);
				OutputDebugStringA(szMsg);
#endif
			}
			
#ifdef _ENABLE_MSG_Q
			MsgItem msg_item;
			msg_item.lCmd = CID_UPDATE_TIME_CODE;
			msg_item.lParm = lTimeCode;

			m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
			packet_maker.Make(CID_UPDATE_TIME_CODE, lTimeCode, m_ulCurFrameTime);
			send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif

	//		sprintf(szMsg, "TCS Server::TC_UpdateTimeCode: CID_UPDATE_TIME_CODE %d\n", lTimeCode);
	//		OutputDebugStringA(szMsg);

			m_ulLastFrameTime = lTimeCode;
		}
	}
}

void Server::EnableDoSynPB(bool bEnable)
{
	m_bDoSynPB = bEnable;
}

void Server::UpdateCurFrameTime(unsigned long ulTime)
{
	m_ulCurFrameTime = ulTime;
}

void Server::Test()
{
}

DWORD WINAPI RecvThread(LPVOID lpParam)
{    
	Server* pObj = (Server*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->ProcessRecvData();
	}
	return 0;
}

#ifdef _ENABLE_MSG_Q
DWORD WINAPI SendMsgThread(LPVOID lpParam)
{
	Server* pObj = (Server*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->SendPacket();
	}
	return 0;
}
#endif
