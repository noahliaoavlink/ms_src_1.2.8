#ifndef _RtspCommon_H
#define _RtspCommon_H

#include <stdlib.h>

#define _RTSP_OBJ_ID 2008

//payload type
#define PCMU 0 
#define P1016 1 
#define G721 2 
#define GSM     3 
#define G723 4 
#define DVI4 5 
#define LPC     7 
#define PCMA 8 
#define G722 9 
#define L16     10 
#define QCELP 12 
#define CN     13 
#define MPA     14 
#define G728 15 
//#define DVI4 16 
#define G729 18 
#define CellB 25 
#define JPEG 26 
#define H261 31 
#define MPV     32 
#define MP2T 33 
#define H263 34 

//for h264
#define H264_NAL_TYPE_NON_IDR_SLICE 1
#define H264_NAL_TYPE_DP_A_SLICE 2
#define H264_NAL_TYPE_DP_B_SLICE 3
#define H264_NAL_TYPE_DP_C_SLICE 0x4
#define H264_NAL_TYPE_IDR_SLICE 0x5
#define H264_NAL_TYPE_SEI 0x6
#define H264_NAL_TYPE_SEQ_PARAM 0x7
#define H264_NAL_TYPE_PIC_PARAM 0x8
#define H264_NAL_TYPE_ACCESS_UNIT 0x9
#define H264_NAL_TYPE_END_OF_SEQ 0xa
#define H264_NAL_TYPE_END_OF_STREAM 0xb
#define H264_NAL_TYPE_FILLER_DATA 0xc
#define H264_NAL_TYPE_SEQ_EXTENSION 0xd

enum RTSPDataFormat
{
	//video
	RTSP_DF_MPEG_V = 1,
	RTSP_DF_MPEG4,
	RTSP_DF_H264,
	RTSP_DF_MJPEG,
	RTSP_DF_H265,
	RTSP_DF_VIDEO_EXTRA_DATA,
	RTSP_DF_AUDIO_EXTRA_DATA,
	//audio
	RTSP_DF_MPEG_A = 1001,
    RTSP_DF_G726, 
    RTSP_DF_MULAW,
	RTSP_DF_ALAW,
	RTSP_DF_AAC,
	RTSP_DF_L16, 
};

enum MJPixelFormat
{
	 MJ_PF_YUV422 = 1,
	 MJ_PF_YUV420
};

enum RtspCallbackEvent
{
	RCE_RECEIVE_AUDIO	= 1,
    RCE_RECEIVE_VIDEO,
	RCE_RECEIVE_VIDEO_EXTRA_DATA,
};

enum RtspEvent
{
	RE_VIDEO_CODEC = 1,
	RE_AUDIO_CODEC,
	RE_FRAME_SIZE,
};

#define _MAX_VIDEO_W 1280
#define _MAX_VIDEO_H 960

typedef struct
{
	unsigned long ulID;      //序號
	int iFormat;             //codec的格式 (mpeg4,h264,mjpg)
	int iLen;                //資料長度
	int iIPCount;
	int iFrameType;
	int iWidth;
	int iHeight;
	int iPixelFormat;      //for mjpeg
	int iBits;               //for audio
	int iSample;             //for audio
	int iChannels;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	unsigned char* pBuffer;  //資料
}RtspFrameInfo;

typedef struct
{
	unsigned long ulID;
	char szUserName[80];
	char szPassword[80];
}RtspUserInfo;

typedef struct
{
	char* pBuffer;
	int iLen;
}RTPItem;    

typedef void (*RtspCallbackProc)(int iType,RtspFrameInfo* pInfo);

class RtspCallbackObj
{
   public:
	    RtspCallbackObj(){};
        ~RtspCallbackObj(){};
		
		virtual void* DoEvent(int iEventID,void* pParameter1,void* pParameter2) = 0;
};

__inline bool ParseUrl1(char* szUrl, char* szPattern, char* szSeparator, char* szSuffix, int* iPort, char* szUserName, char* szPW)
{
	int iPrefixLen;
	int iPortLen = 0;
	char szPortStr[10];
	char szLoginStr[128];
	int iLoginLen = 0;
	char* pTemp = strstr(szUrl, szPattern);
	if (pTemp)
	{
		//strcpy(szPrefix,"rtsp://");
		iPrefixLen = strlen(szPattern);

		//username:pw
		char* pTemp2 = strchr(szUrl + iPrefixLen, '@');
		if (pTemp2)
		{
			iLoginLen = pTemp2 - szUrl - iPrefixLen;
			memcpy(szLoginStr, szUrl + iPrefixLen, iLoginLen);
			szLoginStr[iLoginLen] = '\0';

			char* pLoginTemp = strchr(szLoginStr, ':');
			if (pLoginTemp)
			{
				int iUserNameLen = pLoginTemp - szLoginStr;

				memcpy(szUserName, szLoginStr, iUserNameLen);
				szUserName[iUserNameLen] = '\0';
				strcpy(szPW, pLoginTemp + 1);
			}

			iLoginLen += 1;
		}
		else
		{
			strcpy(szUserName, "");
			strcpy(szPW, "");
		}


		/*
		//port
		char* pTemp3 = strchr(szUrl + iPrefixLen + iLoginLen, ':');
		if (pTemp3)
		{
		char* pTemp4 = strchr(pTemp3 + 1, '/');
		if (pTemp4)
		{
		int iLen = strlen(pTemp3) - strlen(pTemp4) - 1;
		memcpy(szPortStr, pTemp3 + 1, iLen);
		szPortStr[iLen] = '\0';
		}
		else
		strcpy(szPortStr, pTemp3 + 1);
		*iPort = atoi(szPortStr);
		iPortLen = strlen(szPortStr) + 1;   //:xxx

		}
		else
		*iPort = -1;
		*/

		char* pTemp1 = strchr(szUrl + iPrefixLen, '/');
		if (pTemp1)
		{
			int iLen;
			strcpy(szSuffix, pTemp1);

			iLen = strlen(szUrl) - iPrefixLen - strlen(szSuffix) - iPortLen - iLoginLen;
			memcpy(szSeparator, szUrl + iPrefixLen + iLoginLen, iLen);
			szSeparator[iLen] = '\0';

			char* pTemp3 = strchr(szSeparator, ':');
			if (pTemp3)
				*iPort = atoi(pTemp3 + 1);
		}
		else
		{
			int iLen = strlen(szUrl) - iPrefixLen - iPortLen - iLoginLen;
			strcpy(szSuffix, "");
			memcpy(szSeparator, szUrl + iPrefixLen + iLoginLen, iLen);
			szSeparator[iLen] = '\0';

			char* pTemp3 = strchr(szSeparator, ':');
			if (pTemp3)
				*iPort = atoi(pTemp3 + 1);
		}

		return true;
	}
	return false;
}

__inline bool ParseUrl(char* szUrl, char* szPrefix, char* szSeparator, char* szSuffix, int* iPort, char* szUserName, char* szPW)
{
	char szPattern[20] = "rtsp://";
	bool bRet = ParseUrl1(szUrl, szPattern, szSeparator, szSuffix, iPort, szUserName, szPW);
	if (bRet)
	{
		strcpy(szPrefix, szPattern);
	}
	return bRet;
}

__inline int GetDataFormat(char* szMediumName,char* szCodecName)
{
	if(strcmp(szMediumName,"audio") == 0)
	{
		if(strcmp(szCodecName,"MPA") == 0)
			return RTSP_DF_MPEG_A;
        else if(strstr(szCodecName,"G726"))
            return RTSP_DF_G726;
        else if(strstr(szCodecName,"PCMU"))
			return RTSP_DF_MULAW;
		else if(strstr(szCodecName,"PCMA"))
			return RTSP_DF_ALAW;
		else if(strstr(szCodecName,"mpeg4-generic"))
			return RTSP_DF_AAC;
		else if(strstr(szCodecName,"L16"))
			return RTSP_DF_L16;
	}
	else if(strcmp(szMediumName,"video") == 0)
	{
		if(strcmp(szCodecName,"H264") == 0)
			return RTSP_DF_H264;
		else if(strcmp(szCodecName,"MP4V-ES") == 0)
			return RTSP_DF_MPEG4;
		else if(strcmp(szCodecName,"JPEG") == 0)
			return RTSP_DF_MJPEG;
		else if(strcmp(szCodecName,"MPV") == 0)
			return RTSP_DF_MPEG_V;
		else if(strcmp(szCodecName,"H265") == 0)
			return RTSP_DF_H265;
	}
	return 0;
}

__inline bool IsRTSPUrl(char* szUrl)
{
	char* pTemp = strstr(szUrl, "rtsp://");
	if (pTemp)
		return true;
	return false;
}

#endif
