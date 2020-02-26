// MFRecorder.h : main header file for the MFRecorder DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFRecorderApp
// See MFRecorder.cpp for the implementation of this class
//

class CMFRecorderApp : public CWinApp
{
public:
	CMFRecorderApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
