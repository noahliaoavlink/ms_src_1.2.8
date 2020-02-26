#ifndef _MediaFrameInfo_H
#define _MediaFrameInfo_H

#ifdef __cplusplus
extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}
#endif //__cplusplus

enum MediaFrameStatus
{
	MFS_FAIL = 0,
	MFS_SUCCESS = 1, //success
};

enum MediaFrameType
{
	MFT_VIDEO = 1,
	MFT_AUDIO,
	MFT_VIDEO_SW,
};

typedef struct
{
	int iLen;
	int iType;  //video = 1 , audio = 2
	int iStatus;
	long lTimestamp;
	bool bIsPicIFrame;
	int iPicType;
	unsigned char* pBuffer;
}MediaFrameBuffer;

#endif