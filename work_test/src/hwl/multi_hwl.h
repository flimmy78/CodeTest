#ifndef _MULTI_HWL_H_
#define _MULTI_HWL_H_

#include "global_def.h"
#include "global_micros.h"
#include "multi_private.h"
#include "multi_hwl_tags.h"
#include "liu_iic.h"
#include "TunerDemod.h"

#define HWL_MAX_CHANNEL_NUM		8
#define NEW_HARDWARE_FOR_LAST_AMP			1//2013.2.21����DS����ĩ���Ŵ�����Ӧ������롣

#if defined(GM8358Q) || defined(GM8398Q)
#define HWL_TUNER_MAX_NUM   1
#else
#define HWL_TUNER_MAX_NUM   4
#endif

#define HWL_ASI_MAX_NUM   8

#define HWL_NEW_HARD		0   //4350


typedef enum
{
	HWL_DEVICE_SIMU_3650DS,
	HWL_DEVICE_SIMU_3650DR,
	HWL_DEVICE_SIMU_3710A,
	HWL_DEVICE_SIMU_3760A,
	HWL_DEVICE_SIMU_2620B,
	HWL_DEVICE_SIMU_2700X,
	HWL_DEVICE_SIMU_2730X,
#ifdef GN1846
	HWL_DEVICE_SIMU_1846,
#endif
	HWL_DEVICE_SIMU_NUM,
}HWL_DEVICE_SIMU_TYPE;


typedef enum
{
	HWL_CHANNEL_DIRECTION_IN,
	HWL_CHANNEL_DIRECTION_OUT,
	HWL_CHANNEL_DIRECTION_LIMIT
}HWL_ChannelDirection;

typedef enum
{
	HWL_CHANNEL_TYPE_INVALID=-1,
	HWL_CHANNEL_TYPE_ASI=0,
	HWL_CHANNEL_TYPE_IP=1,
	HWL_CHANNEL_TYPE_IP_LOOP=5,
	HWL_CHANNEL_TYPE_IP_DEP=8,
	HWL_CHANNEL_TYPE_TUNER_S=2,
	HWL_CHANNEL_TYPE_E3DS3=3,
	HWL_CHANNEL_TYPE_DVB_C_MODULATOR=4,
	HWL_CHANNEL_TYPE_TUNER_C = 6,
	//add by ding
	HWL_CHANNEL_TYPE_DVB_S_MODULATOR=7,
	//add by xj for 3655
	HWL_CHANNEL_TYPE_IP_LOOP_DEP=10,
	HWL_CHANNEL_TYPE_DTMB_MODULATOR,
	HWL_CHANNEL_TYPE_TUNER_DTMB,
	HWL_CHANNEL_TYPE_IP_OVER_TS_OUT_DVBC,//IPoTS DVBC���� ��������������Ƶ��˵
	HWL_CHANNEL_TYPE_IP_OVER_TS_IN_DVBC,//IPoTS DVBC���� ���룬���������Ƶ��˵
	HWL_CHANNEL_TYPE_CLK_TYPE,
#ifdef GM8358Q
	HWL_CHANNEL_TYPE_ENCODER,
#endif
#ifdef GM8398Q
	HWL_CHANNEL_TYPE_ENCODER_CVBS_DXT8243,
#endif
#if defined(GN1846) || defined(GN1866)
	HWL_CHANNEL_TYPE_ENCODER,
#endif
	HWL_CHANNEL_TYPE_LIMIT
}HWL_ChannelType;//ͨ�����ͣ���Ҫ��������ͨ�����õĲ���


typedef enum
{
	HWL_CHANNEL_SUBTYPE_INVALID,
	HWL_CHANNEL_SUBTYPE_IP,
	HWL_CHANNEL_SUBTYPE_ASI,
	HWL_CHANNEL_SUBTYPE_MODULATOR_AD9789,
	HWL_CHANNEL_SUBTYPE_MODULATOR_BL85KMM,
	HWL_CHANNEL_SUBTYPE_E3DS3,
	HWL_CHANNEL_SUBTYPE_TUNER_S_AVL6211,	
	HWL_CHANNEL_SUBTYPE_TUNER_C,
	HWL_CHANNEL_SUBTYPE_TUNER_DTMB_ATBM8869,
	HWL_CHANNEL_TYPE_IP_OVER_TS_OUT_AD9789,
	HWL_CHANNEL_TYPE_IP_OVER_TS_IN_ATBM8869,
#ifdef GM8358Q
	HWL_CHANNEL_SUBTYPE_ENCODER_CVBS,
	HWL_CHANNEL_SUBTYPE_LIMIT
#endif
#ifdef GM8398Q
	HWL_CHANNEL_SUBTYPE_ENCODER_CVBS,
	HWL_CHANNEL_SUBTYPE_LIMIT
#endif
#ifdef GN1846
	HWL_CHANNEL_SUBTYPE_ENCODER_HI3531A,
#endif
#ifdef GN1866
	HWL_CHANNEL_SUBTYPE_ENCODER_HI3519,
#endif
}HWL_ChanneSublType;//ͨ�������ͣ�������������ͨ����ʵ�ַ���



#ifdef SUPPORT_NEW_HWL_MODULE
typedef struct
{
	/*���ͣ�����;0���룬1���*/
	S16	m_Type;
	S16 m_SubType;

	S32	m_SubChnNum;
	S32	m_StartTsIndex;
	S32 m_CurSubSupport;
}HWL_ChannelInfo;

typedef struct
{
	U32					m_ChipID;
	S32					m_InChnNum;
	HWL_ChannelInfo		m_pInChn[HWL_MAX_CHANNEL_NUM];
	S32					m_OutChnNum;
	HWL_ChannelInfo		m_pOutChn[HWL_MAX_CHANNEL_NUM];

	CHAR_T				m_pFPGARelease[64];

	S32					m_DeviceType;

	S32					m_InTsMax;
	S32					m_OutTsMax;
}HWL_HWInfo;

#else
typedef struct
{
	/*���ͣ�����;0���룬1���*/
	S16 m_Direction;
	S16	m_Type;
	S16 m_SubType;
	S16 m_DemodType;

	S32	m_StartTsIndex;
	S32 m_CurSubSupport;
}HWL_ChannelInfo;

typedef struct
{
	U32					m_ChipID;
	S32					m_ChannelNum;
	HWL_ChannelInfo		m_pInfoList[HWL_MAX_CHANNEL_NUM];
	CHAR_T				m_pFPGARelease[64];
	//S32					m_HWInserterNum;

	S32					m_InTsMax;
	S32					m_OutTsMax;

	S32					m_DeviceType;

	S32					m_TunerCount;
	S32					m_TunerType;

	/*
	PC13    PC12     SV�������      HV��Ӳ����     PLLӲ��        
	1       1         1.04            2.0          ADF4350 
	0       0         2.00            3.0          MAX2871
	*/

#ifdef GQ3760B		//PC13  PC12
	U32					m_HwType;
	S32					m_REFCLKHz;//10M/40M
#endif
}HWL_HWInfo;
#endif

typedef struct
{
	/*IP ����*/
	U32 m_IPAddress;
	U16	m_IPPort;

}HWL_TsParameter;

typedef struct
{
	/*IP ����*/
	U32 m_IPAddress;
	U32 m_IPMask;
	U32 m_IPGate;

}HWL_ChannelParam;


/* Functions prototypes ------------------------------------------------------- */

/*FFPG����------------------------------------------------------------------------------------------------------------------------*/
void HWL_FpgaConfig(void);
BOOL HWL_FPGAGetMuxOK(void);

/*���Ȳ���*/
void HWL_FanInit(void);//��HWL_Initiate������
void HWL_FanEnable(BOOL bEnable);
BOOL HWL_FanStatusGet(void);
void HWL_FanDestroy();//��HWL_Terminate������


/*ģ���������------------------------------------------------------------------------------------------------------------------------*/
BOOL HWL_Initiate(S32 DeviceType);
void HWL_Access(S32 Duration);
void HWL_Pool(void);
void HWL_Terminate(void);

/*Ӳ����Ϣ��ȡ(��������ID��)*/
BOOL HWL_GetHWInfo(HWL_HWInfo *pHWInfo);

/*��������������*/
typedef void (*HWL_TsFilterCB)(S32 SlotIndex, U8 *pTsPacket);
void HWL_AddTsPacketsRequest(S32 SlotIndex, S32 InTsIndex, U16 InPID, HWL_TsFilterCB pDataCB, S32 PESCount);
void HWL_RemoveTsPacketsRequest(S32 SlotIndex);




/*���ò���------------------------------------------------------------------------------------------------------------------------*/
/*PID ӳ�����*/
#define HWL_INTERNAL_TS_INDEX 0xFE
void HWL_PerformPIDMap(void);//���õ�FPGA
void HWL_AddPIDMap(S16 InTsIndex, U16 InPID, S16 OutTsIndex, U16 OutPID, BOOL bScramble, S32 CwGroupIndex);
void HWL_ClearPIDMap(void);

/*�������ͨ��������ʽ: ֱͨ����ӳ��*/
#define HWL_CONST_PASS_MODE_THROUGH		1		//ֱͨ
#define HWL_CONST_PASS_MODE_PIDMAP		0
#define MULT_CHANNEL_SBU_TYPE_TUNER_MAX_NUM      (4)  
typedef struct
{
	U8 	passMode;
	U8	inputChannelId;
	U8	outputChanneldId;
}HWL_PassMode_t;
S32 HWL_PassModeSend(HWL_PassMode_t *passMode);

/*���Ų���------------------------------------------------------------------------------------------------------------------------*/
/*�����ֲ���*/
#define HWL_CONST_EVEN		0		//ż
#define HWL_CONST_ODD		1		//��
typedef struct
{
	U8	cwGroup;		/* �������� ��0 to 127��*/
	U8	channelId;		/* ��ʹ��*/
	U8	evenOrOdd;		/* CW ����ż ��  1�棬0ż*/
	U8	words[8];

}HWL_ControlWord_t;
S32 HWL_ControlWordSet(HWL_ControlWord_t *cw);/**	CW����.*/
S32 HWL_ControlWordSwitchSet(U8 evenOrOdd);/**	CW�л�*/

/*���ü���ģ�鿪��*/
void HWL_ScrambleEnable(BOOL bEnable);

/*״̬����------------------------------------------------------------------------------------------------------------------------*/
/*��ȡͨ�������ʣ�ͨ��ȫ����TsIndex = -1�������ߵ���TS����*/
#ifdef GN1846
U32  HWL_GetBitrate(HANDLE32 Handle, S16 CHNIndex, S16 TsIndex);
#else
U32  HWL_GetBitrate(S16 CHNIndex, S16 TsIndex);
#endif

/*����ģ��PLLֵ*/
U32 HWL_PLLValue();

/*����ģ���¶�.*/
S32 HWL_Temperature();


/**	���ͨ��������״̬.
*	@(channelId):��0��ʼ��[0-7]
*/
BOOL HWL_GetQAMBuffStatus(U8 channelId);
BOOL HWL_GetTunerShortStatus(U8 channelId);


/*TS�����Ͳ���------------------------------------------------------------------------------------------------------------------------*/
/*��CPU����TS����FPGA*/
void HWL_SetDirectOutTsPacket(S16 OutTsIndex, U8 *pTsData, S32 DataSize);


/*Ӳ��TS�����������ܼ���*/

/**	���Save_H_L λ�õ�PSI��Ϣ
*	@(offset): ��� = 0xffff  , Ϊ���ȫ��PSI ��Ϣ.
*/
S32 HWL_HWInserterClear(U16 offset);

/**	���ò���TS���� .*/
S32 HWL_HWInserterSet(S16 SlotIndex, S16 TsIndex, U8 *pData, S32 DataSize, S32 Interval);


/*QAM����------------------------------------------------------------------------------------------------------------------------*/
enum 
{
	HWL_CONST_MODULATOR_NONE = 0,
	HWL_CONST_MODULATOR_AD9789_QAM,
	HWL_CONST_MODULATOR_AD9789_QPSK_H,//�϶�
	HWL_CONST_MODULATOR_AD9789_QPSK_L,//�¶�
	HWL_CONST_MODULATOR_AD9789_DTMB,//�������
	HWL_CONST_MODULATOR_NUM
};


enum 
{
	HWL_CONST_MODULATOR_QAM_16 = 0,
	HWL_CONST_MODULATOR_QAM_32,
	HWL_CONST_MODULATOR_QAM_64,
	HWL_CONST_MODULATOR_QAM_128,
	HWL_CONST_MODULATOR_QAM_256,
	HWL_CONST_MODULATOR_QAM_512,
	HWL_CONST_MODULATOR_QAM_1024,
	HWL_CONST_MODULATOR_QAM_QPSK,
	HWL_CONST_MODULATOR_QAM_4,
	HWL_CONST_MODULATOR_QAM_4NR,
	HWL_CONST_MODULATOR_QAM_NUM
};

enum 
{
	HWL_CONST_MODULATOR_STANDARD_ANNEX_A = 0,
	HWL_CONST_MODULATOR_STANDARD_ANNEX_B,
	HWL_CONST_MODULATOR_STANDARD_ANNEX_C,
	HWL_CONST_MODULATOR_STANDARD_ANNEX_NUM
};

enum 
{
	HWL_CONST_MODULATOR_ANALOG_BAND_8M = 0,
	HWL_CONST_MODULATOR_ANALOG_BAND_6M,
	HWL_CONST_MODULATOR_ANALOG_BAND_7M,
	HWL_CONST_MODULATOR_ANALOG_BAND_NUM
};

typedef enum 
{
	HWL_CONST_MODULATOR_FEC_1_OF_2 = 0,
	HWL_CONST_MODULATOR_FEC_2_OF_3,
	HWL_CONST_MODULATOR_FEC_3_OF_4,
	HWL_CONST_MODULATOR_FEC_5_OF_6,
	HWL_CONST_MODULATOR_FEC_7_OF_8,
	HWL_CONST_MODULATOR_FEC_NUM
}HWL_CONST_MODULATOR_FEC;


enum
{
	HWL_MODULATOR_DTMB_CARRIER_SINGLE = 0,
	HWL_MODULATOR_DTMB_CARRIER_MULTI,
	HWL_MODULATOR_DTMB_CARRIER_NUM
};

enum
{
	HWL_MODULATOR_DTMB_PN420 = 0,
	HWL_MODULATOR_DTMB_PN595,
	HWL_MODULATOR_DTMB_PN945,
	HWL_MODULATOR_DTMB_PN420F,
	HWL_MODULATOR_DTMB_PN595F,
	HWL_MODULATOR_DTMB_PN945F,
	HWL_MODULATOR_DTMB_PN_NUM
};

enum
{
	HWL_MODULATOR_DTMB_TI_240 = 0,
	HWL_MODULATOR_DTMB_TI_720,
	HWL_MODULATOR_DTMB_TI_NUM
};

enum
{
	HWL_MODULATOR_DTMB_CodeRate_04 = 0,
	HWL_MODULATOR_DTMB_CodeRate_06,
	HWL_MODULATOR_DTMB_CodeRate_08,
	HWL_MODULATOR_DTMB_CodeRate_NUM
};

///////////////////////////////////////////////////////
typedef struct
{
	U8						E3DS3Select;					/* E3/DS3ѡ�� */
	U8						BitOrder;						/*MSB/LSB*/
	U8						FrameForm;						/*   */
	S16						InOutPacketLength;				/* 188/204 */
	BOOL					InterweaveCoding;
	BOOL					ScrambleChanger;				/*   */
	BOOL					RSCoding;						/*   */				/*    */
	BOOL					CodeRateRecover;				/*     */

}HWL_E3DS3Param_t;

typedef enum
{
	GS_TUNER_ANNEX_A = 0,
	GS_TUNER_ANNEX_B,
	GS_TUNER_ANNEX_C
}GS_TunerReqTDREQTYPE_ype;

typedef enum
{
	GS_TUNER_PLOAR_NONE = 0,
	GS_TUNER_PLOAR_VER,	
	GS_TUNER_PLOAR_HOR
}GS_TunerPolarMethod;

typedef enum
{
	GS_SPECINV_OFF = 0,
	GS_SPECINV_ON,
	GS_SPECINV_AUTO
}GS_TunerSpecinv;




typedef struct
{
	U8						ModulateStandard;			/*���Ʊ�׼�� ��ö�ٱ��� J.83 Annex A/B/C��0:A*/
	S32						ModulateFreq;				/*����Ƶ�ʣ� Khz�����ݾɳ����³���ΪHZ����*/
	U8						QamMode;					/*����ģʽ�� ��ö�ٱ���*/
	S32						SymRate;					/*�����ʣ� Kboud/s���ݾɳ����³���ΪHZ����*/
	U8						AnalogBand;					/*����ģ����� ��ö�ٱ���*/
	U8						Gain;						/*�Ŵ�������΢����0~20*/
	S16						Attenuator;					/*˥��������΢����0~40*/
	BOOL					SpectrumInvert;				/*Ƶ�׷�ת���أ�0 ����ת��1��ת*/
	BOOL					QamSwitch;					/*���ƿ��أ�0 �رգ�1 ����*/
	BOOL					RFSwitch;					/*RF������أ�0 �رգ�1 ����*/

	S32						QPSK_FEC;					/*�� HWL_CONST_MODULATOR_FEC */
	U8						m_RollOffFactor;			/*����ϵ��ѡ��*/


	S32						DTMB_Carrier;				//0�����ز���1���ز�
	S32						DTMB_PN;				//0��PN420��1��PN575��2��PN945
	S32						DTMB_TI;				//0��240���ţ�1�� 720����
	S32						DTMB_CodeRate;				//0��0.4��1�� 0.6��2��0.8��
	S32						DTMB_DoublePilot;


	S32						FreqAdj;//Ƶ��΢��
}HWL_ModulatorParam_t;

typedef struct
{
	U32 Frequency ;   //unit 1k Hz
	U32 Symbol ;   //unit 0.001MBand
	U32 LocalFreq;
	U8  Modulation;	    //the modulation mode of the signal
	U8  TuneReqType;    //TRT_DVBC TRT_DCABLE TRT_J83C
	U8  PolarMethod;
	U8  Switch_22K;	
	U8  SpectInv;;	

}HWL_TunerParam_t;





/*
typedef struct  
{
U8   ChipID;   
U8   RorW;
U8	 RegisterControl;
U8   FunctionControl;
}HWL_SubTUNER;

typedef struct  
{
HWL_SubTUNER	m_pTUNER[MULT_CHANNEL_SBU_TYPE_TUNER_MAX_NUM];

}HWL_TUNER;


*/

/**
*����ģ���ʼ��������������ѹ�����������ΪĬ��ֵ.
*/
#ifdef GQ3760B
void HWLL_QAMHWTypeInitialize(HWL_HWInfo *pHWInfo);
#endif

void HWL_QAMInitialize(U8 ModulatorType);

void HWL_QAMTerminate();

void HWL_QAMDirectRegSet(U8 ICID, U32 Addr, U32 Value);

/**
*����PLLֵ.
*/
void HWL_QamSetPLLParameter(void);


S32 HWL_QAMParameterApply(S16 TsIndex);

void HWL_QAMApplyParameterWithPQ(U32 PValue, U32 QValue);
/////////////////////////////////////

typedef enum
{
	GS_E3DS3_SELECT_E3 = 0,
	GS_E3DS3_SELECT_DS3 ,
	GS_E3DS3_SELECT_AUTO
}GS_ASIE3DS3;

typedef enum
{
	GS_E3DS3_BITORDER_MSB = 0,
	GS_E3DS3_BITORDER_LSB 
}GS_ASIBitorder;

typedef enum
{
	GS_E3DS3_FRAMEFORM_NO = 0,
	GS_E3DS3_FRAMEFORM_YES
}GS_ASIFrameform;

typedef enum
{
	GS_E3DS3_PACKETLENGTH_188 = 0,
	GS_E3DS3_PACKETLENGTH_204

}GS_ASIPacketlength;

void HWL_TunerSetParameter(S16 TsIndex,HWL_TunerParam_t *lParam);
void HWL_TunerApplyParameter(S16 Tunercount, S16 ChannelType);

///////////////////////////////////////////

void HWL_QAMFDACOffsetSet(S32 FrequenceCalibrate);

/**
*��ȡ������QAM����.
*/
BOOL HWL_QamGetParameter(S16 TsIndex, HWL_ModulatorParam_t *param);

/**
*�˺����ڲ���AdjustParam�Լ�ApplyParameter��
*/
BOOL HWL_QAMParameterSet(S16 ChnInd,HWL_ModulatorParam_t *param);

#ifdef USE_NEW_QAM_SETTING_FUNCTIONS

typedef void (*HWL_ModModuleResetCB)(void);

BOOL HWL_QAMParameterSetNew(S16 ChnInd, HWL_ModulatorParam_t *pParam, HWL_ModModuleResetCB pResetCB);
void HWL_QAMGetCurrentParam(S16 ChnInd, HWL_ModulatorParam_t *pParam);
void HWL_QAMForceOutputProgressChange(S32 lNewValue);
void HWL_QAMForceNotLevelOnly(void);
#endif

/*ϵͳ����------------------------------------------------------------------------------------------------------------------------*/
/*IGMP ������ʽ����*/
#define HWL_CONST_IGMP_MODE_AUTO		2
#define HWL_CONST_IGMP_MODE_V3			3

/*Ӳ���洢�ռ����   
*	@(Index)	[0,7].
*	@(buff):	�û����ݻ�����.������ȫFFʱΪ����������
*	@(buffLen):	����Ϊ10.
*	��80���ֽڵĴ洢�ռ䣬���ṩ��10�ֽ�Ϊ��λ�Ķ�ȡ��
*/
S32 HWL_EncryptChipWrite(U8 *buff , U8 bufflen, U8  Index);
S32 HWL_EncryptChipRead(U8 *buff , U8 bufflen, U8 Index , S32 Timeout);
void HWL_SetI2cAndTunerReset(U8 ChannelNo, U8 Reset,U8 LNBPower);

/************************************************************************************/
/* PIDͳ�ƵĶ˿����� */
/************************************************************************************/
#define HWL_CONST_PID_NUM_MAX			250


/**	PID ͳ�ƵĶ˿�����.*/
S32 HWL_PidStatisticSend(U8 ChnIndex, U8 SubIndex);

/**	 PID ͳ�ƵĶ˿����ʲ�ѯ����.*/
S32 HWL_PidStatisticSearch(U8 ChnIndex, U8 SubIndex);

typedef struct tagHWL_PidRate
{
	U16	pidVal;
	S32 pidRate;

} HWL_PidRate_t;

/**	�ظ��ṹ��*/
typedef struct tagHWL_PidRateArray
{
	//HWL_PidStatics_t search;	//��ʶ��ѯ����.
	U8 physicChannelId;			//
	U32 totalRate;				//ͨ��������
	HWL_PidRate_t pidRateArray[HWL_CONST_PID_NUM_MAX];
	U32 channelRateArraySize;

} HWL_PIDStatisticsArray;

S32 HWL_PidStatisticArray(HWL_PIDStatisticsArray *pArray);

/************************************************************************************/
/* ����IP�˿�����ͳ�Ʋ�ѯ|��� */
/************************************************************************************/
/*IPͳ�ƺ�*/
#define HWL_CONST_IP_STATISTICS_FRAME_ITEM_MAX	124
#define HWL_CONST_IP_STATISTICS_FRAME_NUM_MAX	2
#define HWL_CONST_IP_STATISTICS_TOTAL_ITEM_MAX	(HWL_CONST_IP_STATISTICS_FRAME_ITEM_MAX * HWL_CONST_IP_STATISTICS_FRAME_NUM_MAX)

typedef struct
{
	U32 addressv4;
	U16 portNumber;
	U32 portRate;
	U8	protocol;
}HWL_EthDetectionInfo;

typedef struct
{
	U32	totalRate;
	HWL_EthDetectionInfo ipPortInfoArray[HWL_CONST_IP_STATISTICS_TOTAL_ITEM_MAX];
	U32	ArrySize;
}HWL_IPStatisticsArray;

/**	����IP�˿�����ͳ����Ϣ���..*/
S32 HWL_IPPortStatisticClear(S32 ChnIndex);

/**	����IP�˿�����ͳ����Ϣ��ѯ..*/
S32 HWL_IPPortStatisticSend(S32 ChnIndex);

/*��ǰ����UDP/RTP����ַ��Ϣ�б��ȡ*/
S32 HWL_IPPortStatisticArrayGet(S32 ChnIndex, HWL_IPStatisticsArray *pArray);

void HWL_ASIApply(S32 DeviceIndex, S32 ChnIndex, BOOL bInput);
S32 HWL_ASIAdd(S32 DeviceIndex, S32 ChnIndex, S32 SubIndex, U32 BitRate, BOOL8 Active, BOOL bInput);
void HWL_ASIClear(S32 DeviceIndex, BOOL bInput);


U8 HWL_GetHardwareVersion(void);

S32 HWL_TunerCountCheck(void);
S32 HWL_ATBM8869CountCheck(void);




#ifdef GN2000

#define HWL_MAX_AUDIO_OFFSET_PID_NUM		12
typedef struct  
{
	S16		m_TsInd;
	U16		m_PID;
	S32		m_Offset;//MS������
	U16		m_PCRPID;
}HWL_AudioOffsetNode;


void HWL_EncoderInit(void);
void HWL_EncoderApply(void);
void HWL_EncoderTerm(void);
void HWL_EncoderSetAudioPIDDelay(HWL_AudioOffsetNode *pArray, S32 PIDNum);
#endif

/*FPGA ETH*/
typedef void (*HWL_FPGAEthTSCB)(S32 Index, U8* Buff);
void HWL_FPGAEthSetCB(HWL_FPGAEthTSCB pCB);
void HWL_FPGAEthRedvFromFPGA(U8 *pData, S32 DataLen);
void HWL_FPGAETHSendToFPGA(S32 Index, U8 *pTsData);



void HWL_TDDemodTest(void);
S32 HWL_TDDemodCheck(S32 *pType);
void HWL_TDInitiateTDModule(S32 HWLSubChnType);
void HWL_TDGetSignalFloat(S32 Slot, BOOL *pbLock, F64 *pStrenth, F64 *pQuanlity, F64 *pBER);
S32 HWL_TDGetDVBCConStellation(S32 Slot);
BOOL HWL_TDGetDVBSChannelParam(S32 Slot, TD_DVBSXCHNlInfo *pDVBSXParam);
void HWL_TDApplyParam(S32 Slot, S32 FrequencyHz, S32 SymboRate, S32 Bandwidth, BOOL SpecInv, BOOL bEnable22K);



S32 HWL_QAMDTMBApplyFPGAParameters(BOOL bTone);
BOOL HWL_QAMParameterCheckNeedResetRF(void);


void FIRII_Reload(CHAR_T *pMatlabCoeffFile);


/*������*/
U32 HWL_EncoderGetFPGAID(void);
CHAR_T *HWL_EncoderGetFPGAVersion(void);

void HWL_EncoderAuthFixStorageCB(void* pUserParam, U32 StartAddress, U8 *pData, S32 DataSize, BOOL bRead);



/*ETH*/

#ifdef MULT_SYSTEM_HAVE_PCR_CORRECT_ADJUST_FUNCTION
/*PCR����ģʽ*/
void HWL_SetChnPCRCorrectMode(S32 ChnIndex, U8 PCRUper, U8 PCRLower, BOOL bOpen);
#endif





/*FPGA REG*/
void HWL_FPGARegInitate(void);
BOOL HWL_FPGARegProcess(S32 SlotInd, U16 ChipID, U32 Address, U32 *pData, S32 Size, BOOL bRead, S32 TimeoutMS);
BOOL HWL_FPGARegICPRecv(U8 *pData, S32 DataSize);
void HWL_FPGARegTerminate(void);

void HWL_FPGARegPHYInitate(void);
BOOL HWL_FPGARegPHYDetectOK(S32 Slot);
void HWL_FPGARegPHYDetectAddr(S32 Slot);
S32 HWL_FPGARegPHYGetLinkStatus(S32 Slot);

#ifdef GN1846
#include "hdmi_rx.h"

extern S32 g_LedFlashInterval;
void HWL_RunStatusDebug(void);
void HWL_HdmiRxDebug(void);
void HWL_FpgaSpiDebug(void);

BOOL HWL_HdmiRxDownloadEdid(S32 ChnIndex, S32 EdidType);
BOOL HWL_HdmiRxSetParam(S32 ChnIndex, HDMI_RxCfgParam *pCfgParam);
BOOL HWL_HdmiRxGetStatus(S32 ChnIndex, HDMI_RxStatusParam *pStatus);
#endif

#endif

/*EOF*/



