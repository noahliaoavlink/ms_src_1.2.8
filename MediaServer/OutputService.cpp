/********************************************************************\
Project Name: Timeline Prototype

File Name: OutputService.cpp

Definition of following classes:
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


// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <limits>
#include <sstream>

// ==== Include local library ==== //
#include "OutputService.h"

using namespace Timeline;

/* ====================================================== *\
OutputEvent
\* ====================================================== */

OutputEvent::OutputEvent()
{
	this->MessageType = OutputEvent::Type::None;
}

OutputEvent::OutputEvent(OutputEvent::Type type, const std::string& msg)
{
	this->MessageType = type;
	this->Message = msg;
}

OutputEvent::~OutputEvent()
{
}


/* ====================================================== *\
OutputService
\* ====================================================== */

OutputService::OutputService()
{
}

OutputService::~OutputService()
{
}

void OutputService::SendEvent(OutputEvent& evt)
{
	switch (evt.MessageType)
	{
	case OutputEvent::Type::None:
		break;
	case OutputEvent::Type::Log:
		this->Log(evt.Message);
		break;
	case OutputEvent::Type::Event:
		this->Event(evt.Message);
		break;
	case OutputEvent::Type::Action:
		this->Action(evt.Message);
		break;
	default:
		break;
	}
	
}
