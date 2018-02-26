#ifndef  __SPI_FLASH__H
#define __SPI_FLASH__H

#include "multi_main_internal.h"

#include "../../../../drivers/flash_driver/flash_driver.h"

#define SPI_FLASH_DEV_NAME "/dev/spi_flash_driver"

#define N25Q256_ID 0x20ba19

//Refer to datasheet for N25Q256
#define FLASH_CMD_RDSR				    0x05			//��״̬�Ĵ��� 
#define FLASH_CMD_WRSR				    0x01			//д״̬�Ĵ��� 
#define FLASH_CMD_RD					0x03			//������ 
#define FLASH_CMD_RD_NOVOLATILE_REG		0xB5			//�����׻ӷ����üĴ���
#define FLASH_CMD_PP					0x02			//ҳ����
#define FLASH_CMD_SE					0xd8			//Sector ����������������
#define FLASH_CMD_WREN				    0x06			//дʹ��
#define FLASH_CMD_WRDIS				    0x04			//д��ֹ
#define FLASH_CMD_CE					0xc7			//BULK ������ȫ��������
#define FLASH_CMD_RDID				    0x9f			//��ID, changed from 9f to 9e
#define FLASH_CMD_WREAR			        0xc5			//32M�ĸ߲��ֺ͵Ͳ���ѡ��
#define FLASH_CMD_RD_VOLATILE_REG		0x85			//����ʧ�Ĵ���
#define BANK_LOW_16M_BYTES				0x00			//д����չ��ַ�Ĵ���ֵ����16M
#define BANK_HIGH_16M_BYTES				0x01			//д����չ��ַ�Ĵ���ֵ����16M
#define FLASH_CMD_sector_count          512
#define ADDRESS_16M						0x1000000

#define FLASH_CMD_page_num_per_sector   256
#define FLASH_CMD_page_size             256
#define APP_READ_FLASH_BUF_SIZE			(2048)
#define APP_WRITE_FLASH_BUF_SIZE		512				//��Ҫ��ҳ��С(256)��������

/* FLASH Status Register Mask */
#define STS_WIP		0x01	// Status Write In Progress
#define STS_WEL		0x02	// Status Write Enable Latched
#define STS_SRWP	0x80	// Status Reg. Write Protected
#define TIMEOUT		(0x7FFFFFFF)					// for Polling time out value  changed from 260 to 3*10^12
#define PAGE_PROGRAM_TIMEOUT		(0x7FFFFFFF)	// changed from 100 to 3*10^12

int SpiFlahInit(void);
int CheckFlashId(void);
int FlashWrite(U8 *DataBuf, U32 Address, U32 Length);
int FlashRead(U8 *DataBuf, U32 Address, U32 Length);
int ChipErase(void);
void CmdExecution(long command);
int ChooseSpi(int number);

#define WRITE_ENABLE_NO			0
#define WRITE_ENABLE_YES		1

#define FLASH_CURRENT_BANK_0				   0
#define FLASH_CURRENT_BANK_1				   1
#define FLASH_CURRENT_BANK_UNDEFINED   2

#define CS_CTL_CLR			0
#define CS_CTL_SET			1

#define N25Q256_FLASH_SIZE  0x2000000 //32M bytes
#define N25Q256_FLASH_PAGE_SIZE (256) //256 bytes

//Flash Driver
#define SPI_FLASH_DEV_MINOR 255
#define SPI_FLASH_DRIVER_NAME "spi_flash_driver"

//Choose Spi 
#define SPI_chip_1 0x340
#define SPI_chip_2 0x341
#define SPI_chip_3 0x342
#define SPI_chip_4 0x343


//���Ժ���
#ifdef DEBUG_SPI_FLASH_INTERFACE
void Test_FSPIF(void);
#endif

#endif
