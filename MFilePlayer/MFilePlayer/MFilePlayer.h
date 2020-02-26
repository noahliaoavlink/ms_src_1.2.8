// MFilePlayer.h : main header file for the MFilePlayer DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFilePlayerApp
// See MFilePlayer.cpp for the implementation of this class
//

class CMFilePlayerApp : public CWinApp
{
public:
	CMFilePlayerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
