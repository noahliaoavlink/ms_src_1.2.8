#pragma once

#ifndef _ProbationChecker_H_
#define _ProbationChecker_H_

class ProbationChecker
{
	char m_szDecryptRet[256];
	char m_szEncryptRet[256];

	char m_szTmp[128];
	char m_szDateTime[256];
protected:
public:
	ProbationChecker();
	~ProbationChecker();

	bool CheckPeriodDate();
	char* Encrypt(char *str);
	char* Decrypt(char *str);
	void ReadFromFile(char* szFileName);
	void WriteToFile(char* szFileName, char* szData);

};

#endif