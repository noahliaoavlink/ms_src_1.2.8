#include "stdafx.h"
#include "ExceptionReport.h"

#include "../../../Include/StrConv.h"

#include "StackWalker.h"

#pragma comment(lib, "Dbghelp.lib")

#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

#define XA2T( a )		s2ws( a ).c_str()

#ifndef _printf
#define _printf printf
#endif

std::wstring FormateDateTime(time_t t) 
 { 
	 wchar_t szData[128] = {0}; 
	 tm* ptm = localtime(&t); 
	 wsprintfW(szData, L"%d-%.2d-%.2d %.2d-%.2d-%.2d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
	 return szData; 
 }

//BOOL GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild);
BOOL GetWinVer(wchar_t* pszVersion, int *nVersion, wchar_t* pszMajorMinorBuild);

//static void FormatTime(LPTSTR output, FILETIME TimeToPrint)
static void FormatTime(wchar_t* output, FILETIME TimeToPrint)
{
	output[0] = _T('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
		FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintfW(output, L"%d/%d/%d %02d:%02d:%02d",
			(Date / 32) & 15, Date & 31, (Date / 512) + 1980,
			(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}

VOID strapp (LPWSTR *dest, LPCWSTR source)
{
	SIZE_T length;
	LPWSTR temp;

	temp = *dest;
	length = wcslen(*dest) + wcslen(source);
	*dest = new WCHAR [length + 1];
	wcsncpy_s(*dest, length + 1, temp, _TRUNCATE);
	wcsncat_s(*dest, length + 1, source, _TRUNCATE);
	delete [] temp;
}

std::wstring StringToWString(const std::string& s)
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

std::wstring GetAllUserFolderPath(bool bIncludeSlash)
{
	std::wstring strPath(MAX_PATH, L'\0');
	//TCHAR szPath[MAX_PATH];
	wchar_t wszPath[MAX_PATH];

	if(SUCCEEDED(::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wszPath)))
		strPath = wszPath;

	strPath = strPath + L"\\" + StringToWString("MediaServer\\CrashRPT");

	CreateDirectoryW(strPath.c_str(), NULL);

	if (bIncludeSlash)
	{
		strPath += L'\\';
	}

	return strPath;
}

#if 0

#define WUNKNOWNSTR	_T("unknown Windows version")

#define W95STR			_T("Windows 95")
#define W95SP1STR		_T("Windows 95 SP1")
#define W95OSR2STR		_T("Windows 95 OSR2")
#define W98STR			_T("Windows 98")
#define W98SP1STR		_T("Windows 98 SP1")
#define W98SESTR		_T("Windows 98 SE")
#define WMESTR			_T("Windows ME")

#define WNT351STR		_T("Windows NT 3.51")
#define WNT4STR			_T("Windows NT 4")
#define W2KSTR			_T("Windows 2000")
#define WXPSTR			_T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")

#define WCESTR			_T("Windows CE")

#else

#define WUNKNOWNSTR	L"unknown Windows version"

#define W95STR			L"Windows 95"
#define W95SP1STR		L"Windows 95 SP1"
#define W95OSR2STR		L"Windows 95 OSR2"
#define W98STR			L"Windows 98"
#define W98SP1STR		L"Windows 98 SP1"
#define W98SESTR		L"Windows 98 SE"
#define WMESTR			L"Windows ME"

#define WNT351STR		L"Windows NT 3.51"
#define WNT4STR			L"Windows NT 4"
#define W2KSTR			L"Windows 2000"
#define WXPSTR			L"Windows XP"
#define W2003SERVERSTR	L"Windows 2003 Server"

#define WCESTR			L"Windows CE"

#endif

#define WUNKNOWN	0

#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99

#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003SERVER	105
#define WNTLAST		199

#define WCEFIRST	201
#define WCE			201
#define WCELAST		299

//BOOL GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild)
BOOL GetWinVer(wchar_t* pszVersion, int *nVersion, wchar_t* pszMajorMinorBuild)
{
	if (!pszVersion || !nVersion || !pszMajorMinorBuild)
		return FALSE;
	//lstrcpy(pszVersion, WUNKNOWNSTR);
	wcscpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	wsprintfW(pszMajorMinorBuild, L"%u.%u.%u", dwMajorVersion, dwMinorVersion, dwBuildNumber);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			//lstrcpy(pszVersion, W95STR);
			wcscpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			//lstrcpy(pszVersion, W95SP1STR);
			wcscpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			//lstrcpy(pszVersion, W95OSR2STR);
			wcscpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			//lstrcpy(pszVersion, W98STR);
			wcscpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			//lstrcpy(pszVersion, W98SP1STR);
			wcscpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			//lstrcpy(pszVersion, W98SESTR);
			wcscpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			//lstrcpy(pszVersion, WMESTR);
			wcscpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			//lstrcpy(pszVersion, WNT351STR);
			wcscpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			//lstrcpy(pszVersion, WNT4STR);
			wcscpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			//lstrcpy(pszVersion, W2KSTR);
			wcscpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			//lstrcpy(pszVersion, WXPSTR);
			wcscpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			//lstrcpy(pszVersion, W2003SERVERSTR);
			wcscpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
	}
	/*
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		lstrcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}
	*/
	return TRUE;
}

static HANDLE m_hProcess;

ExceptionReport::ExceptionReport()
{
	SetUnhandledExceptionFilter(WheatyUnhandledExceptionFilter);

	m_hProcess = GetCurrentProcess();
}

ExceptionReport::~ExceptionReport()
{
	//SetUnhandledExceptionFilter( m_previousFilter );
}

LONG WINAPI ExceptionReport::WheatyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo )
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER; 
	std::wstring strTime = FormateDateTime(time(NULL)); 
	std::wstring strRootPath = GetAllUserFolderPath(true); 
	std::wstring strFileName = strRootPath + L"crash" + strTime + L".txt"; 
	std::wstring strFileName1 = strRootPath + L"crash" + strTime + L".dmp"; 

	   HANDLE hFile = ::CreateFileW(  
		   strFileName1.c_str(),   
        GENERIC_WRITE,   
        0,   
        NULL,   
        CREATE_ALWAYS,   
        FILE_ATTRIBUTE_NORMAL,   
        NULL);  
    if(INVALID_HANDLE_VALUE != hFile)  
    {  
		MINIDUMP_EXCEPTION_INFORMATION einfo = {0};  
        einfo.ThreadId          = ::GetCurrentThreadId();  
        einfo.ExceptionPointers = pExceptionInfo;  
        einfo.ClientPointers    = NULL;  
          
        ::MiniDumpWriteDump(  
            ::GetCurrentProcess(),   
            ::GetCurrentProcessId(),   
            hFile,   
            MiniDumpNormal,   
            &einfo,   
            NULL,   
            NULL);  
        ::CloseHandle(hFile);  
     }  

#if 0
	FILE *fp;
	char* pFileName = WCharToChar(strFileName);
	fp = fopen(pFileName,"w+");
	if(fp)
	{
		GenerateExceptionReport( pExceptionInfo ,fp);
		fclose(fp);
	}
#else
	StackWalker sw;
	char* pFileName = WCharToChar(strFileName);
	sw.OpenFile(pFileName);
	FILE* fp = sw.GetFileHandle();
	DumpSystemInformation(fp);
	fprintf(fp,"//=====================================================\r\n");
	sw.ShowCallstack(GetCurrentThread(), pExceptionInfo->ContextRecord);
	sw.CloseFile();
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}

void ExceptionReport::DumpSystemInformation(FILE *fp)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	//TCHAR szTimeBuffer[100];
	wchar_t wszTimeBuffer[100];
	FormatTime(wszTimeBuffer, CurrentTime);

	fprintf(fp,"Error occurred at %s.\r\n", WCharToChar(wszTimeBuffer));

	//TCHAR szModuleName[MAX_PATH*2];
	wchar_t wszModuleName[MAX_PATH * 2];
	ZeroMemory(wszModuleName, sizeof(wszModuleName));
	if (GetModuleFileNameW(0, wszModuleName, _countof(wszModuleName)-2) <= 0)
		//lstrcpy(szModuleName, _T("Unknown"));
		wcscpy(wszModuleName, L"Unknown");

	//TCHAR szUserName[200];
	wchar_t wszUserName[200];
	ZeroMemory(wszUserName, sizeof(wszUserName));
	DWORD UserNameSize = _countof(wszUserName)-2;
	if (!GetUserNameW(wszUserName, &UserNameSize))
		//lstrcpy(wszUserName, _T("Unknown"));
		wcscpy(wszUserName, L"Unknown");

	char szModuleName1[200];
	char szUserName1[200];
	strcpy(szModuleName1,WCharToChar(wszModuleName));
	strcpy(szUserName1,WCharToChar(wszUserName));
	_tprintf( _T("%s, run by %s.\r\n"), szModuleName1, szUserName1);

	// print out operating system
	//TCHAR szWinVer[50], szMajorMinorBuild[50];
	wchar_t wszWinVer[50], wszMajorMinorBuild[50];
	int nWinVer;
	GetWinVer(wszWinVer, &nWinVer, wszMajorMinorBuild);

	char szWinVer1[50];
	char szMajorMinorBuild1[50];
	strcpy(szWinVer1,WCharToChar(wszWinVer));
	strcpy(szMajorMinorBuild1,WCharToChar(wszMajorMinorBuild));
	fprintf(fp,"Operating system:  %s (%s).\r\n", 
		szWinVer1, szMajorMinorBuild1);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	fprintf(fp,"%d processor(s), type %d.\r\n",
		SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	fprintf(fp,"%d%% memory in use.\r\n", MemInfo.dwMemoryLoad);
	fprintf(fp,"%d MBytes physical memory.\r\n", (MemInfo.dwTotalPhys +
		ONEM - 1) / ONEM);
	fprintf(fp,"%d MBytes physical memory free.\r\n", 
		(MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	fprintf(fp,"%d MBytes paging file.\r\n", (MemInfo.dwTotalPageFile +
		ONEM - 1) / ONEM);
	fprintf(fp,"%d MBytes paging file free.\r\n", 
		(MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	fprintf(fp,"%d MBytes user address space.\r\n", 
		(MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	fprintf(fp,"%d MBytes user address space free.\r\n", 
		(MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
}

std::wstring ExceptionReport::GetFolderPath()
{
	return GetAllUserFolderPath(true);
}