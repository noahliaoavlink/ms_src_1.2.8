// TCClient.h : main header file for the TCClient DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTCClientApp
// See TCClient.cpp for the implementation of this class
//

class CTCClientApp : public CWinApp
{
public:
	CTCClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
