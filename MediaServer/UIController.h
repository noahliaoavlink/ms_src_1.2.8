#pragma once

#ifndef _UIController_H
#define _UIController_H

#include "../../../Include/mediafilecommon.h"
#include "../../../Include/UIObjInfo.h"
#include "../../../Include/IDTable.h"

#include "SimpleMediaCtrl.h"
#include "SimpleMixerWin.h"
#include "EffectSelectorWin.h"
#include "AudioConfigWin.h"

//#include "../../../Include/PluginObjMsg.h"

enum UIMode
{
	UIM_NONE = 0,
	UIM_SP_MEDIA = 1, 
	UIM_SP_MIXER,
	UIM_SP_EFFECT_SELECTOR,
	UIM_SP_AUDIO_CONFIG,
};

typedef void(*MsgOutlet_Proc)(MsgInfo* pIn, MsgInfo* pOut);

class UIController
{
	//for UI
	MsgInfo* m_pUIOutMsg;
	MsgInfo* m_pUILocalMsg;
	HINSTANCE m_hUIInst;

	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	CSVReader* m_pCSVReader;

	MediaFileManagerCallback* m_pMediaFileManagerCallback;

	HWND m_hMSMainUIWnd;
	int m_iUIMode;

	CNWin* m_pNWin;
	FilePlayerObj* m_pFilePlayerObj;
	EffectListCtrl* m_pEffectListCtrl;

	SimpleMediaCtrl* m_pSimpleMediaCtrl;
	SimpleMixerWin* m_pSimpleMixerWin;
	EffectSelectorWin* m_pEffectSelectorWin;
	AudioConfigWin* m_pAudioConfigWin;

	int m_iSkinMode;
public:
	UIController();
	~UIController();

	MsgOutlet_Proc UI_SetMsgProc;

	bool LoadLib();
	void FreeLib();

	void Init();

	void SetMediaFileManagerCallback(MediaFileManagerCallback* pObj);
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);
	void SetCSVReader(CSVReader* pObj);
	void SetMainUIHandle(HWND hWnd);
	AudioConfigWin* GetAudioConfigWin();

	void OpenSimpleMediaCtrl();
	void OpenSimpleMixer();
	void OpenEffectSelector();
	void OpenAudioConfig();

	void CancelEvent();
	void ButtonEvent(unsigned long ulCtrlID);
	void ComboBoxEvent(unsigned long ulCtrlID, int iSel);
	void SelChangeEvent(unsigned long ulCtrlID, int iSel);
	void SliderEvent(unsigned long ulCtrlID, int iValue, int iEventType);
};

#endif
