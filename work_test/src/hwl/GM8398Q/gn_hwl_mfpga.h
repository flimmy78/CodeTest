#ifndef GN_HWL_MFPGA_H
#define GN_HWL_MFPGA_H

#include "gn_global.h"
#include "gn_hwl_psi.h"
#include "gn_hwl_dxt8243.h"

enum
{
	HWL_MFPGA_INPUT_ERROR_NONE = 0, /* �޴��� */
	HWL_MFPGA_INPUT_ERROR_CC, /* �����������󣬱�ʾ�������룬��TS�������� */
	HWL_MFPGA_INPUT_ERROR_VIDEO, /* ��Ƶ���ݴ��󣬱�ʾû����Ƶ�������� */
	HWL_MFPGA_INPUT_ERROR_AUDIO, /* ��Ƶ���ݴ��󣬱�ʾû����Ƶ�������� */
};

enum
{
	HWL_MFPGA_OUTPUT_ERROR_NONE = 0,
	HWL_MFPGA_OUTPUT_ERROR_OVERFLOW, /* ������� */
	HWL_MFPGA_OUTPUT_ERROR_NOBITRATE /* ��������� */
};

typedef struct
{
	U16	m_VideoPid;
	U16	m_AudioPid;
	U16	m_PcrPid;
}MFPGA_PidGroup;

typedef struct
{
	MFPGA_PidGroup	m_OldPid[GN_CH_NUM_PER_ENC_BOARD];
	MFPGA_PidGroup	m_NewPid[GN_CH_NUM_PER_ENC_BOARD];
}MFPGA_PidMapParam;

typedef struct
{
	MFPGA_PidGroup	m_PidGroup[GN_CH_NUM_PER_ENC_BOARD];
}MFPGA_PidFilterParam;

/* PLD˵����˵֧��100������ַ0x4DFF������Ҫ��ַ��A0~A15��
��A15��ַ��û�ӣ�������������ȡһ��������8·��Ŀ������PSI����Ŀ��ֵ���� */
typedef struct  
{
	S32	m_Interval; /* ������ms */
	U8		m_Data[MFPGA_MAX_PSI_NUM][MPEG2_TS_PACKET_SIZE];
	U32	m_PsiPacketNum; /* Ҫ��������� */

	U32	m_SdtPacketNum; /* д��SDT���ĸ��� */
	U32	m_SecondSdtPacketPosition; /* �ڶ���SDT�����ܰ����е�λ�� */
}MFPGA_PsiInsertParam;	/* PSI������� */

typedef struct  
{
	struct
	{
		BOOL		m_WorkEn;

		U32	m_VideoPid;
		U32	m_AudioPid;
		U32	m_PcrPid;
		U32	m_PmtPid;

		S32	m_VideoEncStandard;		/* ��Ƶ����ģʽ */
		S32	m_AudioEncStandard;		/* ��Ƶ����ģʽ */

		CHAR_T m_pServiceName[MPEG2_DB_MAX_SERVICE_NAME_BUF_LEN]; /* ��Ŀ�� */
		U32	m_ServiceId; /* ��Ŀ�� */
	}m_ChParam[GN_ENC_CH_NUM];
	U16	m_TsId;			/* transport_stream_id */
	U16	m_OnId;			/* original_network_id */
	U32	m_Charset;		/* �ַ��� */
}HWL_MfpgaCfgParam;

typedef struct
{
	U32	m_InputErrorFlag[GN_ENC_CH_NUM];
	U32	m_OutputErrorFlag;
}HWL_MfpgaStatusParam;

BOOL MFPGA_Init(void);
BOOL MFPGA_Terminate(void);
BOOL MFPGA_ConfigRbf(void);
BOOL MFPGA_GetRelease(CHAR_T *pRelease);
U16 MFPGA_GetFpgaId(void);
U32 MFPGA_GetSecretCardId(void);
U8 MFPGA_GetInputVideoPacketNum(U32 ChIndex);
U8 MFPGA_GetInputAudioPacketNum(U32 ChIndex);
U8 MFPGA_GetInputCCErrorFlag(void);
BOOL MFPGA_GetBufferOverflowStatus(U32 OutBitrate); /* ��λkbps */
/* �������嵽������ʵʱ��Ч��������, ���ص�λkbps */
U32 MFPGA_GetOutValidBitrate(void);
BOOL MFPGA_SetOutBitrate(U32 OutBitrate); /* ��λkbps */
BOOL MFPGA_SetPidMap(U32 EncBoardIndex, MFPGA_PidMapParam *pPidMapPara);
BOOL MFPGA_SetPidFilter(U32 EncBoardIndex, MFPGA_PidFilterParam *pPidFilterPara);
BOOL MFPGA_SetPsiInsert(MFPGA_PsiInsertParam *pPsiInsertPara);
BOOL MFPGA_SetUartSelect(U32 EncBoardIndex);
BOOL HWL_SetParaToMainFpga(HWL_MfpgaCfgParam *pCfgParam);
BOOL HWL_GetParaFromMainFpga(HWL_MfpgaStatusParam *pStatusParam);

#endif 

