// IOSourceCtrl.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IOSourceCtrl.h"

#include "IOSourceAgent.h"

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

// CIOSourceCtrlApp

BEGIN_MESSAGE_MAP(CIOSourceCtrlApp, CWinApp)
END_MESSAGE_MAP()


// CIOSourceCtrlApp construction

CIOSourceCtrlApp::CIOSourceCtrlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CIOSourceCtrlApp object

CIOSourceCtrlApp theApp;


// CIOSourceCtrlApp initialization

BOOL CIOSourceCtrlApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _IO_SA_Create()
{
	return new IOSourceAgent;
}

extern "C" __declspec(dllexport)void _IO_SA_Destroy(void* pIOSAObj)
{
	delete (IOSourceAgent*)pIOSAObj;
}

extern "C" __declspec(dllexport)void _IO_SA_Init(void* pIOSAObj)
{
	((IOSourceAgent*)pIOSAObj)->Init();
}

extern "C" __declspec(dllexport)int _IO_SA_AddMediaFile(void* pIOSAObj,wchar_t* wszFileName)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->Add(wszFileName);
}

extern "C" __declspec(dllexport)int _IO_SA_AddMediaFile2(void* pIOSAObj, wchar_t* wszFileName, bool IsEncrypted)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->Add(wszFileName, IsEncrypted);
}

extern "C" __declspec(dllexport)void _IO_SA_DeleteMediaFile(void* pIOSAObj, int iIndex)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->Delete(iIndex);
}

extern "C" __declspec(dllexport)int _IO_SA_GetFileFormatbyText(void* pIOSAObj, wchar_t* wszText)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetFileFormatbyText(wszText);
}

extern "C" __declspec(dllexport)int _IO_SA_GetTotalOfFileList(void* pIOSAObj, int iFileFormat)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetTotalOfFileList();
}

extern "C" __declspec(dllexport)wchar_t* _IO_SA_GetFileName(void* pIOSAObj, wchar_t* pLongPath)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetFileName(pLongPath);
}

extern "C" __declspec(dllexport)MediaFileInfo* _IO_SA_GetMediaFileInfo(void* pIOSAObj, int iFileFormat, int iIndex)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetMediaFileInfo(iFileFormat,iIndex);
}

extern "C" __declspec(dllexport)MediaFileInfo* _IO_SA_GetMediaFileInfobyFileName(void* pIOSAObj, wchar_t* wszFileName)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetMediaFileInfo(wszFileName);
}

extern "C" __declspec(dllexport)MediaFileInfo* _IO_SA_GetMediaFileInfobyID(void* pIOSAObj, unsigned long ulID)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetMediaFileInfobyID(ulID);
}

extern "C" __declspec(dllexport)void _IO_SA_SaveMediaFile(void* pIOSAObj)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->Save();
}

extern "C" __declspec(dllexport)ThumbnailPicture* _IO_SA_GetPicture(void* pIOSAObj,MediaFileInfo* pInfo)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetPicture(pInfo);
}

extern "C" __declspec(dllexport)void _IO_SA_DoTagSearch(void* pIOSAObj, wchar_t* wszInputKeyword)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->DoTagSearch(wszInputKeyword);
}

extern "C" __declspec(dllexport)void _IO_SA_SetIOSourceCtrlCallback(void* pIOSAObj, void* pObj)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->SetIOSourceCtrlCallback(pObj);
}

extern "C" __declspec(dllexport)void _IO_SA_Reset(void* pIOSAObj)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->Reset();
}

extern "C" __declspec(dllexport)int _IO_SA_AddRTSPUrl(void* pIOSAObj,wchar_t* wszUrl, double dLen)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->AddRTSPUrl(wszUrl, dLen);
}

extern "C" __declspec(dllexport)wchar_t* _IO_SA_GetShortUrl(void* pIOSAObj, wchar_t* wszUrl)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	return pMediaFileManager->GetShortUrl(wszUrl);
}

extern "C" __declspec(dllexport)void _IO_SA_DeleteFile(void* pIOSAObj, wchar_t* wszFileName)
{
	MediaFileManager* pMediaFileManager = ((IOSourceAgent*)pIOSAObj)->GetMediaFileManager();
	pMediaFileManager->Delete(wszFileName);
}

#ifdef _DISABLE_TASK_MANAGER
#else

//task manager
extern "C" __declspec(dllexport)void _IO_SA_AddTask(void* pIOSAObj, DisplayTask* pTask)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	pTaskManager->AddTask(pTask);
}

extern "C" __declspec(dllexport)void _IO_SA_RemoveTask(void* pIOSAObj, int iIndex)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	pTaskManager->RemoveTask(iIndex);
}

extern "C" __declspec(dllexport)int _IO_SA_GetTotalOfTasks(void* pIOSAObj)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	return pTaskManager->GetTotalOfTasks();
}

extern "C" __declspec(dllexport)DisplayTask* _IO_SA_GetTaskInfo(void* pIOSAObj,int iIndex)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	return pTaskManager->GetTaskInfo(iIndex);
}

extern "C" __declspec(dllexport)wchar_t* _IO_SA_GetLayoutText(void* pIOSAObj, int iLayoutID)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	return pTaskManager->GetLayoutText(iLayoutID);
}

extern "C" __declspec(dllexport)void _IO_SA_SaveTask(void* pIOSAObj)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	pTaskManager->Save();
}

extern "C" __declspec(dllexport)void _IO_SA_MakeGrid(void* pIOSAObj,int iLayoutIndex, wchar_t* wszFileName)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	pTaskManager->MakeGrid(iLayoutIndex,wszFileName);
}

extern "C" __declspec(dllexport)RECT* _IO_SA_GetGridRect(void* pIOSAObj, int iIndex)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	return pTaskManager->GetGridRect(iIndex);
}

extern "C" __declspec(dllexport)int _IO_SA_GetTotalOfMonitors(void* pIOSAObj)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	DisplayMonitor* pDisplayMonitor = pTaskManager->GetDisplayMonitor();
	return pDisplayMonitor->GetTotal();
}

extern "C" __declspec(dllexport)MonitorDeviceInfo* _IO_SA_GetMonitorDeviceInfo(void* pIOSAObj,int iIndex)
{
	TaskManager* pTaskManager = ((IOSourceAgent*)pIOSAObj)->GetTaskManager();
	DisplayMonitor* pDisplayMonitor = pTaskManager->GetDisplayMonitor();
	return pDisplayMonitor->GetMonitorDeviceInfo(iIndex);
}
#endif

#ifdef _DISABLE_WALL_PLAYER
#else
//vwall player
extern "C" __declspec(dllexport)void _IO_SA_ShowPlayers(void* pIOSAObj)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->Show();
}

extern "C" __declspec(dllexport)void _IO_SA_HidePlayers(void* pIOSAObj)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->Hide();
}

extern "C" __declspec(dllexport)void _IO_SA_Play(void* pIOSAObj,char* szFileName)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->Play(szFileName);
}

extern "C" __declspec(dllexport)void _IO_SA_Stop(void* pIOSAObj)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->Stop();
}

extern "C" __declspec(dllexport)void _IO_SA_SetDisplayTask(void* pIOSAObj, DisplayTask* pTask)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->SetDisplayTask(pTask);
}

extern "C" __declspec(dllexport)void _IO_SA_DoFadeOut(void* pIOSAObj)
{
	VWallPlayer* pVWallPlayer = ((IOSourceAgent*)pIOSAObj)->GetVWallPlayer();
	pVWallPlayer->DoFadeOut();
}
#endif
/*
void MediaFileManager::SetIOSourceCtrlCallback(void* pObj)
*/

