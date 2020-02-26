#ifndef _IDStreamCommon_H
#define _IDStreamCommon_H

#define _VIDEO_BUFFER_SIZE 1024*1024*2 //2048*2048 //4096 * 4096
#define _AUDIO_BUFFER_SIZE 	40960

#define _MAX_VIDEO_W 4096
#define _MAX_VIDEO_H 4096

enum CSignalType
{
	SIGNAL_TYPE_NORMAL,
	SIGNAL_TYPE_NO_SIGNAL,
	SIGNAL_TYPE_DISABLED,
	SIGNAL_TYPE_NETWORK_DATA_LOSS,
	SIGNAL_TYPE_HARDWARE_ERROR,
	SIGNAL_TYPE_NO_DATA
};

enum IDPacketType 
{
	ID_PACKET_TYPE_NONE = 0,
	ID_PACKET_TYPE_AUDIO,
	ID_PACKET_TYPE_I,
	ID_PACKET_TYPE_P,
	ID_PACKET_TYPE_B,
	ID_PACKET_TYPE_BBP,
	ID_PACKET_TYPE_BP,
	ID_PACKET_TYPE_CSSH,
	ID_PACKET_TYPE_OSD,
	ID_PACKET_TYPE_METADATA,
	ID_PACKET_TYPE_FO,
	ID_PACKET_TYPE_AUDIO_ONLY,
	ID_PACKET_TYPE_AUDIO_2,
	ID_PACKET_TYPE_TWINCSH	    = 128,
	ID_PACKET_TYPE_TWINOSD	    = 129
};

enum PBStreamMode
{
	PBM_LIVE = 0,
	PBM_PB_FORWARD,
	PBM_PB_BACKWARD,
	PBM_PB_IVF,    //download ivf file.
	PBM_PB_LOCAL,  //video source from ivf file.
};

enum PlaybackMode
{
	/// Forward playback
	PLAY_MODE_FORWARD,
	/// Backward playback
	PLAY_MODE_BACKWARD,
	PLAY_MODE_ARCHIVE,
	PLAY_MODE_ARCHIVE_NOLOG,
	PLAY_MODE_FORWARD_PRECISE,
	PLAY_MODE_BACKWARD_PRECISE
};

enum StreamType
{
	ST_LOCAL_FILE = 101,
	ST_BEST = 201,//6
	ST_WORST,//7
	ST_MULTI_STREAM ,//0 , multi-stream
	ST_STREAM1,//18
	ST_STREAM2,//19
	ST_STREAM3,//20
};

enum VideoSourceMode
{
	VSM_STREAM = 0,
	VSM_FILE,
	//RTSP,...
};

enum IDVideoCodecID
{
	IDVCID_UNSUPPORT = -1,
	IDVCID_UNKNOW = 0,
	IDVCID_HIK_VISION = 1,
	IDVCID_MPEG4,
	IDVCID_MJPEG,
	IDVCID_H264,  
	IDVCID_HIK_H264,
	IDVCID_H265, 
};

enum PlayerStatus
{
	IDPS_STOP = 0,
	IDPS_PLAY,
	IDPS_PAUSE,
};

enum IDPlaybackCmd
{
	ID_PB_CMD_LIVE = 1,
	ID_PB_CMD_FAST_BACKWARD_PLAY,
	ID_PB_CMD_BACKWARD_STEP,
	ID_PB_CMD_BACKWARD_PLAY,
	ID_PB_CMD_PAUSE,
	ID_PB_CMD_STOP,
	ID_PB_CMD_FORWARD_PLAY,
	ID_PB_CMD_FORWARD_STEP,
	ID_PB_CMD_FAST_FORWARD_PLAY,
	ID_PB_CMD_SLOW,
	ID_PB_CMD_BACKWARD_SEEK,
	ID_PB_CMD_FORWARD_SEEK,
	ID_PB_CMD_SEEK,
};

enum AudioCodecType
{
	AUDIO_CODEC_PCM = 0x00,  //PCM B16 C1 F8000 OBE
	AUDIO_CODEC_PCMU = 0x01, //G711
	AUDIO_CODEC_L16 = 0x02,  //PCM B16 C1 F8000 OLE
	AUDIO_CODEC_HUNT_ADPCM = 0x03,
	AUDIO_CODEC_AMR = 0x04,
	AUDIO_CODEC_MPA = 0x05,  //MP2
	AUDIO_CODEC_MP4 = 0x06,
	AUDIO_CODEC_PCMA = 0x07, //G711A
	AUDIO_CODEC_G72616 = 0x08,
	AUDIO_CODEC_G72632 = 0x09,
	//AAC
	AUDIO_CODEC_AAC_2CH_96000 = 0x30,
	AUDIO_CODEC_AAC_2CH_88200 = 0x31,
	AUDIO_CODEC_AAC_2CH_64000 = 0x32,
	AUDIO_CODEC_AAC_2CH_48000 = 0x33,
	AUDIO_CODEC_AAC_2CH_44100 = 0x34,
	AUDIO_CODEC_AAC_2CH_32000 = 0x35,
	AUDIO_CODEC_AAC_2CH_24000 = 0x36,
	AUDIO_CODEC_AAC_2CH_22050 = 0x37,
	AUDIO_CODEC_AAC_2CH_16000 = 0x38,
	AUDIO_CODEC_AAC_2CH_12000 = 0x39,
	AUDIO_CODEC_AAC_2CH_11050 = 0x3A,
	AUDIO_CODEC_AAC_2CH_8000 = 0x3B,
	AUDIO_CODEC_AAC_2CH_7350 = 0x3C,
	AUDIO_CODEC_DEFAULT = 0xFF
};

typedef struct
{
	int iID;
	int iSamples;
	int iChannels;
}AACItem;

static AACItem g_AACItems [] = 
{
	{AUDIO_CODEC_AAC_2CH_7350,7350,2},
	{AUDIO_CODEC_AAC_2CH_8000,8000,2},
	{AUDIO_CODEC_AAC_2CH_11050,11050,2},
	{AUDIO_CODEC_AAC_2CH_12000,12000,2},
	{AUDIO_CODEC_AAC_2CH_16000,16000,2},
	{AUDIO_CODEC_AAC_2CH_22050,22050,2},
	{AUDIO_CODEC_AAC_2CH_24000,24000,2},
	{AUDIO_CODEC_AAC_2CH_32000,32000,2},
	{AUDIO_CODEC_AAC_2CH_44100,44100,2},
	{AUDIO_CODEC_AAC_2CH_48000,48000,2},
	{AUDIO_CODEC_AAC_2CH_64000,64000,2},
	{AUDIO_CODEC_AAC_2CH_88200,88200,2},
	{AUDIO_CODEC_AAC_2CH_96000,96000,2},
};

/*
int CshAudioSegment::parse(const char *pData)
{	
		
		case 0x37:
			audioCodec = "AAC-2CH-22050";
			break;
		case 0x36:
			audioCodec = "AAC-2CH-24000";
			break;
		case 0x35:
			audioCodec = "AAC-2CH-32000";
			break;
		case 0x34:
			audioCodec = "AAC-2CH-44100";
			break;
		case 0x33:
			audioCodec = "AAC-2CH-48000";
			break;
		case 0x32:
			audioCodec = "AAC-2CH-64000";
			break;
		case 0x31:
			audioCodec = "AAC-2CH-88200";
			break;
		case 0x30:
			audioCodec = "AAC-2CH-96000";
			break;
		default:
			audioCodec = "";
			break;
	}

	isValid = true;

	return 1;
}
*/

enum StreamEvent
{
	SE_VIDEO_FRAME = 1,
	SE_AUDIO_FRAME,
    SE_SPS_FRAME,  //for h264
	SE_OSD,
	SE_HIK_CSSH,
	SE_HIK_VIDEO_FRAME,
	SE_PACKET_FRAME,
	SE_ERROR,
	SE_CSSH,
	SE_NONE,
	SE_HIK_AUDIO_FRAME,
	SE_PLAY_AUDIO_FRAME,
	SE_DISPLAY_HIK_VIDEO_FRAME,
	SE_NO_SIGNAL,
};

enum PlaybackDirection
{
   PD_FORWARD,
   PD_BACKWARD,
};

typedef struct
{
	unsigned long ulSec;
	unsigned long ulMSec;
}FrameTimestamp;

typedef struct
{
	int iCodecID;
	bool bIsKeyFrame;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	int iLen;
	char* pBuffer;
}StreamFrame;

typedef struct
{
	int iType;
	bool bIsKeyFrame;
	int iWidth;
	int iHeight;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	int iLen;
	char* pBuffer;
}HikVideoFrame;

typedef struct
{
	int iPixelFormat;
	bool bIsKeyFrame;
	int iWidth;
	int iHeight;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	int iLen;
	int iPicType;
	char* pBuffer;
}RawVideoFrame;

typedef struct
{
	int iChannels;
	int iSampleRate;
	int iBits;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	int iTotalOfWOBuffers;
	int iLen;
	char* pBuffer;
}RawAudioFrame;

typedef struct
{
	int iPacketType;
	bool bIsKeyFrame;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	int iLen;
	char* pBuffer;
}PacketFrame;

#endif
