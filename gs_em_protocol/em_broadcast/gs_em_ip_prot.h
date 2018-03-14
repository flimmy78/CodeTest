#ifndef GS_EM_PROTOCOL_H
#define GS_EM_PROTOCOL_H

#include "gs_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define	GS_EM_IP_PROT_MAX_LEN (65535) /* Ӧ�����ݰ����� */ 
#define GS_EM_IP_PROT_TAG (0xFEFD)
#define GS_EM_IP_PROT_VER (0x0100) /* ����0x0100 ��ʾ V01.00 */
#define GS_EM_IP_PROT_DATA_TYPE_REQUEST (1)
#define GS_EM_IP_PROT_DATA_TYPE_RESPONSE (2)

typedef enum {
	IP_PROT_CMD_START_BC = 0x01, /* ��ʼ���� */
	IP_PROT_CMD_STOP_BC = 0x02,

	IP_PROT_CMD_DEV_HEARTBEAT = 0x10,
	IP_PROT_CMD_STAT_QUERY = 0x11,
	IP_PROT_CMD_DEV_PARAM_SET = 0x12,
	IP_PROT_CMD_FAULT_RECOVERY = 0x13,
	IP_PROT_CMD_TASK_SWITCH = 0x14,
	IP_PROT_CMD_BC_REPONSE = 0x15, /* �ն��ϱ�������� */

	IP_PROT_CMD_BC_RECORD_QUERY = 0x20,
	IP_PROT_CMD_AUT = 0x30, /* �����֤ */
	IP_PROT_CMD_AUD_TRANS_START = 0x40, /* ��Ƶ����������뿪ʼ */ 
	IP_PROT_CMD_AUD_TRANS_END = 0x41,

	IP_PROT_CMD_BUTT
} GS_EM_IP_PROT_CMD_TYPE_E;

typedef enum {
	IP_PROT_BC_EM_DRILL_PUBLISH_SYS = 0x01, /* Ӧ������-����ϵͳ���� */
	IP_PROT_BC_EM_DRILL_SIMU, /* Ӧ������-ģ������ */
	IP_PROT_BC_EM_DRILL_ACTUAl, /* Ӧ������-ʵ������ */
	IP_PROT_BC_EM, /* Ӧ���㲥 */
	IP_PROT_BC_NORM, /* ��̬�㲥 */
	IP_PROT_BC_BUTT
} GS_EM_IP_PROT_BC_TYPE_E; /* �㲥���� */

typedef enum {
	IP_PROT_EM_LVL_I = 0x01,
	IP_PROT_EM_LVL_II,
	IP_PROT_EM_LVL_III,
	IP_PROT_EM_LVL_IV,
	IP_PROT_EM_LVL_BUTT
} GS_EM_IP_PROT_EM_LVL_E; /* Ӧ���¼����� */

typedef enum {
	IP_PROT_DEV_WORK_STAT_FREE = 0x01,
	IP_PROT_DEV_WORK_STAT_RUNNING,
	IP_PROT_DEV_WORK_STAT_FAULT,
	IP_PROT_DEV_WORK_STAT_BUTT
} GS_EM_IP_PROT_DEV_WORK_STAT_E;

typedef enum {
	IP_PROT_DEV_PARAM_TAG_VOL = 0x01,
	IP_PROT_DEV_PARAM_TAG_IPV4,
	IP_PROT_DEV_PARAM_TAG_RTP_POSTBACK_ADDR,
	IP_PROT_DEV_PARAM_TAG_DEV_RES_CODE,
	IP_PROT_DEV_PARAM_TAG_SERVER_TCP_ADDR, /* ������ TCP ��ַ */
	IP_PROT_DEV_PARAM_TAG_BUTT
} GS_EM_IP_PROT_DEV_PARAM_TAG_E;

typedef enum {
	GS_EM_IP_PROT_AUD_TRANS_PROT_RTSP = 0x01,
	GS_EM_IP_PROT_AUD_TRANS_PROT_RTMP,
	GS_EM_IP_PROT_AUD_TRANS_PROT_RTP, 
	GS_EM_IP_PROT_AUD_TRANS_PROT_BUTT
} GS_EM_IP_PROT_AUD_TRANS_PROT_TYPE_E; /* ��Ƶ����Э������ */

typedef enum {
	GS_EM_IP_PROT_AUD_ENC_FMT_MP3 = 0x01,
	GS_EM_IP_PROT_AUD_ENC_FMT_MPEG1_L2, 
	GS_EM_IP_PROT_AUD_ENC_FMT_AAC,
	GS_EM_IP_PROT_AUD_ENC_FMT_BUTT
} GS_EM_IP_PROT_AUD_ENC_FMT_E; /* ��Ƶ�������� */

typedef enum {
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_TEXT = 0x01,
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_AUD, 
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_PIC,
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_VID,
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_BUTT
} GS_EM_IP_PROT_ASSIST_DATA_TYPE_E; /* ������������ */

typedef enum {
	GS_EM_IP_PROT_HOSTNAME_TYPE_IP = 0x01,
	GS_EM_IP_PROT_HOSTNAME_TYPE_DOMAIN, 
	GS_EM_IP_PROT_HOSTNAME_TYPE_BUTT
} GS_EM_IP_PROT_HOSTNAME_TYPE_E; 

typedef enum {
	IP_PROT_SUCCESS = 0,

	IP_PROT_ERR_UNRECOG_PROT = 10, /* δ֪ͨ��Э�����ʹ��� */
	IP_PROT_ERR_TIMEOUT, /* ����ʱ���Է���Ӧ�� */
	IP_PROT_ERR_VER_MISMATCH, /* Э��汾��ƥ�� */
	IP_PROT_ERR_DATA_PARSE, /* ���ݰ��������� */
	IP_PROT_ERR_LESS_PARAM, /* ȱ�ٱ�ѡ���� */
	IP_PROT_ERR_CRC, /* CRC У����� */

	IP_PROT_ERR_UNRECOG_SYS_TYPE = 30, /* δ֪ϵͳ������ */
	IP_PROT_ERR_SYS_BUSY, /* ϵͳæ */
	IP_PROT_ERR_NO_MEM_CARD, /* �޴洢�� */
	IP_PROT_ERR_READ_FILE, /* ���ļ�ʧ�� */
	IP_PROT_ERR_WRITE_FILE, /* д�ļ�ʧ�� */
	IP_PROT_ERR_UKEY_NO_INSERT, /* UKey δ���� */
	IP_PROT_ERR_UKEY_ILLEGAL, /* UKey �Ƿ� */

	IP_PROT_ERR_UNRECOG_DATA_VALID = 50, /* δ֪������֤��� */
	IP_PROT_ERR_USERNAME_PWD, /* �û������������ */
	IP_PROT_ERR_CERT_ILLEGAL, /* ����֤��Ƿ� */
	IP_PROT_ERR_INPUT_TIMEOUT, /* ���볬ʱ */
	IP_PROT_ERR_PRAM_ILLEGAL, /* �����Ƿ� */
	IP_PROT_ERR_FUNC_NOSUPPORT, /* ���ܲ�֧�� */
	IP_PROT_ERR_SMG_ILLEGAL, /* ���Ÿ�ʽ�Ƿ� */
	IP_PROT_ERR_NUMBER_INVALID, /* ������Ч */
	IP_PROT_ERR_CONTENT_ILLEGAL, /* ���ݷǷ� */
	IP_PROT_ERR_REG_CODE_INVALID, /* ��Դ������Ч */
	
	IP_PROT_ERR_NORECOG_DEV_TYPE = 70, /* δ֪�ն����� */
	IP_PROT_ERR_DEV_INVALID, /* �ն���Ч */
	IP_PROT_ERR_DEV_LINKDOWN, /* �豸���� */
	IP_PROT_DEV_BUSY, /* �ն�æ */

	IP_PROT_ERR_BUTT
} GS_EM_IP_PORT_ERR_CODE_E;

typedef struct {
	GS_U8	m_ResTypeCode; /* ��Դ������ */  
	GS_U8	m_ResSubTypeCode; /* ��Դ�������� */  
	GS_U8	m_pZoneCode[6]; /* �������� */
	GS_U8	m_ExCode; /* ��չ�� */
} GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S;

typedef struct {
	GS_U32	m_IpAddr;
	GS_U32	m_IpMask;
	GS_U32	m_IpGate;
} GS_EM_IP_PROT_IPV4_S;

typedef struct {
	GS_EM_IP_PROT_HOSTNAME_TYPE_E	m_Type; /* 0x01: IP + Port; 0x02: ���� + �˿� */
	union {
		GS_U32	m_IpAddr;
		GS_CHAR	m_pDomain[64]; /* ������󳤶�����Ϊ63���ַ� */
	} m_HostName; /* �ش� IP ��ַ������ */
	GS_U16	m_Port;
} GS_EM_IP_PROT_NET_ADDR_S;

typedef struct {
	GS_EM_IP_PROT_AUD_TRANS_PROT_TYPE_E	m_AudTransProt;
	GS_EM_IP_PROT_AUD_ENC_FMT_E			m_AudEncFmt;
	GS_EM_IP_PROT_NET_ADDR_S		m_Addr;
} GS_EM_IP_PROT_ASSIST_AUD_DATA_S;
	
typedef struct {
	GS_EM_IP_PROT_NET_ADDR_S		m_Addr;
} GS_EM_IP_PROT_ASSIST_PIC_DATA_S;
	
typedef struct {
	GS_EM_IP_PROT_NET_ADDR_S		m_Addr;
} GS_EM_IP_PROT_ASSIST_VID_DATA_S;

typedef struct {
	GS_EM_IP_PROT_ASSIST_DATA_TYPE_E	m_Type;
	GS_U16								m_Len;
	union {
		GS_CHAR							*m_pText;
		GS_EM_IP_PROT_ASSIST_AUD_DATA_S m_AudInfo;
		GS_EM_IP_PROT_ASSIST_PIC_DATA_S m_PicInfo;
		GS_EM_IP_PROT_ASSIST_VID_DATA_S m_VidInfo;
	} m_Data;
} GS_EM_IP_PROT_ASSIST_DATA_S;

typedef struct {
	GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S	m_EmPlatformId;
	GS_U16								m_Year;
	GS_U8								m_Month;
	GS_U8								m_Day;
	GS_U16								m_SeqCode; /* ˳���� */
} GS_EM_IP_PROT_EM_MSG_ID_S;

typedef struct {
	GS_EM_IP_PROT_DEV_PARAM_TAG_E		m_Tag;
	GS_U8								m_ParamLen;
	union {
		GS_U8							m_Vol;
		GS_EM_IP_PROT_IPV4_S			m_LocalIp;
		GS_EM_IP_PROT_NET_ADDR_S		m_PostBackAddrInfo; /* �ش���ַ��Ϣ */
		GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S	m_DevResCode; /* �豸��Դ���� */
		GS_EM_IP_PROT_NET_ADDR_S		m_ServerTcpAddr; /* ������ TCP ��ַ */
	} m_Param;
} GS_EM_IP_PROT_DEV_PARAM_S;

typedef struct {
	GS_EM_IP_PROT_EM_MSG_ID_S				m_EmMsgId;

	GS_EM_IP_PROT_BC_TYPE_E					m_BCType; /* �㲥���� */
	GS_EM_IP_PROT_EM_LVL_E					m_EmEventLvl; /* Ӧ���¼��ȼ� */
	GS_U8									m_pEmEventType[5]; /* Ӧ���¼����� */
	GS_U8									m_Vol; /* ������0x00 ������0xff �������������䣻0x01 ~ 0x64 ��Ӧ���� 1% �� 100% */
	GS_U32									m_StartTime;
	GS_U32									m_DurationTime; 
	GS_U8									m_AssistDataNum; /* 1 - 4 */
	GS_EM_IP_PROT_ASSIST_DATA_S				m_pAssistData[4];
} GS_EM_IP_PROT_START_BC_S;

typedef struct {
	GS_EM_IP_PROT_EM_MSG_ID_S	m_EmMsgId;
} GS_EM_IP_PROT_STOP_BC_S;

typedef struct {
	GS_EM_IP_PROT_DEV_WORK_STAT_E	m_DevWorkStat;
	GS_U8							m_FirstRegister; /* �Ƿ��һ��ע�ᣬ1 �״�ע�ᣬ2 ���״�ע�� */
	GS_U8							m_PhyAddrCodeLen; 
	GS_U8							*m_pPhyAddrCode; /* �ն�Ψһ��ʶ������ʱ���ɣ��̶����� */
} GS_EM_IP_PROT_DEV_HEARTBEAT_S;

typedef struct {
	GS_U8							m_ParamSetNum;
	GS_EM_IP_PROT_DEV_PARAM_S		*m_pParamData;
} GS_EM_IP_PROT_DEV_PARAM_SET_S;

typedef struct {
	GS_U32									m_CertID; /* ��֤ ID */
	GS_U16									m_SpotsObjNum; /* ��Ҫ�岥Ŀ������� */
	GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S		*m_pSpotsObj;
	GS_EM_IP_PROT_BC_TYPE_E					m_BCType; /* ֻ��Ӧ���ͳ�̬����ѡ�� */
	GS_EM_IP_PROT_AUD_TRANS_PROT_TYPE_E		m_AudTransProt;
	GS_EM_IP_PROT_AUD_ENC_FMT_E				m_AudEncFmt;
	GS_U16									m_BCAddrLen;
	GS_EM_IP_PROT_NET_ADDR_S			m_BCAddr;
} GS_EM_IP_PROT_AUD_TRANS_START_S;

typedef struct {
	GS_U32									m_CertID; /* ��֤ ID */
	GS_U16									m_SpotsObjNum; /* ��Ҫ�����岥Ŀ������� */
	GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S		*m_pSpotsObj;
} GS_EM_IP_PROT_AUD_TRANS_END_S;

typedef struct {
	GS_EM_IP_PORT_ERR_CODE_E			m_RetCode; /* ������� */
	GS_U16								m_DescLen; /* �������ݳ��� */
	GS_CHAR								*m_pDesc; /* �������� */
} GS_EM_IP_PROT_RESPONSE_S;

typedef struct {
	GS_U16	m_Tag;
	GS_U16	m_Ver;
	GS_U32	m_SessionId;
	GS_U8	m_DataType;
	GS_U8	m_SignTag; /* 0:��ǩ�� */
	GS_U16	m_DataLen; /* ����У�����ݺ����ݰ�ͷ */
} GS_EM_IP_PROT_HEADER_S; 

typedef struct {
	GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S	m_DataSrcCompBCDCode; /* ����Դѹ�� BCD ���� */

	GS_U16								m_DataDstNum; /* ����Ŀ����� */
	GS_EM_IP_PROT_RES_PACKED_BCD_CODE_S	*m_pDataDstCompBCDCode; /* ���ݽ��ն˵���Դ���� */

	GS_EM_IP_PROT_CMD_TYPE_E			m_CmdType; /* ҵ���������� */
	GS_U16								m_CmdLen; /* ҵ�����ݳ��� */
	union {
		GS_EM_IP_PROT_START_BC_S		m_StartBCInfo; /* ��ʼ���� */
		GS_EM_IP_PROT_STOP_BC_S			m_StopBCInfo; /* ֹͣ���� */
		GS_EM_IP_PROT_DEV_HEARTBEAT_S		m_HeartBeatInfo; /* �ն����� */
		GS_EM_IP_PROT_DEV_PARAM_SET_S		m_ParamSetInfo; /* �ն˲������� */
		GS_EM_IP_PROT_AUD_TRANS_START_S		m_AudTransStartInfo; /* ��Ƶ����������뿪ʼ��Ϣ */
		GS_EM_IP_PROT_AUD_TRANS_END_S		m_AudTransEndInfo; /* ��Ƶ����������������Ϣ */

		GS_EM_IP_PROT_RESPONSE_S			m_ResponseInfo; /* �ն�Ӧ�� */
	} m_CmdContent;
} GS_EM_IP_PROT_CONTENT_S; 

typedef struct {
	GS_U32	m_Crc32;
} GS_EM_IP_PROT_CHECKDATA_S; 

typedef struct {
	GS_EM_IP_PROT_HEADER_S		m_Header;
	GS_EM_IP_PROT_CONTENT_S		m_Content;
	GS_EM_IP_PROT_CHECKDATA_S	m_CheckData;
} GS_EM_IP_PROT_S; /* Ӧ���㲥 IP ���ݰ���ʽ */

/* Global Functions Declare */
GS_S32	GS_EM_IP_PROT_Construct(GS_U8 *pData, GS_S32 *pLen, GS_EM_IP_PROT_S *pIpProt); /* ���� IP Э����ṹ���ɴ������� */
GS_S32 GS_EM_IP_PROT_ResponseMsgPack(GS_U8 *pData, GS_S32 *pLen, GS_EM_IP_PROT_RESPONSE_S *pResponseInfo, GS_U32 SessionId, GS_U32 CmdType); /* Ӧ����Ϣ��� */
GS_S32 GS_EM_IP_PROT_CmdInfoPack(GS_U8 *pData, GS_S32 *pLen, GS_VOID *pCmdInfo, GS_U8 CmdType, GS_U32 SessionId); /* ҵ�����ݷ�� */
GS_S32	GS_EM_IP_PROT_Parse(GS_U8 *pData, GS_S32 Len, GS_EM_IP_PROT_S *pIpProt, GS_HANDLE *pBufHandle); /* �������ݣ����������ṹ; pBufHandle ����ʹ�õ��ڴ���� */
GS_VOID GS_EM_IP_PROT_ParseClean(GS_HANDLE BufHandle); /* �������İ��ṹʹ����ɺ���Ҫ�����ڴ����� */
GS_VOID GS_EM_IP_PROT_Print(GS_EM_IP_PROT_S *pIpProt);
GS_VOID GS_EM_IP_PROT_ResponseInfoMake(GS_EM_IP_PROT_RESPONSE_S *pResponseInfo, GS_EM_IP_PROT_S *pIpProt, GS_U8 RetCode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* GS_EM_PROTOCOL_H */
/* EOF */
