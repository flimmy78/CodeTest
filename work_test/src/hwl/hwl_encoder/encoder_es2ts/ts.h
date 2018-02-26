/******************************************************************************

                  ��Ȩ���� (C), 2005-2017, GOSPELL���޹�˾

 ******************************************************************************
  �� �� ��   : ts.h
  �� �� ��   : ����
  ��    ��   : ���
  ��������   : 2017��7��14��
  ����޸�   :
  ��������   : ts.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :

   1.��    ��          : 2017��7��14��
      ��    ��          : ���
      �޸�����   : �����ļ�

******************************************************************************/
#ifndef __TS_H__
#define __TS_H__

#include <stdio.h>
#include <stdlib.h>		/* calloc realloc abs div*/
#include <string.h>		/* memcpy memcmp */
#include <stdint.h>		/* uint8_t ... */
#include <stdbool.h>	/* bool true false */
#include <ctype.h>		/* toupper isxdigit isspace */
#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>		/* time_t clock_t tm */

#include "ts_psi.h"
#include "mpi_sys.h"
#include "os_assist.h"

#define MPEG_TS_DEBUG_ENABLE
#define MSG_ERR			0 /*Used to report error conditions */
#define MSG_DEBUG	1 /*Used for debugging messages */
#define MSG_INFO	2 /*Used for information messages */


#ifdef MPEG_TS_DEBUG_ENABLE
	extern int currentDebugLevel;

	#define DBG(level,...) \
	do \
	{ \
		if (level <= currentDebugLevel) \
		{ \
			printf(__VA_ARGS__); \
		}\
	} while (0)
	
#else
	#define DBG(level,fmt,...)	;
#endif

#define TS_PACKET_SIZE 							188
#define PAT_RETRANS_TIME 						40

#define PES_START_CODE_PREFIX     		0X000001
#define PES_AUDIO_STREAM_ID     			0XC0
#define PES_VIDEO_STREAM_ID    	 			0XE0
#define PES_AC3_STREAM_ID     			0XBD

#define PES_INCLUDE_ONLY_PTS            	2
#define PES_INCLUDE_PTS_DTS              	3
#define PES_NOT_INCLUDE_PTS_DTS      	0

/* table ids */
#define PAT_TID   			0x00
#define PMT_TID   			176
#define SDT_TID   			0x42

#define PROGRAM_MAP_PID   49

#define TS_AUDIO_PID  			144
#define TS_VIDEO_PID  				128
#define TS_PCR_PID  160

#define  DEFAULT_PTS_DELAY_TIME            (500000) //500ms
#define  PTS_PCR_MAX_INTERVAL_1s			(50000) //50ms
#define  PTS_PCR_MIN_INTERVAL_1s				(-50000)//-50ms

#define  PTS_PCR_MAX_OVERTURN 			   (60000000) //9000ms
#define  PTS_PCR_MIN_OVERTURN					(-60000000)//-9000ms

#define PSI_TX_INTERVAL_NUM    10
#define AVERAGE_CALC_NUMBER  5

#ifndef LONGLONG
#define LONGLONG long long
#endif

typedef int int32_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;

/* PES Header (Fixed Length  = 9 bytes)*/
typedef struct TS_PES
{
    uint32_t  packet_start_code_prefix;         /* Bit 24, ��ʼ����Ϊ0X00 00 01 */
    uint16_t  stream_id;                                 /*Bit 8, ԭʼ�������ͺ���Ŀ��ȡֵ��1011 1100��1111 1111֮��*/
    uint16_t  PES_packet_length;                   /*Bit 16, ��ʾ�Ӵ��ֽ�֮��PES��������λ�ֽڣ���*/
	                                                               /* 0��ʾPES���������ƣ���ֻ��������ƵPES*/
    uint32_t  reserved;								          /* Bit 2 fixed '10' */
																																		
    uint32_t  PES_scrambling_control;            /* Bit 2, PES��Ч���صļ���ģʽ��*/
                                                                  /* 00��ʾ�����ܣ������ʾ�û��Զ���*/
																																		 
	uint32_t  PES_priority;                              /* Bit 1, PES���ݰ������ȼ���������TS�Ĵ��ֶΡ�*/
																																		 
	uint32_t  data_alignment_indicator;           /* Bit 1,  Ϊ1ʱ�������˷���ͷ��֮��*/
	                                                               /* ������ �������������ж���ķ��ʵ�Ԫ���͡�*/
																																	 
	uint32_t  copyright;                                  /*  Bit 1, ��Ȩ��1��ʾ�а�Ȩ��*/
	                                                              /* �������Ȩ������13818-1 1-2-6-24��0��ʾû�С�*/
																																	 
	uint32_t  original_or_copy;                        /* Bit 1, ��ʾԭʼ���ݣ�0��ʾ����*/
																																	 
	uint32_t  PTS_DTS_flag;								  /*  Bit 2, 10 ��ʾ����PTS�ֶΣ�*/
	                                                              /* 11 ��ʾ����PTS��DTS�ֶΣ�*/
	                                                              /* 00 ��ʾ������PTS��DTS��01�޶��塣*/
	uint32_t  ESCR_flag;                             /* Bit 1, 1 ��ʾESCR��PES�ײ����֣�0��ʾ������*/
																																	 
	uint32_t  ES_rate_flag;                          /* Bit 1, 1 ��ʾPES���麬��ES_rate�ֶΡ�0��ʾ�����С�*/

	uint32_t DSM_trick_mode_flag;              /* Bit 1, 1 ��ʾ��8λ��trick_mode_flag�ֶΣ�*/
	                                                              /* 0  ��ʾ�����ִ��ֶΡ�ֻ��DSM��Ч���ڹ㲥�в���*/

	uint32_t additional_copy_info_flag;        /* Bit 1, 1 ��ʾ��copy_info_flag�ֶΣ�0 ��ʾ�����ִ��ֶΡ�*/

   uint32_t PES_CRC_flag;                         /* Bit 1, 1 ��ʾPES��������CRC�ֶΣ�0 ��ʾ�����ִ��ֶΡ�*/

   uint32_t PES_extension_flag;                 /* Bit 1, 1 ��ʾ��չ�ֶ���PES��ͷ���ڣ�0 ��ʾ��չ�ֶβ�����*/

	uint8_t PES_header_data_length;             /* Bit 8, ��ʾ��ѡ�ֶκ�����ֶ���ռ���ֽ���*/
	
}TS_PES_T;


/* TS Header (Fixed length 4 bytes)  */
typedef struct TS_TS_HEADER
{
    uint32_t    sync_byte;     								/*  Bit 8, ͬ��ͷ,  ��ֵ�̶�Ϊ0x47*/
	 uint32_t   transport_error_indicator; 			/* Bit 1 */
	 
	 uint32_t   payload_unit_start_indicator; 		/*Bit 1, 1 ��ʾ����PES��PSI��ʼ���͵�һ���ֽ�*/
	 
	 uint32_t   transport_priority;   						/*Bit 1*/
	 uint32_t   PID;   												/* Bit 13 */
	 uint32_t   transport_scrambling_control; 	/* Bit 2*/
	 
	 uint32_t   adaptation_field_control; 			/* Bit 2, ָʾ��TS�ײ��Ƿ�����е����ֶ�*/
	                                                               /* ����Ч����*/
																			   /* '10'  ���������ֶΣ�����Ч����*/
																				/* '11' �����ֶκ�Ϊ��Ч����*/
	 
	 uint32_t   continuity_counter; 						/* Bit 4, ����ÿһ��������ͬPID��TS�� �����ӣ�*/
																			   /* �ﵽ���ֵ�ֻظ���0  */
																				/* ���adaptation_field_control Ϊ'00'��'10' ��continuity_counter������*/
}TS_TS_HEADER_T;

/* PCR TS  options */
typedef struct TS_ONLY_PCR_IN_TS_OPTIONS
{
    uint32_t               pcr_pid;
	 uint32_t               cc;
	 uint32_t               discontinuity;
}TS_ONLY_PCR_IN_TS_OPTIONS_T;

/* TS Packets options */
typedef struct TS_OPTIONS
{
	 TS_TS_HEADER_T  ts_header;
	 TS_PES_T             pes_header;
	 void					*m_pUserParam;
	 TS_ONLY_PCR_IN_TS_OPTIONS_T  pcr_ts_opt;
	 void (*tx_ts_to_buffer_call)(uint8_t *data, uint32_t len, void *pUserParam);
	 void (*send_ts_call)(uint8_t *data, uint32_t len, void *pUserParam);
	 
	 LONGLONG     	first_pcr;   /* PCR �ĳ�ʼֵ*/
	 LONGLONG     	first_pts;   /* PTS �ĳ�ʼֵ*/
	 LONGLONG       ts_packet_count; /* TS ������ͳ��*/
	 
	 LONGLONG     	adjust_interval; /* ÿ�����ٸ�PCR�����ͬ��һ��*/
	 LONGLONG			base_time;

	 LONGLONG			prior_pcr_pts_interval; /* ��ǰ��PCR��PTS�ļ��ֵ*/
	 LONGLONG     	adjust_pcr_value; /* PCR�������ۼ�ֵ*/

	 LONGLONG     	adjust_base_time; /* ������base time ����*/
	 LONGLONG			prior_interval_1s;

	 LONGLONG			adjust_begin_time;	
	 LONGLONG 		begin_calc_time;
	 LONGLONG 		previous_system_time; 
     LONGLONG 		delay_value_1s[AVERAGE_CALC_NUMBER];
	 LONGLONG 		pcr_pts_interval[AVERAGE_CALC_NUMBER];
	 
	 uint32_t           write_pcr; /* 0 is not write PCR to video TS, otherwise 1 is write video TS */
	 uint32_t           discontinuity;
	 
	 int32_t          	mux_rate; /* ���õ�TS���͵�����(1kbps -> 1000bps)*/
	 int32_t				real_rate;  /*ʵ�ʵ�TS��������(1kbps -> 1000bps)*/	 
	 int32_t          	pcr_packet_count; /* PCR ������*/
    int32_t          	pcr_packet_period; /* PCR ��������,ÿ�������TS������һ��PCR*/
	 int32_t          	pcr_period; /* PCR���ʱ��,�������ms����һ��PCR*/

	 int32_t            	pat_packet_count; /* PSI���ļ���*/
    int32_t           	pat_packet_period; /* PSI��������,ÿ�����ٸ�TS����һ��PSI */
	 uint32_t				psi_tx_count;

	 int32_t				adjust_num; /*��������*/
	 int32_t				syn_flag; /*ͬ��״̬*/

	 int32_t 				adjust_count;
	 int32_t 				delay_value_count;

	 uint32_t				last_video_cc;
	 int32_t				pts_delay_time; 
	 int32_t				max_pts_pcr_interval;
	 int32_t				min_pts_pcr_interval;
}TS_OPTIONS_T;

uint32_t TS_Crc32Calculate(uint8_t *pData, uint32_t Size);


/*****************************************************************************
* FUNCTION:mpegs_sync_pts()
*
* DESCRIPTION:
*	    ��ʱ���Ļص�����
* INPUT:
*	  TS_OPTIONS_T *glopts
*	  RNGBUF_t * pBuf
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*   ÿ��10ms����һ��
* HISTORY:
*	
*	Review: 
******************************************************************************/
int mpegs_sync_pts(LONGLONG *pUserParam);

/*****************************************************************************
* FUNCTION:mpegts_timer()
*
* DESCRIPTION:
*	    ��ʱ���Ļص�����
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*   ÿ��10ms����һ��
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_timer(TS_OPTIONS_T *glopts,  RNGBUF_t * pVidBuf, RNGBUF_t * pAudBuf, RNGBUF_t * pInsertBuf);

/*****************************************************************************
* FUNCTION:mpegts_get_pcr_packet_period()
*
* DESCRIPTION:
*	    ����PCR����ļ����
* INPUT:
*	 int32_t pcr_period //PCR�������ڣ���λms
*   int32_t mux_rate //TS�������bps
* OUTPUT:
*	 ������ٸ�TS����һ��PCR��
*
* RETURN:
*
* NOTES:

* HISTORY:
*	
*	Review: 
******************************************************************************/
int32_t  mpegts_get_pcr_packet_period(int32_t pcr_period, int32_t mux_rate);

/*****************************************************************************
* FUNCTION:mpegts_get_ts_num()
*
* DESCRIPTION:
*	    ��������������TS��������Ӧ���Ͷ���TS����
* INPUT:
*	  HWL_PSI_OPTION_T *glopts
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:

* HISTORY:
*	
*	Review: 
******************************************************************************/
int32_t  mpegts_get_ts_num(LONGLONG  tx_period, int32_t mux_rate, int32_t *real_mux_rate);

/*****************************************************************************
* FUNCTION:mpegts_insert_null_packet()
*
* DESCRIPTION:
*	    Write a single null transport stream packet
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:

* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_insert_null_packet(uint8_t *out_buf, uint32_t out_len);

/*****************************************************************************
* FUNCTION:mpegts_write_packet()
*
* DESCRIPTION:
*	   ���ȸ�payloadǰ����PES header������PES�ְ��ɶ��TS packets
*   ����TS packet �������Ρ�ÿ��TS packet�ĳ��ȹ̶�188 bytes
*
* INPUT:
*	   uint8_t *payload;  ���������������ƵES��
*                               Video :������һ֡��Ƶ
*                               Audio:����һ֡���֡��Ƶ���롣
*                             
*     int payload_size; ES���ĳ���
*
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_write_packet(TS_OPTIONS_T *glopts,
                                              const uint8_t *payload, 
							 							uint32_t payload_size, 
                         	 						LONGLONG pcr,
							 							LONGLONG pts, 
							 							LONGLONG dts);
							 							
/*****************************************************************************
* FUNCTION:mpegts_write_psi()
*
* DESCRIPTION:
*	   Send PAT and PMT.
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_write_psi(HWL_PSI_OPTION_T *glopts);

/*****************************************************************************
* FUNCTION:mpegts_tx_pcr_only()
*
* DESCRIPTION:
*	   Only send PCR in ts.
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_tx_pcr_only(TS_OPTIONS_T *glopts, LONGLONG pcr);

/*****************************************************************************
* FUNCTION:mpegts_destory()
*
* DESCRIPTION:
*	   �ͷ�TS
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_destory(TS_OPTIONS_T *glopts);

/*****************************************************************************
* FUNCTION:mpegts_psi_destory()
*
* DESCRIPTION:
*	   �ͷ�PSI ��Դ
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
******************************************************************************/
void mpegts_psi_destory(HWL_PSI_OPTION_T *glopts);

/*****************************************************************************
* FUNCTION:mpegts_psi_init()
*
* DESCRIPTION:
*	    ��ʼ��SPTS PSI 
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
HWL_PSI_OPTION_T * mpegts_psi_init(void);

/*****************************************************************************
* FUNCTION:mpegts_set_ts_rate()
*
* DESCRIPTION:
*	    ����TS��������
* INPUT:
*	    TS_OPTIONS_T *glopts//Ts handle
*		 int32_t   rate //Ts rate
*		 int32_t   pcr_period //PCR period.
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
void mpegts_set_ts_rate(TS_OPTIONS_T *glopts, int32_t   ts_rate, int32_t   pcr_period);

/*****************************************************************************
* FUNCTION:mpegts_set_pts_dts_flag
*
* DESCRIPTION:
*	    ����PES header��pts/dts flag
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
void mpegts_set_pts_dts_flag(TS_OPTIONS_T *glopts, uint32_t  pts_dts_flag);

/*****************************************************************************
* FUNCTION:mpegts_set_pid()
*
* DESCRIPTION:
*	    ���ø�TS����PID
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
void mpegts_set_pid(TS_OPTIONS_T *glopts, uint32_t   pid);

/*****************************************************************************
* FUNCTION:mpegts_set_stream_id
*
* DESCRIPTION:
*	    ����PES header��stream id
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
void mpegts_set_stream_id(TS_OPTIONS_T *glopts, uint16_t   stream_id);

/*****************************************************************************
* FUNCTION:mpegts_set_sync_parm()
*
* DESCRIPTION:
*	  ����PTSͬ������
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
void mpegts_set_sync_parm(TS_OPTIONS_T *glopts, int32_t   delay_time, int32_t   max_interval, int32_t   min_interval);

/*****************************************************************************
* FUNCTION:mpegts_init()
*
* DESCRIPTION:
*	    ��ʼ��TS
* INPUT:
*	
* OUTPUT:
*	None.
*
* RETURN:
*
* NOTES:
*
* HISTORY:
*	
*	Review: 
****************************************************************************/
TS_OPTIONS_T * mpegts_init(void);

#define VIDEO_SIZFEOF  8000*7*188

#endif /* __TS_H__ */

/* EOF */

