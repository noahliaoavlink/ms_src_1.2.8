#include "stdafx.h"
#include "AudioConfigWin.h"
#include "ServiceProvider.h"
#include "TimelineEditFacade.h"
#include "MediaServer.h"

extern CMediaServerApp theApp;

AudioConfigWin::AudioConfigWin()
{
	m_bIsOK = false;

	Reset();
}

AudioConfigWin::~AudioConfigWin()
{
}

void AudioConfigWin::SetUIOutMsg(MsgInfo* pObj)
{
	m_pUIOutMsg = pObj;
}

void AudioConfigWin::SetMainUIHandle(HWND hWnd)
{
	m_hMSMainUIWnd = hWnd;
}

void AudioConfigWin::Init()
{
//	DestroyUI();

	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		ShowWindow(hMainWnd, SW_HIDE);
	}
}

void AudioConfigWin::Show(bool bShow)
{
	HWND hMainWnd = (HWND)m_pUIOutMsg->SendMsg(UI_MSG_GET_MAIN_WIN, 0, 0);
	if (hMainWnd)
	{
		if (bShow)
		{
			UpdateUIStatus();

			ShowWindow(hMainWnd, SW_SHOW);
			//SetTopWin(hMainWnd, true);
		}
		else
		{
			ShowWindow(hMainWnd, SW_HIDE);
		}
	}
}

bool AudioConfigWin::IsOK()
{
	return m_bIsOK;
}

void AudioConfigWin::BtnOK()
{
	Show(false);

	m_bIsOK = true;

	//AUDIO_CONFIG_OK

	COPYDATASTRUCT cpd;

	TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pEditViewWnd = m_manager->GetEditViewWnd();

	cpd.dwData = AUDIO_CONFIG_OK;
	cpd.cbData = 0;
	cpd.lpData = 0;

	SendMessage(pEditViewWnd->GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);

	if (m_hMSMainUIWnd)
		SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
}

void AudioConfigWin::BtnCancel()
{
	Show(false);

	m_bIsOK = false;

	if (m_hMSMainUIWnd)
		SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
}

bool AudioConfigWin::IsChecked(int iIndex)
{
	if (m_pUIOutMsg->SendMsg(UI_MSG_IS_CHECKED, (void*)(CB_CHECK_BOX0 + iIndex), 0))
		return true;
	return false;
}

int AudioConfigWin::GetVolume(int iIndex)
{
	return (int)m_pUIOutMsg->SendMsg(UI_MSG_GET_POS, (void*)(SD_SLIDER + iIndex), (void*)0);
}

void AudioConfigWin::SetCheck(int iIndex, bool bCheck)
{
	if(_MAX_SOURCES > iIndex)
		m_bIsChecked[iIndex] = bCheck;
}

void AudioConfigWin::SetVolume(int iIndex,int iVolume)
{
	if (_MAX_SOURCES > iIndex)
		m_iVolume[iIndex] = iVolume;
}

void AudioConfigWin::Reset()
{
	for (int i = 0; i < _MAX_SOURCES; i++)
	{
		m_bIsChecked[i] = false;
		m_iVolume[i] = 50;
	}
}

void AudioConfigWin::UpdateUIStatus()
{
	for (int i = 0; i < _MAX_SOURCES; i++)
	{
		m_pUIOutMsg->SendMsg(UI_MSG_SET_CHECK, (void*)(CB_CHECK_BOX0 + i), (void*)m_bIsChecked[i]);
		m_pUIOutMsg->SendMsg(UI_MSG_SET_POS, (void*)(SD_SLIDER + i), (void*)m_iVolume[i]);
		Slider_Volume(i, m_iVolume[i]);

		if (theApp.m_bSampleVersion)
		{
			if (i >= 6)
			{
				m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)(CB_CHECK_BOX0 + i), (void*)0);
				m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)(SD_SLIDER + i), (void*)0);
				m_pUIOutMsg->SendMsg(UI_MSG_ENABLE_CTRL, (void*)(TX_TEXT2 + i), (void*)0);

				m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)(CB_CHECK_BOX0 + i), (void*)0);
				m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)(SD_SLIDER + i), (void*)0);
				m_pUIOutMsg->SendMsg(UI_MSG_SHOW_CTRL, (void*)(TX_TEXT2 + i), (void*)0);
			}
		}
	}
}

void AudioConfigWin::Slider_Volume(int iIndex,int iSel)
{
	char szText[64];
	sprintf(szText, "%d", iSel);
	m_pUIOutMsg->SendMsg(UI_MSG_SET_TEXT, (void*)(TX_TEXT2 + iIndex), (void*)szText);
}

void AudioConfigWin::ButtonEvent(unsigned long ulCtrlID)
{
	int iIndex = -1;
	switch (ulCtrlID)
	{
		case BN_BUTTON4:
			BtnOK();
			break;
		case BN_BUTTON5:
			BtnCancel();
			break;
	}
}

void AudioConfigWin::SliderEvent(unsigned long ulCtrlID, int iEventType, int iSel)
{
	switch (ulCtrlID)
	{
		case SD_SLIDER:
			Slider_Volume(0, iSel);
			break;
		case SD_SLIDER0:
			Slider_Volume(1, iSel);
			break;
		case SD_SLIDER1:
			Slider_Volume(2, iSel);
			break;
		case SD_SLIDER2:
			Slider_Volume(3, iSel);
			break;
		case SD_SLIDER3:
			Slider_Volume(4, iSel);
			break;
		case SD_SLIDER4:
			Slider_Volume(5, iSel);
			break;
		case SD_SLIDER5:
			Slider_Volume(6, iSel);
			break;
		case SD_SLIDER6:
			Slider_Volume(7, iSel);
		break;
		case SD_SLIDER7:
			Slider_Volume(8, iSel);
			break;
		case SD_SLIDER8:
			Slider_Volume(9, iSel);
			break;
		//case SD_SLIDER9:
			//Slider_Volume(9, iSel);
			//break;
	}
}
