#pragma once

#ifndef _SimpleMediaCtrl_H_
#define _SimpleMediaCtrl_H_

//#include "../../../Include/FilePlayerCommon.h"
#include "../../../Include/mediafilecommon.h"
#include "../../../Include/UIObjInfo.h"
#include "../../../Include/IDTable.h"

#include "TreeCtrlUI.h"
#include "FileListCtrl.h"
#include "IOSourceCtrlDll.h"
#include "CSVReader.h"

#define ENABLE_UI_CTRL 1

#ifdef ENABLE_UI_CTRL
#else
typedef void(*MsgOutlet_Proc)(MsgInfo* pIn, MsgInfo* pOut);
#endif

class SimpleMediaCtrl
{
	//for UI
	MsgInfo* m_pUIOutMsg;
	MsgInfo* m_pUILocalMsg;
	HINSTANCE m_hUIInst;

	TreeCtrlUI* m_pTreeCtrlUI;
	FileListCtrl* m_pFileListCtrl;
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	CSVReader* m_pCSVReader;

	MediaFileManagerCallback* m_pMediaFileManagerCallback;
protected:
public:
	SimpleMediaCtrl();
	~SimpleMediaCtrl();

#ifdef ENABLE_UI_CTRL
	void SetUIOutMsg(MsgInfo* pObj);
#else
	MsgOutlet_Proc UI_SetMsgProc;

	bool LoadLib();
	void FreeLib();
#endif

	void DestroyUI();

	void Init();
	void SetMediaFileManagerCallback(MediaFileManagerCallback* pObj);
	void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);
	void SetCSVReader(CSVReader* pObj);
	void Show(bool bShow);

	void BtnOK();
	void BtnCancel();
	void IconMode();
	void ListMode();
	void SelchangeTagItem(int iIndex);

	void ButtonEvent(unsigned long ulCtrlID);
	void ComboBoxEvent(unsigned long ulCtrlID, int iSel);
};

#endif
