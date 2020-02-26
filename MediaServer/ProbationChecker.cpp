#include "stdafx.h"
#include "ProbationChecker.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/sstring.h"

typedef struct
{
	unsigned short usYear;
	unsigned short usMonth;
	//unsigned short usDayOfWeek; 
	unsigned short usDay;
	unsigned short usHour;
	unsigned short usMinute;
	unsigned short usSecond;
	unsigned short usMilliseconds;
}TimeInfo;

__inline long CalTimeOffset1(TimeInfo* pTime1, TimeInfo* pTime2, long lDuration)
{
	long lOffset;

	COleDateTime Time1(pTime1->usYear, pTime1->usMonth, pTime1->usDay, pTime1->usHour, pTime1->usMinute, pTime1->usSecond);
	COleDateTime Time2(pTime2->usYear, pTime2->usMonth, pTime2->usDay, pTime2->usHour, pTime2->usMinute, pTime2->usSecond);
	COleDateTimeSpan ts = Time2 - Time1;
	long lSecondOffset = ts.GetTotalSeconds() + lDuration;
	return lSecondOffset;
}

__inline void SysTimeToTimeInfo(LPSYSTEMTIME lpSystemTime, TimeInfo* pTimeInfo)
{
	pTimeInfo->usYear = lpSystemTime->wYear;
	pTimeInfo->usMonth = lpSystemTime->wMonth;
	pTimeInfo->usDay = lpSystemTime->wDay;
	pTimeInfo->usHour = lpSystemTime->wHour;
	pTimeInfo->usMinute = lpSystemTime->wMinute;
	pTimeInfo->usSecond = lpSystemTime->wSecond;
	pTimeInfo->usMilliseconds = lpSystemTime->wMilliseconds;
}

ProbationChecker::ProbationChecker()
{

}

ProbationChecker::~ProbationChecker()
{

}

char* ProbationChecker::Encrypt(char *str)
{
	int iLen = strlen(str);
	for (int i = 0; i < iLen; i++)
	{
		m_szEncryptRet[i] = str[i] + 13;
	}

	m_szEncryptRet[iLen] = '\0';

	return m_szEncryptRet;
}

char* ProbationChecker::Decrypt(char *str)
{
	int iLen = 0;
	for (int i = 0; i < 100 && str[i] != '\0'; i++)
	{
		m_szDecryptRet[i] = str[i] - 13;
		iLen++;
	}

	m_szDecryptRet[iLen] = '\0';
	return m_szDecryptRet;
}

bool ProbationChecker::CheckPeriodDate()
{
//	char szDate[64];
	char szTmpDate[64];
	char szEnStr[64];
	char szDeStr[64];

	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	

	SYSTEMTIME st;
	TimeInfo CurTime;   //current
	TimeInfo PeriodDate = { 2019,07,29,0,0,0,0 };  //3,31
	int iTmpPeriodDate[6];

	long lTrailDate = 60 * 60 * 24 * 15;// 15 days

	GetLocalTime(&st);

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName,"%s\\BV.dat", szAPPath);

	ReadFromFile(szFileName);
	
	if (strcmp(m_szDateTime, "") != 0)
	{
		strcpy(szTmpDate, m_szDateTime);

		char* pszDeStr = Decrypt(szTmpDate);
		int iLen2 = strlen(pszDeStr);

		strcpy(szDeStr, pszDeStr);

		for (int i = 0; i < 6; i++)
			iTmpPeriodDate[i] = GetNextNumberFromString(',', szDeStr);

		PeriodDate.usYear = iTmpPeriodDate[0];
		PeriodDate.usMonth = iTmpPeriodDate[1];
		PeriodDate.usDay = iTmpPeriodDate[2];
		PeriodDate.usHour = iTmpPeriodDate[3];
		PeriodDate.usMinute = iTmpPeriodDate[4];
		PeriodDate.usSecond = iTmpPeriodDate[5];
		PeriodDate.usMilliseconds = 0;
	}
	else
	{
		char szCurDateTime[64];
		sprintf(szCurDateTime, "%d,%d,%d,%d,%d,%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		//sprintf(szCurDateTime, "%d,%d,%d,%d,%d,%d", st.wYear, st.wMonth, 15, st.wHour, st.wMinute, st.wSecond);

		strcpy(szTmpDate, szCurDateTime);
		char* pszEnStr = Encrypt(szTmpDate);
		strcpy(szEnStr, pszEnStr);

		WriteToFile(szFileName, szEnStr);

		return true;

	}

	SysTimeToTimeInfo(&st, &CurTime);

	long lTimeOffset = CalTimeOffset1(&CurTime, &PeriodDate, lTrailDate);
	if (lTimeOffset > 0)
		return true;
	return false;
}

void ProbationChecker::ReadFromFile(char* szFileName)
{
	FILE* m_pFile;
	unsigned char ucLen;
	m_pFile = fopen(szFileName, "r+t");
	strcpy(m_szDateTime,"");
	if (m_pFile)
	{
		int iReadLen1 = fread(m_szTmp, 1, 128, m_pFile);

		fread((void*)&ucLen, 1, sizeof(unsigned char), m_pFile);

		int iReadLen2 = fread((void*)m_szDateTime, 1, ucLen, m_pFile);
		fclose(m_pFile);
	}
}

void ProbationChecker::WriteToFile(char* szFileName,char* szData)
{
	FILE* m_pFile;
	int i;
	m_pFile = fopen(szFileName, "w+t");
	if (m_pFile)
	{
		for (i = 0; i < 128; i++)
			m_szTmp[i] = rand() % 255;

		m_szTmp[127] = '\0';

		int iwlen = fwrite(m_szTmp, 1, 128, m_pFile);

		unsigned char ucLen = strlen(szData);

		fwrite(&ucLen, 1, sizeof(unsigned char), m_pFile);

		int iwlen2 = fwrite(szData, 1, strlen(szData), m_pFile);

		for (i = 0; i < 128; i++)
			m_szTmp[i] = rand() % 255;

		iwlen = fwrite(m_szTmp, 1, 128, m_pFile);

		fclose(m_pFile);
	}
}