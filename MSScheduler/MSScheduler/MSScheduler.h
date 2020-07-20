
// MSScheduler.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMSSchedulerApp:
// See MSScheduler.cpp for the implementation of this class
//

class CMSSchedulerApp : public CWinApp
{
public:
	CMSSchedulerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMSSchedulerApp theApp;