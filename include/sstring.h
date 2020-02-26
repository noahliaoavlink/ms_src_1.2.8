#ifndef __SString_H
#define __SString_H
#include <string.h>
#include <stdlib.h>
#include <shlobj.h>
#include <stdio.h>

#include <shlobj.h>

#include "Winver.h"
#pragma comment(lib, "Version.lib")

#define _STR_BUFFER_SIZE 2048
static char szResult[_STR_BUFFER_SIZE];
static wchar_t wszResult[_STR_BUFFER_SIZE];

__inline char* RemoveChar(char* szData,char cChar,bool bAll = false)
{
	int iCount = 0;
	strcpy(szResult,"");
	int iTotal = strlen(szData);

	bool bRemove = true;

	for(int i = 0;i < iTotal;i++)
	{
		if(bAll)
		{
		    if(szData[i]  != cChar)
			   szResult[iCount++] = szData[i];
		}
		else
		{
            if(bRemove)
			{
				if(szData[i]  != cChar)
				{
					szResult[iCount++] = szData[i];
					bRemove = false;
				}
			}
			else 
			    szResult[iCount++] = szData[i];
		}
	}

	if(iCount > 0)
		szResult[iCount] = '\0';

	return szResult;
}

__inline wchar_t* RemoveCharW(wchar_t* wszData,wchar_t cChar,bool bAll = false)
{
	int iCount = 0;
	wcscpy(wszResult,L"");
	int iTotal = wcslen(wszData);

	bool bRemove = true;

	for(int i = 0;i < iTotal;i++)
	{
		if(bAll)
		{
		    if(wszData[i]  != cChar)
			   wszResult[iCount++] = wszData[i];
		}
		else
		{
            if(bRemove)
			{
				if(wszData[i]  != cChar)
				{
					wszResult[iCount++] = wszData[i];
					bRemove = false;
				}
			}
			else 
			    wszResult[iCount++] = wszData[i];
		}
	}

	if(iCount > 0)
		wszResult[iCount] = L'\0';

	return wszResult;
}

__inline bool FindSubStr(char* str,char* substr)
{
	char* szData = strstr(str,substr);
	if(szData)
		return true;
	return false;
}

__inline bool StrCompare(char* str1,char* str2,int iLen = -1)
{
	char szData[256];
	if(iLen == -1)
		strcpy(szData,str1);
	else
	{
	   strncpy(szData,str1,iLen);
	   szData[iLen] = '\0';
	}

	if(strcmp(szData,str2) == 0)
		return true;
	return false;
}

__inline bool IsEmpty(char* szData)
{
	if(strcmp(szData,"") == 0 || szData == 0)
		return true;
	return false;
}

__inline float GetValuef(char* szData)
{
    return (float)atof(szData);
}

__inline int GetValuei(char* szData)
{
    return atoi(szData);
}

__inline long GetValuel(char* szData)
{
    return atol(szData);
}

__inline bool GetValueb(char* szData)
{
   if(atoi(szData))
	   return true;
   return false;
}

__inline long GetValueul(char* szData)
{
    return (unsigned long)atol(szData);
}

__inline long Str16Tol(char* szText)
{
	static long g_lValue;
    char szStop[20];
	g_lValue = strtol( szText, (char**)szStop, 16);
	return g_lValue;
}

__inline char* GetCurDir(char* szPath)
{
	static char szCurDir[256];
	char* szFile = strrchr(szPath,'\\');
	if(!szFile)
	   szFile = strrchr(szPath,'/');

	int iCurLen = strlen(szPath) - strlen(szFile) + 1;
	memcpy(szCurDir,szPath,iCurLen);
	szCurDir[iCurLen] = '\0';
	return szCurDir;
}

__inline void StrCpy(char* szDest,char* szSrc)
{
     int iLen = strlen(szSrc);	
     memcpy(szDest,szSrc,iLen);
     szDest[iLen] = '\0';
}

__inline void ReplaceChar(char* szData,char cOld,char cNew,int iMaxSize = 256)
{
	char* szCurStr = new char[strlen(szData) + 1];
	strcpy(szCurStr,szData);
	int* iPos = new int[iMaxSize];
	int iTotal = 0;
	int iLen = strlen(szData);
	bool bDo = true;
	while(bDo)
	{
		//char* pTemp = strrchr(szCurStr,cOld);
		char* pTemp = strchr(szCurStr,cOld);
		if(pTemp)
		{
			iPos[iTotal++] = strlen(szData) - strlen(pTemp);
			strcpy(szCurStr,pTemp + 1);
		}
		else
			bDo = false;
	}

	int iCount = 0;
	for(int i = 0;i < iLen;i++)
	{
		if(i == iPos[iCount])
		{
		   szData[i] = cNew;
		   if(iCount < iTotal)
		      iCount++;
		}
	}

	delete szCurStr;
	delete iPos;
}

__inline long GetNextNumberFromString(char ch,char* szSrc)
{
	char szNumber[20];
	int iNumber = 0;
	char* pTemp = strchr(szSrc,ch);
	if(pTemp)
	{
	   int iLen = strlen(szSrc) - strlen(pTemp);
	   memcpy(szNumber,szSrc,iLen);
	   szNumber[iLen] = '\0';
	   iNumber = atol(szNumber);

	   if(strlen(pTemp) > 1)
	       strcpy(szSrc,pTemp + 1);
	   else 
		   strcpy(szSrc,"");
	   return iNumber;
	}
	else
	{
		iNumber = atol(szSrc);
		strcpy(szSrc,"");
		return iNumber;
	}
}

__inline char* GetNextPattern(char* szString,char ch,int* iIndex)
{
	int iCount = 0;
	static char szPattern[100];
    int iLen = strlen(szString);
	bool bFind = false;
	for(int i = *iIndex;i < iLen;i++)
	{
		if(szString[i] == ch && bFind)
		{
			szPattern[iCount] = '\0';
			*iIndex = i + 1;
	        return szPattern;
		}
		else if(szString[i] == ch && !bFind)
            bFind = true;
		else if(bFind)
			szPattern[iCount++] = szString[i];
	}
    return 0;
}

#if 0
__inline wchar_t* GetNextPatternW(wchar_t* wszString,wchar_t ch,int* iIndex)
{
	//wchar_t* pch = wcsrchr(wszFileName,'.');
	
	wchar_t wszNumber[64];
	int iNumber = 0;
	wchar_t* pTemp = wcsrchr(wszString,ch);
	if(pTemp)
	{
	   int iLen = wcslen(szSrc) - wcslen(pTemp);
	   memcpy(wszNumber,wszString,iLen);
	   wszNumber[iLen] = '\0';
	   /*
	   iNumber = atol(szNumber);
		*/
	   if(strlen(pTemp) > 1)
	       wcscpy(szSrc,pTemp + 1);
	   else 
		   wcscpy(szSrc,L"");
	   return wszNumber;
	}
	else
	{
		//iNumber = atol(szSrc);
		wcscpy(szSrc,L"");
		return wszNumber;
	}
	
	/*
	int iCount = 0;
	static wchar_t wszPattern[10];
    int iLen = wcslen(wszString);
	bool bFind = false;
	for(int i = *iIndex;i < iLen;i++)
	{
		if(wszString[i] == ch && bFind)
		{
			wszPattern[iCount] = '\0';
			*iIndex = i + 1;
	        return wszPattern;
		}
		else if(wszString[i] == ch && !bFind)
            bFind = true;
		else if(bFind)
			wszPattern[iCount++] = wszString[i];
	}
    return 0;
	*/
}
#endif

__inline char* InsteadStr(char* szString,char* szPattern,char* szNewData)
{
	static char szResult[512];
	char szTemp[512];
	
	int iPatternLen = strlen(szPattern);
	int iPatternCount = 0;
	int iBegin = -1;
	int iEnd = -1;
	int iCount = 0;
	int iBufferLen = 0;
	int iNewDataLen = strlen(szNewData);
	if(FindSubStr(szString,szPattern))
	{
        int iLen = strlen(szString);
		for(int i = 0;i < iLen;i++)
		{
            if(iBegin == -1 && szString[i] == szPattern[0])
			{
				iPatternCount++;
				iBegin = i;
			}
			else if(iBegin != -1 && szString[i] == szPattern[iPatternCount])
			{
				iPatternCount++;
				if(iPatternCount == iPatternLen)
				{
					iEnd = i;
					iBegin = -1;
                    iPatternCount = 0;
				}
			}
			szTemp[iCount++] = szString[i];

			if(iEnd != -1)
			{
				int iTemp = iCount - iPatternLen;
				memcpy(szResult + iBufferLen,szTemp,iTemp);
				iBufferLen += iTemp;
				memcpy(szResult + iBufferLen,szNewData,iNewDataLen);
				iBufferLen += iNewDataLen;
				iCount = 0;
                iEnd = -1;
			}
		}

		if(iCount > 0)
		{
            memcpy(szResult + iBufferLen,szTemp,iCount);
			iBufferLen += iCount;
			szResult[iBufferLen] = '\0';
		}
		return szResult;
	}
	strcpy(szResult,szString);
	return szResult;
}

__inline void GetExecutionPath(HMODULE hModule,char* szPath)
{
	char szExecutionPath[256];
	char szFullPath[256];
    GetModuleFileNameA(hModule,szFullPath,256);//AfxGetInstanceHandle()

	char* pTemp = strrchr(szFullPath,'\\');
	int iLen = strlen(szFullPath) - strlen(pTemp);
	memcpy(szExecutionPath,szFullPath,iLen);
	szExecutionPath[iLen] = '\0';

	strcpy(szPath,szExecutionPath);
}

__inline int ScanData(char* pBuffer,char* szPattern,char* szOut)
{
	int iIndex = -1;
	char szNewline[10] = "\n";
	char szCR[10] = "\r\n";
	char szLineData[2048];
	char* pTemp = strstr(pBuffer,szPattern);
	
	if(pTemp)
	{
		iIndex = strlen(pBuffer) - strlen(pTemp);
		char* pCR = strstr(pTemp,szCR);
		if(pCR)
		{
			int iLen = strlen(pTemp) - strlen(pCR);
			memcpy(szLineData,pTemp,iLen);
			szLineData[iLen] = '\0';
			strcpy(szOut,szLineData + strlen(szPattern));
			iIndex += strlen(szLineData) + 1;
		}
		else
		{
			char* pNL = strstr(pTemp,szNewline);
			if(pNL)
			{
				int iLen = strlen(pTemp) - strlen(pNL);
				memcpy(szLineData,pTemp,iLen);
				szLineData[iLen] = '\0';
				strcpy(szOut,szLineData + strlen(szPattern));
				iIndex += strlen(szLineData) + 1;
			}
			else
			{
					strcpy(szOut,pTemp + strlen(szPattern));
					iIndex += strlen(szPattern);
			}
		}
	}
	return iIndex;
}

__inline bool ReNameFile(char* szSrc,char* szDest)
{
	//if(CopyFile(szSrc,szDest, false))
	if(MoveFileA(szSrc,szDest))
	{
		DeleteFileA(szSrc);
		return true;
	}
	return false;
}

__inline char* GetFileName(char* szFullPath)
{
	char* pch = strrchr(szFullPath,'\\');
	if(pch)
	{
		return pch + 1;
	}
	else
	{
		pch = strrchr(szFullPath,'/');
		if(pch)
			return pch + 1;
	}
	return 0;
}

__inline void GetSysRoot(char* szOut)
{
	char szWinDir[256];
	char szRoot[10];
	GetWindowsDirectoryA(szWinDir,256);
	memcpy(szRoot,szWinDir,2);
	szRoot[2] = '\0';

	strcpy(szOut,szRoot);
}

__inline char* _GetFileVersion(char* szAPPath)
{
	static char g_szVersion[256];
	char szSubBlock[256];
//	SYSTEMTIME filetime;
	DWORD	dwTransInfo;
	LPVOID	lpVoid;
	unsigned int uiSize = GetFileVersionInfoSizeA(szAPPath,0);
	VS_FIXEDFILEINFO	fixedFileInfo;
	LPBYTE pVersionInfo = new BYTE[uiSize];
	LPVOID	lpInfo;
	GetFileVersionInfoA(szAPPath,0,uiSize,pVersionInfo);

	::VerQueryValueA(pVersionInfo, "\\", &lpVoid, &uiSize);
	memcpy(&fixedFileInfo, lpVoid, uiSize);

	::VerQueryValueA(pVersionInfo,"\\VarFileInfo\\Translation",&lpVoid, &uiSize);
	memcpy(&dwTransInfo, lpVoid, sizeof(DWORD));

	sprintf(szSubBlock,"\\StringFileInfo\\%04x%04x\\%s",LOWORD(dwTransInfo),HIWORD(dwTransInfo),"FileVersion");
	::VerQueryValueA(pVersionInfo,szSubBlock,&lpInfo,&uiSize);

	strcpy(g_szVersion,(char*)lpInfo);

	delete pVersionInfo;
	return g_szVersion;
}
/*
#define CSIDL_PROFILE		0x0028
#define CSIDL_PERSONAL 0x0005
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
*/
#endif 