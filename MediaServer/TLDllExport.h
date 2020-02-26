#pragma once

#ifdef TIMELINEDEV
	#define TIMELINELIB_EXPORTS __declspec(dllexport)
#else
	#define TIMELINELIB_EXPORTS
#endif