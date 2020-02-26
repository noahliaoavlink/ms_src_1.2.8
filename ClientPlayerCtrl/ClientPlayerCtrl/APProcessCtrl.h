#pragma once

#ifndef _APProcessCtrl_H
#define _APProcessCtrl_H

class APProcessCtrl
{
	PROCESS_INFORMATION pi;

	/*
	int m_iIndex;
	char m_szName[128];
	DWORD m_dwStartTime;
	bool m_bIsReady;

	int m_iDisappearTime;
	int m_iDisappearWaitTime;
	*/
public:
	APProcessCtrl();
	~APProcessCtrl();

	void Launch(char* szLayoutStr, int iIndex);
	void Close();
};

#endif