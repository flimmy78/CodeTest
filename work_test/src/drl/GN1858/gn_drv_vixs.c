#include "gn_drv.h"
#include "main_fpga_op.h"

static void DRL_SelectUartToVixs(U32 VixsIndex)
{
	//GLOBAL_TRACE(("Index: %d\n", VixsIndex));
	MFPGA_SetUartSelect(VixsIndex);
}

/* �����VIXS�����ӿ� */
BOOL DRL_VixsReset(void *pUserParam)
{
	S32 lVixsIndex = (S32) pUserParam; 

	return MFPGA_SetEncBoardReset(lVixsIndex);
}

S32 DRL_VixsUartOpen(void *pUserParam)
{
	S32 lVixsIndex = (S32) pUserParam; 

	return DRL_UartOpen(lVixsIndex / 2);/* �����Ӱ���һ������ */
}

S32 DRL_VixsUartClose(void *pUserParam, S32 Fd)
{
	return DRL_UartClose(Fd);
}

S32 DRL_VixsUartRead(void *pUserParam, S32 Fd, U8 *pData)
{
	S32 lVixsIndex = (S32)pUserParam;

	/* 1. �л����� */
	DRL_SelectUartToVixs(lVixsIndex);

	/* 2. ������ */
	return DRL_UartRead(Fd, pData); 
}

S32 DRL_VixsUartWrite(void *pUserParam, S32 Fd, U8 *pData, S32 DataSize)
{
	S32 lVixsIndex = (S32)pUserParam;

	/* 1. �л����� */
	DRL_SelectUartToVixs(lVixsIndex);

	/* 2. д���� */
	return DRL_UartWrite(Fd, pData, DataSize); 
}

