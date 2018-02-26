#ifndef GN_HWL_PSI_H
#define GN_HWL_PSI_H

#include "gn_global.h"
#include "gn_hwl_dxt8243.h"

#define MFPGA_MAX_PSI_NUM 32 

enum
{
	HWL_VID_ENC_H264 = 0,
	HWL_VID_ENC_MPEG2,
	HWL_VID_ENC_AVS,
	HWL_VID_ENC_AVS_PLUS /* AVS+ */
};

enum
{
	HWL_AUD_ENC_MPEG1_L2 = 0,
	HWL_AUD_ENC_MPEG2_AAC,
	HWL_AUD_ENC_MPEG4_AAC,
	HWL_AUD_ENC_DRA_2_0,
	HWL_AUD_ENC_DRA_5_1,
	HWL_AUD_ENC_AC3,
	HWL_AUD_ENC_EAC3
};

typedef struct  
{
	U32	m_TsId;
	U8		m_Version;
	U8		m_SyncByteReplaceChar; /* ͬ���ֽ�ָ����ָ��ͬ���ֽڵ�ԭ�����ڵ���SPTSģʽʱ��FPGAͨ��ͬ���ֽ�ȷ����ų���IP�˿� */

	struct {
		U32	m_ProgramNum; /* ��Ŀ�� */
		U32	m_PmtPid; /* PMT��PID */
	}m_ProgramInfo[GN_ENC_CH_NUM];
	U32	m_ProgramLen; /* ��Ŀ��Ŀ */
}PSI_PatInfo;

typedef struct  
{
	U32	m_TsId;
	U32	m_OnId; /* original_network_id */
	U8		m_Version;
	U8		m_SyncByteReplaceChar; /* ͬ���ֽ�ָ����ָ��ͬ���ֽڵ�ԭ�����ڵ���SPTSģʽʱ��FPGAͨ��ͬ���ֽ�ȷ����ų���IP�˿� */

	S32		m_Charset;
	BOOL		m_CharsetMark;

	struct {
		U32	m_ProgramNum; /* ��Ŀ�� */
		CHAR_T	m_pProgramName[MPEG2_DB_MAX_SERVICE_NAME_BUF_LEN]; /* ��Ŀ�� */
		U32	m_PmtPid; /* PMT��PID */
	}m_ProgramInfo[GN_ENC_CH_NUM];
	U32	m_ProgramLen; /* ��Ŀ��Ŀ */
}PSI_SdtInfo;

typedef struct
{
	U8		m_SyncByteReplaceChar; /* ͬ���ֽ�ָ����ָ��ͬ���ֽڵ�ԭ�����ڵ���SPTSģʽʱ��FPGAͨ��ͬ���ֽ�ȷ����ų���IP�˿� */
	U8		m_Version;

	U32	m_ProgramNum; /* ��Ŀ�� */
	CHAR_T	m_pProgramName[MPEG2_DB_MAX_SERVICE_NAME_BUF_LEN]; /* ��Ŀ�� */
	U32	m_PmtPid; /* PMT��PID */

	U32	m_PcrPid;

	U32	m_VidPid;
	S32	m_VidEncMode; /* ��Ƶ����ģʽ */

	U32	m_AudPid;
	S32	m_AudEncMode; /* ��Ƶ����ģʽ */
}PSI_PmtInfo;

typedef struct 
{
	PSI_PatInfo		m_PatInfo;
	PSI_PmtInfo		m_PmtInfo;
	PSI_SdtInfo		m_SdtInfo;
}PSI_TableCreatePara;

typedef struct  
{
	U8		m_pPsiPacket[MFPGA_MAX_PSI_NUM][MPEG2_TS_PACKET_SIZE];
	U32	m_PsiPacketCounter; /* PSI������ */	

	U32	m_SdtPacketNum; /* д��SDT���ĸ��� */
	U32	m_SecondSdtPacketPosition; /* �ڶ���SDT�����ܰ����е�λ�� */
}PSI_PacketInfo;

typedef struct 
{
	U16	m_TsId;			/* transport_stream_id */
	U16	m_OnId;			/* original_network_id */

	U32	m_Charset;
}PSI_TsParam;

typedef struct 
{
	BOOL	m_WorkEn;			/* ����ģʽ */

	U32	m_VidPid;
	U32	m_AudPid;
	U32	m_PcrPid;
	U32	m_PmtPid;

	S32	m_VidEncMode;			/* ��Ƶ����ģʽ */
	S32	m_AudEncMode;		/* ��Ƶ����ģʽ */

	CHAR_T m_pServiceName[MPEG2_DB_MAX_SERVICE_NAME_BUF_LEN]; /* ��Ŀ�� */
	U32	m_ServiceId; /* ��Ŀ�� */
}PSI_ChProgParam;

typedef struct
{
	PSI_TsParam m_TsParam; /* TS������ */
	PSI_ChProgParam m_ProgParam[GN_ENC_CH_NUM]; /* ��Ŀ���� */
}PSI_CreateParam;

BOOL PSI_SetEncPsiParamToHw(PSI_CreateParam *pPsiParam);

#endif /* GN_HWL_H */
/* EOF */
