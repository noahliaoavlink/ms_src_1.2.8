#pragma once

#ifndef _PacketMaker_H_
#define _PacketMaker_H_

#include "..\..\Include\NetCommon.h"

class PacketMaker
{
	unsigned char* m_pBuffer;
	int m_iLen;
	unsigned char m_ucPacketType;
protected:
public:
	PacketMaker();
	~PacketMaker();

	void Make(long lCmdID);
	void Make(long lCmdID, long lData);
	void Make(long lCmdID, long lData1, long lData2);
	void Make(long lCmdID, unsigned char* pBuffer, int iLen);
	void Make(long lCmdID, unsigned char* pBuffer1, int iLen1, unsigned char* pBuffer2, int iLen2);

	unsigned char* GetBuffer();
	int GetBufLen();
};

#endif