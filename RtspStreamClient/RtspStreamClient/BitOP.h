/*
 *  File:	Bitop.h
 *  Description :	
 *  declaration of some bit-wised operation
 *
 *  Authors : Hui-Ping Kuo<joe.hpkuo@gmail.com>                                                                                                                                
 *  Copyright   2006     
                                            
 *  Industrial Technology Research Institute (http://www.itri.org.tw)            
 */
typedef unsigned char BYTE;

static BYTE LMask[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
static BYTE RMask[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
static BYTE Mask[8] =  {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f};

BYTE Get8Bits(BYTE *ptr, long offset);
void GetBits(BYTE *val, BYTE *ptr, long offset, long len);

unsigned short	ShortValue(BYTE *val);
unsigned long	LongValue(BYTE *val);		

