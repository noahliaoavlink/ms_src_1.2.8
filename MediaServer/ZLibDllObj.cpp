#include "stdafx.h"
#include "ZLibDllObj.h"

ZLibDllObj::ZLibDllObj()
{
	m_hInst = 0;
	m_pZipObj = 0;
}

ZLibDllObj::~ZLibDllObj()
{
	FreeLib();
}

void ZLibDllObj::LoadLib()
{
	char szFileName[] = "ZLibDll.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_ZIP_Create = (_ZIP_Create_Proc)GetProcAddress(m_hInst, "_ZIP_Create");
		_ZIP_Destroy = (_ZIP_Destroy_Proc)GetProcAddress(m_hInst, "_ZIP_Destroy");
		_ZIP_CreateZip = (_ZIP_CreateZip_Proc)GetProcAddress(m_hInst, "_ZIP_CreateZip");
		_ZIP_CloseZip = (_ZIP_CloseZip_Proc)GetProcAddress(m_hInst, "_ZIP_CloseZip");
		_ZIP_AddFile = (_ZIP_AddFile_Proc)GetProcAddress(m_hInst, "_ZIP_AddFile");
		_ZIP_OpenUnZip = (_ZIP_OpenUnZip_Proc)GetProcAddress(m_hInst, "_ZIP_OpenUnZip");
		_ZIP_CloseUnZip = (_ZIP_CloseUnZip_Proc)GetProcAddress(m_hInst, "_ZIP_CloseUnZip");
		_ZIP_Extract = (_ZIP_Extract_Proc)GetProcAddress(m_hInst, "_ZIP_Extract");

		m_pZipObj = _ZIP_Create();		
	}
}

void ZLibDllObj::FreeLib()
{
	if (m_hInst)
	{
		_ZIP_Destroy(m_pZipObj);
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void ZLibDllObj::CreateZip(char* szFileName)
{
	if (m_pZipObj)
		_ZIP_CreateZip(m_pZipObj, szFileName);
}

void ZLibDllObj::CloseZip()
{
	if (m_pZipObj)
		_ZIP_CloseZip(m_pZipObj);
}

void ZLibDllObj::AddFile(char* szFileNameInZip, char* szSrcFile)
{
	if (m_pZipObj)
	_ZIP_AddFile(m_pZipObj, szFileNameInZip, szSrcFile);
}

void ZLibDllObj::OpenUnZip(char* szFileName)
{
	if (m_pZipObj)
		_ZIP_OpenUnZip(m_pZipObj, szFileName);
}

void ZLibDllObj::CloseUnZip()
{
	if (m_pZipObj)
		_ZIP_CloseUnZip(m_pZipObj);
}

void ZLibDllObj::Extract(char* szOutPath)
{
	if (m_pZipObj)
		_ZIP_Extract(m_pZipObj, szOutPath);
}
