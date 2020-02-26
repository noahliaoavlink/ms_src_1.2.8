#include "stdafx.h"
#include "SimpleIMCP.h"

struct ICMPheader
{
	unsigned char	byType;
	unsigned char	byCode;
	unsigned short	nChecksum;
	unsigned short	nId;
	unsigned short	nSequence;
};

struct IPheader
{
	unsigned char	byVerLen;
	unsigned char	byTos;
	unsigned short	nTotalLength;
	unsigned short	nId;
	unsigned short	nOffset;
	unsigned char	byTtl;
	unsigned char	byProtocol;
	unsigned short	nChecksum;
	unsigned int	nSrcAddr;
	unsigned int	nDestAddr;
};

unsigned short CalcChecksum(char *pBuffer, int nLen)
{
	//Checksum for ICMP is calculated in the same way as for
	//IP header

	//This code was taken from: http://www.netfor2.com/ipsum.htm

	unsigned short nWord;
	unsigned int nSum = 0;
	int i;

	//Make 16 bit words out of every two adjacent 8 bit words in the packet
	//and add them up
	for (i = 0; i < nLen; i = i + 2)
	{
		nWord = ((pBuffer[i] << 8) & 0xFF00) + (pBuffer[i + 1] & 0xFF);
		nSum = nSum + (unsigned int)nWord;
	}

	//Take only 16 bits out of the 32 bit sum and add up the carries
	while (nSum >> 16)
	{
		nSum = (nSum & 0xFFFF) + (nSum >> 16);
	}

	//One's complement the result
	nSum = ~nSum;

	return ((unsigned short)nSum);
}

bool ValidateChecksum(char *pBuffer, int nLen)
{
	unsigned short nWord;
	unsigned int nSum = 0;
	int i;

	//Make 16 bit words out of every two adjacent 8 bit words in the packet
	//and add them up
	for (i = 0; i < nLen; i = i + 2)
	{
		nWord = ((pBuffer[i] << 8) & 0xFF00) + (pBuffer[i + 1] & 0xFF);
		nSum = nSum + (unsigned int)nWord;
	}

	//Take only 16 bits out of the 32 bit sum and add up the carries
	while (nSum >> 16)
	{
		nSum = (nSum & 0xFFFF) + (nSum >> 16);
	}

	//To validate the checksum on the received message we don't complement the sum
	//of one's complement
	//One's complement the result
	//nSum = ~nSum;

	//The sum of one's complement should be 0xFFFF
	return ((unsigned short)nSum == 0xFFFF);
}

SimpleIMCP::SimpleIMCP()
{
	nSequence = 0;
	Initialize();
}

SimpleIMCP::~SimpleIMCP()
{
	UnInitialize();
}

bool SimpleIMCP::Initialize()
{
	//Initialize WinSock
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR)
		return false;

	SYSTEMTIME time;
	::GetSystemTime(&time);

	//Seed the random number generator with current millisecond value
	srand(time.wMilliseconds);

	return true;
}

bool SimpleIMCP::UnInitialize()
{
	//Cleanup
	if (WSACleanup() == SOCKET_ERROR)
		return false;

	return true;
}

int SimpleIMCP::CreateSocket()
{
	//SOCKET sock;
	m_hSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);	//Create a raw socket which will use ICMP

	return 1;
}

void SimpleIMCP::Close()
{
	if (m_hSocket)
		closesocket(m_hSocket);
}

int SimpleIMCP::Ping(char* szIP)
{
	int iRet = 0;
	int nTimeOut = 5000;	//Request time out for echo request (in milliseconds)
	int nMessageSize = 32;	//The message size that the ICMP echo request should carry with it
	char *pszRemoteIP = NULL, *pSendBuffer = NULL, *pszRemoteHost = NULL;

	ICMPheader sendHdr;

	SOCKADDR_IN dest;	//Dest address to send the ICMP request
	dest.sin_addr.S_un.S_addr = inet_addr(szIP);
	dest.sin_family = AF_INET;
	dest.sin_port = rand();	//Pick a random port

	int nResult = 0;

	fd_set fdRead;
	SYSTEMTIME timeSend, timeRecv;
	int nPacketsSent = 0, nPacketsReceived = 0;

	timeval timeInterval = { 0, 0 };
	timeInterval.tv_usec = nTimeOut * 1000;

	sendHdr.nId = htons(rand());	//Set the transaction Id

	//Create the message buffer, which is big enough to store the header and the message data
	pSendBuffer = new char[sizeof(ICMPheader) + nMessageSize];

	sendHdr.byCode = 0;	//Zero for ICMP echo and reply messages
	sendHdr.nSequence = htons(nSequence++);
	sendHdr.byType = 8;	//Eight for ICMP echo message
	sendHdr.nChecksum = 0;	//Checksum is calculated later on

	memcpy_s(pSendBuffer, sizeof(ICMPheader), &sendHdr, sizeof(ICMPheader));	//Copy the message header in the buffer
	memset(pSendBuffer + sizeof(ICMPheader), 'x', nMessageSize);	//Fill the message with some arbitary value

																	//Calculate checksum over ICMP header and message data
	sendHdr.nChecksum = htons(CalcChecksum(pSendBuffer, sizeof(ICMPheader) + nMessageSize));

	//Copy the message header back into the buffer
	memcpy_s(pSendBuffer, sizeof(ICMPheader), &sendHdr, sizeof(ICMPheader));

	nResult = sendto(m_hSocket, pSendBuffer, sizeof(ICMPheader) + nMessageSize, 0, (SOCKADDR *)&dest, sizeof(SOCKADDR_IN));

	//Save the time at which the ICMP echo message was sent
	::GetSystemTime(&timeSend);

	++nPacketsSent;

	if (nResult == SOCKET_ERROR)
	{
		UnInitialize();
		delete[]pSendBuffer;
		return -1;
	}

	FD_ZERO(&fdRead);
	FD_SET(m_hSocket, &fdRead);

	if ((nResult = select(0, &fdRead, NULL, NULL, &timeInterval))
		== SOCKET_ERROR)
	{
		delete[]pSendBuffer;
		return -1;
	}

	if (nResult > 0 && FD_ISSET(m_hSocket, &fdRead))
	{
		//Allocate a large buffer to store the response
		char *pRecvBuffer = new char[1500];

		if ((nResult = recvfrom(m_hSocket, pRecvBuffer, 1500, 0, 0, 0))
			== SOCKET_ERROR)
		{
			UnInitialize();
			delete[]pSendBuffer;
			delete[]pRecvBuffer;
			return -1;
		}

		//Get the time at which response is received
		::GetSystemTime(&timeRecv);

		//We got a response so we construct the ICMP header and message out of it
		ICMPheader recvHdr;
		char *pICMPbuffer = NULL;

		//The response includes the IP header as well, so we move 20 bytes ahead to read the ICMP header
		pICMPbuffer = pRecvBuffer + sizeof(IPheader);

		//ICMP message length is calculated by subtracting the IP header size from the 
		//total bytes received
		int nICMPMsgLen = nResult - sizeof(IPheader);

		//Construct the ICMP header
		memcpy_s(&recvHdr, sizeof(recvHdr), pICMPbuffer, sizeof(recvHdr));

		//Construct the IP header from the response
		IPheader ipHdr;
		memcpy_s(&ipHdr, sizeof(ipHdr), pRecvBuffer, sizeof(ipHdr));

		recvHdr.nId = recvHdr.nId;
		recvHdr.nSequence = recvHdr.nSequence;
		recvHdr.nChecksum = ntohs(recvHdr.nChecksum);

		//Check if the response is an echo reply, transaction ID and sequence number are same
		//as for the request, and that the checksum is correct
		if (recvHdr.byType == 0 &&
			recvHdr.nId == sendHdr.nId &&
			recvHdr.nSequence == sendHdr.nSequence &&
			ValidateChecksum(pICMPbuffer, nICMPMsgLen) &&
			memcmp(pSendBuffer + sizeof(ICMPheader), pRecvBuffer + sizeof(ICMPheader) + sizeof(IPheader),
				nResult - sizeof(ICMPheader) - sizeof(IPheader)) == 0)
		{
			//All's OK
			int nSec = timeRecv.wSecond - timeSend.wSecond;
			if (nSec < 0)
			{
				nSec = nSec + 60;
			}

			int nMilliSec = abs(timeRecv.wMilliseconds - timeSend.wMilliseconds);

			nRoundTripTime = 0;
			nRoundTripTime = abs(nSec * 1000 - nMilliSec);

			iRet = 1;
			++nPacketsReceived;
		}
		else
			iRet = -2;

		delete[]pRecvBuffer;
	}
	else
		iRet = -3;

	delete[]pSendBuffer;

	return iRet;
}

int SimpleIMCP::GetRoundTripTime()
{
	return nRoundTripTime;
}