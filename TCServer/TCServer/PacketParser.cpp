#include "stdafx.h"
#include "PacketParser.h"

PacketParser::PacketParser()
{
	m_pTmpBuffer = new unsigned char[_MAX_BUFFER_LEN * 2];
	m_pTmpBuffer2 = new unsigned char[_MAX_BUFFER_LEN * 2];
	m_pDataBuffer = new unsigned char[_MAX_BUFFER_LEN];
	m_pDataBuffer2 = new unsigned char[_MAX_BUFFER_LEN];
	m_iTmpBufLen = 0;
}

PacketParser::~PacketParser()
{
	if (m_pTmpBuffer)
		delete m_pTmpBuffer;

	if (m_pTmpBuffer2)
		delete m_pTmpBuffer2;

	if (m_pDataBuffer)
		delete m_pDataBuffer;

	if (m_pDataBuffer2)
		delete m_pDataBuffer2;
}

void PacketParser::Input(unsigned char* pBuffer, int iLen)
{
	memcpy(m_pTmpBuffer + m_iTmpBufLen, pBuffer, iLen);
	m_iTmpBufLen += iLen;
}

int PacketParser::Parse()
{
	char szMsg[512];
	int iRet = -1;
	int iCount = 0;

	if (m_iTmpBufLen == 0)
		return -1;

	bool bHasHeader = CheckPacketHeader(m_pTmpBuffer, m_iTmpBufLen);
	if (bHasHeader)
	{
		iRet = 0;

		iCount += 3;
		m_PacketItem.ucPacketType = m_pTmpBuffer[iCount];
		iCount += sizeof(unsigned char);
		memcpy(&m_PacketItem.lID, m_pTmpBuffer + iCount, sizeof(long));
		iCount += sizeof(long);

		switch (m_PacketItem.ucPacketType)
		{
			case PT_CMD_ONLY:
				break;
			case PT_SIMPLE:
				memcpy(&m_PacketItem.lData, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				break;
			case PT_DUAL_LONG:
				memcpy(&m_PacketItem.lData, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData2, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				break;
			case PT_STRING:
				memcpy(&m_PacketItem.iLen, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, m_pTmpBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);
				break;
			case PT_STRING2:
				memcpy(&m_PacketItem.iLen, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, m_pTmpBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);

				memcpy(&m_PacketItem.iLen2, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer2, m_pTmpBuffer + iCount, m_PacketItem.iLen2);
				m_pDataBuffer2[m_PacketItem.iLen2] = '\0';
				iCount += m_PacketItem.iLen2;

				//m_PacketItem.pData2 = m_pDataBuffer2;
				memcpy(m_PacketItem.szString2, m_pDataBuffer2, m_PacketItem.iLen2 + 1);
				break;
		}

		bool bHasTailer = CheckPacketTailer(m_pTmpBuffer + iCount, m_iTmpBufLen - iCount);

		if (bHasTailer)
		{
			iCount += 3;

			int iNewLen = m_iTmpBufLen - iCount;

			memcpy(m_pTmpBuffer2, m_pTmpBuffer + iCount, iNewLen);
			memcpy(m_pTmpBuffer, m_pTmpBuffer2, iNewLen);
			m_iTmpBufLen = iNewLen;

			iRet = 1;
		}
		else
		{
			sprintf(szMsg, "TC PacketParser::Parse() - lose Tailer!!\n");
			OutputDebugStringA(szMsg);
		}
	}
	else
	{
		sprintf(szMsg, "TC PacketParser::Parse() - no Header!!\n");
		OutputDebugStringA(szMsg);
	}

	return iRet;
}

PacketItem* PacketParser::GetPacketItem()
{
	return &m_PacketItem;
}

bool PacketParser::CheckPacketHeader(unsigned char* pBuffer, int iLen)
{
	if (iLen >= 3)
	{
		if (pBuffer[0] == '<' && pBuffer[1] == 'N' && pBuffer[2] == 'P')
			return true;
	}
	return false;
}

bool PacketParser::CheckPacketTailer(unsigned char* pBuffer, int iLen)
{
	if (iLen >= 3)
	{
		if (pBuffer[0] == 'N' && pBuffer[1] == 'P' && pBuffer[2] == '>')
			return true;
	}
	return false;
}
