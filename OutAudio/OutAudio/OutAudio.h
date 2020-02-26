// OutAudio.h : main header file for the OutAudio DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COutAudioApp
// See OutAudio.cpp for the implementation of this class
//

class COutAudioApp : public CWinApp
{
public:
	COutAudioApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
