#pragma once
//LogCommon
#ifndef _LogCommon_H_
#define _LogCommon_H_

enum MCTreeCtrlCmd
{
	MCTCC_ADD_FOLDER = 1,
	MCTCC_ADD_FILE,
	MCTCC_DEL_FOLDER,
	MCTCC_DEL_FILE,
	MCTCC_RESET,
	MCTCC_COPY_FILE,
	MCTCC_LOAD_CSV,
	MCTCC_DUMP,
};

enum LogLevel
{
	LL_INFO = 0,
	LL_DEBUG,
	LL_ERROR,
};


#endif

