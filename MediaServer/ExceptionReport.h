#ifndef _ExceptionReport_H_
#define _ExceptionReport_H_

#include <iostream>
#include <string>
#include <DbgHelp.h>
#include <TlHelp32.h>

class ExceptionReport
{
//		static HANDLE m_hProcess;
   protected:
   public:
		ExceptionReport();
        ~ExceptionReport();

		static LONG WINAPI WheatyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo );
		static void DumpSystemInformation(FILE *fp);
		static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
		//static HANDLE m_hReportFile;

		std::wstring GetFolderPath();
};

#endif