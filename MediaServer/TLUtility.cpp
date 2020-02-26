/********************************************************************\
Project Name: Timeline Prototype

File Name: TLUtility.cpp

Definition of following classes:

Definition of following functions:
ShowMessageBox
DumpToLog
DumpToEvent
DumpToAction

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/


// ==== Inlcude Microsoft library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <string>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "ServiceProvider.h"


using namespace Timeline;

std::wstring Timeline::towstring(const std::string& v)
{
	std::wstring out(v.size() + 1, L'\0');

	int size = MultiByteToWideChar(CP_UTF8, 0, v.c_str(), -1, &out[0], out.size());

	out.resize(size - 1);
	return out;
}

void Timeline::ShowMessageBox(const std::string& msg)
{
	CString output;
	output = msg.c_str();
	AfxMessageBox(output);
}

void Timeline::DumpToLog(const std::string& msg)
{
	auto service = ServiceProvider::Instance()->GetOutputService();
	if (service != nullptr)
	{
		service->SendEvent(OutputEvent(OutputEvent::Type::Log, msg));
	}
}

void Timeline::DumpToEvent(const std::string& msg)
{
	auto service = ServiceProvider::Instance()->GetOutputService();
	if (service != nullptr)
	{
		service->SendEvent(OutputEvent(OutputEvent::Type::Event, msg));
	}
}

void Timeline::DumpToAction(const std::string& msg)
{
	auto service = ServiceProvider::Instance()->GetOutputService();
	if (service != nullptr)
	{
		service->SendEvent(OutputEvent(OutputEvent::Type::Action, msg));
	}
}

Gdiplus::Rect Timeline::CRectToRect(const CRect& rect)
{
	return Gdiplus::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}