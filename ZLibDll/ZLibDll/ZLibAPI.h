#pragma once

#ifndef _ZLibAPI_H_
#define _ZLibAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

# include <direct.h>
# include <io.h>

#include "zip.h"
#include "unzip.h"

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

class ZLibAPI
{
	zipFile m_zFileHandle;

	void* m_pBuf;
	uInt m_iBufSize;

	int opt_compress_level;
public:
	ZLibAPI();
	~ZLibAPI();

	void CreateZip(char* szFileName);
	void CloseZip();
	void AddFile(char* szFileNameInZip, char* szSrcFile);

	void OpenUnZip(char* szFileName);
	void CloseUnZip();
	void Extract(char* szOutPath);
	int DoExtract(unzFile uf, char* szOutputPath, int opt_overwrite, const char* password);
	int DoExtractCurrentfile(unzFile uf, char* szOutputPath, int* popt_overwrite, const char* password);
};

#endif