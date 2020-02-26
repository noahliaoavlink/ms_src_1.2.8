// OutXAudio2.h : main header file for the OutXAudio2 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COutXAudio2App
// See OutXAudio2.cpp for the implementation of this class
//

class COutXAudio2App : public CWinApp
{
public:
	COutXAudio2App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
