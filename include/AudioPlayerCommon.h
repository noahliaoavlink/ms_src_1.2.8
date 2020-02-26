#ifndef _AudioPlayerCommon_H_
#define _AudioPlayerCommon_H_

enum AudioPlayerMsg
{
   AP_MSG_SET_WAVE_FORMAT = 4001,
   AP_MSG_ADD_FRAME,
   AP_MSG_RESET,
   AP_MSG_PLAY,
   AP_MSG_STOP,
   AP_MSG_PAUSE,
   AP_MSG_CLOSE,
   AP_MSG_ADD_TIMESTAMP,
};

typedef struct
{
	int nChannels;
	int nFrequency;
	int nBits;
}AudioWaveFormat;

typedef struct
{
	int iLen;
	unsigned long ulTimestamp;
	unsigned char* pBuf;
}MSAudioFrame;

#endif