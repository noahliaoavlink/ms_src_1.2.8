#include "stdafx.h"
#include "ZLibAPI.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (81960)
#define MAXFILENAME (256)

#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)

__inline void MakeDir(char* szPath)
{
	int iCurLen = 0;
	char szTempFolder[256];
	char szNewFolder[256];
	strcpy(szTempFolder, szPath);
	bool bDo = true;
	while (bDo)
	{
		char* pTemp = strchr(szTempFolder, '/');

		if (!pTemp)
			pTemp = strchr(szTempFolder, '\\');

		if (pTemp)
		{
			iCurLen += strlen(szTempFolder) - strlen(pTemp) + 1;

			memcpy(szNewFolder, szPath, iCurLen);
			szNewFolder[iCurLen] = '\0';
			strcpy(szTempFolder, pTemp + 1);
		}

		if (!pTemp || strlen(szTempFolder) == 0)
		{
			CreateDirectoryA(szPath, NULL);
			bDo = false;
		}
		else
			CreateDirectoryA(szNewFolder, NULL);
	}
}

void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date)
{
	HANDLE hFile;
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
	DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftm);
	SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
	CloseHandle(hFile);
}

ZLibAPI::ZLibAPI()
{
	m_zFileHandle = 0;

	m_iBufSize = WRITEBUFFERSIZE;
	m_pBuf = (void*)malloc(m_iBufSize);

	opt_compress_level = Z_DEFAULT_COMPRESSION;   //Z_NO_COMPRESSION
}

ZLibAPI::~ZLibAPI()
{
	if(m_pBuf)
		free(m_pBuf);
}

void ZLibAPI::CreateZip(char* szFileName)
{
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64(&ffunc);

	//m_zFileHandle = zipOpen2_64(szFileName, APPEND_STATUS_CREATE, NULL, &ffunc);
	m_zFileHandle = zipOpen2_64(szFileName, APPEND_STATUS_CREATE, NULL, NULL);
}

void ZLibAPI::CloseZip()
{
	int err = 0;
	if(m_zFileHandle)
		err = zipClose(m_zFileHandle, NULL);
}

void ZLibAPI::AddFile(char* szFileNameInZip,char* szSrcFile)
{
	FILE* srcfp = NULL;

	const char* password = NULL;
	unsigned long crcFile = 0;
	int zip64 = 0;
	int err = 0;

	zip_fileinfo zi;
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;

	char szNew_file_name[MAX_PATH];
	memset(szNew_file_name, 0, sizeof(szNew_file_name));
	strcat(szNew_file_name, szFileNameInZip);
	if (szSrcFile == NULL)
	{
		strcat(szNew_file_name, "/");
	}

	err = zipOpenNewFileInZip3_64(m_zFileHandle, szNew_file_name, &zi,
		NULL, 0, NULL, 0, NULL /* comment*/,
		Z_DEFLATED,//(opt_compress_level != 0) ? Z_DEFLATED : 0,
		opt_compress_level,//Z_DEFAULT_COMPRESSION,//opt_compress_level
		0,
		/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		password, crcFile, zip64);

	if (szSrcFile != NULL)
	{
		//打开源文件  
		srcfp = fopen(szSrcFile, "rb");
		if (srcfp == NULL)
		{
			zipCloseFileInZip(m_zFileHandle);
			return;
		}


		int numBytes = 0;
		while (!feof(srcfp))
		{
			numBytes = fread(m_pBuf, 1, sizeof(m_pBuf), srcfp);
			zipWriteInFileInZip(m_zFileHandle, m_pBuf, numBytes);
			if (ferror(srcfp))
				break;
		}

		fclose(srcfp);
	}

	zipCloseFileInZip(m_zFileHandle);
}

void ZLibAPI::OpenUnZip(char* szFileName)
{
	//	int opt_overwrite = 1;
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64(&ffunc);

	m_zFileHandle = unzOpen2_64(szFileName, &ffunc);
	//m_zFileHandle = unzOpen2_64(szFileName, NULL);
}

void ZLibAPI::CloseUnZip()
{
	if(m_zFileHandle)
		unzClose(m_zFileHandle);
}

void ZLibAPI::Extract(char* szOutPath)
{
	int opt_overwrite = 1;
	const char* password = NULL;
	DoExtract(m_zFileHandle, szOutPath,opt_overwrite,password);
}

int ZLibAPI::DoExtract(unzFile uf, char* szOutputPath, int opt_overwrite, const char* password)
{
	uLong i;
	unz_global_info64 gi;
	int err;
	FILE* fout = NULL;

	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
		printf("error %d with zipfile in unzGetGlobalInfo \n", err);

	for (i = 0; i<gi.number_entry; i++)
	{
		if (DoExtractCurrentfile(uf, szOutputPath,
			&opt_overwrite,
			password) != UNZ_OK)
			break;

		if ((i + 1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n", err);
				break;
			}
		}
	}

	return 0;
}

int ZLibAPI::DoExtractCurrentfile(unzFile uf, char* szOutputPath, int* popt_overwrite, const char* password)
{
	char filename_inzip[256];
	char szOutputFileName[512];

	char* p;
	int err = UNZ_OK;
	FILE *fout = NULL;

	unz_file_info64 file_info;
	uLong ratio = 0;
	err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

	if (err != UNZ_OK)
	{
		printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
		return err;
	}

	sprintf(szOutputFileName, "%s\\%s", szOutputPath, filename_inzip);


	const char* write_filename;
	int skip = 0;

	write_filename = szOutputFileName;

	err = unzOpenCurrentFilePassword(uf, password);
	if (err != UNZ_OK)
	{
		printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
	}

	if (((*popt_overwrite) == 0) && (err == UNZ_OK))
	{
		char rep = 0;
		FILE* ftestexist;
		ftestexist = FOPEN_FUNC(write_filename, "rb");
		if (ftestexist != NULL)
		{
			fclose(ftestexist);
			do
			{
				char answer[128];
				int ret;

				printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ", write_filename);
				ret = scanf("%1s", answer);
				if (ret != 1)
				{
					exit(EXIT_FAILURE);
				}
				rep = answer[0];
				if ((rep >= 'a') && (rep <= 'z'))
					rep -= 0x20;
			} while ((rep != 'Y') && (rep != 'N') && (rep != 'A'));
		}

		if (rep == 'N')
			skip = 1;

		if (rep == 'A')
			*popt_overwrite = 1;
	}

	if ((skip == 0) && (err == UNZ_OK))
	{
		fout = FOPEN_FUNC(write_filename, "wb");
		/* some zipfile don't contain directory alone before file */
		if (fout == NULL)
		{
			char szCurPath[512];

			char* pTemp = strrchr(szOutputFileName, '\\');
			int iLen = strlen(szOutputFileName) - strlen(pTemp);
			memcpy(szCurPath, szOutputFileName, iLen);
			szCurPath[iLen] = '\0';

			MakeDir((char*)szCurPath);
			fout = FOPEN_FUNC(write_filename, "wb");
		}

		if (fout == NULL)
		{
			printf("error opening %s\n", write_filename);
		}
	}

	if (fout != NULL)
	{
		printf(" extracting: %s\n", write_filename);

		do
		{
			err = unzReadCurrentFile(uf, m_pBuf, m_iBufSize);
			if (err<0)
			{
				printf("error %d with zipfile in unzReadCurrentFile\n", err);
				break;
			}
			if (err>0)
				if (fwrite(m_pBuf, err, 1, fout) != 1)
				{
					printf("error in writing extracted file\n");
					err = UNZ_ERRNO;
					break;
				}
		} while (err>0);
		if (fout)
			fclose(fout);
		/*
		if (err == 0)
			change_file_date(write_filename, file_info.dosDate,
				file_info.tmu_date);
				*/
	}

	if (err == UNZ_OK)
	{
		err = unzCloseCurrentFile(uf);
		if (err != UNZ_OK)
		{
			printf("error %d with zipfile in unzCloseCurrentFile\n", err);
		}
	}
	else
		unzCloseCurrentFile(uf); /* don't lose the error */

	return err;
}