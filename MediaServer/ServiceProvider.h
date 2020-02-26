/********************************************************************\
Project Name: Timeline Prototype

File Name: ServiceProvider.h

Declaration of following classes:
ServiceProvider

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
#include "stdafx.h"

// ==== Inlcude STL library ==== //

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "OutputService.h"
#include "TimelineService.h"


namespace Timeline
{

	/* ====================================================== *\
	ServiceProvider
	\* ====================================================== */

	class TIMELINELIB_EXPORTS ServiceProvider
	{
		// ==== Constructor & Destructor ==== //
	public:
		ServiceProvider();
		virtual ~ServiceProvider();

		// ==== Public member methods ==== //
	public:
		static ServiceProvider* Instance();
		OutputService* GetOutputService();
		void SetOutputService(OutputService* service);

		TimelineService* GetTimelineService();
		void SetTimelineService(TimelineService* service);

		// ==== Private member methods ==== //
	private:
		static ServiceProvider m_this;
		OutputService* m_outputService;
		TimelineService* m_timelineService;
	};

}