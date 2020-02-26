/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCode.h

Declaration of following classes:
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

#pragma once

// ==== Inlcude Microsoft MFC library ==== //


// ==== Inlcude STL library ==== //
#include <string>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

	/* ====================================================== *\
			   TimeCode
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TimeCode
	{
	public:
		// ==== Enumeration ==== //
		enum class Format
		{
			Format_24_Fps,
			Format_25_Fps,
			Format_30_Fps,
			Format_30_DropFrame,
			Format_30_NonDrop
		};

		// ==== Constructor & Destructor ==== //
	public:
		TimeCode();
		TimeCode(int hours, int minutes, int seconds);
		TimeCode(int hours, int minutes, int seconds, int milliseconds);
		TimeCode(long milliseconds);
		virtual ~TimeCode();

		// ==== Public member operators ==== //
	public:
		TimeCode operator +(TimeCode t);
		bool operator ==(TimeCode t);
		bool operator >(TimeCode t);
		bool operator >=(TimeCode t);
		bool operator !=(TimeCode t);
		bool operator <(TimeCode t);
		bool operator <=(TimeCode t);
		TimeCode operator -(TimeCode t);
		TimeCode SignedTimeCode(int hours, int minutes, int seconds, int milliseconds);


		// ==== Public member methods ==== //
	public:
		long TotalMilliseconds();
		void SetTotalMilliseconds(long milliseconds);

		int MilliSeconds();
		void SetMilliSeconds(int milliseconds);

		int Hours();
		void SetHours(int hours);

		int Minutes();
		void SetMinutes(int minutes);

		int Seconds();
		void SetSeconds(int seconds);

		int Sign();
		void SetSign(int sign);

		int GetHashCode();
		std::string ToString();

		//static bool TryParse(std::string& str, TimeCode time); // TODO

		// ==== Public member fields ==== //
	public:
		static const TimeCode Zero;
		static const TimeCode TimeMax;
		static const TimeCode TimeMin;

		// ==== Private member fields ==== //
	private:
		static const int HourMax = 1000;
		static const int MinuteMax = 60;
		static const int SecondMax = 60;
		static const int MilliSecondMax = 1000;

		long m_ms;
	};

}