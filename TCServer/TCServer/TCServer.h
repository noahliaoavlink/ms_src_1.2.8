// TCServer.h : main header file for the TCServer DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTCServerApp
// See TCServer.cpp for the implementation of this class
//

class CTCServerApp : public CWinApp
{
public:
	CTCServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
