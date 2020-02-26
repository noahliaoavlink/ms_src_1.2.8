/********************************************************************\
Project Name: Timeline Prototype

File Name: TLCommands.h

Declaration of following classes:
TLCommandType
TLCommand
TLCommandNone
TLCommandStop
TLCommandJump

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

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

	/* ====================================================== *\
			  TLCommandType
	\* ====================================================== */
	enum class TIMELINELIB_EXPORTS TLCommandType
	{
		None = 0, Stop = 1, Jump = 2
	};

	/* ====================================================== *\
			  TLCommand
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLCommand
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLCommand(TLCommandType type);
		virtual ~TLCommand();

		// ==== Public member methods ==== //
	public:
		std::string ToString();

		// ==== Protected member fields ==== //
	protected:
		TLCommandType m_type;

	};

	/* ====================================================== *\
			  TLCommandNone
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLCommandNone : TLCommand
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLCommandNone();
		virtual ~TLCommandNone();
	};

	/* ====================================================== *\
			  TLCommandStop
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLCommandStop : TLCommand
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLCommandStop();
		virtual ~TLCommandStop();
	};

	/* ====================================================== *\
			  TLCommandJump
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLCommandJump : TLCommand
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLCommandJump();
		virtual ~TLCommandJump();

		// ==== Public member methods ==== //
	public:
		GUID Target();
		void SetTarget(GUID& guid);

		std::string ToString();

		// ==== Private member fields ==== //
	private:
		GUID m_target;
	};

}