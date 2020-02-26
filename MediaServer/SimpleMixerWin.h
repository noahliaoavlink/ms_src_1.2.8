#pragma once

#ifndef _SimpleMixerWin_H
#define _SimpleMixerWin_H


#include "../../../Include/mediafilecommon.h"
#include "../../../Include/UIObjInfo.h"
#include "../../../Include/IDTable.h"

#include "SimpleMediaCtrl.h"
#include "FilePlayerObj.h"

#include "RoundSliderCtrl.h"
#include "BitSlider.h"
#include "ImageFile.h"
#include "SIniFileW.h"

enum EditMode
{
	EM_NONE = 0,
	EM_ADD,
	EM_EDIT,
};

class SimpleMixerWin : public EventCallback
{
	//for UI
	MsgInfo* m_pUIOutMsg;

	HWND m_hMSMainUIWnd;
	HWND m_hCurDlgWnd;

	TreeCtrlUI* m_pTreeCtrlUI;
	FileListCtrl* m_pFileListCtrl;
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	CSVReader* m_pCSVReader;

	EffectListCtrl* m_pEffectListCtrl;
	CNWin* m_pNWin;
	FilePlayerObj* m_pFilePlayerObj;
	char m_szAPPath[512];
	char m_szFileName[512];

	int m_iSkinMode;

	int m_iEditMode;
	int m_iSelIndex;
	char m_szCurItemName[256];

	/*
	CRoundSliderCtrl* m_pVolumeCtrl;
	CBitSlider* m_pVolumeSlider;
	ImageFile m_VolumeCtrlBmp;
	*/
//	char m_szFileName[512];
	wchar_t wszIniFileName[512];
	SIniFileW* m_pSIniFileW;
public:
	SimpleMixerWin();
	~SimpleMixerWin();

	void Init();
	void DestroyUI();
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);
	void SetMainUIHandle(HWND hWnd);
	void SetUIOutMsg(MsgInfo* pObj);
	void SetFilePlayerObj(FilePlayerObj* pObj);
	void SetNWin(CNWin* pWin);
	void SetEffectListCtrl(EffectListCtrl* pCtrl);
	void SetSkinMode(int iMode);
	void Show(bool bShow);
	void SelectEffectItem(int iSel);
	void Slider_Level(int iSel);
	void Slider_Param1(int iSel);
	void Slider_Param2(int iSel);
	void Slider_Param3(int iSel);
	void Slider_Param4(int iSel);
	void Slider_Param5(int iSel);
	void Slider_Pos(int iEventType, int iPos);
	bool IsConfigMode();

	void ShowCtrl(int iIndex, bool bShow, char* szParamText);

	void BtnCancel();
	void BtnChange();
	void BtnAdd();
	void BtnDelete();
	void BtnEdit();
	void BtnStop();
	void BtnPlay();
	void BtnPause();

	void Config_BtnOK();
	void Config_BtnCancel();

	void ButtonEvent(unsigned long ulCtrlID);
	void ComboBoxEvent(unsigned long ulCtrlID, int iSel);
	void SliderEvent(unsigned long ulCtrlID, int iEventType, int iSel);

	void Save();
	void Load();

	void CheckSourceFile();
	void EnableButtons(bool bEnable);
	void UpdateUIStatus();

	void SetDefault();

	virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);

};

#endif
