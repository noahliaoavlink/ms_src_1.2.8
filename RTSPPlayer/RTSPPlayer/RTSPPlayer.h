// RTSPPlayer.h : main header file for the RTSPPlayer DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRTSPPlayerApp
// See RTSPPlayer.cpp for the implementation of this class
//

class CRTSPPlayerApp : public CWinApp
{
public:
	CRTSPPlayerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
