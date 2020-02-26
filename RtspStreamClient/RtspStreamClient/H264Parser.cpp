#include "stdafx.h"
#include "H264Parser.h"

#define H264_START_CODE 0x000001
#define H264_NAL_TYPE_SEQ_PARAM 0x7

#define uint unsigned int

typedef struct h264_decode_t {
  uint8_t profile;
  uint8_t level;
  uint32_t chroma_format_idc;
  uint8_t residual_colour_transform_flag;
  uint32_t bit_depth_luma_minus8;
  uint32_t bit_depth_chroma_minus8;
  uint8_t qpprime_y_zero_transform_bypass_flag;
  uint8_t seq_scaling_matrix_present_flag;
  uint32_t log2_max_frame_num_minus4;
  uint32_t log2_max_pic_order_cnt_lsb_minus4;
  uint32_t pic_order_cnt_type;
  uint8_t frame_mbs_only_flag;
  uint8_t pic_order_present_flag;
  uint8_t delta_pic_order_always_zero_flag;
  int32_t offset_for_non_ref_pic;
  int32_t offset_for_top_to_bottom_field;
  uint32_t pic_order_cnt_cycle_length;
  int16_t offset_for_ref_frame[256];

  uint8_t nal_ref_idc;
  uint8_t nal_unit_type;
  
  uint8_t field_pic_flag;
  uint8_t bottom_field_flag;
  uint32_t frame_num;
  uint32_t idr_pic_id;
  uint32_t pic_order_cnt_lsb;
  int32_t delta_pic_order_cnt_bottom;
  int32_t delta_pic_order_cnt[2];

  uint32_t pic_width, pic_height;
  uint32_t slice_type;

  /* POC state */
  int32_t  pic_order_cnt;        /* can be < 0 */

  uint32_t  pic_order_cnt_msb;
  uint32_t  pic_order_cnt_msb_prev;
  uint32_t  pic_order_cnt_lsb_prev;
  uint32_t  frame_num_prev;
  int32_t  frame_num_offset;
  int32_t  frame_num_offset_prev;

  uint8_t NalHrdBpPresentFlag;
  uint8_t VclHrdBpPresentFlag;
  uint8_t CpbDpbDelaysPresentFlag;
  uint8_t pic_struct_present_flag;
  uint8_t cpb_removal_delay_length_minus1;
  uint8_t time_offset_length;
  uint32_t cpb_cnt_minus1;
  uint8_t initial_cpb_removal_delay_length_minus1;
} h264_decode_t;

bool h264_is_start_code (const uint8_t *pBuf) 
{
  if (pBuf[0] == 0 && 
      pBuf[1] == 0 && 
      ((pBuf[2] == 1) ||
       ((pBuf[2] == 0) && pBuf[3] == 1))) {
    return true;
  }
  return false;
}

uint8_t h264_nal_unit_type (const uint8_t *buffer)
{
  uint32_t offset;
  if (buffer[2] == 1) 
	  offset = 3;
  else 
	  offset = 4;
  return buffer[offset] & 0x1f;
}

uint32_t h264_find_next_start_code (const uint8_t *pBuf, 
					       uint32_t bufLen)
{
  uint32_t val, temp;
  uint32_t offset;

  offset = 0;
  if (pBuf[0] == 0 && 
      pBuf[1] == 0 && 
      ((pBuf[2] == 1) ||
       ((pBuf[2] == 0) && pBuf[3] == 1))) 
  {
    pBuf += 3;
    offset = 3;
  }
  val = 0xffffffff;
  while (offset < bufLen - 3) 
  {
    val <<= 8;
    temp = val & 0xff000000;
    val &= 0x00ffffff;
    val |= *pBuf++;
    offset++;
    if (val == H264_START_CODE) 
	{
      if (temp == 0) 
		  return offset - 4;
      return offset - 3;
    }
	if(offset > 2000)
		return -1;
  }
  return 0;
}

static uint8_t exp_golomb_bits[256] = {
8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 
};

uint32_t h264_ue (CBitstream *bs)
{
  uint32_t bits, read;
  int bits_left;
  uint8_t coded;
  bool done = false;
  bits = 0;
  // we want to read 8 bits at a time - if we don't have 8 bits, 
  // read what's left, and shift.  The exp_golomb_bits calc remains the
  // same.
  while (done == false) 
  {
    bits_left = bs->bits_remain();
    if (bits_left < 8) 
	{
      read = bs->PeekBits(bits_left) << (8 - bits_left);
      done = true;
    } 
	else 
	{
      read = bs->PeekBits(8);
      if (read == 0) 
	  {
		bs->GetBits(8);
		bits += 8;
      } 
	  else 
	  {
		done = true;
      }
    }
  }
  coded = exp_golomb_bits[read];
  bs->GetBits(coded);
  bits += coded;

  //  printf("ue - bits %d\n", bits);
  return bs->GetBits(bits + 1) - 1;
}

int32_t h264_se (CBitstream *bs) 
{
  uint32_t ret;
  ret = h264_ue(bs);
  if ((ret & 0x1) == 0) 
  {
    ret >>= 1;
    int32_t temp = 0 - ret;
    return temp;
  } 
  return (ret + 1) >> 1;
}

void h264_decode_annexb( uint8_t *dst, int *dstlen,
                                const uint8_t *src, const int srclen )
{
  uint8_t *dst_sav = dst;
  const uint8_t *end = &src[srclen];

  while (src < end)
  {
    if (src < end - 3 && src[0] == 0x00 && src[1] == 0x00 &&
        src[2] == 0x03)
    {
      *dst++ = 0x00;
      *dst++ = 0x00;

      src += 3;
      continue;
    }
    *dst++ = *src++;
  }

  *dstlen = dst - dst_sav;
}

void scaling_list (uint sizeOfScalingList, CBitstream *bs)
{
  uint lastScale = 8, nextScale = 8;
  uint j;

  for (j = 0; j < sizeOfScalingList; j++) 
  {
    if (nextScale != 0) 
	{
      int deltaScale = h264_se(bs);
      nextScale = (lastScale + deltaScale + 256) % 256;
    }
    if (nextScale == 0) 
	{
      lastScale = lastScale;
    } 
	else 
	{
      lastScale = nextScale;
    }
  }
}

int h264_read_seq_info (const uint8_t *buffer, 
			uint32_t buflen, 
			h264_decode_t *dec)
{
  CBitstream bs;
  uint32_t header;
  uint8_t tmp[2048]; /* Should be enough for all SPS (we have at worst 13 bytes and 496 se/ue in frext) */
  int tmp_len;

  if (buffer[2] == 1) header = 4;
  else header = 5;

  h264_decode_annexb( tmp, &tmp_len, buffer + header, MIN(buflen-header,2048) );
  bs.init(tmp, tmp_len * 8);

  //bs.set_verbose(true);
  try {
    dec->profile = bs.GetBits(8);
    bs.GetBits(1 + 1 + 1 + 1 + 4);
    dec->level = bs.GetBits(8);
    h264_ue(&bs); // seq_parameter_set_id
    if (dec->profile == 100 || dec->profile == 110 ||
	dec->profile == 122 || dec->profile == 144) {
      dec->chroma_format_idc = h264_ue(&bs);
      if (dec->chroma_format_idc == 3) {
	dec->residual_colour_transform_flag = bs.GetBits(1);
      }
      dec->bit_depth_luma_minus8 = h264_ue(&bs);
      dec->bit_depth_chroma_minus8 = h264_ue(&bs);
      dec->qpprime_y_zero_transform_bypass_flag = bs.GetBits(1);
      dec->seq_scaling_matrix_present_flag = bs.GetBits(1);
      if (dec->seq_scaling_matrix_present_flag) {
	for (uint ix = 0; ix < 8; ix++) {
	  if (bs.GetBits(1)) {
	    scaling_list(ix < 6 ? 16 : 64, &bs);
	  }
	}
      }
    }
    dec->log2_max_frame_num_minus4 = h264_ue(&bs);
    dec->pic_order_cnt_type = h264_ue(&bs);
    if (dec->pic_order_cnt_type == 0) {
      dec->log2_max_pic_order_cnt_lsb_minus4 = h264_ue(&bs);
    } else if (dec->pic_order_cnt_type == 1) {
      dec->delta_pic_order_always_zero_flag = bs.GetBits(1);
      dec->offset_for_non_ref_pic = h264_se(&bs); // offset_for_non_ref_pic
      dec->offset_for_top_to_bottom_field = h264_se(&bs); // offset_for_top_to_bottom_field
      dec->pic_order_cnt_cycle_length = h264_ue(&bs); // poc_cycle_length
      for (uint32_t ix = 0; ix < dec->pic_order_cnt_cycle_length; ix++) {
        dec->offset_for_ref_frame[MIN(ix,255)] = h264_se(&bs); // offset for ref fram -
      }
    }
    h264_ue(&bs); // num_ref_frames
    bs.GetBits(1); // gaps_in_frame_num_value_allowed_flag
    uint32_t PicWidthInMbs = h264_ue(&bs) + 1;
    dec->pic_width = PicWidthInMbs * 16;
    uint32_t PicHeightInMapUnits = h264_ue(&bs) + 1;

    dec->frame_mbs_only_flag = bs.GetBits(1);
    dec->pic_height = 
      (2 - dec->frame_mbs_only_flag) * PicHeightInMapUnits * 16;
#if 0
    if (!dec->frame_mbs_only_flag) {
      printf("    mb_adaptive_frame_field_flag: %u\n", bs->GetBits(1));
    }
    printf("   direct_8x8_inference_flag: %u\n", bs->GetBits(1));
    temp = bs->GetBits(1);
    printf("   frame_cropping_flag: %u\n", temp);
    if (temp) {
      printf("     frame_crop_left_offset: %u\n", h264_ue(bs));
      printf("     frame_crop_right_offset: %u\n", h264_ue(bs));
      printf("     frame_crop_top_offset: %u\n", h264_ue(bs));
      printf("     frame_crop_bottom_offset: %u\n", h264_ue(bs));
    }
    temp = bs->GetBits(1);
    printf("   vui_parameters_present_flag: %u\n", temp);
    if (temp) {
      h264_vui_parameters(bs);
    }
#endif
  } catch (...) {
    return -1;
  }
  return 0;
}

bool ffmpeg_find_h264_size (const uint8_t *ud, uint32_t ud_size,int* w,int* h)
{
	//int iCount = 0;
	int iRet = 0;
	uint32_t offset = 0;
	do 
	{
        if (h264_is_start_code(ud + offset)) 
		{
			if (h264_nal_unit_type(ud + offset) == H264_NAL_TYPE_SEQ_PARAM) 
			{
				h264_decode_t dec;
				memset(&dec, 0, sizeof(dec));
				if (h264_read_seq_info(ud + offset, ud_size - offset, &dec) == 0) 
				{
					*w = dec.pic_width;
					*h = dec.pic_height;
					return true;
				}
			}
			else
				return false;
		}
		iRet = h264_find_next_start_code(ud + offset, ud_size - offset);
		if(iRet > 0)
			offset += iRet;//h264_find_next_start_code(ud + offset, ud_size - offset);
		else
			return false;
		//TRACE("iCount - %d offset:%d iRet:%d\n",iCount,offset,iRet);
		//iCount++;
  } while (offset < ud_size);
  return false;
}

