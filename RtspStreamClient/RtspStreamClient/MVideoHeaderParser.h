#ifndef _MVideoHeaderParser_H
#define _MVideoHeaderParser_H

typedef  __int64 REFERENCE_TIME;
typedef unsigned char BYTE;

//video object type indication
#define		SIMPLE_OBJECT_TYPE				1
#define		SIMPLE_SCALABLE_OBJECT_TYPE		2
#define		CORE_OBJECT_TYPE				3
#define		MAIN_OBJECT_TYPE				4
#define		NBIT_OBJECT_TYPE				5
#define		BASIC_ANIM_2D_TEXTURE			6
#define		ANIM_2D_MESH					7
#define		SIMPLE_FACE						8
#define		STILL_SCALABLE_TEXTURE			9
#define		ADVANCED_REALTIME_SIMPLE		10
#define		CORE_SCALABLE					11
#define		ADVANCED_CODING_EFFICIENCY		12
#define		ADVANCED_SCALABLE_TEXTURE		13
#define		SIMPLE_FBA						14
#define		SIMPLE_STUDIO					15
#define		CORE_STUDIO						16
#define		ADVANCED_SIMPLE					17
#define		FINE_GRANULARITY_SCALABLE		18

//aspect_ratio_info
#define		AR_SQUARE						1
#define		AR_12_11						2
#define		AR_10_11						3
#define		AR_16_11						4
#define		AR_40_33						5
#define		AR_EXT_PAR						6

//video_object_layer_shape
#define		RECTANGULAR						0
#define		BINARY							1
#define		BINARY_ONLY						2
#define		GRAYSCALE						3

int GetJpegFrameSize(unsigned char* pBuffer,int* iW,int* iH);
long ParseMPEG4Header(BYTE *hdr, long hdrlen, long *width, 
										  long *height, REFERENCE_TIME *time);
int CheckFrameType(unsigned char* pBuffer);

#endif