/********************************************************************\
Project Name: Timeline Prototype

File Name: OutputService.h

Declaration of following classes:
OutputEvent
OutputService

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
#include <string>
#include <vector>
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

	/* ====================================================== *\
	OutputEvent
	\* ====================================================== */

	class TIMELINELIB_EXPORTS OutputEvent
	{
	public:
		enum Type
		{
			None = 0,
			Log = 1,
			Event = 2,
			Action = 3
		};

	public:
		OutputEvent();
		OutputEvent(OutputEvent::Type type, const std::string& msg);
		virtual ~OutputEvent();

		std::string Message;
		OutputEvent::Type MessageType;
	};



	/* ====================================================== *\
	OutputService
	\* ====================================================== */

	class TIMELINELIB_EXPORTS OutputService
	{
		using Signal = boost::signals2::signal<void(std::string)>;
		// ==== Constructor & Destructor ==== //
	public:
		OutputService();
		virtual ~OutputService();

		// ==== Public member methods ==== //
	public:
		void SendEvent(OutputEvent& evt);

		Signal Log;
		Signal Event;
		Signal Action;

		// ==== Private member fields ==== //
	private:

	};

}