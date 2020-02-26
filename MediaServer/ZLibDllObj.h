#pragma once

#ifndef ZLibDllObj_H
#define ZLibDllObj_H

typedef void* (*_ZIP_Create_Proc)();
typedef void (*_ZIP_Destroy_Proc)(void* pZIPObj);
typedef void (*_ZIP_CreateZip_Proc)(void* pZIPObj, char* szFileName);
typedef void (*_ZIP_CloseZip_Proc)(void* pZIPObj);
typedef void (*_ZIP_AddFile_Proc)(void* pZIPObj, char* szFileNameInZip, char* szSrcFile);
typedef void (*_ZIP_OpenUnZip_Proc)(void* pZIPObj, char* szFileName);
typedef void (*_ZIP_CloseUnZip_Proc)(void* pZIPObj);
typedef void (*_ZIP_Extract_Proc)(void* pZIPObj, char* szOutPath);

class ZLibDllObj
{
	HINSTANCE m_hInst;
	void* m_pZipObj;
public:
	ZLibDllObj();
	~ZLibDllObj();

	void LoadLib();
	void FreeLib();

	void CreateZip(char* szFileName);
	void CloseZip();
	void AddFile(char* szFileNameInZip, char* szSrcFile);

	void OpenUnZip(char* szFileName);
	void CloseUnZip();
	void Extract(char* szOutPath);

	_ZIP_Create_Proc _ZIP_Create;
	_ZIP_Destroy_Proc _ZIP_Destroy;
	_ZIP_CreateZip_Proc _ZIP_CreateZip;
	_ZIP_CloseZip_Proc _ZIP_CloseZip;
	_ZIP_AddFile_Proc _ZIP_AddFile;
	_ZIP_OpenUnZip_Proc _ZIP_OpenUnZip;
	_ZIP_CloseUnZip_Proc _ZIP_CloseUnZip;
	_ZIP_Extract_Proc _ZIP_Extract;
	
};
#endif