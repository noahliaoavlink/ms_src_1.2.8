#include "stdafx.h"
#include "IOSourceCtrlDll.h"
#include "../../../Include/StrConv.h"

#ifdef _DISABLE_PLANE_SCENE3D
#else
#include "PlaneScene3D.h"
#endif

IOSourceCtrlDll::IOSourceCtrlDll()
{
	m_hInst = 0;
	m_pIOSAObj = 0;

	m_pLogFileDll = 0;
	m_pLogFileObj = 0;
}

IOSourceCtrlDll::~IOSourceCtrlDll()
{
	if (m_pIOSAObj)
		_IO_SA_Destroy(m_pIOSAObj);

	FreeLib();
}

bool IOSourceCtrlDll::LoadLib()
{
	char szFileName[] = "IOSourceCtrl.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_IO_SA_Create = (_IO_SA_Create_Proc)GetProcAddress(m_hInst, "_IO_SA_Create");
		_IO_SA_Destroy = (_IO_SA_Destroy_Proc)GetProcAddress(m_hInst, "_IO_SA_Destroy");
		_IO_SA_Init = (_IO_SA_Init_Proc)GetProcAddress(m_hInst, "_IO_SA_Init");
		_IO_SA_AddMediaFile = (_IO_SA_AddMediaFile_Proc)GetProcAddress(m_hInst, "_IO_SA_AddMediaFile");
		_IO_SA_DeleteMediaFile = (_IO_SA_DeleteMediaFile_Proc)GetProcAddress(m_hInst, "_IO_SA_DeleteMediaFile");
		_IO_SA_GetFileFormatbyText = (_IO_SA_GetFileFormatbyText_Proc)GetProcAddress(m_hInst, "_IO_SA_GetFileFormatbyText");
		_IO_SA_GetTotalOfFileList = (_IO_SA_GetTotalOfFileList_Proc)GetProcAddress(m_hInst, "_IO_SA_GetTotalOfFileList");
		_IO_SA_GetFileName = (_IO_SA_GetFileName_Proc)GetProcAddress(m_hInst, "_IO_SA_GetFileName");
		_IO_SA_GetMediaFileInfo = (_IO_SA_GetMediaFileInfo_Proc)GetProcAddress(m_hInst, "_IO_SA_GetMediaFileInfo");
		_IO_SA_GetMediaFileInfobyFileName = (_IO_SA_GetMediaFileInfobyFileName_Proc)GetProcAddress(m_hInst, "_IO_SA_GetMediaFileInfobyFileName");
		_IO_SA_GetMediaFileInfobyID = (_IO_SA_GetMediaFileInfobyID_Proc)GetProcAddress(m_hInst, "_IO_SA_GetMediaFileInfobyID");

		_IO_SA_SaveMediaFile = (_IO_SA_SaveMediaFile_Proc)GetProcAddress(m_hInst, "_IO_SA_SaveMediaFile");
		_IO_SA_GetPicture = (_IO_SA_GetPicture_Proc)GetProcAddress(m_hInst, "_IO_SA_GetPicture");
		_IO_SA_DoTagSearch = (_IO_SA_DoTagSearch_Proc)GetProcAddress(m_hInst, "_IO_SA_DoTagSearch");
		_IO_SA_SetIOSourceCtrlCallback = (_IO_SA_SetIOSourceCtrlCallback_Proc)GetProcAddress(m_hInst, "_IO_SA_SetIOSourceCtrlCallback");
		_IO_SA_AddMediaFile2 = (_IO_SA_AddMediaFile2_Proc)GetProcAddress(m_hInst, "_IO_SA_AddMediaFile2");
		_IO_SA_Reset = (_IO_SA_Reset_Proc)GetProcAddress(m_hInst, "_IO_SA_Reset");
		_IO_SA_AddRTSPUrl = (_IO_SA_AddRTSPUrl_Proc)GetProcAddress(m_hInst, "_IO_SA_AddRTSPUrl");
		_IO_SA_GetShortUrl = (_IO_SA_GetShortUrl_Proc)GetProcAddress(m_hInst, "_IO_SA_GetShortUrl");

		_IO_SA_DeleteFile = (_IO_SA_DeleteFile_Proc)GetProcAddress(m_hInst, "_IO_SA_DeleteFile");


		/*
		_IO_SA_AddTask = (_IO_SA_AddTask_Proc)GetProcAddress(m_hInst, "_IO_SA_AddTask");
		_IO_SA_RemoveTask = (_IO_SA_RemoveTask_Proc)GetProcAddress(m_hInst, "_IO_SA_RemoveTask");
		_IO_SA_GetTotalOfTasks = (_IO_SA_GetTotalOfTasks_Proc)GetProcAddress(m_hInst, "_IO_SA_GetTotalOfTasks");
		_IO_SA_GetTaskInfo = (_IO_SA_GetTaskInfo_Proc)GetProcAddress(m_hInst, "_IO_SA_GetTaskInfo");
		_IO_SA_GetLayoutText = (_IO_SA_GetLayoutText_Proc)GetProcAddress(m_hInst, "_IO_SA_GetLayoutText");
		_IO_SA_SaveTask = (_IO_SA_SaveTask_Proc)GetProcAddress(m_hInst, "_IO_SA_SaveTask");
		_IO_SA_MakeGrid = (_IO_SA_MakeGrid_Proc)GetProcAddress(m_hInst, "_IO_SA_MakeGrid");
		_IO_SA_GetGridRect = (_IO_SA_GetGridRect_Proc)GetProcAddress(m_hInst, "_IO_SA_GetGridRect");
		_IO_SA_GetTotalOfMonitors = (_IO_SA_GetTotalOfMonitors_Proc)GetProcAddress(m_hInst, "_IO_SA_GetTotalOfMonitors");
		_IO_SA_GetMonitorDeviceInfo = (_IO_SA_GetMonitorDeviceInfo_Proc)GetProcAddress(m_hInst, "_IO_SA_GetMonitorDeviceInfo");

		_IO_SA_ShowPlayers = (_IO_SA_ShowPlayers_Proc)GetProcAddress(m_hInst, "_IO_SA_ShowPlayers");
		_IO_SA_HidePlayers = (_IO_SA_HidePlayers_Proc)GetProcAddress(m_hInst, "_IO_SA_HidePlayers");
		_IO_SA_Play = (_IO_SA_Play_Proc)GetProcAddress(m_hInst, "_IO_SA_Play");
		_IO_SA_Stop = (_IO_SA_Stop_Proc)GetProcAddress(m_hInst, "_IO_SA_Stop");
		_IO_SA_SetDisplayTask = (_IO_SA_SetDisplayTask_Proc)GetProcAddress(m_hInst, "_IO_SA_SetDisplayTask");
		_IO_SA_DoFadeOut = (_IO_SA_DoFadeOut_Proc)GetProcAddress(m_hInst, "_IO_SA_DoFadeOut");
		*/


		m_pIOSAObj = _IO_SA_Create();

		SetIOSourceCtrlCallback(this);
		return true;
	}
	return false;
}

void IOSourceCtrlDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void IOSourceCtrlDll::Init()
{
	if (m_pIOSAObj)
		_IO_SA_Init(m_pIOSAObj);
}

int IOSourceCtrlDll::AddMediaFile(wchar_t* wszFileName)
{
	if (m_pIOSAObj)
		return _IO_SA_AddMediaFile(m_pIOSAObj, wszFileName);
	return 0;
}

void IOSourceCtrlDll::DeleteMediaFile(int iIndex)
{
	if (m_pIOSAObj)
		_IO_SA_DeleteMediaFile(m_pIOSAObj, iIndex);
}

void IOSourceCtrlDll::DeleteFile(wchar_t* wszFileName)
{
	if (m_pIOSAObj)
		_IO_SA_DeleteFile(m_pIOSAObj, wszFileName);
}

int IOSourceCtrlDll::GetFileFormatbyText(wchar_t* wszText)
{
	if (m_pIOSAObj)
		return _IO_SA_GetFileFormatbyText(m_pIOSAObj, wszText);
}

int IOSourceCtrlDll::GetTotalOfFileList(int iFileFormat)
{
	if (m_pIOSAObj)
		return _IO_SA_GetTotalOfFileList(m_pIOSAObj, iFileFormat);
}

wchar_t* IOSourceCtrlDll::GetFileName(wchar_t* pLongPath)
{
	if (m_pIOSAObj)
		return _IO_SA_GetFileName(m_pIOSAObj, pLongPath);
}

MediaFileInfo* IOSourceCtrlDll::GetMediaFileInfo(int iFileFormat, int iIndex)
{
	if (m_pIOSAObj)
		return _IO_SA_GetMediaFileInfo(m_pIOSAObj, iFileFormat, iIndex);
}

MediaFileInfo* IOSourceCtrlDll::GetMediaFileInfo(wchar_t* wszFilename)
{
	if (m_pIOSAObj)
		return _IO_SA_GetMediaFileInfobyFileName(m_pIOSAObj, wszFilename);
}

MediaFileInfo* IOSourceCtrlDll::GetMediaFileInfo(int iIndex)
{
	return GetMediaFileInfo(MFF_ALL, iIndex);
}

MediaFileInfo* IOSourceCtrlDll::GetMediaFileInfobyID(unsigned long ulID)
{
	if (m_pIOSAObj)
		return _IO_SA_GetMediaFileInfobyID(m_pIOSAObj, ulID);
}

void IOSourceCtrlDll::SaveMediaFile()
{
	if (m_pIOSAObj)
		_IO_SA_SaveMediaFile(m_pIOSAObj);
}

ThumbnailPicture* IOSourceCtrlDll::GetPicture(MediaFileInfo* pInfo)
{
	if (m_pIOSAObj)
		return _IO_SA_GetPicture(m_pIOSAObj, pInfo);
}

void IOSourceCtrlDll::DoTagSearch(wchar_t* wszInputKeyword)
{
	if (m_pIOSAObj)
		_IO_SA_DoTagSearch(m_pIOSAObj, wszInputKeyword);
}

void IOSourceCtrlDll::Reset()
{
	if (m_pIOSAObj)
		_IO_SA_Reset(m_pIOSAObj);
}

/*
void IOSourceCtrlDll::AddTask(DisplayTask* pTask)
{
	if (m_pIOSAObj)
		_IO_SA_AddTask(m_pIOSAObj, pTask);
}

void IOSourceCtrlDll::RemoveTask(int iIndex)
{
	if (m_pIOSAObj)
		_IO_SA_RemoveTask(m_pIOSAObj, iIndex);
}

int IOSourceCtrlDll::GetTotalOfTasks()
{
	if (m_pIOSAObj)
		return _IO_SA_GetTotalOfTasks(m_pIOSAObj);
}

DisplayTask* IOSourceCtrlDll::GetTaskInfo(int iIndex)
{
	if (m_pIOSAObj)
		return _IO_SA_GetTaskInfo(m_pIOSAObj, iIndex);
}

void IOSourceCtrlDll::SaveTask()
{
	if (m_pIOSAObj)
		_IO_SA_SaveTask(m_pIOSAObj);
}

wchar_t* IOSourceCtrlDll::GetLayoutText(int iLayoutID)
{
	if(m_pIOSAObj)
		return _IO_SA_GetLayoutText(m_pIOSAObj, iLayoutID);
}

void IOSourceCtrlDll::MakeGrid(int iLayoutIndex, wchar_t* wszFileName)
{
	if (m_pIOSAObj)
		_IO_SA_MakeGrid(m_pIOSAObj,iLayoutIndex, wszFileName);
}

RECT* IOSourceCtrlDll::GetGridRect(int iIndex)
{
	if (m_pIOSAObj)
		return _IO_SA_GetGridRect(m_pIOSAObj,iIndex);
}

int IOSourceCtrlDll::GetTotalOfMonitors()
{
	if (m_pIOSAObj)
		return _IO_SA_GetTotalOfMonitors(m_pIOSAObj);
}

MonitorDeviceInfo* IOSourceCtrlDll::GetMonitorDeviceInfo(int iIndex)
{
	if (m_pIOSAObj)
		return _IO_SA_GetMonitorDeviceInfo(m_pIOSAObj, iIndex);
}

void IOSourceCtrlDll::Show() 
{
	if (m_pIOSAObj)
		_IO_SA_ShowPlayers(m_pIOSAObj);
}

void IOSourceCtrlDll::Hide()
{
	if (m_pIOSAObj)
		_IO_SA_HidePlayers(m_pIOSAObj);
}

void IOSourceCtrlDll::SetDisplayTask(DisplayTask* pTask)
{
	if (m_pIOSAObj)
		_IO_SA_SetDisplayTask(m_pIOSAObj, pTask);
}

void IOSourceCtrlDll::Play(char* szFileName)
{
	if (m_pIOSAObj)
		_IO_SA_Play(m_pIOSAObj, szFileName);
}

void IOSourceCtrlDll::Stop()
{
	if (m_pIOSAObj)
		_IO_SA_Stop(m_pIOSAObj);
}

void IOSourceCtrlDll::DoFadeOut()
{
	if (m_pIOSAObj)
		_IO_SA_DoFadeOut(m_pIOSAObj);
}

*/

void IOSourceCtrlDll::SetIOSourceCtrlCallback(void* pObj)
{
	if (m_pIOSAObj)
		_IO_SA_SetIOSourceCtrlCallback(m_pIOSAObj, pObj);
}

void IOSourceCtrlDll::BeginOpenFile(wchar_t* wszFileName)
{
	if (g_PannelSetting.iEncodeMedia != 0)
	{
		CString strTemp(wszFileName);
#ifdef PLAY_3D_MODE
		CPlaneScene3D::FileDecoder(strTemp, "", FALSE);
#endif
	}
}

void IOSourceCtrlDll::EndOpenFile(wchar_t* wszFileName)
{
	if (g_PannelSetting.iEncodeMedia != 0)
	{
		CString strTemp(wszFileName);
#ifdef PLAY_3D_MODE
		CPlaneScene3D::FileEncoder(strTemp, FALSE);
#endif
	}
}

int IOSourceCtrlDll::AddMediaFile2(wchar_t* wszFileName, bool IsEncrypted)
{
	if (m_pIOSAObj)
		return _IO_SA_AddMediaFile2(m_pIOSAObj, wszFileName, IsEncrypted);
}

void IOSourceCtrlDll::SetLogFileDll(LogFileDll* pDllObj, void* pLogFileObj)
{
	m_pLogFileDll = pDllObj;
	m_pLogFileObj = pLogFileObj;
}

void IOSourceCtrlDll::Event(int iType, char* szMsg)
{
	if (m_pLogFileDll)
	{
		if (m_pLogFileObj)
		{
			//wchar_t wszText[512];
			wcscpy(m_wszText, ANSIToUnicode(szMsg));
			m_pLogFileDll->_Out_LMC_AddLog(m_pLogFileObj, MCTCC_DUMP, m_wszText, L"", 0);
		}
	}
}

int IOSourceCtrlDll::AddRTSPUrl(wchar_t* wszUrl, double dLen)
{
	if (m_pIOSAObj)
		return _IO_SA_AddRTSPUrl(m_pIOSAObj, wszUrl, dLen);
	return -1;
}

wchar_t* IOSourceCtrlDll::GetShortUrl(wchar_t* wszUrl)
{
	if (m_pIOSAObj)
		return _IO_SA_GetShortUrl(m_pIOSAObj, wszUrl);
	return L"";
}
