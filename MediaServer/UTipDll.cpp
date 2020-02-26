#include "stdafx.h"
#include "UTipDll.h"

UTipDll::UTipDll()
{
	m_hInst = 0;
}

UTipDll::~UTipDll()
{
	FreeLib();
}

void UTipDll::LoadLib()
{
	char szFileName[] = "UTip.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_UT_Create = (_UT_Create_Proc)GetProcAddress(m_hInst, "_UT_Create");
		_UT_Destroy = (_UT_Destroy_Proc)GetProcAddress(m_hInst, "_UT_Destroy");
		_UT_SetTextColor = (_UT_SetTextColor_Proc)GetProcAddress(m_hInst, "_UT_SetTextColor");
		_UT_SetBKColor = (_UT_SetBKColor_Proc)GetProcAddress(m_hInst, "_UT_SetBKColor");
		_UT_Init = (_UT_Init_Proc)GetProcAddress(m_hInst, "_UT_Init");
		_UT_Add = (_UT_Add_Proc)GetProcAddress(m_hInst, "_UT_Add");
		_UT_Reset = (_UT_Reset_Proc)GetProcAddress(m_hInst, "_UT_Reset");
		_UT_ShowTip = (_UT_ShowTip_Proc)GetProcAddress(m_hInst, "_UT_ShowTip");

		m_pUTObj = _UT_Create();
	}
}

void UTipDll::FreeLib()
{
	if (m_hInst)
	{
		_UT_Destroy(m_pUTObj);
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void* UTipDll::GetUTObj()
{
	return m_pUTObj;
}