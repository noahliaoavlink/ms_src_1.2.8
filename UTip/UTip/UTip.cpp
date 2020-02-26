// UTip.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UTip.h"

#include "UTooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CUTipApp

BEGIN_MESSAGE_MAP(CUTipApp, CWinApp)
END_MESSAGE_MAP()


// CUTipApp construction

CUTipApp::CUTipApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CUTipApp object

CUTipApp theApp;


// CUTipApp initialization

BOOL CUTipApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _UT_Create()
{
	return new UTooltip;
}

extern "C" __declspec(dllexport)void _UT_Destroy(void* pUTObj)
{
	delete (UTooltip*)pUTObj;
}

extern "C" __declspec(dllexport)void _UT_SetTextColor(void* pUTObj,BYTE ucR, BYTE ucG, BYTE ucB)
{
	((UTooltip*)pUTObj)->SetTextColor(ucR, ucG,ucB);
}

extern "C" __declspec(dllexport)void _UT_SetBKColor(void* pUTObj, BYTE ucR, BYTE ucG, BYTE ucB)
{
	((UTooltip*)pUTObj)->SetBKColor(ucR, ucG, ucB);
}

extern "C" __declspec(dllexport)void _UT_Init(void* pUTObj)
{
	((UTooltip*)pUTObj)->Init();
}

extern "C" __declspec(dllexport)void _UT_Add(void* pUTObj, HWND hWnd, _Region rect, wchar_t* wszMsg, unsigned long ulTipID)
{
	TipInfoW tip_info;
	tip_info.hWnd = hWnd;
	tip_info.rect.left = rect.left;
	tip_info.rect.top = rect.top;
	tip_info.rect.right = rect.right;
	tip_info.rect.bottom = rect.bottom;
	tip_info.ulTipID = ulTipID;

	wcscpy(tip_info.wszMsg, wszMsg);

	((UTooltip*)pUTObj)->Add(&tip_info);
}

extern "C" __declspec(dllexport)void _UT_Reset(void* pUTObj)
{
	((UTooltip*)pUTObj)->Reset();
}

extern "C" __declspec(dllexport)void _UT_ShowTip(void* pUTObj, HWND hWnd, int x, int y)
{
	((UTooltip*)pUTObj)->ShowTip(hWnd,x,y);
}
