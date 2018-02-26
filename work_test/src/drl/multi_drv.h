#ifndef HWL_DEVICE_H
#define HWL_DEVICE_H

#include "global_def.h"
#include "platform_assist.h"

/*PSģʽ����FPGA�߼�------------------------------------------------------------------------------------------------------------------------*/
int DRL_FpgaConfiguration(const char *dev_path, const char *fpga_path);
BOOL DRL_ZYNQInitiate(void);
/*
BitValue = 0x01 �� ��PB17��PB16 = b01��Ϊ3C55�İ��ӣ��ɣ�����R461��R560�� 
BitValue = 0x00 �� ��PB17��PB16 = b00��Ϊ4C55�İ��ӣ��£�����R561��R560�� 
*/
#define GM2730X_4CE55_VALUE 0x00
#define GM2730X_3C55_VALUE 0x01

U32 DRL_GetMainBoardVersionBitValue(void);
BOOL DRL_GetMainBoardIS4CE55(void);
/*��������------------------------------------------------------------------------------------------------------------------------*/
int DRL_ICPInitate(void);
void DRL_FpgaDestroy(void);

/*������ģʽ------------------------------------------------------------------------------------------------------------------------*/

/** �ú���������д��һ������������
 * �ڲ��Զ���ȡ���ͷ���.,��Զ��seek==0 ��ʼ����д�����.
 * ��������@(buf)��׼����ɺ󣬱������ụ��Ľ���д��FPGA.
 */
int DRL_FpgaWriteLock(unsigned char *buf , unsigned int size );

/**
 * ��FPGAģ���ж������ݵ�����������������СӦ��Ϊ .����>=1024
 * @(buff):�������ݻ�����.
 * @(size):��������С.
 * @(return): ���@(return)<4 ��˵������������Ч.
 */
int DRL_FpgaReadLock(unsigned char *buff, unsigned int size);



/*��ַģʽ------------------------------------------------------------------------------------------------------------------------*/


/*���ȿ���------------------------------------------------------------------------------------------------------------------------*/
#define HWL_CONST_ON		1
#define HWL_CONST_OFF		0
int DRL_FanInit(void);
int DRL_FanStatusSet(int val);
int DRL_FanStatusGet(void);
void DRL_FanDestroy(void);

int DRL_FanHwGet(void);



BOOL DRL_FpgaConfig(U32 FpgaIndex);

/*GM-8358Q�Ľӿ�*/
BOOL Init_FPGA( void );
unsigned short READ_FPGA(unsigned char B_TYPE, unsigned long ADDRESS);
void WRITE_FPGA(unsigned char B_TYPE, unsigned long ADDRESS, int DATA);

#ifdef GN1846
S32 DRL_FspiWrite(U16 RegAddr, U8 *pBuf, U32 BufSize);
S32 DRL_FspiRead(U16 RegAddr, U8 *pBuf, U32 BufSize);
BOOL DRL_GetFspiComIsOk(void);
#endif

#endif

