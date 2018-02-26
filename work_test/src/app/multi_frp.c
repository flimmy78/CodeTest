
/* Includes-------------------------------------------------------------------- */
#include "global_micros.h"
#include "platform_assist.h"
#include "multi_main_internal.h"
#include "frp_device.h"

#ifndef DEBUG_DISABLE_FRP
/* Global Variables (extern)--------------------------------------------------- */
extern int g_auto_reboot_times;//20130603 ���Ӹ���Ϣ����豸�����BUG�������µ��Զ������õĴ�����

/* Macro (local)--------------------------------------------------------------- */
/* Private Constants ---------------------------------------------------------- */
/* Private Types -------------------------------------------------------------- */
#ifdef USE_FRP_PRE_CONSOLE
HANDLE32 s_TmpFRpHandle = NULL;
void MULT_FRPPreConsoleIntiate(void)
{
	s_TmpFRpHandle = FRP_DeviceCreate(1);
	FRP_DeviceSetLED(s_TmpFRpHandle, FRP_LED_BACK, FRP_LED_BACK_ON);
	FRP_DeviceSetLED(s_TmpFRpHandle, FRP_LED_STATUS, FRP_LED_GREEN);
	FRP_DeviceSetLED(s_TmpFRpHandle, FRP_LED_ALARM, FRP_LED_GREEN);
	FRP_DeviceSetCursorIndex(s_TmpFRpHandle, GLOBAL_INVALID_INDEX);
	FRP_DeviceSetLCDText(s_TmpFRpHandle, 0, "    INIT DEVICE     ");
	FRP_DeviceSetLCDText(s_TmpFRpHandle, 1, "                    ");
	FRP_DeviceUpdateAll(s_TmpFRpHandle);
}

void MULT_FRPPreConsoleSetText(CHAR_T *pLineText, S32 Line)
{
	FRP_DeviceSetLCDText(s_TmpFRpHandle, Line, pLineText);
	FRP_DeviceUpdateAll(s_TmpFRpHandle);
}

void MULT_FRPPreConsoleTerminate(void)
{
	FRP_DeviceDestroy(s_TmpFRpHandle);
}

#endif

BOOL MULT_FRPDetectNewCardFRP(void)
{
	BOOL lRet = FALSE;
	HANDLE32 lHandle;
	lHandle = FRP_DeviceCreate(1);
	lRet = FRP_DeviceDetectFirmwareVersion(lHandle, NULL, NULL);
	FRP_DeviceDestroy(lHandle);
	return lRet;
}

#ifdef USE_CARD_FRP
#include "frp_card.h"
static S32 s_LastMainCount = 0;
#ifdef GN1846

#define KEY_INFO (1)
#define KEY_VR	(254) // ǰ������ⰴ������

BOOL FRPL_CardCB(void* pUserParam, S32 CMD, U32 Value)
{
	GLOBAL_TRACE(("cmd:%d val:%d\n", CMD, Value));

	return TRUE;
}

void FRP_CardInitiate(void)
{
	FRPCARD_Param lParam;
	GLOBAL_ZEROSTRUCT(lParam);

	lParam.m_COMInd = 1; /* ���ﴮ�ں�Ĭ���� ttyPS*(*��ʾ����������) */
	lParam.m_AlarmBlinkDurationMS = 5000;
	lParam.m_pCMDCB = FRPL_CardCB;
	lParam.m_pUserParam = NULL;

	lParam.m_bUseOLEDPanel = TRUE;
	lParam.m_OLEDPanelType = OLED_SCREEN_TYPE_ZJYDZ_128X32;
	GLOBAL_STRCPY(lParam.m_pFontPath, "/tmp/arialuni_U16.bin");
	lParam.m_InfoMaxNum = 20;

	FRPCARD_Initiate(&lParam);
}

static void FRP_CardUserKeyCB(void* pUserParam, U8 Key)
{
	MULT_Handle *plHandle = (MULT_Handle *)pUserParam;
	MULT_Monitor *plMonitor;
	S32 lCount, lLogNum, i;
	CAL_LogConfig lLogConfig;
	S32 lTotalAlarmCount = 0;

	switch (Key) {
		case KEY_VR: /* ��ȡ�豸������Ϣ */
			plMonitor = &plHandle->m_Monitor;

			lLogNum = CAL_LogGetLogCFGCount(plMonitor->m_LogHandle);
			for (i = 0; i < lLogNum; i++) {
				if (CAL_LogGetUsedMark(plMonitor->m_LogHandle, i)) {
					CAL_LogProcConfig(plMonitor->m_LogHandle, i, &lLogConfig, TRUE);
					if (lLogConfig.m_bFP) { /* ǰ�����ʾ���� */
						lCount = CAL_LogGetLogInfoCount(plMonitor->m_LogHandle, i);
						lTotalAlarmCount += lCount;
					}
				}
			}
			FRP_CardSetAlarm(FRP_ALARM_TOTAL, (lTotalAlarmCount > 0));
			break;
		case KEY_INFO:
			break;
		default:
			// GLOBAL_TRACE(("Unrecognize Key: %d\n", Key));
			break;
	}
}

/* ��ʾ��ʼ��������Ϣ */
void FRP_CardShowInitateProgressString(CHAR_T *pString)
{
	FRPCARD_INFO lInfo;

	GLOBAL_ZEROSTRUCT(lInfo);
	lInfo.m_bUTF8 = FALSE;
	GLOBAL_STRCPY(lInfo.m_pInfoText[0], pString);
	GLOBAL_STRCPY(lInfo.m_pInfoText[1], "Please Wait......   ");
	FRPCARD_SetInfoNode(0, &lInfo);
}

/* ǰ�����ʾ���� */
void FRP_CardDisplayInfoUpdate(MULT_Handle *pHandle) 
{
	MULT_Config *plConfig;
	MULT_Information *plInfo;
	CHAR_T plStrBuf[64];
	S32 lOffst;
	FRPCARD_INFO lInfo;
	U32 lSlot = 0;
	const U8 *plInfoName[9][2] = {{"1.0 IP Address",	"1.0 ��̫����ַ"},
	{"2.0 SubNet Mask",	"2.0 ��������"},
	{"3.0 Gate Way",	"3.0 ����"},
	{"4.0 MAC",			"4.0 ����������ַ"},
	{"5.0 SN",			"5.0 �豸���к�"},
	{"6.0 SoftWare Ver","6.0 ����汾"},
	{"7.0 HardWare Ver","7.0 Ӳ���汾"},
	{"8.0 Soft Release","8.0 �������"},
	{"9.0 FPG Release",	"9.0 FPGA ����"}};

	GLOBAL_ZEROSTRUCT(lInfo);
	lInfo.m_bUTF8 = FALSE;

	plConfig = &pHandle->m_Configuration;
	plInfo = &pHandle->m_Information;

	lOffst = CAL_StringCenterOffset(plInfo->m_pModelName, 21);
	if ((lOffst < (21 - GLOBAL_STRLEN(plInfo->m_pModelName))) && (lOffst >= 0))
	{
		GLOBAL_MEMSET(plStrBuf, ' ', sizeof(plStrBuf));
		GLOBAL_STRCPY(&plStrBuf[lOffst], plInfo->m_pModelName);
	}
	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plStrBuf, sizeof(lInfo.m_pInfoText));

	if (plConfig->m_FrpLanguage == 0)
	{
		GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pLCDENG, sizeof(lInfo.m_pInfoText));
	}
	else
	{
		GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pLCDCHN, sizeof(lInfo.m_pInfoText));
	}
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[0][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], PFC_SocketNToA(plConfig->m_ManageIPv4Addr), sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[1][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], PFC_SocketNToA(plConfig->m_ManageIPv4Mask), sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[2][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], PFC_SocketNToA(plConfig->m_ManageIPv4Gate), sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[3][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	CAL_StringBinToMAC(plConfig->m_pMAC, sizeof(plConfig->m_pMAC), plStrBuf, sizeof(plStrBuf));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plStrBuf, strlen(plStrBuf) / 2);
	lInfo.m_pInfoText[1][strlen(plStrBuf) / 2] = '\0';
	FRPCARD_SetInfoNode(lSlot++, &lInfo);
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], &plStrBuf[strlen(plStrBuf) / 2], sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[4][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pSNString, strlen(plInfo->m_pSNString) / 2);
	lInfo.m_pInfoText[1][strlen(plInfo->m_pSNString) / 2] = '\0';
	FRPCARD_SetInfoNode(lSlot++, &lInfo);
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], &plInfo->m_pSNString[strlen(plInfo->m_pSNString) / 2], sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[5][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pSoftVersion, sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[6][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pHardVersion, sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[7][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pSoftRelease, sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);

	GLOBAL_STRNCPY(lInfo.m_pInfoText[0], plInfoName[8][plConfig->m_FrpLanguage], sizeof(lInfo.m_pInfoText));
	GLOBAL_STRNCPY(lInfo.m_pInfoText[1], plInfo->m_pFPGARelease, sizeof(lInfo.m_pInfoText));
	FRPCARD_SetInfoNode(lSlot++, &lInfo);
}

/* ���ñ���/����/������� */
void FRP_CardSetAlarm(S32 AlarmType, BOOL IsAlarm)
{
	S32 lLedIndex = 0;

	switch (AlarmType) {
		case FRP_ALARM_TOTAL:
			if (IsAlarm)
				FRPCARD_SetCardAlarm(GLOBAL_INVALID_INDEX, 1);
			else
				FRPCARD_SetCardAlarm(GLOBAL_INVALID_INDEX, 0);
			return;
		case FRP_ALARM_INPUT:
			lLedIndex = 3;
			break;
		case FRP_ALARM_OUTPUT:
			lLedIndex = 0;
			break;
		default:
			break;
	}

	if (IsAlarm) {
		FRPCARD_SetCardStatus(lLedIndex, FRPCARD_CARD_STATUS_ERROR);
	}
	else {
		FRPCARD_SetCardStatus(lLedIndex, FRPCARD_CARD_STATUS_NORMAL);
	}
}

void FRP_CardSetInit(MULT_Handle *pHandle, BOOL bEnableInit)
{
	S32 i = 0;

	if (bEnableInit) {
		// ��� GN1846 ��ʼ��ʲô����������
	}
	else {
		// ���ð�����Ӧ
		FRPCARD_SetUserKeyCB(FRP_CardUserKeyCB, pHandle);
	}
}

void FRP_CardAccess(MULT_Handle *pHandle, S32 Duration)
{
	MULT_Monitor *plMonitor;
	S32 lCount, lLogNum, i;
	CAL_LogConfig lLogConfig;
	S32 lTotalAlarmCount = 0;

	/* ����ʵ�� CPLD �����Ź��Ĺ��� */
	plMonitor = &pHandle->m_Monitor;

	lLogNum = CAL_LogGetLogCFGCount(plMonitor->m_LogHandle);
	for (i = 0; i < lLogNum; i++) {
		if (CAL_LogGetUsedMark(plMonitor->m_LogHandle, i)) {
			CAL_LogProcConfig(plMonitor->m_LogHandle, i, &lLogConfig, TRUE);
			if (lLogConfig.m_bFP) { /* ǰ�����ʾ���� */
				lCount = CAL_LogGetLogInfoCount(plMonitor->m_LogHandle, i);
				lTotalAlarmCount += lCount;
			}
		}
	}
	FRP_CardSetAlarm(FRP_ALARM_TOTAL, (lTotalAlarmCount > 0));

	return;
}

void FRP_CardTerminate(void)
{
	FRPCARD_Terminate();
}
#endif
#else
BOOL FRPL_CardCB(void* pUserParam, S32 CMD, U32 Value)
{
	if (CMD == FRPCARD_CMD_RESET_IP)
	{
		CHAR_T plTmpStr[128];
		GLOBAL_TRACE(("Default IP Address Temporary!!! \n"));
		GLOBAL_SNPRINTF((plTmpStr, sizeof(plTmpStr), "ifconfig eth%d ", 0));
		GLOBAL_STRCAT(plTmpStr, PFC_SocketNToA(MULTI_DEFAULT_IP_ADDR));
		GLOBAL_STRCAT(plTmpStr, " netmask ");
		GLOBAL_STRCAT(plTmpStr, PFC_SocketNToA(MULTI_DEFAULT_IP_MASK));
		GLOBAL_TRACE(("%s\n", plTmpStr));
		PFC_System(plTmpStr);
	}
}

void FRP_CardInitiate(void)
{
	FRPCARD_Param lParam;
	GLOBAL_ZEROSTRUCT(lParam);

	lParam.m_COMInd = 1;
	lParam.m_AlarmBlinkDurationMS = 5000;
	lParam.m_pCMDCB = FRPL_CardCB;
	lParam.m_pUserParam = NULL;


	FRPCARD_Initiate(&lParam);
}

void FRP_CardSetInit(BOOL bEnableInit)
{
	FRPCARD_SetInitStatus(bEnableInit);
}

void FRP_CardAccess(MULT_Handle *pHandle, S32 Duration)
{
	S32 i, lCount, lTotalCount, lLogNum;
	BOOL blErrorMark;
	CAL_LogConfig lLogConfig;
	MULT_Monitor *plMonitor;

	plMonitor = &pHandle->m_Monitor;

	lTotalCount = 0;
	blErrorMark = FALSE;
	lLogNum = CAL_LogGetLogCFGCount(plMonitor->m_LogHandle);
	for (i = 0; i < lLogNum; i++)
	{
		if (CAL_LogGetUsedMark(plMonitor->m_LogHandle, i))
		{
			CAL_LogProcConfig(plMonitor->m_LogHandle, i, &lLogConfig, TRUE);
			if (lLogConfig.m_bFP)
			{
				lCount = CAL_LogGetLogInfoCount(plMonitor->m_LogHandle, i);
				if (lCount > 0)
				{
					//GLOBAL_TRACE(("Alarm Ind = %d, Count = %d\n", i, lCount));
					lTotalCount += lCount;
					if (i == MULT_MONITOR_TYPE_FPGA)
					{
						blErrorMark = TRUE;
					}
				}
			}
		}
	}


#ifdef ENCODER_CARD_PLATFORM

	{
		S32 lModuleState;
		for (i = 0; i < 6; i++)
		{
			lModuleState = CARD_ModuleStateGet(i);
			if (lModuleState == CARD_STATE_NORMAL)
			{
				FRPCARD_SetCardStatus(i, FRPCARD_CARD_STATUS_NORMAL);
				FRPCARD_SetCardAlarm(i, CARD_ModuleAlarmCountGet(i));
			}
			else if ((lModuleState > CARD_STATE_INIT && lModuleState < CARD_STATE_NORMAL) || (lModuleState == CARD_STATE_TERMINATE))
			{
				FRPCARD_SetCardStatus(i, FRPCARD_CARD_STATUS_INIT);
			}
			else if (lModuleState == CARD_STATE_WAIT_CARD_REMOVE)
			{
				FRPCARD_SetCardStatus(i, FRPCARD_CARD_STATUS_WAIT_REMOVE);
			}
			else if (lModuleState == CARD_STATE_ERROR)
			{
				FRPCARD_SetCardStatus(i, FRPCARD_CARD_STATUS_ERROR);
			}
			else
			{
				FRPCARD_SetCardStatus(i, FRPCARD_CARD_STATUS_NO_CARD);
			}
		}
	}

#endif


	if (blErrorMark)
	{
		FRPCARD_SetCardStatus(GLOBAL_INVALID_INDEX, FRPCARD_CARD_STATUS_ERROR);
	}
	else
	{
		FRPCARD_SetCardStatus(GLOBAL_INVALID_INDEX, FRPCARD_CARD_STATUS_NORMAL);
		FRPCARD_SetCardAlarm(GLOBAL_INVALID_INDEX, lTotalCount);
	}
	s_LastMainCount = lTotalCount;
}

void FRP_CardTerminate(void)
{
	FRPCARD_Terminate();
}
#endif
#else

S32 FRP_AgentCB(S32 MenuInfoID, void *pParam, void *pUserParam)
{
	S32 lRet = FRP_MENU_OK;
	MULT_Handle *plHandle = (MULT_Handle *)pUserParam;
	if (plHandle)
	{
		MULT_Config *plConfig;
		MULT_Information *plInfo;
		CHAR_T *plTmpStr;
		U32 lTmpValue;
		U8 lIPFirst;
		plConfig = &plHandle->m_Configuration;
		plInfo = &plHandle->m_Information;

		switch(MenuInfoID)
		{
		case FRP_MENU_ID_NET_IP:
			{
				plTmpStr = (CHAR_T*)pParam;
				GLOBAL_TRACE(("IP Set = %s\n", plTmpStr));
				lTmpValue = PFC_SocketAToN(plTmpStr);
				lIPFirst = ((lTmpValue >> 24) & 0xFF);
				if ( (lIPFirst > 0) && (lIPFirst < 224) )
				{
					plConfig->m_ManageIPv4Addr = lTmpValue;

#ifdef MULT_SUPPORT_FPGA_ETH
					{
						TUN_InitParam *plTunParam = MULT_FPGAEthGetParameterPtr();
						if ((lTmpValue & plConfig->m_ManageIPv4Mask) == (plTunParam->m_TUNIPAddr & plTunParam->m_TUNIPMask))
						{
							GLOBAL_TRACE(("Sub Net Conflicted!!!!!!!\n"));
							lRet = FRP_MENU_NO;
						}
						else
						{
							MULTL_ManagePortConfig(plHandle);//����IP��ַ
							MULTL_SaveConfigurationXML(plHandle);
							MULTL_SaveParamterToStorage(plHandle);
						}
					}
#else
					MULTL_ManagePortConfig(plHandle);//����IP��ַ
					MULTL_SaveConfigurationXML(plHandle);
					MULTL_SaveParamterToStorage(plHandle);
#endif

				}
				else
				{
					GLOBAL_TRACE(("Can Not Use Muticast Addr As Interface IP!!!!!!!\n"));
					lRet = FRP_MENU_NO;
				}
			}
			break;
		case FRP_MENU_ID_NET_MASK:
			{
				plTmpStr = (CHAR_T*)pParam;
				GLOBAL_TRACE(("Mask Set = %s\n", plTmpStr));
				plConfig->m_ManageIPv4Mask = PFC_SocketAToN(plTmpStr);
#ifdef MULT_SUPPORT_FPGA_ETH
				{
					TUN_InitParam *plTunParam = MULT_FPGAEthGetParameterPtr();
					if ((lTmpValue & plConfig->m_ManageIPv4Addr) == (plTunParam->m_TUNIPAddr & plTunParam->m_TUNIPMask))
					{
						GLOBAL_TRACE(("Sub Net Conflicted!!!!!!!\n"));
						lRet = FRP_MENU_NO;
					}
					else
					{
						MULTL_ManagePortConfig(plHandle);//����IP��ַ
						MULTL_SaveConfigurationXML(plHandle);
						MULTL_SaveParamterToStorage(plHandle);
					}
				}
#else
				MULTL_ManagePortConfig(plHandle);//����IP��ַ
				MULTL_SaveConfigurationXML(plHandle);
				MULTL_SaveParamterToStorage(plHandle);
#endif
			}
			break;
		case FRP_MENU_ID_NET_GATE:
			{
				plTmpStr = (CHAR_T*)pParam;
				GLOBAL_TRACE(("Gate Set = %s\n", plTmpStr));
				lTmpValue = PFC_SocketAToN(plTmpStr);
				lIPFirst = ((lTmpValue >> 24) & 0xFF);
				if ( (lIPFirst > 0) && (lIPFirst < 224) )
				{
					plConfig->m_ManageIPv4Gate = lTmpValue;
					MULTL_ManagePortConfig(plHandle);//����IP��ַ
					MULTL_SaveConfigurationXML(plHandle);
					MULTL_SaveParamterToStorage(plHandle);
				}
				else
				{
					lRet = FRP_MENU_NO;
				}

			}
			break;
		case FRP_MENU_ID_RESET:
			{
				GLOBAL_TRACE(("Parameter Reset!!!!!!!!!!!!\n"));
				MULTL_ParameterReset(plHandle);
				/*����*/
				MULTL_RebootSequence(plHandle);
			}
			break;
		case FRP_MENU_ID_FACTORY:
			{
				GLOBAL_TRACE(("Factory Preset!!!!!!!!!!!!\n"));
				MULTL_FactoryPreset(plHandle);
				/*����*/
				MULTL_RebootSequence(plHandle);

			}
			break;;
		case FRP_MENU_ID_WARN:
			{
				GLOBAL_TRACE(("Alarm Clear!!!!!!!!!!!!\n"));
				MULTL_ResetAlarmCount(plHandle, GLOBAL_INVALID_INDEX);
			}
			break;;
		case FRP_MENU_ID_LANG:
			{
				S32 lNewLanIndex = (*(S32*)pParam);
				GLOBAL_TRACE(("Language Set = %d!!!!!!!!!!!!\n", lNewLanIndex));
				plConfig->m_FrpLanguage = lNewLanIndex;
				if (plConfig->m_FrpLanguage == 0)
				{
					FRP_MenuBuffSet(FRP_MENU_INFO_ID_ROOT2, plInfo->m_pLCDENG);
				}
				else
				{
					FRP_MenuBuffSet(FRP_MENU_INFO_ID_ROOT2, plInfo->m_pLCDCHN);
				}
				MULTL_SaveConfigurationXML(plHandle);
				MULTL_SaveParamterToStorage(plHandle);
			}
			break;;
		default:
			break;
		}
	}
	return lRet;
}




/* Private Variables (static)-------------------------------------------------- */
/* Private Function prototypes ------------------------------------------------ */
/* Functions ------------------------------------------------------------------ */
//BufSize�����������ĳ���


void FRP_AgentInitiate(MULT_Handle *pHandle)
{

#if 0
	//FRP DEVICEģ�����
	{
		HANDLE32 lTmpHandle;

		lTmpHandle = FRP_DeviceCreate(1);
		FRP_DeviceSetLED(lTmpHandle, FRP_LED_BACK, FRP_LED_BACK_OFF);
		FRP_DeviceSetLED(lTmpHandle, FRP_LED_STATUS, FRP_LED_GREEN);
		FRP_DeviceSetLED(lTmpHandle, FRP_LED_ALARM, FRP_LED_GREEN);
		FRP_DeviceSetLCDText(lTmpHandle, 0, "    Test Program    ");
		FRP_DeviceSetLCDText(lTmpHandle, 1, " Initiate Complete  ");
		FRP_DeviceUpdateAll(lTmpHandle);
		FRP_DeviceDestroy(lTmpHandle);

	}
#endif



#if 0
	//PFC ����ͨ��ģ������
	{
		BOOL lRet = FALSE;
		HANDLE lCom1;
		U8 lCommand[15];

		GLOBAL_TRACE(("Initiate Test COM1\n"));

		lCom1 = PL_ComDeviceOpen(1, TRUE);

		lRet = PL_ComDeviceSetState(lCom1, 19200, 8, 'N', 2);//��Ҫ��У��λ���ǲ�ʹ�ã�

		while(1)
		{
			GLOBAL_MEMSET(lCommand, 0xFF, 15);
			lCommand[0] = 0x4E;
			lCommand[1] = 0xA0;
			lCommand[2] = 0x40;
			lCommand[3] = 0x81;
			lCommand[4] = 0x82;
			lCommand[14] = CRYPTO_CRCXOR(&lCommand[1], 13, 0);
			lRet = PL_ComDeviceWrite(lCom1, lCommand, 15);
			if (lRet != 15)
			{
				GLOBAL_TRACE(("Send Error!\n"));
			}
			else
			{
				CAL_PrintDataBlock("Send Data", lCommand, 15);
			}

			lRet = PL_ComDeviceRead(lCom1, lCommand, 15);
			if (lRet > 0)
			{
				if (lCommand[0] == 0x4E && lCommand[2] == 0x04)
				{
					GLOBAL_TRACE(("Exit!!!  \n"));
					break;
				}
				else
				{
					CAL_PrintDataBlock("Recv Data", lCommand, lRet);
				}
			}
			else
			{
				GLOBAL_TRACE(("No Data!!!  \n"));
			}

		}

		PL_ComDeviceClose(lCom1);

		lCom1 = NULL;

	}

#endif
	MULT_Config *plConfig;
	MULT_Information *plInfo;
	CHAR_T plStrBuf[64];
	S32 lOffst;

	plConfig = &pHandle->m_Configuration;
	plInfo = &pHandle->m_Information;


#ifndef GA2620B
	/**����ģ���ʼ��.�����ȵ���.*/
	FRP_MenuInitiate(1, NULL);	
#endif

	FRP_MenuSetLED(FRP_LED_BACK, FRP_LED_BACK_ON);
	/*��������*/
	FRP_MenuLangSet(plConfig->m_FrpLanguage);

	/**�ص���������.,,�û����޸�Ϊ�Զ��庯��.*/
	FRP_MenuCallBackSet(FRP_AgentCB, pHandle);

	/*�˵���Ϣ���*/
	lOffst = CAL_StringCenterOffset(plInfo->m_pModelName, 21);
	if ((lOffst < (21 - GLOBAL_STRLEN(plInfo->m_pModelName))) && (lOffst >= 0))
	{
		GLOBAL_MEMSET(plStrBuf, ' ', sizeof(plStrBuf));
		GLOBAL_STRCPY(&plStrBuf[lOffst], plInfo->m_pModelName);
	}
	
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_ROOT1, plStrBuf);//��Ҫ���в���
	if (plConfig->m_FrpLanguage == 0)
	{
		FRP_MenuBuffSet(FRP_MENU_INFO_ID_ROOT2, plInfo->m_pLCDENG);
	}
	else
	{
		FRP_MenuBuffSet(FRP_MENU_INFO_ID_ROOT2, plInfo->m_pLCDCHN);
	}
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_IP, PFC_SocketNToA(plConfig->m_ManageIPv4Addr));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_MASK, PFC_SocketNToA(plConfig->m_ManageIPv4Mask));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_GATE, PFC_SocketNToA(plConfig->m_ManageIPv4Gate));
	CAL_StringBinToMAC(plConfig->m_pMAC, sizeof(plConfig->m_pMAC), plStrBuf, sizeof(plStrBuf));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_MAC, plStrBuf);
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_SN, plInfo->m_pSNString);
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_SOFTV, plInfo->m_pSoftVersion);
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_HARDV, plInfo->m_pHardVersion);
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_SOFT_DATE, plInfo->m_pSoftRelease);
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_FPGA_DATE, plInfo->m_pFPGARelease);

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_TEMP, "Temp Abnormal", "�¶��쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_FPGA, "Data Module Error", "���ݴ���ģ���쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_PLL, "PLL Lock Lost", "���໷ʧ��");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_CHANNEL_IN, "Input Error", "�������ʴ���");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_CHANNEL_OUT, "Output Error", "������ʴ���");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_NTP, "NTP Failure", "ʱ��ͬ��ʧ��");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SCS_EMM, "EMM Error", "EMM ����");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SCS_ECM, "ECM Error", "ECM ����");
	
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_BUFFER_STATUS, "QAM Overflow", "����ͨ��01���");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_TUNER_SHORT_STATUS, "Tuner Short", "Tuner 01 ��·");


	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_GPS_LOCK_LOST, "SAT Lock Lost", "SAT ʧ��");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_GPS_ERROR, "SAT MODULE ERROR", "SAT ģ���쳣");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_EXT_10M_LOST, "EXT 10M Lost", "�ⲿ10M ��ʧ");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_EXT_1PPS_LOST, "EXT 1PPS Lost", "�ⲿ1PPS��ʧ");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_INT_10M_LOST, "INT 10M Lost", "�ڲ�10M ��ʧ");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_INT_1PPS_LOST, "INT 1PPS Lost", "�ڲ�1PPS��ʧ");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SFN_SIP_ERROR, "SFN SIP ERROR", "SFN SIP �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SFN_SIP_CRC32_ERROR, "SIP CRC ERR", "SIP CRC �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SFN_ERROR, "SFN ERR", "SFN �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_SFN_SIP_CHANGE, "SIP CHANGED", "SIP �仯");
#ifdef SUPPORT_NTS_DPD_BOARD
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_DPD_PARAMETER_SAVE_ERROR, "DPD Error", "����Ԥʧ���쳣");
#endif
#ifdef SUPPORT_CLK_ADJ_MODULE
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_CLK_SYNC_ERROR, "CLK SYNC Error", "ʱ��ͬ���쳣");
#endif

#ifdef GM8358Q
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN, "CVBS LOCK ERROR", "CVBS �쳣");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO, "INPUT VIDEO1 ERROR", "������Ƶ �쳣");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_OUTPUT_ERROR_NOBITRATE, "OUTPUT NO BITRATE", "");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_OUTPUT_ERROR_OVERFLOW, "OUTPUT OVERFLOW", "������");
#endif

#ifdef GM8398Q
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN1, "CVBS1 LOCK ERROR", "CVBS1 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN2, "CVBS2 LOCK ERROR", "CVBS2 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN3, "CVBS3 LOCK ERROR", "CVBS3 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN4, "CVBS4 LOCK ERROR", "CVBS4 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN4, "CVBS5 LOCK ERROR", "CVBS5 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN5, "CVBS6 LOCK ERROR", "CVBS6 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN7, "CVBS7 LOCK ERROR", "CVBS7 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_CVBS_LOCK_CHN8, "CVBS8 LOCK ERROR", "CVBS8 �쳣");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO1, "INPUT VIDEO1 ERROR", "������Ƶ1 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO2, "INPUT VIDEO2 ERROR", "������Ƶ2 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO3, "INPUT VIDEO3 ERROR", "������Ƶ3 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO4, "INPUT VIDEO4 ERROR", "������Ƶ4 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO5, "INPUT VIDEO5 ERROR", "������Ƶ5 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO6, "INPUT VIDEO6 ERROR", "������Ƶ6 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO7, "INPUT VIDEO7 ERROR", "������Ƶ7 �쳣");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_INPUT_ERROR_VIDEO8, "INPUT VIDEO8 ERROR", "������Ƶ8 �쳣");

	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_OUTPUT_ERROR_NOBITRATE, "OUTPUT NO BITRATE", "");
	FRP_MenuAlarmConfig(MULT_MONITOR_TYPE_ENCODER_OUTPUT_ERROR_OVERFLOW, "OUTPUT OVERFLOW", "������");
#endif

	GLOBAL_SPRINTF((plStrBuf, "LC%.2X-%.2X-%.2X", pHandle->m_Information.m_LicenseValid, pHandle->m_Information.m_LicenseMode + 1, pHandle->m_Information.m_LicenseInASINum));
	FRP_MenuInfoConfig(0, plStrBuf, plStrBuf);
	GLOBAL_SPRINTF((plStrBuf, "%s %s", __DATE__, __TIME__));
	FRP_MenuInfoConfig(1, plStrBuf, plStrBuf);
	GLOBAL_SPRINTF((plStrBuf, "Auto %d", g_auto_reboot_times));
	FRP_MenuInfoConfig(2, plStrBuf, plStrBuf);
	GLOBAL_SPRINTF((plStrBuf, "%s", PFC_GetRelease()));
	FRP_MenuInfoConfig(3, plStrBuf, plStrBuf);

#ifdef MULT_SUPPORT_FPGA_ETH
	{
		TUN_InitParam *plTunParam = MULT_FPGAEthGetParameterPtr();
		if (plConfig->m_FrpLanguage == 0)
		{
			GLOBAL_SPRINTF((plStrBuf, "S IP: %s", PFC_SocketNToA(plTunParam->m_TUNIPAddr)));
		}
		else
		{
			GLOBAL_SPRINTF((plStrBuf, "S IP: %s", PFC_SocketNToA(plTunParam->m_TUNIPAddr)));
		}
		FRP_MenuInfoConfig(4, plStrBuf, plStrBuf);
	}
#endif

	FRP_MenuUseInfoNode();
	

	FRP_MenuSetFreeze(FALSE);

}

/*��ȡ������Ϣ�����ֵ�ǰ�����*/
void FRP_AgentAccess(MULT_Handle *pHandle, S32 Duration)
{
	S32 i, lCount, lLogNum;
	CAL_LogConfig lLogConfig;
	MULT_Monitor *plMonitor;

	plMonitor = &pHandle->m_Monitor;

	lLogNum = CAL_LogGetLogCFGCount(plMonitor->m_LogHandle);
	for (i = 0; i < lLogNum; i++)
	{
		CAL_LogProcConfig(plMonitor->m_LogHandle, i, &lLogConfig, TRUE);
		if (lLogConfig.m_bFP)
		{
			lCount = CAL_LogGetLogInfoCount(plMonitor->m_LogHandle, i);
			if (lCount > 0)
			{
				if (i == MULT_MONITOR_TYPE_CHANNEL_IN)
				{
					FRP_MenuAlarmSetCount(i, TRUE, lCount);
				}
				else
				{
					FRP_MenuAlarmSetCount(i, FALSE, lCount);
				}
			}
		}
	}
	
}


void FRP_AgentSetManagePortRelatedData(MULT_Handle *pHandle)
{
	MULT_Config *plConfig;
	CHAR_T plStrBuf[64];

	plConfig = &pHandle->m_Configuration;

	FRP_MenuBuffSet(FRP_MENU_INFO_ID_IP, PFC_SocketNToA(plConfig->m_ManageIPv4Addr));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_MASK, PFC_SocketNToA(plConfig->m_ManageIPv4Mask));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_GATE, PFC_SocketNToA(plConfig->m_ManageIPv4Gate));
	CAL_StringBinToMAC(plConfig->m_pMAC, sizeof(plConfig->m_pMAC), plStrBuf, sizeof(plStrBuf));
	FRP_MenuBuffSet(FRP_MENU_INFO_ID_MAC, plStrBuf);
}



void FRP_AgentTerminate(void)
{
	FRP_MenuTerminate();
}

#endif


/*EOF*/
