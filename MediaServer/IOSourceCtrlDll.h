#pragma once
#ifndef _IOSourceCtrlDll_H_
#define _IOSourceCtrlDll_H_

#include "../../../Include/MediaFileCommon.h"
//#include "../../Include/MediaFileCommon.h"
#include "LogFileDll.h"

//media file manager
typedef void* (*_IO_SA_Create_Proc)();
typedef void(*_IO_SA_Destroy_Proc)(void* pIOSAObj);
typedef void(*_IO_SA_Init_Proc)(void* pIOSAObj);
typedef int(*_IO_SA_AddMediaFile_Proc)(void* pIOSAObj, wchar_t* wszFileName);
typedef void(*_IO_SA_DeleteMediaFile_Proc)(void* pIOSAObj, int iIndex);
typedef int(*_IO_SA_GetFileFormatbyText_Proc)(void* pIOSAObj, wchar_t* wszText);
typedef int(*_IO_SA_GetTotalOfFileList_Proc)(void* pIOSAObj, int iFileFormat);
typedef wchar_t* (*_IO_SA_GetFileName_Proc)(void* pIOSAObj, wchar_t* pLongPath);
typedef MediaFileInfo* (*_IO_SA_GetMediaFileInfo_Proc)(void* pIOSAObj, int iFileFormat, int iIndex);
typedef MediaFileInfo* (*_IO_SA_GetMediaFileInfobyFileName_Proc)(void* pIOSAObj, wchar_t* wszFileName);
typedef MediaFileInfo* (*_IO_SA_GetMediaFileInfobyID_Proc)(void* pIOSAObj, unsigned long ulID);
typedef void(*_IO_SA_SaveMediaFile_Proc)(void* pIOSAObj);
typedef ThumbnailPicture* (*_IO_SA_GetPicture_Proc)(void* pIOSAObj, MediaFileInfo* pInfo);
typedef void(*_IO_SA_DoTagSearch_Proc)(void* pIOSAObj, wchar_t* wszInputKeyword);
typedef void(*_IO_SA_SetIOSourceCtrlCallback_Proc)(void* pIOSAObj, void* pObj);
typedef void(*_IO_SA_DeleteFile_Proc)(void* pIOSAObj, wchar_t* wszFileName);

typedef int(*_IO_SA_AddMediaFile2_Proc)(void* pIOSAObj, wchar_t* wszFileName, bool IsEncrypted);
typedef void(*_IO_SA_Reset_Proc)(void* pIOSAObj);
typedef int(*_IO_SA_AddRTSPUrl_Proc)(void* pIOSAObj, wchar_t* wszUrl, double dLen);
typedef wchar_t* (*_IO_SA_GetShortUrl_Proc)(void* pIOSAObj, wchar_t* wszUrl);

/*
//task manager
typedef void(*_IO_SA_AddTask_Proc)(void* pIOSAObj, DisplayTask* pTask);
typedef void(*_IO_SA_RemoveTask_Proc)(void* pIOSAObj, int iIndex);
typedef int(*_IO_SA_GetTotalOfTasks_Proc)(void* pIOSAObj);
typedef DisplayTask* (*_IO_SA_GetTaskInfo_Proc)(void* pIOSAObj, int iIndex);
typedef wchar_t* (*_IO_SA_GetLayoutText_Proc)(void* pIOSAObj, int iLayoutID);
typedef void(*_IO_SA_SaveTask_Proc)(void* pIOSAObj);
typedef void(*_IO_SA_MakeGrid_Proc)(void* pIOSAObj, int iLayoutIndex, wchar_t* wszFileName);
typedef RECT* (*_IO_SA_GetGridRect_Proc)(void* pIOSAObj, int iIndex);
typedef int(*_IO_SA_GetTotalOfMonitors_Proc)(void* pIOSAObj);
typedef MonitorDeviceInfo* (*_IO_SA_GetMonitorDeviceInfo_Proc)(void* pIOSAObj, int iIndex);
//vwall player
typedef void(*_IO_SA_ShowPlayers_Proc)(void* pIOSAObj);
typedef void(*_IO_SA_HidePlayers_Proc)(void* pIOSAObj);
typedef void(*_IO_SA_Play_Proc)(void* pIOSAObj, char* szFileName);
typedef void(*_IO_SA_Stop_Proc)(void* pIOSAObj);
typedef void(*_IO_SA_SetDisplayTask_Proc)(void* pIOSAObj, DisplayTask* pTask);
typedef void(*_IO_SA_DoFadeOut_Proc)(void* pIOSAObj);
*/

class IOSourceCtrlDll : public IOSourceCtrlCallback
{
	HINSTANCE m_hInst;
	void* m_pIOSAObj;

	LogFileDll* m_pLogFileDll;
	void* m_pLogFileObj;

	wchar_t m_wszText[512];
protected:
public:
	IOSourceCtrlDll();
	~IOSourceCtrlDll();

	bool LoadLib();
	void FreeLib();

	void Init();
	int AddMediaFile(wchar_t* wszFileName);
	void DeleteMediaFile(int iIndex);
	int GetFileFormatbyText(wchar_t* wszText);
	int GetTotalOfFileList(int iFileFormat = MFF_ALL);
	wchar_t* GetFileName(wchar_t* pLongPath);
	MediaFileInfo* GetMediaFileInfo(int iFileFormat, int iIndex);
	MediaFileInfo* GetMediaFileInfo(int iIndex);
	MediaFileInfo* GetMediaFileInfo(wchar_t* wszFilename);
	MediaFileInfo* GetMediaFileInfobyID(unsigned long ulID);
	void SaveMediaFile();
	ThumbnailPicture* GetPicture(MediaFileInfo* pInfo);
	void DoTagSearch(wchar_t* wszInputKeyword);
	void SetIOSourceCtrlCallback(void* pObj);
	int AddMediaFile2(wchar_t* wszFileName, bool IsEncrypted);
	void Reset();
	void SetLogFileDll(LogFileDll* pDllObj, void* pLogFileObj);
	void DeleteFile(wchar_t* wszFileName);

	//rtsp
	int AddRTSPUrl(wchar_t* wszUrl, double dLen);
	wchar_t* GetShortUrl(wchar_t* wszUrl);

	/*
	void AddTask(DisplayTask* pTask);
	void RemoveTask(int iIndex);
	int GetTotalOfTasks();
	DisplayTask* GetTaskInfo(int iIndex);
	void SaveTask();
	wchar_t* GetLayoutText(int iLayoutID);
	void MakeGrid(int iLayoutIndex, wchar_t* wszFileName);
	RECT* GetGridRect(int iIndex);
	int GetTotalOfMonitors();
	MonitorDeviceInfo* GetMonitorDeviceInfo(int iIndex);
	

	void Show();
	void Hide();
	void SetDisplayTask(DisplayTask* pTask);
	void Play(char* szFileName);
	void Stop();
	void DoFadeOut();
	*/

	//callback
	void BeginOpenFile(wchar_t* wszFileName);
	void EndOpenFile(wchar_t* wszFileName);
	void Event(int iType, char* szMsg);

	
	_IO_SA_Create_Proc _IO_SA_Create;
	_IO_SA_Destroy_Proc _IO_SA_Destroy;
	_IO_SA_Init_Proc _IO_SA_Init;
	_IO_SA_AddMediaFile_Proc _IO_SA_AddMediaFile;
	_IO_SA_DeleteMediaFile_Proc _IO_SA_DeleteMediaFile;
	_IO_SA_GetFileFormatbyText_Proc _IO_SA_GetFileFormatbyText;
	_IO_SA_GetTotalOfFileList_Proc _IO_SA_GetTotalOfFileList;
	_IO_SA_GetFileName_Proc _IO_SA_GetFileName;
	_IO_SA_GetMediaFileInfo_Proc _IO_SA_GetMediaFileInfo;
	_IO_SA_GetMediaFileInfobyFileName_Proc _IO_SA_GetMediaFileInfobyFileName;
	_IO_SA_GetMediaFileInfobyID_Proc _IO_SA_GetMediaFileInfobyID;
	_IO_SA_SaveMediaFile_Proc _IO_SA_SaveMediaFile;
	_IO_SA_GetPicture_Proc _IO_SA_GetPicture;
	_IO_SA_DoTagSearch_Proc _IO_SA_DoTagSearch;
	_IO_SA_SetIOSourceCtrlCallback_Proc _IO_SA_SetIOSourceCtrlCallback;
	_IO_SA_AddMediaFile2_Proc _IO_SA_AddMediaFile2;
	_IO_SA_Reset_Proc _IO_SA_Reset;
	_IO_SA_AddRTSPUrl_Proc _IO_SA_AddRTSPUrl;
	_IO_SA_GetShortUrl_Proc _IO_SA_GetShortUrl;
	_IO_SA_DeleteFile_Proc _IO_SA_DeleteFile;

	/*
	_IO_SA_AddTask_Proc _IO_SA_AddTask;
	_IO_SA_RemoveTask_Proc _IO_SA_RemoveTask;
	_IO_SA_GetTotalOfTasks_Proc _IO_SA_GetTotalOfTasks;
	_IO_SA_GetTaskInfo_Proc _IO_SA_GetTaskInfo;
	_IO_SA_GetLayoutText_Proc _IO_SA_GetLayoutText;
	_IO_SA_SaveTask_Proc _IO_SA_SaveTask;
	_IO_SA_MakeGrid_Proc _IO_SA_MakeGrid;
	_IO_SA_GetGridRect_Proc _IO_SA_GetGridRect;
	_IO_SA_GetTotalOfMonitors_Proc _IO_SA_GetTotalOfMonitors;
	_IO_SA_GetMonitorDeviceInfo_Proc _IO_SA_GetMonitorDeviceInfo;

	_IO_SA_ShowPlayers_Proc _IO_SA_ShowPlayers;
	_IO_SA_HidePlayers_Proc _IO_SA_HidePlayers;
	_IO_SA_Play_Proc _IO_SA_Play;
	_IO_SA_Stop_Proc _IO_SA_Stop;
	_IO_SA_SetDisplayTask_Proc _IO_SA_SetDisplayTask;
	_IO_SA_DoFadeOut_Proc _IO_SA_DoFadeOut;
	*/
};

#endif