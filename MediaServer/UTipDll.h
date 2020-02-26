#pragma once

#ifndef UTipDll_H
#define UTipDll_H

#include "..\\..\\..\\Include\\UIObjInfo.h"

typedef void* (*_UT_Create_Proc)();
typedef void (*_UT_Destroy_Proc)(void* pUTObj);
typedef void(*_UT_SetTextColor_Proc)(void* pUTObj, BYTE ucR, BYTE ucG, BYTE ucB);
typedef void(*_UT_SetBKColor_Proc)(void* pUTObj, BYTE ucR, BYTE ucG, BYTE ucB);
typedef void(*_UT_Init_Proc)(void* pUTObj);
typedef void(*_UT_Add_Proc)(void* pUTObj, HWND hWnd, _Region rect, wchar_t* wszMsg, unsigned long ulTipID);
typedef void(*_UT_Reset_Proc)(void* pUTObj);
typedef void(*_UT_ShowTip_Proc)(void* pUTObj, HWND hWnd, int x, int y);

class UTipDll
{
	HINSTANCE m_hInst;
	void* m_pUTObj;
public:
	UTipDll();
	~UTipDll();

	void LoadLib();
	void FreeLib();

	void* GetUTObj();

	_UT_Create_Proc _UT_Create;
	_UT_Destroy_Proc _UT_Destroy;
	_UT_SetTextColor_Proc _UT_SetTextColor;
	_UT_SetBKColor_Proc _UT_SetBKColor;
	_UT_Init_Proc _UT_Init;
	_UT_Add_Proc _UT_Add;
	_UT_Reset_Proc _UT_Reset;
	_UT_ShowTip_Proc _UT_ShowTip;
};
#endif