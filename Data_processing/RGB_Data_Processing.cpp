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
 * @param: 	width:视频帧的宽度
 * 			height:视频帧的高度
 * 			num:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int simplest_rgb24_split(char *url, int width, int height, int num)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_r.y","wb+");
	FILE *fp2 = fopen("output_g.y","wb+");
	FILE *fp3 = fopen("output_b.y","wb+");

	unsigned char *pic = (unsigned char)*malloc(width*height*3);

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
}

int simple_rgb24_to_bmp(const char *rgb24path, int width, int height, const char *bmppath)
{
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







