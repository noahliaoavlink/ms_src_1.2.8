#include "stdafx.h"
#include "MVideoHeaderParser.h"
#include "BitOP.h"

unsigned char* SkipJpegData(unsigned char* p)
{
	//DQT
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xDB)
	{
		unsigned char ucLen = *(p + 3);
		p += 2;
		p += ucLen;
	}

	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xFE)
	{
#if 0
		unsigned char ucLen = *(p + 3);
		p += 2;
		p += ucLen;
#else
		unsigned short usLen = *(p + 3) + (*(p + 2) << 8);
		p += 2;
		p += usLen;
#endif
	}

	//DHT
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xC4)
	{
		p += 2;
		unsigned short usLen = (*(p) << 8) + *(p + 1);
		p += usLen;
	}

	//DRI
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xDD)
	{
		p += 2;
		unsigned short usLen = (*(p) << 8) + *(p + 1);
		p += usLen;
	}

	//SOS
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xDA)
	{
		p += 2;
		unsigned short usLen = (*(p) << 8) + *(p + 1);
		p += usLen;
	}

	return p;
}

int GetJpegFrameSize(unsigned char* pBuffer,int* iW,int* iH)
{
	int i;
    int iPixelFormat = 0;
	//parse jpeg
	unsigned char* p = pBuffer;
	//SOI
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xD8)
		p += 2;

	//APP0
	if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xE0)
	{
        p += 2;
		p += 16;
	}

	for(i = 0;i < 16;i++)
	{
		//APP0~15
		if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == (0xE0 + i))
		{
			p += 2;
			unsigned short usLen = (*(p) << 8) + *(p + 1);
			p += usLen;
		}
	}

	while(1)
	{
		if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xC0)
			break;
		if((unsigned char)*p != 0xFF)
			return -1;
		p = SkipJpegData(p);
	}

	//SOF0
    if((unsigned char)*p == 0xFF && (unsigned char)*(p+1) == 0xC0)
	{
		*iH = (*(p + 5) << 8) + *(p + 6);  //5,6
		*iW = (*(p + 7) << 8) + *(p + 8);  //7,8

		 unsigned char ucComponentID[3];
		 unsigned char ucHV[3];
		 unsigned char ucHSamplingFactor[3];
		 unsigned char ucVSamplingFactor[3];
		 unsigned char ucQTable[3];
		 unsigned char ucColorComponents = *(p + 9);
		 int iCount = 10;
		 for(i = 0;i < ucColorComponents;i++)
		 {
			 ucComponentID[i] = *(p + iCount);
			 iCount++;
			 ucHV[i] = *(p + iCount);
			 ucHSamplingFactor[i] = ucHV[i] >> 4;
			 ucVSamplingFactor[i] = ucHV[i] % 16;
			 iCount++;
			 ucQTable[i] = *(p + iCount);
			 iCount++;
		 }

		 if(ucVSamplingFactor[0] == 2 && ucVSamplingFactor[1] == 1 && ucVSamplingFactor[2] == 1)
			 iPixelFormat = 1;
		 else
			 iPixelFormat = 0;
	}
	return iPixelFormat;
}

long FindMPEGStartCode(BYTE **ptr,int iBufLen)
{
	int iCount = 0;
	while( !(**ptr==0x00 && *(*ptr+1)==0x00 && *(*ptr+2)==0x01) ) {
		*ptr+=1;

		iCount++;
		if(iCount >= iBufLen || iCount >= 200)
			return -1;
	}
	return 0;
}

long Get_fvopi_Length(BYTE *vop_tir)
{
	long i=0;
	short temp = ShortValue(vop_tir);
	while( temp != 0 ) {
		temp = temp >> 1;
		i++;
	}
	return i;
}

long ParseMPEG4Header(BYTE *hdr, long hdrlen, long *width, 
										  long *height, REFERENCE_TIME *time)
{
	BYTE *ptr = hdr;
	int iCount = 0;

	int iRet = FindMPEGStartCode( &ptr,hdrlen);
	while( !(*ptr==0x00 && *(ptr+1)==0x00 && *(ptr+2)==0x01 &&
		    (*(ptr+3) & 0xf0) == 0x20) ) 
	{
		//move the pointer until Video_Object_Layer_Start_Code is found
		ptr++;
		iRet = FindMPEGStartCode( &ptr,hdrlen - iCount);
		iCount++;
		if(hdrlen <= iCount || iCount >= 200)
			break;
	}

	if(iRet == -1)
		return -1;

	//now the ptr is pointing to the Video_Object_Layer_Start_Code
	BYTE voti[10];			//video_object_type_indication
	GetBits(voti, ptr, 33, 8);
	if(*voti == FINE_GRANULARITY_SCALABLE ) {
	}
	else {
		long offset = 41;

		BYTE oli[10];						//object_layer_identifier
		BYTE vol_verid[10];					//video_object_layer_verid, valid only if oli==TRUE
		BYTE vol_priority[10];				//video_object_layer_priority, valid only if oli==TRUE
		
		BYTE par[1];
		BYTE parw[10];						//par width, 8bits, valid only if par == TRUE;
		BYTE parh[10];						//par height, 8bits, valid only if par == TRUE;
		
		BYTE vcp[10];						//vol_control_parameters

		BYTE vols[10];						//video object layer shape

		BYTE vop_tir[10];					//vop time incremental reslution
		BYTE f_vop_r[10];					//fixed vop rate?
		BYTE f_vop_ti[10];					//fixed vop rate increment

		BYTE vol_width[10];					//width
		BYTE vol_height[10];				//height

		//==============================start to parse===================================

		//check is_object_layer_identifier
		GetBits(oli, ptr, offset, 1);
		offset++;

		if( *oli) 
		{
			GetBits(vol_verid, ptr, 42, 4);
			GetBits(vol_priority, ptr, 46, 3);
			offset += 7;
		}
		
		//check aspect ratio info
		GetBits(par, ptr, offset, 4);
		offset += 4;

		if( *par == AR_EXT_PAR) {
			//just ignore parw and parh now
			offset += 16;
		}
		
		//check vol_control_parameter
		GetBits(vcp, ptr, offset, 1);
		offset++;

		if(*vcp) {
			//3 bits for vol control parameters
			offset += 3;
			//BYTE *vbv = NULL;
			BYTE vbv[10];
			GetBits(vbv, ptr, offset, 1);
			offset++;

			if( *vbv) {
				//79 bits for vbv parameters
				offset += 79;
			}
		}
	
		GetBits(vols, ptr, offset, 2);
		offset+=2;

		if( *vols == GRAYSCALE && *vol_verid!=NULL && *vol_verid != 1 ) {
			//4 bits for video object layer shape extension
			offset += 4;
		}

		offset++;					//skip marker bit

		GetBits(vop_tir, ptr, offset, 16);

		offset += 17;				//skip 16bits vop_tir and marker bit
		GetBits(f_vop_r, ptr, offset, 1);

		offset ++;
		if( *f_vop_r) {
			long len = Get_fvopi_Length(vop_tir);
			GetBits(f_vop_ti, ptr, offset, len);
			long t;
			if(len >8) {
				t = (long)ShortValue(f_vop_ti);
			}
			else {
				t = (long)(*f_vop_ti);
			}
			
			*time = (_int64)(10000000 * ((double)t / (double)ShortValue(vop_tir)));

			offset += len;
		}

		if( *vols != BINARY_ONLY ) {
			if( *vols == RECTANGULAR ) {
				offset++;			//skip marker bit
				GetBits(vol_width, ptr, offset, 13);
				*width = (long)ShortValue(vol_width);
				offset+=14;			//add 13 bits and skip marker bit
				GetBits(vol_height, ptr, offset, 13);
				*height = (long)ShortValue(vol_height);
			}
		}
	}
	return 0;
}

int CheckFrameType(unsigned char* pBuffer)
{
	unsigned   char   vt_byte   =   pBuffer[4];   
	vt_byte   &=   0xC0;   
	vt_byte   =   vt_byte   >>   6;   
/*
	unsigned char* p = pBuffer;
	TRACE("Mpeg4 [%x , %x , %x , %x , %x , %x , %x , %x , %x , %x]\n",
		p[0],p[1],p[2],p[3],p[4],
		p[5],p[6],p[7],p[8],p[9]);
*/
	if(pBuffer[3] != 0xb6)
		return 0;

	switch(vt_byte)   
	{   
		case   0:   
			return   0;//i_frame;   
			break;   
		case   1:   
			return   1;//p_frame;   
			break;   
		case   2:   
			return   2;//b_frame;   
			break;   
		case   3:   
			return   3;//d_frame;   
			break;   
        default:
			return -1;
	}
}