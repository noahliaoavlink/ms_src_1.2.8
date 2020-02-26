#pragma once

#ifndef _EffectSelectorWin_H
#define _EffectSelectorWin_H

#include "../../../Include/mediafilecommon.h"
#include "../../../Include/UIObjInfo.h"
#include "../../../Include/IDTable.h"

#include "SimpleMediaCtrl.h"
#include "FilePlayerObj.h"

class EffectSelectorWin : public EventCallback
{
	//for UI
	MsgInfo* m_pUIOutMsg;

	HWND m_hMSMainUIWnd;
	CNWin* m_pNWin;
	FilePlayerObj* m_pFilePlayerObj;

	EffectListCtrl* m_pEffectListCtrl;
	char m_szAPPath[512];
	char m_szFileName[512];

	EffectInfo m_EffectInfo;
	int m_iSkinMode;

	IOSourceCtrlDll* m_pIOSourceCtrlDll;
public:
	EffectSelectorWin();
	~EffectSelectorWin();

	void Init();
	void DestroyUI();
	void SetMainUIHandle(HWND hWnd);
	void SetUIOutMsg(MsgInfo* pObj);
	void SetFilePlayerObj(FilePlayerObj* pObj);
	void SetNWin(CNWin* pWin);
	void SetEffectListCtrl(EffectListCtrl* pCtrl);
	void SetSkinMode(int iMode);
	void Show(bool bShow);

	void CheckSourceFile();
	void SetFileName(char* szFileName);
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);

	void BtnOK();
	void BtnCancel();

	void ButtonEvent(unsigned long ulCtrlID);

	virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);
};

#endif