//============================================================================
// Name        : H264.cpp
// Author      : Hurley
// Mail		   : 1150118636@qq.com
// Version     : 1.0.0
// Create On   : Dec 22, 2018
// Copyright   : Copyright (c) 2018 Hurley All rights reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	NALU_TYPE_SLICE 	= 1,
	NALU_TYPE_DPA 		= 2,
	NALU_TYPE_DPB 		= 3,
	NALU_TYPE_DPC 		= 4,
	NALU_TYPE_IDR 		= 5,
	NALU_TYPE_SEI 		= 6,
	NALU_TYPE_SPS 		= 7,
	NALU_TYPE_PPS 		= 8,
	NALU_TYPE_AUD 		= 9,
	NALU_TYPE_EOSEQ 	= 10,
	NALU_TYPE_EOSTREAM 	= 11,
	NALU_TYPE_FILL 		= 12,
}NaluType;

typedef enum{
	NALU_PRIORITY_DISPOSABLE 	= 0,
	NALU_PRIORITY_LOW 			= 1,
	NALU_PRIORITY_HIGHT 		= 2,
	NALU_PRIORITY_HEIGHEST 		= 3,
}NaluPriority;

typedef struct{
	int startcodeprefix_len;	//4用于参数集和第一个切片图片，3用于其他一切（建议）
	unsigned len;				//NAL单位的长度（不包括起始码，不属于NALU）
	unsigned max_size;			//Nal单位缓冲区大小
	int forbidden_bit;
	int nalreference_idc;
	int nal_unit_type;
	char *buf;					//包含EBSP后跟的第一个字节
}NALU_t;

FILE *h264bitstream = NULL;

int info2 = 0,info3 = 0;

static int FindStartCode2(unsigned char *Buf)
{
	if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 1)	//0x000001?
		return 0;
	else
		return 1;
}


static int FindStartCode3 (unsigned char *Buf){
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1)  //0x00000001?
		return 0;
	else return 1;
}









