#pragma once

#ifndef _KPCheckerCtrl_H_
#define _KPCheckerCtrl_H_

//#include "NWin.h"

class KPCheckerCtrl
{
	/*
	PROCESS_INFORMATION pi;

	CNWin* m_pNWin;
	*/

	char m_szAPPath[512];
public:
	KPCheckerCtrl();
	~KPCheckerCtrl();

	void Launch();
	
protected:
public:

};

#endif