#include "stdafx.h"
#include "PacketMaker.h"

PacketMaker::PacketMaker()
{
	m_pBuffer = new unsigned char[_MAX_BUFFER_LEN];
	m_iLen = 0;
}

PacketMaker::~PacketMaker()
{
	if (m_pBuffer)
		delete m_pBuffer;
}

void PacketMaker::Make(long lCmdID)
{
	int iCount = 0;
	m_ucPacketType = PT_CMD_ONLY;

	memcpy(m_pBuffer,"<NP",strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);
	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}

void PacketMaker::Make(long lCmdID,long lData)
{
	int iCount = 0;
	m_ucPacketType = PT_SIMPLE;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}

void PacketMaker::Make(long lCmdID, long lData1, long lData2)
{
	int iCount = 0;
	m_ucPacketType = PT_DUAL_LONG;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData1, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData2, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}

void PacketMaker::Make(long lCmdID, unsigned char* pBuffer,int iLen)
{
	int iCount = 0;
	m_ucPacketType = PT_STRING;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &iLen, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer, iLen);
	iCount += iLen;

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}

void PacketMaker::Make(long lCmdID, unsigned char* pBuffer1, int iLen1, unsigned char* pBuffer2, int iLen2)
{
	int iCount = 0;
	m_ucPacketType = PT_STRING2;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &iLen1, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer1, iLen1);
	iCount += iLen1;

	memcpy(m_pBuffer + iCount, &iLen2, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer2, iLen2);
	iCount += iLen2;

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}

unsigned char* PacketMaker::GetBuffer()
{
	return m_pBuffer;
}

int PacketMaker::GetBufLen()
{
	return m_iLen;
}