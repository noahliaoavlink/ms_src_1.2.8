/*
 *  File:	Bitop.cpp
 *  Description :	
 *  implementation of some bit-wised operation
 *
 *  Authors : Hui-Ping Kuo<joe.hpkuo@gmail.com>                                                                                                                                
 *  Copyright   2006     
                                            
 *  Industrial Technology Research Institute (http://www.itri.org.tw)            
 */
/*
//#include "stdafx.h"
#ifdef _DEBUG	// debug memory leak
   #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#include <stdlib.h>
#include <math.h>
#include "BitOP.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
*/

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "BitOP.h"

//BYTE seq[8] = {0xff, 0xa0, 0xAA, 0xc3, 0x81, 0xe7, 0xd4, 0xb6};

BYTE Get8Bits(BYTE *ptr, long offset)
{
	long boffset = (long) floor( (double) offset /8.0 );
	long k = offset % 8;
	BYTE lb = ((*(ptr+boffset) & LMask[k]) << k);
	BYTE rb = ((*(ptr+boffset+1) & RMask[k]) >> (8-k)); 

	return lb | rb;
	//return ((*(ptr+boffset) & LMask[k]) << k) | ((*(ptr+boffset+1) & RMask[k]) >> (8-k));
}


void GetBits(BYTE *val, BYTE *ptr, long offset, long len)
{
	long blen = (long)ceil( (double) len/8 );
	long boffset = (long)floor( (double) offset /8 );
	long k = offset % 8;
	long j = len % 8;
//	*val = new BYTE[ blen ];

	BYTE *p = val;

	for(int i=(blen-1); i>=0; i--) {
		if(i!=0) {
			BYTE bb = Get8Bits(ptr, offset+ len - 8);
			*(p+i) = bb ;
		}
		else {
			if(j==0) {
				j=8;
			}
				
			if(j+k<=8) {
				//the bits of the first byte does not cross two bytes
				*p = (*(ptr+boffset) >> (8-k-j)) & Mask[j];
			}
			else {
				long r = (k+j-8);
				BYTE lb = ( (*(ptr+boffset) & Mask[8-k]) << r );
				BYTE rb = ((*(ptr+boffset+1) >> (8-r) )  & Mask[r] );
				*p = lb | rb;
			}
		}
	}

}

unsigned short ShortValue(BYTE *val)
{
	return (val[0] << 8) + val[1];
}

unsigned long LongValue(BYTE *val)
{
	return (val[0] << 24) + (val[1] << 16) + (val[2] << 8) + val[3];
}
	