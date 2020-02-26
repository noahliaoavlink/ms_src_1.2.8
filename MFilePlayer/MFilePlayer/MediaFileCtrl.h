#pragma once

#ifndef _MediaFileCtrl_H
#define _MediaFileCtrl_H

#include "FFMediaFileDll.h"
#include "..\\..\\Include\\SQList.h"

class MediaFileCtrl
{
	SQList* m_pFFMediaFileList;

	FFMediaFileDll* m_pCurSelObj;
public:
	MediaFileCtrl();
	~MediaFileCtrl();

	void CloseAllFiles();
	int SearchFileName(char* szFileName);

	int Open(char* szFileName);
	void Close();
	FFMediaFileDll* GetCurSelObj();
	int CreateNewObj();
	void SetTarget(int iIndex);

};
#endif
