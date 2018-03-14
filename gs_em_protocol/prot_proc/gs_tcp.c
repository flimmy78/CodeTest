#include <pthread.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include "gs_tcp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

GS_DEBUG_SET_LEVEL(GS_DBG_LVL_DBG); 

typedef struct {
	GS_TCP_INIT_PARAM_S		m_InitParam;

	GS_HANDLE				m_RingBuffHandle;

	GS_BOOL					m_IsTcpConnectOk; /* TCP �Ƿ��������� */

	GS_S32					m_SockFd;

	pthread_t				m_TcpMonitorId;
	pthread_t				m_TcpRecvId;
	pthread_t				m_TcpSendId;
} GS_TCP_HANDLE_S;


/*
	����ָ�������㷨������������
	˵������ connect ��ʱ�����һ��������������һ�����غ��ص�ϵͳ�ϣ��ͺ��п������Ӵ���
			�����������Է����ɶ�ʱ�����Ե���ʱ�����ԡ�
*/
static GS_S32 GS_TCP_ConnectRetry(GS_S32 SockFd, const struct sockaddr *Addr, socklen_t Alen)
{
#define MAXSLEEP 16
	GS_S32 i;

	for (i = 1; i <= MAXSLEEP; i <<= 1) {
		GS_DBGWRN("TCP ReConnect!\n");
		if (connect(SockFd, Addr, Alen) == 0) {
			GS_COMM_Printf("TCP Connect SUCCESS!\n");
			return GS_SUCCESS;
		}

		if (i <= MAXSLEEP / 2) {
			sleep(i);
		}
	}

	return GS_FAILURE;
}

#define GS_TCP_MONITOR_INTERVAL (5) /* ��λ s */
static GS_VOID *GS_TCP_MonitorThread(GS_VOID *pParam)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)pParam;

	while (1) {
		pthread_testcancel();
		if (plHandle->m_IsTcpConnectOk) {
			sleep(GS_TCP_MONITOR_INTERVAL); /* �������������������£�ÿ��һ��ʱ�����һ������״̬���� */
		}
		else { /* ���ӳ������⣬һֱ���г������� */
			struct sockaddr_in lServAddr;

			if (plHandle->m_SockFd > 0) {
				shutdown(plHandle->m_SockFd, SHUT_RDWR);
				close(plHandle->m_SockFd);
			}
			
			plHandle->m_SockFd = socket(AF_INET, SOCK_STREAM, 0); 
			if (plHandle->m_SockFd < 0) {
				GS_COMM_ErrSys("socket error!");
			}
			/* ���� socket Ϊ������״̬������ connect ������ ping ͨ����ַ����ʱ��̫�� */
			if (fcntl(plHandle->m_SockFd, F_SETFL, fcntl(plHandle->m_SockFd, F_GETFL, 0) | O_NONBLOCK) == -1) {
				GS_COMM_ErrSys("fcntl error!");
			}

			bzero(&lServAddr, sizeof(lServAddr));
			lServAddr.sin_family = AF_INET;
			lServAddr.sin_port = htons(plHandle->m_InitParam.m_ServAddr.m_Port);
			if (GS_EM_IP_PROT_HOSTNAME_TYPE_IP == plHandle->m_InitParam.m_ServAddr.m_Type) { /* ��ǰֻ֧�� IP �ķ��������� */
				GS_CHAR plTmp[INET_ADDRSTRLEN];
				GS_U32 lIpAddr = plHandle->m_InitParam.m_ServAddr.m_HostName.m_IpAddr;
				
				snprintf (plTmp, INET_ADDRSTRLEN, "%d.%d.%d.%d", (lIpAddr >> 24) & 0xFF, (lIpAddr >> 16) & 0xFF, (lIpAddr >> 8) & 0xFF, lIpAddr & 0xFF);
				GS_DBGINFO("Reconnect Server: %s - %d\n", plTmp, plHandle->m_InitParam.m_ServAddr.m_Port);
				inet_pton(AF_INET, plTmp, &lServAddr.sin_addr);
			}
			else if (GS_EM_IP_PROT_HOSTNAME_TYPE_DOMAIN == plHandle->m_InitParam.m_ServAddr.m_Type) { 
				/* ʹ�����������ӷ�ʽ����Ҫ���� DNS ��������Ҫ�� DNS ������֧�� */
			}
				
			if (GS_TCP_ConnectRetry(plHandle->m_SockFd, (struct sockaddr *)&lServAddr, sizeof(lServAddr)) == 0) { /* ���ӳɹ� */
				plHandle->m_IsTcpConnectOk = GS_TRUE;
			}
			else {
				sleep(GS_TCP_MONITOR_INTERVAL); /* ѭ���������ӷ����� */
			}
		}
	}

	return GS_NULL;
}

/* �����Ч������������ɷ��� TRUE����Ҫ������䷵�� FALSE */
static GS_BOOL GS_TCP_FullPacket(GS_U16	DstLen, GS_U16 *pCurLen, GS_U8 *pDst, GS_U8 *pSrc, GS_S32 SrcSize)
{
	GS_U16 lRemLen = DstLen - *pCurLen; /* ʣ����Ҫ�������ݳ��� */

	memcpy(pDst, pSrc, GS_MIN(lRemLen, SrcSize));
	*pCurLen += GS_MIN(lRemLen, SrcSize);
	if (DstLen == *pCurLen) {
		return GS_TRUE;
	}

	return GS_FALSE;
}

static GS_S32 GS_TCP_TimeOutRead(GS_S32 Fd, GS_U8 *pBuf, GS_U32 BufSize, GS_U32 TimeOutMS)
{
	fd_set lFds;
	GS_S32 lRet;
	struct timeval lTv;
	
	FD_ZERO(&lFds);
	FD_SET(Fd, &lFds);
	lTv.tv_sec = TimeOutMS / 1000;
	lTv.tv_usec = TimeOutMS % 1000;

	while (1) {
		lRet = select(Fd + 1, &lFds, NULL, NULL, &lTv);
		if (lRet == -1) {
			GS_COMM_ErrRet("select error");
			return -1;
		}
		else if(lRet == 0) { /* TimeOut */
			return TCP_READ_TIMEOUT;
		}
		else {
			if (FD_ISSET(Fd, &lFds)) {
				return read(Fd, pBuf, BufSize);
			}
		}
	}

	return -1;
}

static GS_S32 GS_TCP_BlockSend(GS_S32 Fd, GS_U8 *pBuf, GS_U32 BufSize)
{
	fd_set lFds;
	GS_S32 lRet;

	FD_ZERO(&lFds);
	FD_SET(Fd, &lFds);

	while (1) {
		lRet = select(Fd + 1, NULL, &lFds, NULL, NULL);
		if (lRet == -1) {
			GS_COMM_ErrRet("select error");
			return -1;
		}
		else {
			if (FD_ISSET(Fd, &lFds)) {
				return send(Fd, pBuf, BufSize, 0);
			}
		}
	}

	return -1;
}

static GS_VOID GS_TCP_ValidPacketProc(GS_TCP_HANDLE_S *pHandle, GS_U8 *pData, GS_S32 DataLen)
{
	GS_S32 lCrc32, lReadCrc32;
	GS_EM_IP_PROT_S *pIpProt;
	GS_S32 lLen;
	GS_EM_IP_PROT_RESPONSE_S lResponseInfo;
	GS_U32 lSessionId;
	GS_MSGBUF_S lMsgQBuf;
	GS_S32 lRet;
	GS_HANDLE lBufHandle;
	GS_U8 *plBuf;

	if ((plBuf = (GS_U8 *)malloc(GS_EM_IP_PROT_MAX_LEN)) == GS_NULL) 
		GS_COMM_ErrSys("malloc error");

	if ((pIpProt = (GS_EM_IP_PROT_S *)malloc(sizeof(GS_EM_IP_PROT_S))) == GS_NULL) 
		GS_COMM_ErrSys("malloc error");

	/* �ж� CRC32 */
	lCrc32 = GS_COMM_Crc32Calc(pData, DataLen - 4);
	GS_MSB32_D(&pData[DataLen - 4], lReadCrc32);
	if (lCrc32 != lReadCrc32) {
		GS_DBGERR("CRC32 Check Failed!\n");

		GS_MSB32_D(&pData[4], lSessionId); /* sessionId */
		/* ����У��ʧ�ܵ���Ϣ�� send �߳� */
		GS_EM_IP_PROT_ResponseInfoMake(&lResponseInfo, pIpProt, IP_PROT_ERR_CRC); 
		GS_EM_IP_PROT_ResponseMsgPack(plBuf, &lLen, &lResponseInfo, lSessionId, IP_PROT_CMD_BUTT);

		lMsgQBuf.m_MsgType = TCP_SEND_MSG_TYPE;
		lMsgQBuf.m_MsgText.m_MsgTag = TCP_SEND_MSG_TAG_RESPONSE;
		lMsgQBuf.m_MsgText.m_MsgParam1 = 0;
		lMsgQBuf.m_MsgText.m_MsgParam2 = 0;
		lMsgQBuf.m_MsgText.m_MsgDataLen = lLen;
		lMsgQBuf.m_MsgText.m_pMsgData = plBuf;
		GS_COMM_MsgQueueSend(pHandle->m_InitParam.m_TcpSndMsgQ, &lMsgQBuf, GS_FALSE); /* ������������Ϣ */  
		free(pIpProt);

		return;
	}

	if ((lRet = GS_EM_IP_PROT_Parse(pData, DataLen, pIpProt, &lBufHandle)) == GS_SUCCESS) {
		GS_DBGINFO("IP Protocol Parse Success!\n");
		if (pIpProt->m_Header.m_DataType == GS_EM_IP_PROT_DATA_TYPE_REQUEST) { /* ֻ��������Ϣ�ᷢ����Ӧ��Ϣ */
			/* ������Ӧ��Ϣ�������� */
			GS_EM_IP_PROT_ResponseInfoMake(&lResponseInfo, pIpProt, IP_PROT_SUCCESS); 
			GS_EM_IP_PROT_ResponseMsgPack(plBuf, &lLen, &lResponseInfo, pIpProt->m_Header.m_SessionId, pIpProt->m_Content.m_CmdType);

			lMsgQBuf.m_MsgType = TCP_SEND_MSG_TYPE;
			lMsgQBuf.m_MsgText.m_MsgTag = TCP_SEND_MSG_TAG_RESPONSE;
			lMsgQBuf.m_MsgText.m_MsgParam1 = 0;
			lMsgQBuf.m_MsgText.m_MsgParam2 = 0;
			lMsgQBuf.m_MsgText.m_MsgDataLen = lLen;
			lMsgQBuf.m_MsgText.m_pMsgData = plBuf;
			GS_COMM_MsgQueueSend(pHandle->m_InitParam.m_TcpSndMsgQ, &lMsgQBuf, GS_FALSE); /* ������������Ϣ */  

			/* ת��Ϊ�豸��Ϣ�����͸��豸 */
			lMsgQBuf.m_MsgType  = TCP_RECV_MSG_TYPE_DEV_DATA;
			lMsgQBuf.m_MsgText.m_MsgTag = 0;
			lMsgQBuf.m_MsgText.m_MsgParam1 = (GS_S32)lBufHandle; /* �ò����������ٽ���ʱʹ�õĹ��� Buffer */
			lMsgQBuf.m_MsgText.m_MsgParam2 = 0;
			lMsgQBuf.m_MsgText.m_MsgDataLen = sizeof(GS_EM_IP_PROT_S);
			lMsgQBuf.m_MsgText.m_pMsgData = (GS_U8 *)pIpProt;
			GS_COMM_MsgQueueSend(pHandle->m_InitParam.m_TcpRecvMsgQ, &lMsgQBuf, GS_FALSE); /* ������������Ϣ */  
		}
		else {
			/* �豸�˽��յ�����Ӧ��Ϣֻ������������Ӧ��֪ͨ�����߳���Ӧ���յ� */
			lMsgQBuf.m_MsgType  = TCP_RECV_MSG_TYPE_HEARTBEAT_RESPONSE;
			lMsgQBuf.m_MsgText.m_MsgTag = 0;
			lMsgQBuf.m_MsgText.m_MsgParam1 = (GS_S32)lBufHandle; /* �ò����������ٽ���ʱʹ�õĹ��� Buffer */
			lMsgQBuf.m_MsgText.m_MsgParam2 = 0;
			lMsgQBuf.m_MsgText.m_MsgDataLen = sizeof(GS_EM_IP_PROT_S);
			lMsgQBuf.m_MsgText.m_pMsgData = (GS_U8 *)pIpProt;
			GS_COMM_MsgQueueSend(pHandle->m_InitParam.m_TcpRecvMsgQ, &lMsgQBuf, GS_FALSE); /* ������������Ϣ */ 
			free(plBuf);
		}
	}
	else {
		GS_DBGWRN("IP Protocol Parse Failed!\n");

		GS_MSB32_D(&pData[4], lSessionId); /* sessionId */
		/* ������Ӧ��Ϣ�������� */
		GS_EM_IP_PROT_ResponseInfoMake(&lResponseInfo, pIpProt, IP_PROT_ERR_DATA_PARSE); 
		GS_EM_IP_PROT_ResponseMsgPack(plBuf, &lLen, &lResponseInfo, lSessionId, pIpProt->m_Content.m_CmdType);

		lMsgQBuf.m_MsgType = TCP_SEND_MSG_TYPE;
		lMsgQBuf.m_MsgText.m_MsgTag = TCP_SEND_MSG_TAG_RESPONSE;
		lMsgQBuf.m_MsgText.m_MsgParam1 = 0;
		lMsgQBuf.m_MsgText.m_MsgParam2 = 0;
		lMsgQBuf.m_MsgText.m_MsgDataLen = lLen;
		lMsgQBuf.m_MsgText.m_pMsgData = plBuf;
		GS_COMM_MsgQueueSend(pHandle->m_InitParam.m_TcpSndMsgQ, &lMsgQBuf, GS_FALSE); /* ������������Ϣ */  
		free(pIpProt);
		GS_EM_IP_PROT_ParseClean(lBufHandle);
	}
}

static GS_VOID *GS_TCP_RecvThread(GS_VOID *pParam)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)pParam;
	GS_U8 plBuf[GS_EM_IP_PROT_MAX_LEN] = {0};
	GS_S32 lReadLen = 0;
	GS_U16 lValidPackLen = 0, lCount;
	GS_BOOL	lIsReadContent = GS_FALSE, lIsReadHeaderTag1 = GS_TRUE, lIsReadHeaderTag2 = GS_FALSE, lIsReadHeaderRemData = GS_FALSE;
	GS_U8 plValidData[GS_EM_IP_PROT_MAX_LEN] = {0};
	GS_U8 lCh;

	while (1) {
		pthread_testcancel();
		if (plHandle->m_IsTcpConnectOk) {
			if ((lReadLen = GS_TCP_TimeOutRead(plHandle->m_SockFd, plBuf, sizeof(plBuf), 2000)) > 0) {
				if (GS_ISDEBUG())
					GS_COMM_PrintDataBlock("Client Recv Data", plBuf, lReadLen);

				GS_COMM_RingBufferWrite(plHandle->m_RingBuffHandle, plBuf, lReadLen);

				while (GS_COMM_RingBufferRead(plHandle->m_RingBuffHandle, &lCh, 1)) {
					if (lIsReadContent) { 
						if (GS_TCP_FullPacket(lValidPackLen, &lCount, &plValidData[lCount], &lCh, 1)) {
							GS_TCP_ValidPacketProc(plHandle, plValidData, lValidPackLen);
							lIsReadContent = GS_FALSE;
							lIsReadHeaderTag1 = GS_TRUE;
							lIsReadHeaderTag2 = GS_FALSE;
							lIsReadHeaderRemData = GS_FALSE;
						}
					}
					else {
						if (lIsReadHeaderRemData) {
							GS_TCP_FullPacket(0xffff, &lCount, &plValidData[lCount], &lCh, 1);
							if (lCount == 12) { /* ͷ����Ϊ 12 */
								GS_MSB16_D(&plValidData[lCount - 2], lValidPackLen);
								lIsReadContent = GS_TRUE;
							}
						}
						else if (lIsReadHeaderTag2) {
							if (lCh == (GS_EM_IP_PROT_TAG & 0xFF)) {
								GS_TCP_FullPacket(0xffff, &lCount, &plValidData[lCount], &lCh, 1);
								lIsReadHeaderRemData = GS_TRUE;
							}
							else {
								lIsReadHeaderTag2 = GS_FALSE;
							} 
						}
						else if (lIsReadHeaderTag1) {
							if (lCh == ((GS_EM_IP_PROT_TAG >> 8) & 0xFF)) {
								lCount = 0;
								GS_TCP_FullPacket(0xffff, &lCount, &plValidData[lCount], &lCh, 1);
								lIsReadHeaderTag2 = GS_TRUE;
							}
						}
					}
				}
			}
			else { /* ���Ͷ˹رջ��������� */
				if (lReadLen == TCP_READ_TIMEOUT) {
					continue; /* ��ʱ���ǳ�ʱ���Ĵ��� */
				}

				if (lReadLen < 0)
					GS_COMM_ErrRet("recv error");
				else
					GS_DBGERR("recv return 0!!!\n");
				plHandle->m_IsTcpConnectOk = GS_FALSE;
			}
		}
		else {
			lIsReadContent = GS_FALSE;
			lIsReadHeaderTag1 = GS_TRUE;
			lIsReadHeaderTag2 = GS_FALSE;
			lIsReadHeaderRemData = GS_FALSE;
			sleep(GS_TCP_MONITOR_INTERVAL);
		}
	}

	return GS_NULL;
}

static GS_VOID *GS_TCP_SendThread(GS_VOID *pParam)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)pParam;

	while (1) {
		pthread_testcancel();
		if (plHandle->m_IsTcpConnectOk) {
			GS_MSGBUF_S lMsgBuf;

			if (GS_COMM_MsgQueueRecv(plHandle->m_InitParam.m_TcpSndMsgQ, &lMsgBuf, 0, GS_TRUE) == GS_SUCCESS) {
				if (GS_ISDEBUG())
					GS_COMM_PrintDataBlock("Client Send Data", lMsgBuf.m_MsgText.m_pMsgData, lMsgBuf.m_MsgText.m_MsgDataLen);
				if (GS_TCP_BlockSend(plHandle->m_SockFd, lMsgBuf.m_MsgText.m_pMsgData, lMsgBuf.m_MsgText.m_MsgDataLen) < 0) {
					GS_COMM_ErrRet("send error");
					plHandle->m_IsTcpConnectOk = GS_FALSE;
				}
				
				free(lMsgBuf.m_MsgText.m_pMsgData);
				lMsgBuf.m_MsgText.m_pMsgData = GS_NULL;
			}
		}
		else {
			sleep(GS_TCP_MONITOR_INTERVAL);
		}
	}

	return GS_NULL;
}

GS_BOOL GS_TCP_GetConnStat(GS_HANDLE Handle)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)Handle;

	return plHandle->m_IsTcpConnectOk;
}

GS_VOID GS_TCP_SetConnStat(GS_HANDLE Handle, GS_BOOL IsConnOk)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)Handle;

	plHandle->m_IsTcpConnectOk = IsConnOk;
}

GS_HANDLE GS_TCP_Create(GS_TCP_INIT_PARAM_S *pInitParam)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)malloc(sizeof(GS_TCP_HANDLE_S));
	GS_S32 lErrNo;

	if (!plHandle) {
		GS_COMM_ErrSys("malloc error!");
	}

	bzero(plHandle, sizeof(GS_TCP_HANDLE_S));
	memcpy(&plHandle->m_InitParam, pInitParam, sizeof(GS_TCP_INIT_PARAM_S));
	plHandle->m_IsTcpConnectOk = GS_FALSE;

	if ((plHandle->m_RingBuffHandle = GS_COMM_RingBufferCreate(GS_EM_IP_PROT_MAX_LEN * 2)) == GS_NULL) 
		GS_COMM_ErrQuit("GS_COMM_RingBufferCreate error!\n");

	if ((lErrNo = pthread_create(&plHandle->m_TcpMonitorId, NULL, GS_TCP_MonitorThread, plHandle)) != 0)
		GS_COMM_ErrExit(lErrNo, "pthread_create error!");

	if ((lErrNo = pthread_create(&plHandle->m_TcpRecvId, NULL, GS_TCP_RecvThread, plHandle)) != 0)
		GS_COMM_ErrExit(lErrNo, "pthread_create error!");

	if ((lErrNo = pthread_create(&plHandle->m_TcpSendId, NULL, GS_TCP_SendThread, plHandle)) != 0)
		GS_COMM_ErrExit(lErrNo, "pthread_create error!");

	return plHandle;
}

GS_VOID GS_TCP_Destroy(GS_HANDLE Handle)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)Handle;
	GS_S32 lErrNo;
	GS_VOID *plTRet; /* �̷߳���ֵ */

	if (plHandle) {
		/* �����߳� */
		if ((lErrNo = pthread_cancel(plHandle->m_TcpMonitorId)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_cancel error!");
		if ((lErrNo = pthread_cancel(plHandle->m_TcpRecvId)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_cancel error!");
		if ((lErrNo = pthread_cancel(plHandle->m_TcpSendId)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_cancel error!");

		/* �ȴ��߳̽��� */
		if ((lErrNo = pthread_join(plHandle->m_TcpMonitorId, &plTRet)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_join error!");
		if ((lErrNo = pthread_join(plHandle->m_TcpRecvId, &plTRet)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_join error!");
		if ((lErrNo = pthread_join(plHandle->m_TcpSendId, &plTRet)) != 0)
			GS_COMM_ErrExit(lErrNo, "pthread_join error!");

		if (plHandle->m_SockFd > 0)
			close(plHandle->m_SockFd);

		GS_COMM_RingBufferDestroy(plHandle->m_RingBuffHandle);

		free(plHandle);
	}
}

GS_VOID GS_TCP_ReconfigServerAddr(GS_HANDLE Handle, GS_EM_IP_PROT_NET_ADDR_S *pServerAddr)
{
	GS_TCP_HANDLE_S *plHandle = (GS_TCP_HANDLE_S *)Handle;

	memcpy(&plHandle->m_InitParam.m_ServAddr, pServerAddr, sizeof(GS_EM_IP_PROT_NET_ADDR_S));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
