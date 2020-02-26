#ifndef _H264Parser_H
#define _H264Parser_H

#include "mpeg4ip_bitstream.h"

bool ffmpeg_find_h264_size (const uint8_t *ud, uint32_t ud_size,int* w,int* h);
bool h264_is_start_code(const uint8_t *pBuf);
uint8_t h264_nal_unit_type(const uint8_t *buffer);

//void decode_mpeg4 (uint8_t *vol, uint32_t len);

#endif