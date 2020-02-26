#include "stdafx.h"
#include "DummySink.h"
#include <stdlib.h>
#include <ATLComTime.h>

//#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 2000000 //100000

const unsigned char * m_pStart;
unsigned short m_nLength;
int m_nCurrentBit;

unsigned int ReadBit()
{
//	assert(m_nCurrentBit <= m_nLength * 8);
	int nIndex = m_nCurrentBit / 8;
	int nOffset = m_nCurrentBit % 8 + 1;

	m_nCurrentBit++;
	return (m_pStart[nIndex] >> (8 - nOffset)) & 0x01;
}

unsigned int ReadBits(int n)
{
	int r = 0;
	int i;
	for (i = 0; i < n; i++)
	{
		r |= (ReadBit() << (n - i - 1));
	}
	return r;
}

unsigned int ReadExponentialGolombCode()
{
	int r = 0;
	int i = 0;

	while ((ReadBit() == 0) && (i < 32))
	{
		i++;
	}

	r = ReadBits(i);
	r += (1 << i) - 1;
	return r;
}

unsigned int ReadSE()
{
	int r = ReadExponentialGolombCode();
	if (r & 0x01)
	{
		r = (r + 1) / 2;
	}
	else
	{
		r = -(r / 2);
	}
	return r;
}

void Parse(const unsigned char * pStart, unsigned short nLen)
{
	m_pStart = pStart;
	m_nLength = nLen;
	m_nCurrentBit = 0;

	int chroma_format_idc = 1;

	int frame_crop_left_offset = 0;
	int frame_crop_right_offset = 0;
	int frame_crop_top_offset = 0;
	int frame_crop_bottom_offset = 0;

	int forbidden_zero_bit = ReadBit();
	int nal_ref_idc = ReadBits(2);
	int nal_unit_type = ReadBits(5);

	int profile_idc = ReadBits(8);
	int constraint_set0_flag = ReadBit();
	int constraint_set1_flag = ReadBit();
	int constraint_set2_flag = ReadBit();
	int constraint_set3_flag = ReadBit();
	int constraint_set4_flag = ReadBit();
	int constraint_set5_flag = ReadBit();
	int reserved_zero_2bits = ReadBits(2);
	int level_idc = ReadBits(8);
	int seq_parameter_set_id = ReadExponentialGolombCode();


	if (profile_idc == 100 || profile_idc == 110 ||
		profile_idc == 122 || profile_idc == 244 ||
		profile_idc == 44 || profile_idc == 83 ||
		profile_idc == 86 || profile_idc == 118)
	{
		chroma_format_idc = ReadExponentialGolombCode();

		if (chroma_format_idc == 3)
		{
			int residual_colour_transform_flag = ReadBit();
		}
		int bit_depth_luma_minus8 = ReadExponentialGolombCode();
		int bit_depth_chroma_minus8 = ReadExponentialGolombCode();
		int qpprime_y_zero_transform_bypass_flag = ReadBit();
		int seq_scaling_matrix_present_flag = ReadBit();

		if (seq_scaling_matrix_present_flag)
		{
			int i = 0;
			for (i = 0; i < 8; i++)
			{
				int seq_scaling_list_present_flag = ReadBit();
				if (seq_scaling_list_present_flag)
				{
					int sizeOfScalingList = (i < 6) ? 16 : 64;
					int lastScale = 8;
					int nextScale = 8;
					int j = 0;
					for (j = 0; j < sizeOfScalingList; j++)
					{
						if (nextScale != 0)
						{
							int delta_scale = ReadSE();
							nextScale = (lastScale + delta_scale + 256) % 256;
						}
						lastScale = (nextScale == 0) ? lastScale : nextScale;
					}
				}
			}
		}
	}

	int log2_max_frame_num_minus4 = ReadExponentialGolombCode();
	int pic_order_cnt_type = ReadExponentialGolombCode();
	if (pic_order_cnt_type == 0)
	{
		int log2_max_pic_order_cnt_lsb_minus4 = ReadExponentialGolombCode();
	}
	else if (pic_order_cnt_type == 1)
	{
		int delta_pic_order_always_zero_flag = ReadBit();
		int offset_for_non_ref_pic = ReadSE();
		int offset_for_top_to_bottom_field = ReadSE();
		int num_ref_frames_in_pic_order_cnt_cycle = ReadExponentialGolombCode();
		int i;
		for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
		{
			ReadSE();
			//sps->offset_for_ref_frame[ i ] = ReadSE();
		}
	}
	int max_num_ref_frames = ReadExponentialGolombCode();
	int gaps_in_frame_num_value_allowed_flag = ReadBit();
	int pic_width_in_mbs_minus1 = ReadExponentialGolombCode();
	int pic_height_in_map_units_minus1 = ReadExponentialGolombCode();
	int frame_mbs_only_flag = ReadBit();
	if (!frame_mbs_only_flag)
	{
		int mb_adaptive_frame_field_flag = ReadBit();
	}
	int direct_8x8_inference_flag = ReadBit();
	int frame_cropping_flag = ReadBit();
	if (frame_cropping_flag)
	{
		frame_crop_left_offset = ReadExponentialGolombCode();
		frame_crop_right_offset = ReadExponentialGolombCode();
		frame_crop_top_offset = ReadExponentialGolombCode();
		frame_crop_bottom_offset = ReadExponentialGolombCode();
	}
	int vui_parameters_present_flag = ReadBit();
	pStart++;

	int width = (pic_width_in_mbs_minus1 + 1) * 16;
	int height = ((2 - frame_mbs_only_flag)* (pic_height_in_map_units_minus1 + 1) * 16);

	unsigned int crop_unit_x;
	unsigned int crop_unit_y;

	if (frame_cropping_flag)
	{
		
		if (0 == chroma_format_idc) // monochrome
		{
			crop_unit_x = 1;
			crop_unit_y = 2 - frame_mbs_only_flag;
		}
		else if (1 == chroma_format_idc) // 4:2:0
		{
			crop_unit_x = 2;
			crop_unit_y = 2 * (2 - frame_mbs_only_flag);
		}
		else if (2 == chroma_format_idc) // 4:2:2
		{
			crop_unit_x = 2;
			crop_unit_y = 2 - frame_mbs_only_flag;
		}
		else // 3 == sps.chroma_format_idc   // 4:4:4
		{
			crop_unit_x = 1;
			crop_unit_y = 2 - frame_mbs_only_flag;
		}

		width -= crop_unit_x * (frame_crop_left_offset + frame_crop_right_offset);
		height -= crop_unit_y * (frame_crop_top_offset + frame_crop_bottom_offset);
	}

	int Width = ((pic_width_in_mbs_minus1 + 1) * 16) - frame_crop_right_offset * 2 - frame_crop_left_offset * 2;
	int Height = ((2 - frame_mbs_only_flag)* (pic_height_in_map_units_minus1 + 1) * 16) - (frame_crop_bottom_offset * 2) - (frame_crop_top_offset * 2);


	printf("\n\nWxH = %dx%d\n\n", width, height);

}

long GetCurTimestamp()
{
	long lOffset;

#if 0
	SYSTEMTIME utc_time;
	GetSystemTime(&utc_time);

	COleDateTime Time1(1970,1,1,0,0,0);
	COleDateTime Time2(utc_time.wYear,utc_time.wMonth,utc_time.wDay,utc_time.wHour,utc_time.wMinute,utc_time.wSecond);
	COleDateTimeSpan ts = Time2 - Time1;
	long lSecondOffset = ts.GetTotalSeconds();
	return lSecondOffset;
#else
	CTime m_curtime = CTime::GetCurrentTime();
	long lSecondOffset = (long)m_curtime.GetTime();
	return lSecondOffset;
#endif
}

DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId,long bufferSize,CallbackObj* pObj) 
{
	return new DummySink(env, subsession, streamId,bufferSize,pObj);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId,long bufferSize,CallbackObj* pObj)
  : MediaSink(env),
    fSubsession(subsession), fBufferSize(bufferSize)
{
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[bufferSize];

	m_ulID = 0;
	m_pCallbackObj = pObj;

	strcpy(m_SPropParameterSetsStr,"");
	m_bHaveWrittenFirstFrame = false;

	m_iWidth = -1;
	m_iHeight = -1;

	m_iTempBufferLen = 0;
	m_pTempBuffer = 0;
	m_iIPCount = 0;
	m_iOffset = 0;
	m_bStop = false;

	m_szSpropVPS = 0;
	m_szSpropSPS = 0;
	m_szSpropPPS = 0;

	m_iExtraDataLen = 0;

	m_bHasVPS = false;

	char szMsg[2048];
	const char* pSDPLines = subsession.savedSDPLines();
	int iLen = strlen(pSDPLines);
	if (iLen < 2048)
	{
		sprintf(szMsg, "SDP: [%s]\n", subsession.savedSDPLines());
		OutputDebugString(szMsg);
	}

//	sprintf(szMsg,"type:[%s] codec: [%s] Frequency: [%d] channel:%d\n",subsession.mediumName(),subsession.codecName(),subsession.rtpTimestampFrequency(),subsession.numChannels());
//	OutputDebugString(szMsg);

	m_iSample = subsession.rtpTimestampFrequency();
	m_iBits = 0;
	m_iChannels = 1;
	//G726-16/24/32
	if (strcmp(subsession.mediumName(), "audio") == 0)
	{
		if(strstr((char*)subsession.codecName(),"G726"))
		{
			char* pTemp = strstr((char*)subsession.codecName(),"-");
			if(pTemp)
			{
				m_iBits = atoi(pTemp + 1);
			}
		}
		else if(strstr((char*)subsession.codecName(),"MPEG4-GENERIC") || strstr((char*)subsession.codecName(),"mpeg4-generic"))
		{
			m_iChannels = subsession.numChannels();
			m_iBits = m_iChannels * 8;
			//m_iSample = subsession.rtpTimestampFrequency() * subsession.numChannels();
		}
	}

	//mpeg4-generic/8000/2

	m_iDataFormat = GetDataFormat((char*)subsession.mediumName(),(char*)subsession.codecName());

	if (strcmp(subsession.mediumName(), "video") == 0)
	{
		if (pObj)
			pObj->DoEvent2(RE_VIDEO_CODEC, (void*)subsession.codecName(), (void*)m_iDataFormat);
	}
	else if (strcmp(subsession.mediumName(), "audio") == 0)
	{
		if (pObj)
			pObj->DoEvent2(RE_AUDIO_CODEC, (void*)subsession.codecName(), (void*)m_iDataFormat);
	}

	if (strcmp(subsession.mediumName(), "video") == 0 
					&& strcmp(subsession.codecName(), "H264") == 0 
					&& subsession.fmtp_spropparametersets() != NULL) 
	{
		strcpy(m_SPropParameterSetsStr,(char*)subsession.fmtp_spropparametersets());

		m_iWidth = subsession.videoWidth();
		m_iHeight = subsession.videoHeight();

		//sprintf(szMsg, "DummySink::DummySink w:%d h:%d\n", m_iWidth, m_iHeight);
		//OutputDebugString(szMsg);
	}
	else if(strcmp(subsession.mediumName(), "video") == 0 
					&& strcmp(subsession.codecName(), "H265") == 0 )
	{
		if(subsession.fmtp_spropvps() != NULL)
		{
			int iVPSLen = strlen(subsession.fmtp_spropvps());
			m_szSpropVPS = new char[iVPSLen + 1];
			strcpy(m_szSpropVPS,(char*)subsession.fmtp_spropvps());
		}

		if(subsession.fmtp_spropsps() != NULL)
		{
			int iSPSLen = strlen(subsession.fmtp_spropsps());
			m_szSpropSPS = new char[iSPSLen + 1];
			strcpy(m_szSpropSPS,(char*)subsession.fmtp_spropsps());
		}

		if(subsession.fmtp_sproppps() != NULL)
		{
			int iPPSLen = strlen(subsession.fmtp_sproppps());
			m_szSpropPPS = new char[iPPSLen + 1];
			strcpy(m_szSpropPPS,(char*)subsession.fmtp_sproppps());
		}

		m_iWidth = subsession.videoWidth();
		m_iHeight = subsession.videoHeight();

		//sprintf(szMsg,"DummySink::DummySink w:%d h:%d\n",m_iWidth,m_iHeight);
		//OutputDebugString(szMsg);

		//RE_FRAME_SIZE
			//RE_VIDEO_HEIGHT,
	}

	m_ulStartTimestamp = GetCurTimestamp();
}

DummySink::~DummySink() 
{
  delete[] fReceiveBuffer;
  delete[] fStreamId;

  if(m_pTempBuffer)
		delete m_pTempBuffer;

  if(m_szSpropVPS)
		delete m_szSpropVPS;
  if(m_szSpropSPS)
		delete m_szSpropSPS;
  if(m_szSpropPPS)
	  delete m_szSpropPPS;
}

void DummySink::SetCallbackObj(CallbackObj* pObj)
{
	m_pCallbackObj = pObj;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  struct timeval presentationTime, unsigned durationInMicroseconds) 
{
	DummySink* sink = (DummySink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

// If you don't want to see debugging output for each received frame, then comment out the following line:
//#define DEBUG_PRINT_EACH_RECEIVED_FRAME 1

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
				  struct timeval presentationTime, unsigned /*durationInMicroseconds*/) 
{
	char szMsg[512];
	long iW,iH;

	bool bSkip = false;

//	sprintf(szMsg,"afterGettingFrame numTruncatedBytes = %d\n",numTruncatedBytes);
//	OutputDebugString(szMsg);

	MediaFrameInfo frame_info;

	double dTimestamp = fSubsession.getNormalPlayTime(presentationTime);
	frame_info.ulTimestampSec = m_ulStartTimestamp + dTimestamp;//presentationTime.tv_sec;
	frame_info.ulTimestampMSec = presentationTime.tv_usec;

	switch(m_iDataFormat)
	{
		case RTSP_DF_MJPEG:
			{
				int iPixelFormat = GetJpegFrameSize(fReceiveBuffer,(int*)&iW,(int*)&iH);
				if(iPixelFormat == -1)
				{
					return ;
				}

				m_iIPCount = 0;
				frame_info.iIsIFrame = 1;
				m_iWidth = iW;
				m_iHeight = iH;

				if(iPixelFormat == 0)
					m_iPixelFormat = MJ_PF_YUV420;
				else
					m_iPixelFormat = MJ_PF_YUV422;

				frame_info.iPixelFormat = m_iPixelFormat;

//				sprintf(szMsg,"RTSP_DF_MJPEG %d %d %d \n",m_iWidth,m_iHeight,iPixelFormat);
//				OutputDebugString(szMsg);
			}
			break;
		case RTSP_DF_MPEG4:
			{
				if(CheckFrameType(fReceiveBuffer) == 0)
				{
					REFERENCE_TIME time;
					long lRet = ParseMPEG4Header(fReceiveBuffer,(frameSize + m_iOffset), &iW,&iH,&time);
					if(lRet != -1)
					{
						m_iIPCount = 0;
						frame_info.iIsIFrame = 1;
						m_iWidth = iW;
						m_iHeight = iH;

//						sprintf(szMsg,"RTSP_DF_MPEG4 %d %d \n",m_iWidth,m_iHeight);
//						OutputDebugString(szMsg);
					}
					else
						m_iIPCount++;
				}
			}
			break;
		case RTSP_DF_H264:
			{
			
				if (h264_is_start_code(fReceiveBuffer))
				{
					unsigned char ucType = h264_nal_unit_type(fReceiveBuffer);

					frame_info.iFrameType = ucType;
					/*
					switch (ucType)
					{
						case H264_NAL_TYPE_SEQ_PARAM:
							sprintf(szMsg, "DummySink::afterGettingFrame - type = SPS [%d]\n", m_iOffset);
							OutputDebugString(szMsg);

							memcpy(m_SPS, fReceiveBuffer, frameSize);
							m_iSPSLen = frameSize;
							//bSkip = true;
							break;
						case H264_NAL_TYPE_PIC_PARAM:
							sprintf(szMsg, "DummySink::afterGettingFrame - type = PPS [%d]\n", m_iOffset);
							OutputDebugString(szMsg);
							memcpy(m_PPS, fReceiveBuffer, frameSize);
							m_iPPSLen = frameSize;
							//bSkip = true;
							break;
						case H264_NAL_TYPE_NON_IDR_SLICE:
							sprintf(szMsg, "DummySink::afterGettingFrame - type = NON_IDR_SLICE\n");   //P
							OutputDebugString(szMsg);
							break;
						case H264_NAL_TYPE_IDR_SLICE:
							sprintf(szMsg, "DummySink::afterGettingFrame - type = H264_NAL_TYPE_IDR_SLICE\n");  //I
							OutputDebugString(szMsg);
							break;
						default:
							sprintf(szMsg, "DummySink::afterGettingFrame - type = Other (%d)\n", ucType);
							OutputDebugString(szMsg);
					}*/

				}
				
				bool bRet = ffmpeg_find_h264_size (fReceiveBuffer,(frameSize + m_iOffset), (int*)&iW,(int*)&iH);
				if(bRet)
				{
					if (frame_info.iFrameType == H264_NAL_TYPE_IDR_SLICE)
					{
						m_iIPCount = 0;
						frame_info.iIsIFrame = 1;
					}
					else if (frame_info.iFrameType == H264_NAL_TYPE_SEQ_PARAM || frame_info.iFrameType == H264_NAL_TYPE_PIC_PARAM)
							m_iIPCount = -1;
					m_iWidth = iW;
					m_iHeight = iH;

//					sprintf(szMsg,"RTSP_DF_H264 %d %d \n",m_iWidth,m_iHeight);
//					OutputDebugString(szMsg);
				}
				else
				{
					if (frame_info.iFrameType == H264_NAL_TYPE_SEQ_PARAM || frame_info.iFrameType == H264_NAL_TYPE_PIC_PARAM)
						m_iIPCount = -1;
					else if (frame_info.iFrameType == H264_NAL_TYPE_NON_IDR_SLICE)
						m_iIPCount++;
				}
			}
			break;
		case RTSP_DF_H265:
			{
				if(m_bHasVPS)
				{
					MediaFrameInfo extra_frame_info;
					extra_frame_info.iFormat = RTSP_DF_VIDEO_EXTRA_DATA;

					extra_frame_info.iLen = m_iExtraDataLen;
					extra_frame_info.pBuffer = (unsigned char*)m_szExtraData;

					if(m_pCallbackObj)
						m_pCallbackObj->ProcessFrameData(m_ulID,&extra_frame_info);

					m_bHasVPS = false;
				}
			}
			break;
		case RTSP_DF_MPEG_A:
		case RTSP_DF_G726:
		case RTSP_DF_MULAW:
		case RTSP_DF_ALAW:
		case RTSP_DF_AAC:
		case RTSP_DF_L16:
			{
				frame_info.iSample = m_iSample;
				frame_info.iBits = m_iBits;
				frame_info.iChannels = m_iChannels;
			}
			break;
	}

	frame_info.iFormat = m_iDataFormat;
	frame_info.iLen = frameSize + m_iOffset;
	frame_info.iWidth = m_iWidth;
	frame_info.iHeight = m_iHeight;
	frame_info.pBuffer = fReceiveBuffer;
	frame_info.iIPCount = m_iIPCount;
	if(m_pCallbackObj)
		m_pCallbackObj->ProcessFrameData(m_ulID,&frame_info);
  
	if(!m_bStop)
	{
	  // Then continue, to request the next frame of data:
	  continuePlaying();
	}
}

Boolean DummySink::continuePlaying() 
{
	if (fSource == NULL) 
		return False; // sanity check (should not happen)

	int iOffset = 0;
	unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};

	bool bSkip = false;

	if(m_iDataFormat == RTSP_DF_H264)
	{
		/*
		if (!m_bHaveWrittenFirstFrame) 
		{
			unsigned numSPropRecords;
			SPropRecord* sPropRecords = parseSPropParameterSets(m_SPropParameterSetsStr, numSPropRecords);

			for (unsigned i = 0; i < numSPropRecords; ++i) {

				if (sPropRecords[i].sPropLength == 0) continue; // bad data
				u_int8_t nal_unit_type = (sPropRecords[i].sPropBytes[0])&0x1F;
				if (nal_unit_type == 7) //SPS
				{
					memcpy(fReceiveBuffer + iOffset,start_code, 4);
					iOffset += 4;
					memcpy(fReceiveBuffer + iOffset,sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
					iOffset += sPropRecords[i].sPropLength;
					
					Parse(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
				}
				else if (nal_unit_type == 8) //PPS
				{
					memcpy(fReceiveBuffer + iOffset,start_code, 4);
					iOffset += 4;
					memcpy(fReceiveBuffer + iOffset,sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
					iOffset += sPropRecords[i].sPropLength;
				}
				
			}
			delete[] sPropRecords;
			m_bHaveWrittenFirstFrame = true; // for next time	
		}*/

		memcpy(fReceiveBuffer + iOffset,start_code, 4);
		iOffset += 4;
	}
	else if(m_iDataFormat == RTSP_DF_H265)
	{
		// extra_data [start_code + VPS + start_code + SPS + start_code + PPS]
		if (!m_bHaveWrittenFirstFrame)
		{
			u_int8_t* vps = NULL; unsigned vpsSize = 0;
			u_int8_t* sps = NULL; unsigned spsSize = 0;
			u_int8_t* pps = NULL; unsigned ppsSize = 0;

			m_iExtraDataLen = 0;
			memset(m_szExtraData,0,1024);

			SPropRecord* sPropRecords[3];
			unsigned numSPropRecords[3];

			sPropRecords[0] = parseSPropParameterSets(m_szSpropVPS, numSPropRecords[0]);
			sPropRecords[1] = parseSPropParameterSets(m_szSpropSPS, numSPropRecords[1]);
			sPropRecords[2] = parseSPropParameterSets(m_szSpropPPS, numSPropRecords[2]);

			for (unsigned j = 0; j < 3; ++j) 
			{
				SPropRecord* records = sPropRecords[j];
				unsigned numRecords = numSPropRecords[j];

				for (unsigned i = 0; i < numRecords; ++i) 
				{
					if (records[i].sPropLength == 0) 
						continue; // bad data

					u_int8_t nal_unit_type = ((records[i].sPropBytes[0])&0x7E)>>1;
					if (nal_unit_type == 32)  //VPS
					{
						vps = records[i].sPropBytes;
						vpsSize = records[i].sPropLength;

						//start code
						memcpy(m_szExtraData + m_iExtraDataLen,start_code, 4);
						m_iExtraDataLen += 4;

						memcpy(m_szExtraData + m_iExtraDataLen,records[i].sPropBytes, records[i].sPropLength);
						m_iExtraDataLen += records[i].sPropLength;
						m_bHasVPS = true;
					} 
					else if (nal_unit_type == 33)  //SPS
					{
						sps = records[i].sPropBytes;
						spsSize = records[i].sPropLength;

						//start code
						memcpy(m_szExtraData + m_iExtraDataLen,start_code, 4);
						m_iExtraDataLen += 4;

						memcpy(m_szExtraData + m_iExtraDataLen,records[i].sPropBytes, records[i].sPropLength);
						m_iExtraDataLen += records[i].sPropLength;
					} 
					else if (nal_unit_type == 34)  //PPS
					{
						pps = records[i].sPropBytes;
						ppsSize = records[i].sPropLength;

						//start code
						memcpy(m_szExtraData + m_iExtraDataLen,start_code, 4);
						m_iExtraDataLen += 4;

						memcpy(m_szExtraData + m_iExtraDataLen,records[i].sPropBytes, records[i].sPropLength);
						m_iExtraDataLen += records[i].sPropLength;
					}
				}
			}
			
			delete[] sPropRecords[0]; delete[] sPropRecords[1]; delete[] sPropRecords[2];
			m_bHaveWrittenFirstFrame = true; // for next time	
		}
		else
		{
			u_int8_t nal_unit_type = ((fReceiveBuffer[0])&0x7E)>>1;
			switch (nal_unit_type) 
			{
				/* video parameter set (VPS) */
				case 32:
				/* sequence parameter set (SPS) */
				case 33:
				/* picture parameter set (PPS) */
				case 34:
					{
						bSkip = true;
					}
					break;
			}

			memcpy(fReceiveBuffer + iOffset,start_code, 4);
			iOffset += 4;
		}
	}

	m_iOffset = iOffset;

	// Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer + iOffset, fBufferSize - iOffset,
						afterGettingFrame, this,
						onSourceClosure, this);
  return True;
}

int DummySink::GetDataFormat(char* szMediumName,char* szCodecName)
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
		else if(strstr(szCodecName,"mpeg4-generic")||strstr(szCodecName,"MPEG4-GENERIC"))
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

void DummySink::Stop()
{
	m_bStop = true;
}
