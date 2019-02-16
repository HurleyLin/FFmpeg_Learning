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

#if 0
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

			fwrite(sample+2, 1, 2, fp1);
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

/*
 * @Description: 将PCM16LE双声道音频采样数据转换为WAVE格式音频数据
 * @param: 	pcmpath:pcm文件
 * 			wavepath:wave文件
 * 			sample_rate:采样频率
 * 			channels：通道数
 * @return: int
 * @Author: Hurley	2018-12-22
 */
int sample_pcm16le_to_wave(const char *pcmpath, int channels, int sample_rate, const char *wavepath)
{
	typedef struct WAVE_HEADER{
		char fccID[4];
		unsigned long dwSize;
		char fccType[4];
	}WAVE_HEADER;

	typedef struct WAVE_FNT{
		char fccID[4];
		unsigned long 	dwSize;
		unsigned short 	wFormatTag;
		unsigned short 	wChannels;
		unsigned long 	dwSamplePerSec;
		unsigned long 	dwAvgBytesPerSec;
		unsigned short 	wBlockAlign;
		unsigned short 	uiBitsPerSample;
	}WAVE_FMT;

	typedef struct WAVE_DATA{
		char fccID[4];
		unsigned long dwSize;
	}WAVE_DATA;

	if (channels == 0 || sample_rate == 0)
	{
		channels = 2;
		sample_rate = 44100;
	}

	int bits = 16;

	WAVE_HEADER pcmHEADER;
	WAVE_FMT 	pcmFMT;
	WAVE_DATA 	pcmDATA;

	unsigned short m_pcmData;
	FILE *fp,*fpout;

	fp = fopen(pcmpath, "rb+");
	if (fp == NULL)
	{
		printf("open pcm file error\n");
		return -1;
	}

	fpout = fopen(wavepath, "wb+");
	if (fpout == NULL)
	{
		printf("open wav file error\n");
		return -1;
	}

	memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF"));
	memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));
	fseek(fpout,sizeof(WAVE_HEADER),1);

	pcmFMT.dwSamplePerSec = sample_rate;
	pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplePerSec*sizeof(m_pcmData);
	pcmFMT.uiBitsPerSample = bits;

	memcpy(pcmFMT.fccID,"fmt",strlen("fmt"));
	pcmFMT.dwSize = 16;
	pcmFMT.wBlockAlign = 2;
	pcmFMT.wChannels = channels;
	pcmFMT.wFormatTag = 1;

	fwrite(&pcmFMT,sizeof(WAVE_FMT),1,fpout);

	memcpy(pcmDATA.fccID,"data",strlen("data"));
	pcmDATA.dwSize = 0;
	fseek(fpout,sizeof(WAVE_DATA),SEEK_CUR);

	fread(&m_pcmData,sizeof(unsigned short),1,fp);
	while(!feof(fp))
	{
		pcmDATA.dwSize += 2;
		fwrite(&m_pcmData,sizeof(unsigned short),1,fpout);
		fread(&m_pcmData,sizeof(unsigned short),1,fp);
	}

	pcmHEADER.dwSize = 44 + pcmDATA.dwSize;

	rewind(fpout);
	fwrite(&pcmHEADER,sizeof(WAVE_HEADER),1,fpout);
	fseek(fpout,sizeof(WAVE_FMT),SEEK_CUR);
	fwrite(&pcmDATA,sizeof(WAVE_DATA),1,fpout);

	fclose(fp);
	fclose(fpout);

	return 0;
}
#endif

int samplest_pcm161e_split(char *url)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_l.pcm","wb+");
	FILE *fp2 = fopen("output_r.pcm","wb+");

	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		fread(sample, 1, 4, fp);
		fwrite(sample, 1, 2, fp1);
		fwrite(sample+2, 1, 2, fp2);
	}

	free(sample);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	return 0;
}

int simplest_pcm161e_halfvolumeleft(char *url)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp1 = fopen("output_halfleft.pcm","wb+");

	int cnt=0;

	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		short *samplenum = NULL;
		fread(sample, 1, 4, fp);

		samplenum = (short *)sample;
		*samplenum = *samplenum/2;

		fwrite(sample, 1, 2, fp1);
		fwrite(sample+2, 1, 2, fp1);

		cnt++;
	}

	printf("Sample Cnt:%d\n",cnt);
	free(sample);
	fclose(fp);
	fclose(fp1);
	return 0;
}

int simplest_pcm161e_doublespeed(char *url)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_doublespeed.pcm","wb+");

	int cnt = 0;
	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		fread(sample, 1, 4, fp);

		if (cnt%2 != 0)
		{
			fwrite(sample, 1, 2, fp1);
			fwrite(sample+2, 1, 2, fp1);
		}

		cnt++;
	}

	printf("Sample Cnt:%d",cnt);

	free(sample);
	fclose(fp);
	fclose(fp1);

	return 0;
}

int simplest_pcm161e_to_pcm8(char *url)
{
	FILE *fp = fopen(url, "rb+");
	FILE *fp1 = fopen("output_8.pcm","wb+");

	int cnt = 0;

	unsigned char *sample = (unsigned char *)malloc(4);

	while(!feof(fp))
	{
		short *samplenum16 = NULL;
		char samplenum8 = 0;
		unsigned char samplenum8_u = 0;
		fread(sample, 1, 4, fp);

		samplenum16 = (short *)sample;
		samplenum8 = (*samplenum16)>>8;

		samplenum8_u = samplenum8 + 128;

		fwrite(&samplenum8_u, 1, 1, fp1);

		samplenum16 = (short *)(sample+2);
		samplenum8 = (*samplenum16)>>8;
		samplenum8_u = samplenum8 + 128;

		fwrite(&samplenum8_u, 1, 1, fp1);

		samplenum16 = (short *)(sample + 2);
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

int samplest_pcm161e_cut_singlechannel(char *url,int start_num, int dur_num)
{
	FILE *fp = fopen(url,"rb+");
	FILE *fp1 = fopen("output_cut.pcm","wb+");
	FILE *fp_stat = fopen("output_cut.txt","wb+");

	unsigned char *sample = (unsigned char *)malloc(2);

	int cnt = 0;
	while(!feof(fp))
	{
		fread(sample, 1, 2, fp);
		if (cnt>start_num&&cnt<=(start_num+dur_num))
		{
			fwrite(sample, 1, 2, fp1);
			short samplenum = sample[1];
			samplenum = samplenum*256;
			samplenum = samplenum+sample[0];

			fprintf(fp_stat,"%6d",samplenum);
			if (cnt%10 == 0)
				fprintf(fp_stat,"\n",samplenum);
		}
		cnt++;
	}

	free(sample);
	fclose(fp);
	fclose(fp1);
	fclose(fp_stat);
	return 0;
}

int samplest_pcm161e_to_wave(const char *pcmpath, int channels, int sample_rate,const char *wavepath)
{
	typedef struct WAVE_HEADER
	{
		char fccID[4];
		unsigned long dwSize;
		char fccType[4];
	}WAVE_HEADER;

	typedef struct WAVE_FMT
	{
		char fccID[4];
		unsigned long dwSize;
		unsigned short wFormatTag;
		unsigned short wChannels;
		unsigned long dwSamplesPerSec;
		unsigned long dwAvgBytesPerSec;
		unsigned short wBlockAlign;
		unsigned short uiBitsPerSample;
	}WAVE_FMT;

	typedef struct WAVE_DATA
	{
		char fccID[4];
		unsigned long dwSize;
	}WAVE_DATA;

	if(channels == 0 || sample_rate == 0)
	{
		channels = 2;
		sample_rate = 44100;
	}

	int bits = 16;

	WAVE_HEADER pcmHEADER;
	WAVE_FMT pcmFMT;
	WAVE_DATA pcmDATA;

	unsigned short m_pcmData;
	FILE *fp,*fpout;

	fp = fopen(pcmpath,"rb+");

	if (fp == NULL)
	{
		printf("open pcm file error\n");
		return -1;
	}

	fpout = fopen(wavepath, "wb+");
	if (fpout == NULL)
	{
		printf("create wav file error\n");
		return -1;
	}

	memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF"));
	memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));
	fseek(fpout,sizeof(WAVE_HEADER), 1);

	pcmFMT.dwSamplesPerSec = sample_rate;
	pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec * sizeof(m_pcmData);
	pcmFMT.uiBitsPerSample = bits;
	memcpy(pcmFMT.fccID,"fmt",strlen("fmt"));
	pcmFMT.dwSize = 16;
	pcmFMT.wBlockAlign = 2;
	pcmFMT.wChannels = channels;
	pcmFMT.wFormatTag = 1;

	fwrite(&pcmFMT,sizeof(WAVE_FMT), 1, fpout);

	memcpy(pcmDATA.fccID,"data",strlen("data"));
	pcmDATA.dwSize = 0;
	fseek(fpout, sizeof(WAVE_DATA),SEEK_CUR);

	fread(&m_pcmData,sizeof(unsigned short), 1, fp);
	while(!feof(fp))
	{
		pcmDATA.dwSize += 2;
		fwrite(&m_pcmData,sizeof(unsigned short), 1, fpout);
		fread(&m_pcmData,sizeof(unsigned short), 1, fp);
	}

	pcmHEADER.dwSize = 44 + pcmDATA.dwSize;
	rewind(fpout);
	fwrite(&pcmHEADER, sizeof(WAVE_FMT), 1, fpout);
	fseek(fpout, sizeof(WAVE_FMT),SEEK_CUR);
	fwrite(&pcmDATA, sizeof(WAVE_DATA), 1, fpout);

	fclose(fp);
	fclose(fpout);

	return 0;
}

typedef struct WAVE_HEADER
{
	char fccID[4];
	unsigned long dwSize;
	char fccType[4];
}WAVE_HEADER;

typedef struct WAVE_FMT
{
	char fccID[4];
	unsigned long dwSize;
	unsigned short wFormatFag;
	unsigned short wChannels;
	unsigned long dwSamplesPerSec;
	unsigned long dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short uiBitPerSample;
}WAVE_FMT;

typedef struct WAVE_DATA
{
	char fccID[4];
	unsigned long dwSize;
}WAVE_DATA;












