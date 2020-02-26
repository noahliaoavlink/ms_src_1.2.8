// IOSourceCtrl.h : main header file for the IOSourceCtrl DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIOSourceCtrlApp
// See IOSourceCtrl.cpp for the implementation of this class
//

class CIOSourceCtrlApp : public CWinApp
{
public:
	CIOSourceCtrlApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
