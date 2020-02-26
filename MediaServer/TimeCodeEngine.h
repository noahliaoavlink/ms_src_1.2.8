/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCodeEngine.h

Declaration of following classes:
TimeCodeThread
TimeCodeEngine

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

#pragma once

// ==== Inlcude Microsoft MFC library ==== //
#include <afxmt.h> // CCriticalSection
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <string>
#include <vector>
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimeCode.h"
#include <winsock2.h>


namespace Timeline
{

	using TimecodeUpdateSignal = boost::signals2::signal<void(TimeCode)>;

	/* ====================================================== *\
	TimeCodeThread
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimeCodeThread : public CWinThread
	{
	public:
		DECLARE_DYNAMIC(TimeCodeThread)
		TimeCodeThread();
		~TimeCodeThread();

		// Attributes
	public:
		HANDLE m_hEventKill;
		HANDLE m_hEventDead;
		static CCriticalSection m_critSection;

		CRITICAL_SECTION m_CriticalSection;

		TimecodeUpdateSignal TimeCodeUpdate;

	public:
		void KillThread();
		void SingleStep();

	public:
		
		void Delete();
		void Start();
		void Stop();
		bool IsAlive();
		void SetInterval(int iValue);
		int GetInterval();
		void ApplySetting();

	protected:
		virtual BOOL InitInstance();

		DECLARE_MESSAGE_MAP()

	private:
		bool m_isStart;
		DWORD m_dwLastTime;
		DWORD m_dwCount;
		int m_iInterval;
	};

	/* ====================================================== *\
	TimeCodeUiThread
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimeCodeUiThread : public CWinThread
	{
	public:
		DECLARE_DYNAMIC(TimeCodeUiThread)
		TimeCodeUiThread();
		~TimeCodeUiThread();

		// Attributes
	public:
		HANDLE m_hEventKill;
		HANDLE m_hEventDead;
		static CCriticalSection m_critSection;

		TimecodeUpdateSignal TimeCodeUpdate;

	public:
		void KillThread();
		void SingleStep();

	public:

		void Delete();
		void Start();
		void Stop();
		bool IsAlive();

	protected:
		virtual BOOL InitInstance();

		DECLARE_MESSAGE_MAP()

	private:
		double GetTime();
		double GetWallTime();
		double GetCpuTime();

	private:
		double m_beginTime;
		double m_enterTime;
		double m_exitTime;

		bool m_isStart;
		DWORD m_dwLastTime;
		DWORD m_dwCount;
	};

	/* ====================================================== *\
	TimeCodeEngine
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TimeCodeEngine
	{
	public:
		// ==== Enumeration ==== //


		// ==== Constructor & Destructor ==== //
	public:
		TimeCodeEngine();
		virtual ~TimeCodeEngine();

		// ==== Public member methods ==== //
	public:
		void StartTimeCodeUpdate();
		void StopTimeCodeUpdate();
		void PauseTimeCodeUpdate();

		void SetTotalTimecode(TimeCode timecode);
		TimeCode GetTotalTimecode();
		void SetTimecode(TimeCode timecode);
		bool IsAlive();
		void SetInterval(int iValue);
		void DoUpdateTimeCode();

		void ResetTimeCode();
		void SetRefTimeCode(DWORD dwRefTime);
		void SetIPAdress(char* szIP);
		void SetPort(int iValue);
		void SetMaster(bool bEnable);
		void CloseSocket();
		void ApplySetting();

		int CheckStatus();
		void DoWait();

		// ==== Public member fields ==== //
	public:
		TimecodeUpdateSignal TimeCodeUpdate;
		TimecodeUpdateSignal UiUpdate;
		TimecodeUpdateSignal UiUpdate2;

		// ==== Private member methods ==== //
	private:
		void UpdateTimeCode(TimeCode timecode);
		void UpdateUi(TimeCode timecode);

#ifndef _ENABLE_TC_NET
		void InitSocket(BOOL bMaster);
		static UINT SocketThread(LPVOID pParam);
		void SocketFun();
#endif

		// ==== Private member fields ==== //
	private:
		TimeCodeThread* m_timecodeThread = 0;
		TimeCodeUiThread* m_uiThread = 0;
		TimeCode m_timecode;
		TimeCode m_total_timecode;
		DWORD m_dwRefTime;
		DWORD m_dwStartTime;
		bool m_bMaster;

		int m_iIndex;
		TimeCode m_tmp_timecode[100];

		int m_iStatus;
		int m_iWaitCount;

#ifndef _ENABLE_TC_NET
		SOCKET m_Socket;
		SOCKADDR_IN m_sBrotherAddr;
		CWinThread* m_pSoketThread;
		bool m_bIsRunning;
#endif

		char m_szIPAdress[64];
		int m_iPort;

	};

}
