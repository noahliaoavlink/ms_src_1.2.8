#pragma once

#ifndef _IPDetector_H_
#define _IPDetector_H_

class IPDetector
{
	int m_iCount;
	char m_szIPAdressList[16][32];
protected:
public:
	IPDetector();
	~IPDetector();

	void CheckIPAddress();
	char* GetIPAddress(int iIndex);
	int GetCount();
};

#endif