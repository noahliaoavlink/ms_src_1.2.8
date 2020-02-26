#ifndef _StrConv_H
#define _StrConv_H

#include <string>

__inline std::string MultiByteToUtf8( const char* pszMultiByte )  
{  
    std::string strUtf8;  
  
    if( NULL == pszMultiByte )  
    {  
        return strUtf8;  
    }  
  
    int iNumCharacter = 0;  
  
    //convert from MultiByte to WideChar  
    std::wstring wcsText;  
    iNumCharacter = MultiByteToWideChar( CP_ACP, 0, pszMultiByte, -1, NULL, 0 );  
    wcsText.resize( iNumCharacter + 1 );//addition 1 for '\0'  
  
    MultiByteToWideChar( CP_ACP, 0, pszMultiByte, -1, &wcsText[ 0 ], iNumCharacter );  
  
    //convert from WideChar to utf8  
    iNumCharacter = WideCharToMultiByte( CP_UTF8, 0, &wcsText[ 0 ], -1, NULL, 0, NULL, NULL );  
    strUtf8.resize( iNumCharacter + 1 );  
    WideCharToMultiByte( CP_UTF8, 0, &wcsText[ 0 ], -1, &strUtf8[ 0 ], iNumCharacter, NULL, NULL );  
  
    return strUtf8;  
} 

__inline char* WCharToChar(const std::wstring& str)  
{  
    static char pElementText[1024];  
     int    iTextLen;  
     // wide char to multi char  
     iTextLen = WideCharToMultiByte( CP_ACP,  
             0,  
             str.c_str(),  
             -1,  
             NULL,  
             0,  
    NULL,  
             NULL );    
     memset( ( void* )pElementText, 0, 1024);  
     ::WideCharToMultiByte( CP_ACP,  
             0,  
             str.c_str(),  
             -1,  
             pElementText,  
             iTextLen,  
             NULL,  
             NULL );    
     return pElementText; 
} 

__inline std::string UTF8ToMultiByte(std::string szUTF8)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, szUTF8.c_str(), -1, NULL, 0);
	wchar_t *pWStr = new wchar_t[size+1];
	MultiByteToWideChar(CP_UTF8, 0, szUTF8.c_str(), -1, pWStr, size);
	int ansiSize = WideCharToMultiByte(CP_ACP, 0, pWStr, -1, NULL, 0, NULL, false);
	char* ansiStr = new char[ansiSize+1];
	WideCharToMultiByte(CP_ACP, 0, pWStr, -1, ansiStr, ansiSize, NULL, false);

	std::string szRet(ansiStr);

	delete[] pWStr;
	delete[] ansiStr;

	return szRet;
}

__inline wchar_t* ANSIToUnicode( const char* str )  
{  
	 int  len = 0;  
	 len = strlen(str);  
	 int  unicodeLen = ::MultiByteToWideChar( CP_ACP,  
				0,  
				str,  
				-1,  
				NULL,  
				0 );    
	 wchar_t pUnicode[512];    
	 //pUnicode = new  wchar_t[unicodeLen+1];    
	 //memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));    
	 ::MultiByteToWideChar( CP_ACP,  
			 0,  
			 str,  
			 -1,  
			 (LPWSTR)pUnicode,  
			 unicodeLen );  
			 
	 //wstring  rt;    
	 //rt = ( wchar_t* )pUnicode;  
	 //delete  pUnicode;  
	 pUnicode[unicodeLen] = '\0'; 
	   
	 return  pUnicode;    
}

__inline std::string ws2s(const std::wstring& wstr)
{
	//setlocale(LC_CTYPE, "en_US.utf-8");

    int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), 0, 0, 0, 0); 
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), &strTo[0], size_needed, 0, 0); 

    return strTo;
}

__inline wchar_t* UTF8ToUnicode(const char* pu8)  
{  
	wchar_t pUnicode[512];

	int Length = MultiByteToWideChar(CP_UTF8, 0, pu8, -1, NULL, NULL);

	/*
    // convert an UTF8 string to widechar   
    int nUtf8Len = MultiByteToWideChar(CP_UTF8, 0, pu8, -1, NULL, 0);  
    if (nUtf8Len <=0)  
    {  
        return 0;  
    }  

	int  iUnicodeLen = ::MultiByteToWideChar( CP_UTF8, 0,  pu8, nUtf8Len, NULL, 0 );
	*/
    //pun.resize(nLen);  
    int nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, -1, pUnicode, Length);  
	//MultiByteToWideChar(CP_UTF8, 0, InputString, -1, OutputString, Length);
  
    if(nRtn != Length)  
    {  
   //     pun.clear();  
        return 0;  
    }  
  
    return pUnicode;  
} 

__inline std::string UnicodeToUTF8(const wchar_t* pun)  
{  
    std::string strUtf8; 
    // convert an widechar string to utf8  
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, pun, -1, NULL, 0, NULL, NULL);  
    if (utf8Len<=0)  
    {  
        return "";  
    }  
    strUtf8.resize(utf8Len);  
    int nRtn = WideCharToMultiByte(CP_UTF8, 0, pun, -1, &*strUtf8.begin(), utf8Len, NULL, NULL);  
  
    if (nRtn != utf8Len)  
    {  
        strUtf8.clear();  
        return "";  
    }  
    return strUtf8;
}  
/*
//WideCharToMultiByte( CP_UTF8, 0, &wcsText[ 0 ], -1, &strUtf8[ 0 ], iNumCharacter, NULL, NULL ); 
#ifdef _WINDOWS_
__inline void CStringToChar(char* szOut,CString csInStr)
{
	int length;
	length = csInStr.GetLength(); 

	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK|WC_DEFAULTCHAR, csInStr.GetBuffer(length), 
                    length+1, szOut, 1024, 0, 0);
}
#endif
*/
#endif