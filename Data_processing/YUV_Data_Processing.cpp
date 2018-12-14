//============================================================================
// Name        : YUV_Data_Processing.cpp
// Author      : Hurley
// Mail		   : 1150118636@qq.com
// Version     : 1.0.0
// Create On   : Dec 13, 2018
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
 * @Description: 分离YUV420P像素数据中的Y、U、V分量
 * 一帧YUV420P像素数据一共占用width*height*3/2 Byte的数据
 * 其中前width*height Byte存储Y，接着的width*height*1/4 Byte存储U，最后width*height*1/4 Byte存储V
 * @param: 	width:视频帧的宽度
 * 			height:视频帧的高度
 * 			num:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int simplest_yuv420_spilit(char *url, int width, int height, int num)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp_y = fopen("output_420_y.y","wb+");
	FILE *fp_u = fopen("output_420_y.u","wb+");
	FILE *fp_v = fopen("output_420_y.v","wb+");

	unsigned char *pic = (unsigned char *)malloc(width * height * 3/2);

	for (int i= 0; i < num ; i++)
	{
		fread(pic, 1, width * height * 3/2,fp);
		fwrite(pic, 1, width * height, fp_y);
		fwrite(pic + width * height, 1, width * height/4, fp_u);
		fwrite(pic + width * height * 5/4, 1, width * height/4, fp_v);
	}

	free(pic);
	fclose(fp);
	fclose(fp_y);
	fclose(fp_u);
	fclose(fp_v);

	return 0;
}

/*
 * @Description: 分离YUV444P像素数据中的Y、U、V分量
 * 一帧YUV444P像素数据一共占用width*height*3/2 Byte的数据
 * 其中前width*height Byte存储Y，接着的width*height Byte存储U，最后width*height Byte存储V
 * @param: 	width:视频帧的宽度
 * 			height:视频帧的高度
 * 			num:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int simplest_yuv444_spilit(char *url, int width, int height, int num)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp_y = fopen("output_444_y.y","wb+");
	FILE *fp_u = fopen("output_444_y.u","wb+");
	FILE *fp_v = fopen("output_444_y.v","wb+");

	unsigned char *pic = (unsigned char *)malloc(width * height * 3/2);

	for (int i= 0; i < num ; i++)
	{
		fread(pic, 1, width * height * 3,fp);
		fwrite(pic, 1, width * height, fp_y);
		fwrite(pic + width * height, 1, width * height, fp_u);
		fwrite(pic + width * height * 2, 1, width * height, fp_v);
	}

	free(pic);
	fclose(fp);
	fclose(fp_y);
	fclose(fp_u);
	fclose(fp_v);

	return 0;
}

/*
 * @Description:将YUV420P像素数据去掉颜色（变成灰度图）
 * 如果想把YUV格式像素数据变成灰度图像，只需要将U、V分量设置成128即可
 * @param: 	width:视频帧的宽度
 * 			height:视频帧的高度
 * 			num:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int samplest_yuv420_gray(char *url, int width, int height, int num)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp_gray = fopen("output_gray.yuv", "wb+");
	unsigned char *pic = (unsigned char *)malloc(width*height*3/2);

	for (int i = 0; i < num ; i++)
	{
		fread(pic, 1, width*height*3/2,fp);

		memset(pic+width*height,128,width*height/2);
		fwrite(pic, 1, width* height * 3/2, fp_gray);
	}

	free(pic);
	fclose(fp_gray);
	fclose(fp);

	return 0;
}


/*
 * @Description:将YUV420P像素数据的亮度减半
 * 将图像的亮度减半，只要将图像的每个像素的Y值取出来分别进行除以2的工作就可以了
 * @param: 	width:视频帧的宽度
 * 			height:视频帧的高度
 * 			num:视频帧数
 * @return: int
 * @Author: Hurley	2018-12-13
 */
int samplest_yuv420_halfy (char *url, int width, int height, int num)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp_halfy = fopen("output_half.yuv","wb+");

	unsigned char *pic = (unsigned char *)malloc(width*height*3/2);

	for (int i=0; i < num; i++)
	{
		fread(pic, 1, width*height*3/2, fp);

		for (int j=0; j<width*height; j++)
		{
			unsigned char temp = pic[j]/2;
			pic[j] = temp;
		}
		fwrite(pic, 1, width*height*3/2, fp_halfy);
	}

	free(pic);
	fclose(fp);
	fclose(fp_halfy);

	return 0;
}

int simplest_yuv420_graybar(int width, int height, int ymin, int ymax, int barnum, char *url_out)
{
	int barwidth;
	float lum_inc;
	unsigned char lum_temp;
	int uv_width,uv_height;
	FILE *fp = NULL;

	unsigned char *data_y = NULL;
	unsigned char *data_u = NULL;
	unsigned char *data_v = NULL;
	int t=0, i=0,j=0;

	barwidth = width * height;
	lum_inc=((float)(ymax-ymin))/((float)(barnum-1));
	uv_width = width/2;
	uv_height = height/2;

	data_y = (unsigned char *)malloc(width*height);
	data_u = (unsigned char *)malloc(uv_width*uv_height);
	data_v = (unsigned char *)malloc(uv_width*uv_height);

	if ((fp  = fopen(url_out, "wb+")) == NULL)
	{
		printf("Error:Cannot create file!");
		return -1;
	}

	printf("Y, U, V value from picture's left to right:\n");
	for(t=0;t<(width/barwidth);t++)
	{
		lum_temp = ymin + (char)(t*lum_inc);
		printf("%3d, 128, 128\n",lum_temp);
	}

	for(j = 0; j < height; j++)
	{
		for(i = 0;i < width; i++)
		{
			t = i/barwidth;
			lum_temp = ymin + (char)(t*lum_inc);
			data_y[j*width+i] = lum_temp;
		}
	}

	for(j = 0;j<uv_height;j++)
	{
		for (i = 0;i<uv_width; i++)
		{
			data_u[j*uv_width+i] = 128;
		}
	}

	for(j = 0;j<uv_height;j++)
	{
		for (i = 0;i<uv_width; i++)
		{
			data_v[j*uv_width+i] = 128;
		}
	}

	fwrite(data_y, width*height, 1, fp);
	fwrite(data_u, uv_width*uv_height, 1, fp);
	fwrite(data_v, uv_width*uv_height, 1, fp);
	fclose(fp);
	free(data_y);
	free(data_u);
	free(data_v);

	return 0;
}









