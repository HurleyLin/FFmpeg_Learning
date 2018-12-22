//============================================================================
// Name        : PCM_Data_Processing.cpp
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
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

/*
 * @Description: 分离PCM16LE双声道音频采样数据的左声道和右声道
 * @param: 	url:pcm文件
 * @return: int
 * @Author: Hurley	2018-12-22
 */
int simplest_pcm16le_split(char *url)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_l.pcm","wb+");
	FILE *fp2 = fopen("output_r.pcm","wb+");

	unsigned char *sample = (unsigned char*)malloc(4);

	while(!feof(fp))
	{
		fread(sample, 1, 4, fp);

		fwrite(sample, 1, 2, fp1);		//L

		fwrite(sample+2, 1, 2, fp2);	//R
	}

	free(sample);
	fclose(fp1);
	fclose(fp2);
	fclose(fp);

	return 0;
}

/*
 * @Description: 将PCM16LE双声道音频采样数据中左声道的音量降一半
 * @param: 	url:pcm文件
 * @return: int
 * @Author: Hurley	2018-12-22
 */
int sample_pcm16le_halfvolumeleft(char *url)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("potput_halfleft.pcm","wb+");

	int cnt = 0;

	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		short *samplenum = NULL;
		fread(sample, 1, 4, fp);

		samplenum = (short *)sample;
		*samplenum = *samplenum/2;

		fwrite(sample, 1, 2, fp1);
		fwrite(sample+2,1,2,fp1);

		cnt++;
	}

	printf("Sample Cnt:%d\n",cnt);

	free(sample);
	fclose(fp);
	fclose(fp1);

	return 0;
}

/*
 * @Description: 将PCM16LE双声道音频采样数据的声音速度提高一倍
 * @param: 	url:pcm文件
 * @return: int
 * @Author: Hurley	2018-12-22
 */
int sample_pcm16le_doublespeed(char *url)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp1 = fopen("output_doublespeed.pcm","wb+");

	int cnt = 0;

	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		fread(sample, 1, 4, fp);

		if (cnt%2 == 0)
		{
			fwrite(sample, 1, 2, fp1);

			fwrite(sample+2, 1, 2, fp1)
		}
	}

	printf("Sample Cnt:%d\n",cnt);

	free(sample);
	fclose(fp);
	fclose(fp1);

	return 0;
}

/*
 * @Description: 将PCM16LE双声道音频采样数据转换为PCM8音频采样数据
 * @param: 	url:pcm文件
 * @return: int
 * @Author: Hurley	2018-12-22
 */
int sample_cpm16le_to_pcm8(char *url)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp1 = fopen("output_8.pcm","wb+");

	int cnt = 0;

	unsigned char *sample = (unsigned char*)malloc(4);

	while(!feof(fp))
	{
		short *samplenum16 = NULL;
		char samplenum8 = 0;
		unsigned char samplenum8_u = 0;
		fread(sample, 1, 4, fp);

		samplenum16 = (short *)sample;		//(-32768-32767)
		samplenum8 = (*samplenum16)>>8;

		samplenum8_u = samplenum8 + 128;	//(0-255)

		fwrite(&samplenum8_u, 1, 1, fp1);

		samplenum16 = (short *)(sample+2);
		samplenum8 = (*samplenum16)>>8;

		samplenum8_u = samplenum8 + 128;

		fwrite(&samplenum8_u, 1, 1, fp1);
		cnt++;
	}

	printf("Sample Cnt:%d\n",cnt);

	free(sample);
	fclose(fp);
	fclose(fp1);

	return 0;
}


