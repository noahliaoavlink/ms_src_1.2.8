// ZLibDll.h : main header file for the ZLibDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZLibDllApp
// See ZLibDll.cpp for the implementation of this class
//

class CZLibDllApp : public CWinApp
{
public:
	CZLibDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
