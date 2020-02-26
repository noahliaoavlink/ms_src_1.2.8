/********************************************************************\
Project Name: Timeline Prototype

File Name: TLCommands.cpp

Definition of following classes:
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

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <limits>
#include <sstream>

// ==== Include local library ==== //
#include "TLCommands.h"


using namespace Timeline;

/* ====================================================== *\
          TLCommand
\* ====================================================== */

TLCommand::TLCommand(TLCommandType type)
{
	this->m_type = type;
}

TLCommand::~TLCommand()
{
}

std::string TLCommand::ToString()
{
	std::string typeStr[]{std::string("None"), std::string("Stop"), std::string("Jump")};

	std::stringstream stream;
	stream << typeStr[(int)this->m_type] << " Command";

	std::string output = stream.str();

	return output;
}

/* ====================================================== *\
          TLCommandNone
\* ====================================================== */

TLCommandNone::TLCommandNone()
	: TLCommand(TLCommandType::None)
{
}

TLCommandNone::~TLCommandNone()
{
}

/* ====================================================== *\
          TLCommandStop
\* ====================================================== */

TLCommandStop::TLCommandStop()
	: TLCommand(TLCommandType::Stop)
{
}

TLCommandStop::~TLCommandStop()
{
}

/* ====================================================== *\
          TLCommandJump
\* ====================================================== */

TLCommandJump::TLCommandJump()
	: TLCommand(TLCommandType::Jump)
{
}

TLCommandJump::~TLCommandJump()
{
}

GUID TLCommandJump::Target()
{
	return this->m_target;
}

void TLCommandJump::SetTarget(GUID& guid)
{
	this->m_target = guid;
}

std::string TLCommandJump::ToString()
{
	std::stringstream stream;

	//todo: change guid_target to guid
	stream << TLCommand::ToString() << " (target = " << + "guid_target";

	std::string output = stream.str();

	return output;
}
