#ifndef _ShellCommon_H
#define _ShellCommon_H

#include "SString.h"
#include "Iphlpapi.h"

#define CSIDL_PROFILE		0x0028
#define CSIDL_PERSONAL 0x0005
#define CSIDL_COMMON_APPDATA          0x0023
__inline int GetFolderPath(int iFID,char* szOutPath)
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	int iRet = 0;

	if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,iFID,&pidl))) 
		{
			SHGetPathFromIDListA(pidl, szOutPath);
			pShellMalloc->Free(pidl);
			iRet = 1;
		}
		else
			iRet = -2;
		pShellMalloc->Release();
	}
	else 
		iRet = -1;
	return iRet;
}

__inline bool SelFolder(HWND hWnd,char* szPath,char* szTitle)
{
	bool bRet = false;
	LPMALLOC	pMalloc;
    // Gets the Shell's default allocator
    if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
        BROWSEINFOA bi;
        char pszBuffer[MAX_PATH];
        LPITEMIDLIST pidl;

        bi.hwndOwner = hWnd;
        bi.pidlRoot = NULL;
        bi.pszDisplayName = pszBuffer;
        bi.lpszTitle = szTitle;//_T(szTitle); //_T("Select a directory...");
        bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS| 0x0040;//0x0040為可新增資料匣
        bi.lpfn = NULL;
        bi.lParam = 0;

        // This next call issues the dialog box.
        if ((pidl = ::SHBrowseForFolderA(&bi)) != NULL)
        {
            if (::SHGetPathFromIDListA(pidl, pszBuffer))
            { 
	            // At this point pszBuffer contains the selected path
				strcpy(szPath,pszBuffer);
				bRet = true;
			}

            // Free the PIDL allocated by SHBrowseForFolder.
            pMalloc->Free(pidl);
		}
        // Release the shell's allocator.
        pMalloc->Release();
    }
    return bRet;
}

__inline void MakeDir(char* szPath)
{
	int iCurLen = 0;
	char szTempFolder[256];
	char szNewFolder[256];
    strcpy(szTempFolder,szPath);
	bool bDo = true;
	while(bDo)
	{
	    char* pTemp = strchr(szTempFolder,'/');

		if(!pTemp)
			pTemp = strchr(szTempFolder,'\\');

		if(pTemp)
		{
	       iCurLen += strlen(szTempFolder) - strlen(pTemp) + 1;

	       memcpy(szNewFolder,szPath,iCurLen);
		   szNewFolder[iCurLen] = '\0';
		   strcpy(szTempFolder,pTemp + 1);
		}

	    if(!pTemp || strlen(szTempFolder) == 0)
		{
		    CreateDirectoryA(szPath,NULL);
		    bDo = false;
		}
	    else
		    CreateDirectoryA(szNewFolder,NULL);
	}
}

__inline bool FolderIsExist(char* szFolder)
{
	char szPath[256];
	WIN32_FIND_DATAA FindData;
	FindData.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
	sprintf(szPath,"%s\\*.*",szFolder);
	HANDLE hFind = FindFirstFileA(szPath,&FindData);
	if(hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

__inline void Shutdown(bool bReboot = true)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp;
    OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken);
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if(bReboot)
		ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
	else
		ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0);	
}

__inline void AutoLogon(char* szUserName,char* szPassword)
{
	HKEY  hKey = NULL;
	char  szSession[256];	
	char szDefaultName[256] = "";      //電腦的帳號
	char szDefaultPasswd[256] = "";    //電腦的密碼
	char szAPPath[256];
	char szPath[256];
	DWORD dwType,dwSizeOfData = sizeof(szDefaultName);
	bool bAutoLogon = true;

	strcpy(szDefaultName,szUserName);
	strcpy(szDefaultPasswd,szPassword);

	strcpy(szSession,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
	if (ERROR_SUCCESS == RegCreateKeyA(HKEY_LOCAL_MACHINE,szSession,&hKey))
	{
		if (bAutoLogon)
		{
			long lRet = RegSetValueExA(hKey,"DefaultUserName",0,REG_SZ,(LPBYTE)szDefaultName,strlen(szDefaultName));
			
			if(lRet != ERROR_SUCCESS)
			{
				int kk;
				kk = 0;
			}
			
			lRet = RegSetValueExA(hKey,"DefaultPassword",0,REG_SZ,(LPBYTE)szDefaultPasswd,strlen(szDefaultPasswd));

			lRet = RegSetValueExA(hKey,"AutoAdminLogon",0,REG_SZ,(LPBYTE)"1",strlen("1"));
			lRet = RegSetValueExA(hKey,"AutoLogonCount",0,REG_SZ,(LPBYTE)"7",strlen("7"));
		}
		else
		{
			char szname[32];
			RegQueryValueExA(hKey,"AltDefaultUserName",0,&dwType,(LPBYTE)szname,&dwSizeOfData);
			RegSetValueExA(hKey,"DefaultUserName",0,REG_SZ,(LPBYTE)szname,strlen(szname));
			RegDeleteValueA(hKey,"DefaultPassword");
		}
		RegCloseKey(hKey);
	}

#if 0
	GetExecutionPath(NULL,szAPPath);
	sprintf(szPath,"%s\\WiseNVR.exe",szAPPath);
#else
	GetModuleFileNameA(NULL,szAPPath,512);
#endif
	//strcpy(szSession,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
	strcpy(szSession,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    
	if(ERROR_SUCCESS == RegOpenKeyA(HKEY_LOCAL_MACHINE,szSession,&hKey))
	{
		RegSetValueExA(hKey,"Reboot",0,REG_SZ,(LPBYTE)szPath,strlen(szPath));
	    RegCloseKey(hKey);
	}
}

__inline void RemoveReboot()
{
	HKEY  hKey = NULL;
	char  szSession[256];	
	char szAPPath[256];
	char szPath[256];

	GetExecutionPath(NULL,szAPPath);
	//strcpy(szSession,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
	strcpy(szSession,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    sprintf(szPath,"%s\\WiseNVR.exe",szAPPath);
	if(ERROR_SUCCESS == RegOpenKeyA(HKEY_LOCAL_MACHINE,szSession,&hKey))
	{
		RegDeleteValueA(hKey,"Reboot");
		//RegSetValueEx(hKey,"Reboot",0,REG_SZ,(LPBYTE)szPath,strlen(szPath));
	    RegCloseKey(hKey);
	}
}

//#define _WIN32_WINNT 0x0500
typedef void (*ProgressCallbackProc)(int iPos);

__inline DWORD CALLBACK CopyProgressRoutine(
           LARGE_INTEGER TotalFileSize,          // file size
           LARGE_INTEGER TotalBytesTransferred,  // bytes transferred
           LARGE_INTEGER StreamSize,             // bytes in stream
           LARGE_INTEGER StreamBytesTransferred, // bytes transferred for stream
           DWORD dwStreamNumber,                 // current stream
           DWORD dwCallbackReason,               // callback reason
           HANDLE hSourceFile,                   // handle to source file
           HANDLE hDestinationFile,              // handle to destination file
           LPVOID lpData                         // from CopyFileEx
)
{
	static int g_iCurPos;
	static int g_iOldPos;
	static int g_iTotal;
	static int g_iCount;
	if ( dwCallbackReason == CALLBACK_STREAM_SWITCH )
	{
		g_iCount = 0;
		g_iOldPos = -1;
		DWORD nRange = TotalFileSize.LowPart / (1024*64);
		g_iTotal = nRange;
//		TRACE("nRange:%d\n",nRange);
	}
	else 
	{
		++g_iCount;
		g_iCurPos = g_iCount * 100 / g_iTotal;
		if(g_iOldPos != g_iCurPos)
		{
			g_iOldPos = g_iCurPos;
			((ProgressCallbackProc)lpData)(g_iCurPos);
		}
	}

	return PROGRESS_CONTINUE;
}

__inline void _CopyFile_Callback(char* szSrcFile,char* szDestFile,ProgressCallbackProc pProc)
{
	int Cancel = FALSE;
	CopyFileExA(szSrcFile,szDestFile,(LPPROGRESS_ROUTINE)CopyProgressRoutine,pProc,&Cancel,COPY_FILE_FAIL_IF_EXISTS);
}

/*
sample code

#pragma comment(lib, "../../Lib/iphlpapi.lib")
	char* szMACData[10];
	for(int i = 0;i < 10;i++)
		szMACData[i] = new char[100];

	int GetMACaddress(char** szMACData);
	int iRet = GetMACaddress(szMACData);

	for(i = 0;i < 10;i++)
		delete szMACData[i];
*/
__inline int GetMACaddress(char** szMACData)
{
	int iCount = 0;
	IP_ADAPTER_INFO AdapterInfo[16]; 
	DWORD dwBufLen = sizeof(AdapterInfo);
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);
    ASSERT(dwStatus == ERROR_SUCCESS);
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    do 
	{
		sprintf(szMACData[iCount],"%02X-%02X-%02X-%02X-%02X-%02X",
			pAdapterInfo->Address[0],
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5]);
		pAdapterInfo = pAdapterInfo->Next;
		iCount++;
	}
	while(pAdapterInfo);

	return iCount;
}

__inline int GetIPAddress(char** szIPData)
{
	int iCount = 0;
	IP_ADAPTER_INFO AdapterInfo[16]; 
	DWORD dwBufLen = sizeof(AdapterInfo);
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);
    ASSERT(dwStatus == ERROR_SUCCESS);
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    do 
	{
		strcpy(szIPData[iCount],pAdapterInfo->IpAddressList.IpAddress.String);
		pAdapterInfo = pAdapterInfo->Next;
		iCount++;
	}
	while(pAdapterInfo);

	return iCount;
}
#endif