#include "stdafx.h"
#include "MidiCtrlReciever.h"
#include "ServiceProvider.h"
#include "TimelineManager.h"
#include "ServiceProvider.h"

DWORD WINAPI MCRecieverThread(LPVOID lpParam);

MidiCtrlReciever::MidiCtrlReciever()
{
	m_pNWin = 0;

	m_hMCRecieverThread = 0;

	m_pTimeCodeQueue = new SQQueue;
	m_pTimeCodeQueue->Alloc(sizeof(double), 100);
}

MidiCtrlReciever::~MidiCtrlReciever()
{
	Stop();

	m_pTimeCodeQueue->Reset();
	delete m_pTimeCodeQueue;

	if (m_pNWin)
		delete m_pNWin;
}

void MidiCtrlReciever::Init()
{
	char szWinName[128];
	sprintf(szWinName, "MidiCtrl_Reciever");
	m_pNWin = new CNWin(szWinName);
	m_pNWin->SetCallback(this);
}

void MidiCtrlReciever::ParseCopyData(COPYDATASTRUCT* pCopyDataStruct)
{
	TimelineManager* manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	switch (pCopyDataStruct->dwData)
	{
		case MCC_STOP:
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
			manager->DoPlayback_AllOutDevices(PBC_CLOSE);
			manager->ClearAll();
			Stop();
			break;
		case MCC_PLAY:
			manager->FreezeShape(true);
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
			Play();
			manager->FreezeShape(false);
			break;
		case MCC_PAUSE:
			SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
			break;
		case MCC_UPDATE_TIME_CODE:
			{
				long* lTimeCode = (long*)pCopyDataStruct->lpData;
				double dTime = *lTimeCode / 1000.0;
				AddTimeCodeToQueue(dTime);
			}
			break;
		case KPCC_RET_FAILED:
			{
				TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
				CWnd* pEditViewWnd = m_manager->GetEditViewWnd();
				::PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
				::MessageBoxA(NULL,"Cannot find dongle!!", "Warning", MB_OK | MB_TOPMOST);
				//PostMessage(WM_CLOSE);
				PostQuitMessage(0);
			}
			break;
		case KPCC_RET_OK:
			break;
	}
}

void* MidiCtrlReciever::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case WM_COPYDATA:
			{
				COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)pParameter2;
				ParseCopyData(pCopyDataStruct);
			}
			break;
	}
	return 0;
}

void MidiCtrlReciever::Play()
{
	Stop();

	m_bIsAlive = true;
	DWORD nThreadID;
	m_hMCRecieverThread = CreateThread(0, 0, MCRecieverThread, (void *)this, 0, &nThreadID);

	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto tc_adapter = tl_manager->GetTCAdapter();
	tc_adapter->SetMode(TCM_MIDI_CTRL);
}

void MidiCtrlReciever::Stop()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hMCRecieverThread, 10000);

	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto tc_adapter = tl_manager->GetTCAdapter();
	tc_adapter->SetMode(TCM_NORMAL);
}

void MidiCtrlReciever::Pause()
{
}

bool MidiCtrlReciever::IsAlive()
{
	return m_bIsAlive;
}

void MidiCtrlReciever::AddTimeCodeToQueue(double dTimecode)
{
	m_pTimeCodeQueue->Add((unsigned char*)&dTimecode, sizeof(double));
}

double* MidiCtrlReciever::GetTimeCodeFromeQueue()
{
	unsigned char* p = m_pTimeCodeQueue->GetNext();
	m_pTmpTimeCode = (double*)p;
	return m_pTmpTimeCode;
}

void MidiCtrlReciever::ProcessTimeCode()
{
	double* pdTimeCode = GetTimeCodeFromeQueue();
	if (pdTimeCode)
	{
		UpdateTimeCodeFromMidiCtrl(*pdTimeCode);
	}
}

void MidiCtrlReciever::UpdateTimeCodeFromMidiCtrl(double dTimeCode)
{
	auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
	
	TimeCode total_time_code = time_code_engine->GetTotalTimecode();
	total_time_code.TotalMilliseconds();

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	unsigned long ulTimeCode = dTimeCode * 1000.0;
	for (int i = ST_STREAM0; i <= ST_STREAM9; i++)
	{
		OutDevice* pOutDevice = manager->GetOutDevice(i);
		if (pOutDevice)
		{
			if (pOutDevice->IsOpened())
			{
				if (manager->CheckItemName(pOutDevice->GetGroupName()))
				{
					pOutDevice->UpdateGlobalTimeCode(dTimeCode);
				}
			}
		}
	}

	char szMsg[512];
	sprintf(szMsg, "MidiCtrlReciever::UpdateTimeCodeFromMidiCtrl %5.3f\n", dTimeCode);
	OutputDebugString(szMsg);

	time_code_engine->SetTimecode(dTimeCode * 1000);
	time_code_engine->DoUpdateTimeCode();
}

void MidiCtrlReciever::TimeCodeLoop()
{
	EnterCriticalSection(&m_CriticalSection);

	ProcessTimeCode();

	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

DWORD WINAPI MCRecieverThread(LPVOID lpParam)
{
	MidiCtrlReciever* pObj = (MidiCtrlReciever*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->TimeCodeLoop();
	}
	return 0;
}
