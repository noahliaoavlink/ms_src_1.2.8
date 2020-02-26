#include "stdafx.h"
#include "ThumbnailCtrl.h"
#include "..\\..\\Include\\SString.h"

ThumbnailFile::ThumbnailFile()
{
	m_pFile = 0;
	m_iIndex = 0;
	m_pPicture = new ThumbnailPicture;
	m_pPicture->pBuffer = new unsigned char[_THUMBNAIL_FRAME_LEN];
	m_pPicture->iLen = _THUMBNAIL_FRAME_LEN;
}

ThumbnailFile::~ThumbnailFile()
{
	if(m_pPicture)
	{
		if(m_pPicture->pBuffer)
			delete m_pPicture->pBuffer;
		delete m_pPicture;
	}
}

void ThumbnailFile::Open(int iIndex)
{
	char szAPPath[512];
	char szFileName[512];
	//char szMode[10];

	GetExecutionPath(NULL, szAPPath);
	//sprintf(szFileName,"Thumbnail%d.dat",iIndex);
	sprintf(szFileName, "%s//Thumbnail%d.dat", szAPPath,iIndex);

#if 0
	if(PathFileExistsA(szFileName))
		strcpy(szMode,"r+b");
	else
		strcpy(szMode,"w+b");

	m_pFile  = fopen(szFileName, szMode);
	if(m_pFile)
	{
		unsigned long ulLength = GetLength();
		if(ulLength == 0)
			Reset();
		else
			ThumbnailFileHeader* p = ReadHeader();
	}
#else
	if(!PathFileExistsA(szFileName))
	{
		m_pFile  = fopen(szFileName, "w+b");
		if(m_pFile)
			fclose(m_pFile);
	}

	if(PathFileExistsA(szFileName))
	{
		m_pFile  = fopen(szFileName, "r+b");
		if(m_pFile)
		{
			unsigned long ulLength = GetLength();
			if(ulLength == 0)
				Reset();
			else
				ThumbnailFileHeader* p = ReadHeader();
		}
	}

#endif
}

void ThumbnailFile::Close()
{
	WriteHeader();
	if(m_pFile)
		fclose(m_pFile);
}

void ThumbnailFile::Reset()
{
	m_FileHeader.ulTotal = 0;
	for(int i = 0;i < _MAX_THUMBNAIL_ITEMS;i++)
		m_FileHeader.bIsUsed[i] = false;
	WriteHeader();
}

unsigned long ThumbnailFile::GetLength()
{
	if(m_pFile)
	{
	    unsigned long ulCurPos = ftell(m_pFile);
	    fseek(m_pFile, 0,SEEK_END);
        m_ulFileLen = ftell(m_pFile);
	    fseek(m_pFile, ulCurPos,SEEK_SET);
	    return m_ulFileLen;
	}
	return 0;
}

void ThumbnailFile::WriteHeader()
{
	int iRet = 0;
	if(m_pFile)
	{
		Seek(0);
		iRet = fwrite(&m_FileHeader,1,sizeof(ThumbnailFileHeader),m_pFile);
		fflush(m_pFile);
		GetLength();
	}
}

ThumbnailFileHeader* ThumbnailFile::ReadHeader()
{
	if(m_pFile)
	{
		Seek(0);
		int iRet = fread((void*) &m_FileHeader,1,sizeof(ThumbnailFileHeader),m_pFile);
		if(iRet != sizeof(ThumbnailFileHeader))
			return 0;
		return &m_FileHeader;
	}
	return 0;
}

void ThumbnailFile::WritePicture(long lPicIndex,unsigned char* pBuffer,int iLen)
{
	int iRet = 0;
	if(m_pFile)
	{
		unsigned long ulOffset = sizeof(ThumbnailFileHeader) + lPicIndex * _THUMBNAIL_FRAME_LEN + _DATA_OFFSET;
		Seek(ulOffset);
		iRet = fwrite(pBuffer,1,iLen,m_pFile);
		fflush(m_pFile);
		GetLength();
	}
}

void ThumbnailFile::ReadPicture(long lPicIndex)
{
	int iRet;
	if(m_pFile)
	{
		unsigned long ulOffset = sizeof(ThumbnailFileHeader) + lPicIndex * _THUMBNAIL_FRAME_LEN + _DATA_OFFSET;
		Seek(ulOffset);
		iRet = fread(m_pPicture->pBuffer,1,_THUMBNAIL_FRAME_LEN,m_pFile);
	}
}

bool ThumbnailFile::Seek(unsigned long ulOffset)
{
	if(m_pFile)
	{
	    fseek(m_pFile,ulOffset,SEEK_SET);
		unsigned long ulPos = ftell(m_pFile);
		return true;
	}
	return false;
}

long ThumbnailFile::GetNextAvailableIndex()
{
	for(int i = 0;i < _MAX_THUMBNAIL_ITEMS;i++)
	{
		if(m_FileHeader.bIsUsed[i] == false)
			return i;
	}
}

void ThumbnailFile::SetUsed(long lPicIndex,bool bUsed)
{
	if(lPicIndex < _MAX_THUMBNAIL_ITEMS)
		m_FileHeader.bIsUsed[lPicIndex] = bUsed;
}

bool ThumbnailFile::IsUsed(long lPicIndex)
{
	if(lPicIndex < _MAX_THUMBNAIL_ITEMS)
		return m_FileHeader.bIsUsed[lPicIndex];
	return false;
}

void ThumbnailFile::AddPicture(long lPicIndex,unsigned char* pBuffer,int iLen)
{
	WritePicture(lPicIndex,pBuffer,iLen);
	SetUsed(lPicIndex,true);
	m_FileHeader.ulTotal++;
	WriteHeader();
}

void ThumbnailFile::DeletePicture(long lPicIndex)
{
	SetUsed(lPicIndex,false);
	m_FileHeader.ulTotal--;
	if(m_FileHeader.ulTotal < 0)
		; //error
	WriteHeader();
}

ThumbnailPicture* ThumbnailFile::GetPicture(long lPicIndex)
{
	if(lPicIndex < _MAX_THUMBNAIL_ITEMS && lPicIndex >= 0)
	{
		if(m_FileHeader.bIsUsed[lPicIndex])
		{
			ReadPicture(lPicIndex);
			return m_pPicture;
		}
	}
	return 0;
}

ThumbnailCtrl::ThumbnailCtrl()
{
	//m_pFFAVCodecDLL = 0;
	m_pFFMediaFileDll = 0;
	m_pRTSPPlayerDll = 0;
	m_pRTSPObj = 0;
	m_pThumbnailFile = new ThumbnailFile;
	
}

ThumbnailCtrl::~ThumbnailCtrl()
{
	Close();
	if(m_pThumbnailFile)
		delete m_pThumbnailFile;
}
/*
void ThumbnailCtrl::SetFFAVCodecDLL(FFAVCodecDLL* pObj)
{
	m_pFFAVCodecDLL = pObj;
}
*/
void ThumbnailCtrl::SetFFMediaFileDll(FFMediaFileDll* pObj)
{
	m_pFFMediaFileDll = pObj;
}

void ThumbnailCtrl::SetRTSPPlayerDll(RTSPPlayerDll* pDllObj,void* pObj)
{
	m_pRTSPPlayerDll = pDllObj;
	m_pRTSPObj = pObj;
}

void ThumbnailCtrl::Open()
{
	m_pThumbnailFile->Open(0);
}

void ThumbnailCtrl::Close()
{
	m_pThumbnailFile->Close();
}

ThumbnailPicture* ThumbnailCtrl::GetPicture(long lPicIndex)
{
	return m_pThumbnailFile->GetPicture(lPicIndex);
}

long ThumbnailCtrl::AddPicture()
{
	char szMsg[512];
	
	long lPicIndex = -1;

	//m_pFFMediaFileDll
	if (m_pFFMediaFileDll)
	{
		AVCodecContext* pVideoCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_VIDEO);
		if (pVideoCodecContext)
		{
			bool bDo = true;
			int w, h;
			MediaFrameBuffer framebuf;
			m_pFFMediaFileDll->GetVideoFrameSize(&w, &h);

			int iBufferLen = w * h * 3 / 2;
			framebuf.pBuffer = new unsigned char[iBufferLen];
			unsigned char* pPicBuffer = new unsigned char[iBufferLen];

			double dLength = m_pFFMediaFileDll->GetLength();

			if (dLength > 10.0)
				m_pFFMediaFileDll->SeekTo(5.0);
			else if (dLength < 30.0 && dLength >= 10.0)
				m_pFFMediaFileDll->SeekTo(3.0);

			int iCount = 0;

			while (bDo)
			{
				int iRet = m_pFFMediaFileDll->GetNextFrameBuffer(&framebuf);
				if (iRet > 0)
				{
					if (framebuf.iType == MFT_VIDEO)
					{
						//int iScaleRet = m_pFFAVCodecDLL->Scale(0, AV_PIX_FMT_YUV420P, framebuf.pBuffer, pPicBuffer, w, h, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
						int iScaleRet = m_pFFMediaFileDll->Scale(AV_PIX_FMT_YUV420P, framebuf.pBuffer, pPicBuffer, w, h, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
						lPicIndex = m_pThumbnailFile->GetNextAvailableIndex();
						m_pThumbnailFile->AddPicture(lPicIndex, pPicBuffer, _THUMBNAIL_FRAME_LEN);
						bDo = false;
						break;
					}
				}
				else
				{
					if (framebuf.iStatus == MFS_FAIL)
					{
						sprintf(szMsg, "#DIB# ThumbnailCtrl::AddPicture() framebuf.iStatus == MFS_FAIL \n");
						OutputDebugStringA(szMsg);
						break;
					}
					/*
					iCount++;
					if(iCount > 3)
					{
					sprintf(szMsg,"#DIB# ThumbnailCtrl::AddPicture() iCount > 3 \n");
					OutputDebugStringA(szMsg);
					break;
					}
					*/
				}
			}
			delete framebuf.pBuffer;
			delete pPicBuffer;
			return lPicIndex;
		}
	}
	return -1;
}

long ThumbnailCtrl::AddPictureFromRTSP()
{
	long lPicIndex = -1;

	if (m_pRTSPPlayerDll && m_pRTSPObj)
	{
		int w = m_pRTSPPlayerDll->_In_RTSP_GetVideoWidth(m_pRTSPObj);
		int h = m_pRTSPPlayerDll->_In_RTSP_GetVideoHeight(m_pRTSPObj);

		if (w > 0 && h > 0)
		{
			int iBufferLen = w * h * 3 / 2;
			unsigned char* pPicBuffer = new unsigned char[iBufferLen];

			unsigned char* pFrameBuffer = m_pRTSPPlayerDll->_In_RTSP_GetFrameBuffer(m_pRTSPObj);

			int iScaleRet = m_pFFMediaFileDll->Scale(AV_PIX_FMT_YUV420P, pFrameBuffer, pPicBuffer, w, h, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
			lPicIndex = m_pThumbnailFile->GetNextAvailableIndex();
			m_pThumbnailFile->AddPicture(lPicIndex, pPicBuffer, _THUMBNAIL_FRAME_LEN);
			delete pPicBuffer;
		}
		return lPicIndex;
	}
	return -1;
}

void ThumbnailCtrl::DeletePicture(long lPicIndex)
{
	m_pThumbnailFile->DeletePicture(lPicIndex);
}

void ThumbnailCtrl::RemoveAll()
{
	m_pThumbnailFile->Reset();
}

void ThumbnailCtrl::SetUsed(long lPicIndex,bool bUsed)
{
	m_pThumbnailFile->SetUsed(lPicIndex,bUsed);
}
