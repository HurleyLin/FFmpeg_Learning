//============================================================================
// Name        : RGB_Data_Processing.cpp
// Author      : Hurley
// Mail		   : 1150118636@qq.com
// Version     : 1.0.0
// Create On   : Dec 20, 2018
// Copyright   : Copyright (c) 2018 Hurley All rights reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>


/*
 * @Description: 分离RGB24像素数据中的R、G、B分量
 * 一帧宽高分别为w、h的RGB24图像一共占用width*height*3 Byte的存储空间
 * @param: 	width	:视频帧的宽度
 * 			height	:视频帧的高度
 * 			num		:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int simplest_rgb24_split(char *url, int width, int height, int num)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_r.y","wb+");
	FILE *fp2 = fopen("output_g.y","wb+");
	FILE *fp3 = fopen("output_b.y","wb+");

	unsigned char *pic=(unsigned char *)malloc(width*height*3);

	for (int i = 0; i < num; i++)
	{
		fread(pic, 1, width*height*3, fp);

		for (int j = 0; j < width*height*3; j = j+3)
		{
			fwrite(pic+j, 1, 1, fp1);		//R
			fwrite(pic+j+1, 1, 1, fp2);		//G
			fwrite(pic+j+2, 1, 1, fp3);		//B
		}
	}

	free(pic);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;
}

int simple_rgb24_to_bmp(const char *rgb24path, int width, int height, const char *bmppath)
{
	typedef  struct  tagBITMAPFILEHEADER
	{
	unsigned short int  bfType;       //位图文件的类型，必须为BM
	unsigned long       bfSize;       //文件大小，以字节为单位
	unsigned short int  bfReserverd1; //位图文件保留字，必须为0
	unsigned short int  bfReserverd2; //位图文件保留字，必须为0
	unsigned long       bfbfOffBits;  //位图文件头到数据的偏移量，以字节为单位
	}BITMAPFILEHEADER;
	typedef  struct  tagBITMAPINFOHEADER
	{
	long biSize;                        //该结构大小，字节为单位
	long  biWidth;                     //图形宽度以象素为单位
	long  biHeight;                     //图形高度以象素为单位
	short int  biPlanes;               //目标设备的级别，必须为1
	short int  biBitcount;             //颜色深度，每个象素所需要的位数
	short int  biCompression;        //位图的压缩类型
	long  biSizeImage;              //位图的大小，以字节为单位
	long  biXPelsPermeter;       //位图水平分辨率，每米像素数
	long  biYPelsPermeter;       //位图垂直分辨率，每米像素数
	long  biClrUsed;            //位图实际使用的颜色表中的颜色数
	long  biClrImportant;       //位图显示过程中重要的颜色数
	}BITMAPINFOHEADER;

	typedef struct
	{
		long imageSize;
		long blank;
		long startPosition;
	}BmpHead;

	typedef struct
	{
		long Length;
		long width;
		long height;
		unsigned short colorPlane;
		unsigned short bitColor;
		long ZipFormat;
		long realSize;
		long xPels;
		long yPels;
		long colorUse;
		long colorImportant;
	}InfoHead;

	int i = 0,j = 0;
	BmpHead m_BMPHeader = {0};
	InfoHead m_BMPInfoHeader = {0};
	char bfType[2] = {'B','M'};
	int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
	unsigned char *rgb24_buffer = NULL;
	FILE *fp_rgb24 = NULL,*fp_bmp = NULL;

	if((fp_rgb24 = fopen(rgb24path,"rb")) == NULL)
	{
		printf("Error:Cannot open input RGB24 file.\n");
		return -1;
	}

	if ((fp_bmp = fopen(bmppath,"wb")) == NULL)
	{
		printf("Error:Connot open output BMP file.\n");
		return -1;
	}

	rgb24_buffer = (unsigned char *)malloc(width*height*3);
	fread(rgb24_buffer, 1, width*height*3, fp_rgb24);

	m_BMPHeader.imageSize = 3*width*height + header_size;
	m_BMPHeader.startPosition = header_size;

	m_BMPInfoHeader.Length = sizeof(InfoHead);
	m_BMPInfoHeader.width = width;

	m_BMPInfoHeader.height = height;
	m_BMPInfoHeader.colorPlane = 1;
	m_BMPInfoHeader.bitColor = 24;
	m_BMPInfoHeader.realSize = 3*width*height;

	fwrite(bfType, 1, sizeof(bfType),fp_bmp);
	fwrite(&m_BMPHeader, 1, sizeof(m_BMPInfoHeader), fp_bmp);
	fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader),fp_bmp);

	for (j = 0;j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			char temp = rgb24_buffer[(j*width+i)*3 + 2];
			rgb24_buffer[(j*width+i)*3+2]=rgb24_buffer[(j*width+i)*3+0];
			rgb24_buffer[(j*width+i)*3+0]=temp;
		}
	}

	fwrite(rgb24_buffer, 3*width*height, 1, fp_bmp);
	fclose(fp_rgb24);
	fclose(fp_bmp);
	free(rgb24_buffer);
	printf("Finish generate %s!\n",bmppath);

	return 0;
}

unsigned char clip_value(unsigned char x, unsigned char min_val, unsigned char max_val)
{
	if (x > max_val)
		return max_val;
	else if (x < min_val)
		return min_val;
	else
		return x;
}

//RGB到YUV的转换公式
//Y = 0.299*R+0.587*G+0.114*B
//U =-0.147*R-0.289*G+0.463*B
//V = 0.615*R-0.515*G-0.100*B
bool RGB24_TO_YUV420(unsigned char *RgbBuf, int width, int height, unsigned char *yuvBuf)
{
	unsigned char *ptrY,*ptrU,*ptrV,*ptrRGB;
	memset(yuvBuf, 0, width*height*3/2);

	ptrY = yuvBuf;
	ptrU = yuvBuf + width*height;
	ptrV = ptrU + (width*height*1/4);

	unsigned char y,u,v,r,g,b;
	for (int j = 0;j<height;j++)
	{
		ptrRGB = RgbBuf + width*height*3;
		for (int i=0;i<width;i++)
		{
			r = *(ptrRGB++);
			g = *(ptrRGB++);
			b = *(ptrRGB++);
			y = (unsigned char)( ( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16;
			u = (unsigned char)( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ;
			v = (unsigned char)( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ;
			*(ptrY++) = clip_value(y,0,255);
			if (j%2==0&&i%2 ==0)
			{
				*(ptrU++) =clip_value(u,0,255);
			}
			else
			{
				if (i%2==0)
				{
					*(ptrV++) =clip_value(v,0,255);
				}
			}
		}
	}
	return true;
}

/**
 * Convert RGB24 file to YUV420P file
 * @param url_in  Location of Input RGB file.
 * @param w       Width of Input RGB file.
 * @param h       Height of Input RGB file.
 * @param num     Number of frames to process.
 * @param url_out Location of Output YUV file.
 */
int simplest_rgb24_to_yuv420(char *url_in, int w, int h,int num,char *url_out){
	FILE *fp=fopen(url_in,"rb+");
	FILE *fp1=fopen(url_out,"wb+");

	unsigned char *pic_rgb24=(unsigned char *)malloc(w*h*3);
	unsigned char *pic_yuv420=(unsigned char *)malloc(w*h*3/2);

	for(int i=0;i<num;i++){
		fread(pic_rgb24,1,w*h*3,fp);
		RGB24_TO_YUV420(pic_rgb24,w,h,pic_yuv420);
		fwrite(pic_yuv420,1,w*h*3/2,fp1);
	}

	free(pic_rgb24);
	free(pic_yuv420);
	fclose(fp);
	fclose(fp1);

	return 0;
}




