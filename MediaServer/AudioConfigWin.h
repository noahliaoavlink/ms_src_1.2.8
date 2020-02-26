#pragma once

#ifndef _AudioConfigWin_H
#define _AudioConfigWin_H

#include "../../../Include/mediafilecommon.h"
#include "../../../Include/UIObjInfo.h"
#include "../../../Include/IDTable.h"
#include "TrackContentItem.h"

//#include "SimpleMediaCtrl.h"
//#include "FilePlayerObj.h"

class AudioConfigWin //: public EventCallback
{
	//for UI
	MsgInfo* m_pUIOutMsg;
	
	HWND m_hMSMainUIWnd;
	bool m_bIsOK;

	bool m_bIsChecked[_MAX_SOURCES];
	int m_iVolume[_MAX_SOURCES];
public:
	AudioConfigWin();
	~AudioConfigWin();

	void SetUIOutMsg(MsgInfo* pObj);
	void SetMainUIHandle(HWND hWnd);

	void Init();

	void ButtonEvent(unsigned long ulCtrlID);
	void SliderEvent(unsigned long ulCtrlID, int iEventType, int iSel);

	void BtnOK();
	void BtnCancel();
	void Show(bool bShow);
	void Slider_Volume(int iIndex, int iSel);

	bool IsOK();
	bool IsChecked(int iIndex);
	int GetVolume(int iIndex);
	void SetCheck(int iIndex, bool bCheck);
	void SetVolume(int iIndex, int iVolume);
	void Reset();
	void UpdateUIStatus();

	/*
	
	void DestroyUI();
	
	
	void SetFilePlayerObj(FilePlayerObj* pObj);
	void SetNWin(CNWin* pWin);
	void SetEffectListCtrl(EffectListCtrl* pCtrl);
	void SetSkinMode(int iMode);
	

	void CheckSourceFile();
	void SetFileName(char* szFileName);
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);

	

	

	virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);
	*/
};

#endif
