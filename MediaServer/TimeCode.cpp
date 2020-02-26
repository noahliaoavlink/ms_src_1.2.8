/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCode.cpp

Definition of following classes:
TimeCode

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

#include <math.h>
#include <boost/format.hpp>
#include <boost/format/group.hpp>

// ==== Include local library ==== //
#include "TimeCode.h"

using namespace Timeline;

/* ====================================================== *\
          TimeCode
\* ====================================================== */
const TimeCode TimeCode::Zero = TimeCode(0);
const TimeCode TimeCode::TimeMax = TimeCode(2147483647);
const TimeCode TimeCode::TimeMin = TimeCode(-2147483648);

TimeCode::TimeCode()
{
	
	this->m_ms = 0;
}

TimeCode::TimeCode(int hours, int minutes, int seconds)
	: TimeCode(hours, minutes, seconds, 0)
{

}

TimeCode::TimeCode(int hours, int minutes, int seconds, int milliseconds)
{
	if (hours < 0 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60 || milliseconds < 0 || milliseconds >= 1000)
	{
		this->m_ms = 0;
		return;
	}
	this->m_ms = ((hours * 60 + minutes) * 60 + seconds) * 1000 + milliseconds;
}

TimeCode::TimeCode(long milliseconds)
{
	this->m_ms = milliseconds;
}

TimeCode::~TimeCode()
{
}

TimeCode TimeCode::operator +(TimeCode t)
{
	return TimeCode(this->m_ms + t.m_ms);
}

bool TimeCode::operator ==(TimeCode t)
{
	return this->m_ms == t.m_ms;
}

bool TimeCode::operator >(TimeCode t)
{
	return this->m_ms > t.m_ms;
}

bool TimeCode::operator >=(TimeCode t)
{
	return this->m_ms >= t.m_ms;
}

bool TimeCode::operator !=(TimeCode t)
{
	return this->m_ms != t.m_ms;
}

bool TimeCode::operator <(TimeCode t)
{
	return this->m_ms < t.m_ms;
}

bool TimeCode::operator <=(TimeCode t)
{
	return this->m_ms <= t.m_ms;
}

TimeCode TimeCode::operator -(TimeCode t)
{
	return TimeCode(this->m_ms - t.m_ms);
}

TimeCode TimeCode::SignedTimeCode(int hours, int minutes, int seconds, int milliseconds)
{
	TimeCode timeCode = TimeCode(abs(hours), abs(minutes), abs(seconds), abs(milliseconds));
	if (hours < 0 || minutes < 0 || seconds < 0 || milliseconds < 0)
	{
		timeCode.SetSign(-1);
	}
	return timeCode;
}

long TimeCode::TotalMilliseconds()
{
	return this->m_ms;
}

void TimeCode::SetTotalMilliseconds(long milliseconds)
{
	this->m_ms = milliseconds;
}

int TimeCode::MilliSeconds()
{
	return abs(this->m_ms) % 1000;
}

void TimeCode::SetMilliSeconds(int milliseconds)
{
	if (milliseconds < 0 || milliseconds >= 1000)
	{
		//throw Exception ("Illegal MilliSeconds value");
	}

	/*long ms = this->m_ms;
	int msHours = this->Hours() * 1000 * 60 * 60;
	int msMinutes = this->Minutes() * 1000 * 60;
	int msSeconds = this->Seconds() * 1000;
	
	ms = msHours + msMinutes + msSeconds + milliseconds;
	this->m_ms = ms;*/

	long ms = this->m_ms;

	this->m_ms = ms + (milliseconds - this->MilliSeconds()) * this->Sign();
}

int TimeCode::Hours()
{
	return abs(this->m_ms) / 3600000;
}

void TimeCode::SetHours(int hours)
{
	if (hours < 0)
	{
		//throw new ArgumentException("Illegal Hours value");
	}

	long ms = this->m_ms;
	this->m_ms = ms + (hours - this->Hours()) * 1000 * 60 * 60 * this->Sign();
}

int TimeCode::Minutes()
{
	return abs(this->m_ms) / 60000 % 60;
}

void TimeCode::SetMinutes(int minutes)
{
	if (minutes < 0 || minutes >= 60)
	{
		//throw new ArgumentException("Illegal Minutes value");
	}
	long ms = this->m_ms;
	this->m_ms = ms + (minutes - this->Minutes()) * 1000 * 60 * this->Sign();
}

int TimeCode::Seconds()
{
	return abs(this->m_ms) / 1000 % 60;
}

void TimeCode::SetSeconds(int seconds)
{
	if (seconds < 0 || seconds >= 60)
	{
		//throw new ArgumentException("Illegal Seconds value");
	}
	long ms = this->m_ms;
	this->m_ms = ms + (seconds - this->Seconds()) * 1000 * this->Sign();
}

int TimeCode::Sign()
{
	if (this->m_ms < 0)
	{
		return -1;
	}
	return 1;
}

void TimeCode::SetSign(int sign)
{
	if (sign != 1 && sign != -1)
	{
		//throw new ArgumentException("Sign must be 1 or -1");
	}
	this->m_ms = abs(this->m_ms) * sign;
}

int TimeCode::GetHashCode()
{
	return this->m_ms;
}

std::string TimeCode::ToString()
{
	std::string signStr = "";
	if (this->Sign() == -1)
	{
		signStr = "-";
	}

	auto format = boost::format("%s%02d:%02d:%02d.%03d");
	std::string output = boost::str(format % signStr %this->Hours() % this->Minutes() % this->Seconds() % this->MilliSeconds());

	return output;
}

//TODO
//bool TimeCode::TryParse(std::string& str, TimeCode time)
//{
//
//}
