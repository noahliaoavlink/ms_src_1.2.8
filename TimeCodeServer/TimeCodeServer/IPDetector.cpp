#include "stdafx.h"
#include "IPDetector.h"

#pragma comment(lib, "IPHLPAPI.lib")

IPDetector::IPDetector()
{
	m_iCount = 0;
}

IPDetector::~IPDetector()
{
}

void IPDetector::CheckIPAddress()
{
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	m_iCount = 0;

	if (dwStatus == ERROR_SUCCESS)
		;
	else
	{
		switch (dwStatus)
		{
		case ERROR_NO_DATA:
			break;
		case ERROR_INVALID_DATA:
			break;
		}
	}

	if (dwStatus == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		while (pAdapterInfo)
		{
			if (atoi(pAdapterInfo->IpAddressList.IpAddress.String) != 0)
			{
				strcpy(m_szIPAdressList[m_iCount], pAdapterInfo->IpAddressList.IpAddress.String);
				pAdapterInfo = pAdapterInfo->Next;
				m_iCount++;
			}
			else
				pAdapterInfo = pAdapterInfo->Next;
		}
	}
}

char* IPDetector::GetIPAddress(int iIndex)
{
	if (m_iCount > iIndex)
		return m_szIPAdressList[iIndex];
	return "";
}

int IPDetector::GetCount()
{
	return m_iCount;
}