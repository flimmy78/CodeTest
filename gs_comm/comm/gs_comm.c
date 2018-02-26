#include "gs_comm.h"
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

GS_DEBUG_SET_LEVEL(GS_DBG_LVL_DBG);

/* ==============================��׼��ӡ�������============================ */

/*
����:	��ʽ����ӡ����
����:	�䳤����
����ֵ:	�� 
˵��:	�ú���ʹ��stderr��Ϊ�������������printf�Ļ�����Ӱ��
*/
GS_VOID GS_COMM_Printf(const GS_CHAR *format, ...) 
{
	va_list argList;

	fflush(stdout);

	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
}

/*
����:	�����ӡ����
����:	ErrnoFlag	--	����Ŷ�Ӧ�ַ�����ӡ��־��0������ӡ 1����ӡ
		Error		--	�����
		Fmt			--	��ӡ��ʽ
		Ap			--	�ɱ�����б�
����ֵ:	�� 
˵��:	��stderr�����Ҫ��ӡ�Ĵ�����Ϣ
*/
static GS_VOID GS_COMM_ErrDoit(GS_S32 ErrnoFlag, GS_S32 Error, const GS_CHAR *Fmt, va_list Ap)
{
#define MAXLINE 500
	GS_CHAR buf[MAXLINE];

	vsnprintf(buf, MAXLINE, Fmt, Ap);
	if (ErrnoFlag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf), ": %s", strerror(Error));
	strcat(buf, "\n");

	fflush(stdout); /* �����׼����ͱ�׼����ʹ��ͬ���ĺ��� */
	fputs(buf, stderr);
	fflush(NULL); /* flush��������� */
}

/*
����:	��ӡ������
����:	ErrNo	--	����ţ���ʽ��������
		Fmt		--	��ӡ��ʽ�ַ���
		...		--	�ɱ�����б�
����ֵ:	�� 
˵��:	����ϵͳ���õ����������ӡ����ӡ���ݲ��쳣�˳�����ӡ�Ĵ�������ⲿ��ʽ����
*/
GS_VOID GS_COMM_ErrExit(GS_S32 ErrNo, const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(1, ErrNo, Fmt, ap);
	va_end(ap);
	exit(1);
}

/*
����:	��ӡ������
����:	Fmt			--	��ӡ��ʽ
		...			--	�ɱ�����б�
����ֵ:	�� 
˵��:	����ϵͳ���õķ����������ӡ����ӡ���ݲ�����
*/
GS_VOID GS_COMM_ErrRet(const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(1, errno, Fmt, ap); /* ��ӡ�������ݣ�ֻ��errno��������Ĳ��ܴ�ӡ��ȷ�� */
	va_end(ap);
}

/*
����:	��ӡ������
����:	Fmt			--	��ӡ��ʽ
		...			--	�ɱ�����б�
����ֵ:	�� 
˵��:	����ϵͳ���õ����������ӡ�������ں���Ϣת������ֹ
*/
GS_VOID GS_COMM_ErrDump(const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(1, errno, Fmt, ap); /* ��ӡ�������ݣ�ֻ��errno��������Ĳ��ܴ�ӡ��ȷ�� */
	va_end(ap);
	abort(); /* dump core and terminate */
	exit(1); /* shouldn't get here */
}

/*
����:	��ӡ������
����:	Fmt			--	��ӡ��ʽ
		...			--	�ɱ�����б�
����ֵ:	�� 
˵��:	���ڷ�ϵͳ���õĴ����ӡ����ӡ�󷵻�
*/
GS_VOID GS_COMM_ErrMsg(const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(0, 0, Fmt, ap); /* ��ϵͳ���ô�����Ϣ������errno����������ֻ�Ǵ�ӡ������Ϣ */
	va_end(ap);
}

/*
����:	��ӡ������
����:	Fmt			--	��ӡ��ʽ
		...			--	�ɱ�����б�
����ֵ:	�� 
˵��:	���ڷ�ϵͳ���õ����������ӡ����ӡ���쳣�˳�
*/
GS_VOID GS_COMM_ErrQuit(const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(0, 0, Fmt, ap); /* ��ϵͳ���ô�����Ϣ������errno����������ֻ�Ǵ�ӡ������Ϣ */
	va_end(ap);
	exit(1);
}

/*
����:	��ӡ������
����:	Fmt			--	��ӡ��ʽ
		...			--	�ɱ�����б�
����ֵ:	�� 
˵��:	����ϵͳ���õ����������ӡ����ӡ���ݲ��쳣�˳�
*/
GS_VOID GS_COMM_ErrSys(const GS_CHAR *Fmt, ...)
{
	va_list ap;

	va_start(ap, Fmt);
	GS_COMM_ErrDoit(1, errno, Fmt, ap);
	va_end(ap);
	exit(1);
}

/* ==============================��׼��ӡ�����������============================ */

/*
����:	ϵͳshell����
����:	�䳤����
����ֵ:	ͬsystem�ķ���ֵ
˵��:	system��ʹ��sh -c����pCmdָ��������
*/
GS_S32 GS_COMM_System(const GS_CHAR *pCmd, ...)
{
	GS_CHAR plString[20*1024];

	va_list lArgument;
	va_start(lArgument, pCmd);
	vsprintf(plString, (GS_CHAR*)pCmd, lArgument);
	va_end(lArgument);

	return system(plString);
}

/*
����:	��ʽ��ӡ���ݿ�
����:	pTitle	-- ��ӡ��˵��
		pBuf	-- ��ӡ�ľ�����������
		Size	-- pBuf�Ĵ�С
����ֵ:	��
˵��:	��ӡ��ʽ����
		[test data]----------------------128
		00000000h: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ; ................
		00000010h: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f ; ................
		00000020h: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f ;  !"#$%&'()*+,-./
		00000030h: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f ; 0123456789:;<=>?
		00000040h: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f ; @ABCDEFGHIJKLMNO
		00000050h: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f ; PQRSTUVWXYZ[\]^_
		00000060h: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f ; `abcdefghijklmno
		00000070h: 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f ; pqrstuvwxyz{|}~.
		-----------------------
*/
GS_VOID GS_COMM_PrintDataBlock(GS_CHAR* pTitle, GS_U8 *pBuf, GS_S32 Size)
{
#define LINE_LEN 16
	GS_S32 i = 0, j = 0;

	GS_COMM_Printf("\n[%s]----------------------%d\n", pTitle, Size);
	for (i=0; i<Size; i++)
	{
		if (i == 0)
			GS_COMM_Printf("%08xh: ", i);
		else if (i % LINE_LEN == 0)
		{
			GS_COMM_Printf("; ");	
			for (j=LINE_LEN; j>0; j--)
			{
				if (isascii(pBuf[i - j]) && !iscntrl(pBuf[i - j]))
					GS_COMM_Printf("%c", pBuf[i - j]);
				else
					GS_COMM_Printf(".");
			}
			GS_COMM_Printf("\r\n%08xh: ", i);
		}
		GS_COMM_Printf("%02x ", pBuf[i]);
	}
	if (i != 0)
	{
		GS_S32 rem = (i - 1) % LINE_LEN + 1;

		for (j=0; j<(LINE_LEN - rem); j++)
		{
			GS_COMM_Printf("   ");	
		}
		GS_COMM_Printf("; ");	
		for (j=rem; j>0; j--)
		{
			if (isascii(pBuf[i - j]) && !iscntrl(pBuf[i - j]))
				GS_COMM_Printf("%c", pBuf[i - j]);
			else
				GS_COMM_Printf(".");
		}
	}

	GS_COMM_Printf("\r\n-----------------------\n\n");
}

/*
����:	��ȡtickֵ
����:	��
����ֵ:	linux��Ԫ��ʼ���Tickֵ����λms
˵��:	
*/
GS_U64 GS_COMM_GetTickCount(GS_VOID)
{
	struct timespec ts;
	GS_S32 lRet = 0;

	if ((lRet = clock_gettime(CLOCK_REALTIME, &ts)) != 0)
		GS_COMM_ErrExit(lRet, "clock_gettime error");

	return ((GS_U64)ts.tv_sec * 1000 + (GS_U64)ts.tv_nsec / 1000 / 1000);
}

/*
����:	��ȡ�����ַ���ASCII��ֵ
����:	��
����ֵ:	��ȡ��ASCIIֵ
˵��:	�ú����޳��˻��еȸ����ַ�
*/
GS_S32 GS_COMM_GetChar(GS_VOID)
{
	GS_S32 lRetVal = 0;
	GS_S32 lChar = 0;

	lRetVal = getchar();

	if ((lRetVal != '\r') && (lRetVal != '\n') && (lRetVal != EOF))
	{
		while ((lChar != '\r') && (lChar != '\n') && (lChar != EOF))
			lChar = getchar();
	}

	return lRetVal;
}

#define CRC32_MAX_COEFFICIENTS	256
static const GS_U32 sc_Crc32Table[CRC32_MAX_COEFFICIENTS]=
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

/*
����:	���� CRC32 ֵ
����:	pData	-- ��Ҫ���� CRC ��������ݵ�ַ
		Size	-- ��Ҫ���� CRC ��������ݳ���
����ֵ:	������� CRC32 ֵ
˵��:	
*/
GS_U32 GS_COMM_Crc32Calc(GS_U8 *pData, GS_U32 Size)
{
	GS_U32 lCrc32Val = 0xFFFFFFFF;	
	GS_U32 i;

	for (i=0; i<Size; i++)	
	{
		lCrc32Val = (lCrc32Val << 8 ) ^ sc_Crc32Table[((lCrc32Val >> 24) ^ *(pData++)) & 0xFF]; 
	}

	return lCrc32Val;
}

/* ==============================�û� Buffer ����============================ */

typedef struct {
	GS_S32	m_BufSize;
	GS_S32	m_UseIndex;

	GS_U8	*m_pBuf;
} GS_COMM_USER_BUF_S;
/*
����:	�����û� Buffer
����:	Len	-- ��Ҫ�������û� Buffer �Ĵ�С
����ֵ:	�ɹ�	--	���ش����õ� Buffer �ṹ��ַ��
		ʧ��	--	���� NULL
˵��:	
*/
GS_HANDLE GS_COMM_UserBufCreate(GS_S32 Len)
{
	GS_COMM_USER_BUF_S *plHandle = (GS_COMM_USER_BUF_S *)malloc(sizeof(GS_COMM_USER_BUF_S));

	if (!plHandle) {
		GS_COMM_ErrRet("malloc error!");
		return NULL;
	}

	plHandle->m_BufSize = Len;
	plHandle->m_UseIndex = 0;
	plHandle->m_pBuf = (GS_U8 *)malloc(Len);
	if (!plHandle->m_pBuf) {
		GS_COMM_ErrRet("malloc error!");
		free (plHandle);
		return NULL;
	}

	return plHandle;
}

/*
����:	�����û� Buffer
����:	Handle	-- ��Ҫ���ٵ��û� Buffer ����
����ֵ:	��
˵��:	
*/
GS_VOID GS_COMM_UserBufDestroy(GS_HANDLE Handle)
{	
	GS_COMM_USER_BUF_S *plHandle = (GS_COMM_USER_BUF_S *)Handle;

	if (plHandle) {
		if (plHandle->m_pBuf) {
			free (plHandle->m_pBuf);
			plHandle->m_pBuf = NULL;
		}

		free(plHandle);
		plHandle = NULL;
	}
}

/*
����:	��ȡ�û� Buffer
����:	Handle	--	�û� Buffer ����
		Len		--	��ȡ Buffer ����
����ֵ:	�����û� Buffer �ĵ�ַ
˵��:	
*/
GS_U8 *GS_COMM_UserBufMalloc(GS_HANDLE Handle, GS_S32 Len)
{
	GS_U8 *plRet = NULL;
	GS_COMM_USER_BUF_S *plHandle = (GS_COMM_USER_BUF_S *)Handle;

	if (plHandle) {
		if (plHandle->m_BufSize - plHandle->m_UseIndex < Len) {
			GS_COMM_ErrMsg("GS_COMM_UserBufMalloc Error!");
			return NULL;
		}

		plRet = &plHandle->m_pBuf[plHandle->m_UseIndex];
		plHandle->m_UseIndex += Len;
	}

	return plRet;
}

/* ==============================�û� Buffer ��������============================ */

/* ��Ϣ���в��� */

typedef struct {
	GS_S32 m_MsgqId; /* ��Ϣ���б�ʶ�� */
}GS_MSGQ_S;

/*
����:	������Ϣ����
����:	MaxQueueSize	--	��Ϣ������󳤶�
����ֵ:	GS_HANDLE���ͣ���ʾ������Ϣ���еľ����NULL��ʾ����ʧ��
˵��:	�ú����´���һ����Ϣ���У�����Ϣ����ʹ����ɺ����ʹ��GS_COMM_MsgQueueDestroyж��
*/
GS_HANDLE GS_COMM_MsgQueueCreate(GS_S32 MaxQueueSize)
{
	GS_S32 lMsgqId;
	struct msqid_ds lBuf;
	GS_MSGQ_S *plMsgqHandle = (GS_MSGQ_S *) malloc (sizeof(GS_MSGQ_S));

	if (plMsgqHandle == NULL)
		GS_COMM_ErrSys("malloc error");

	if ((lMsgqId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		free(plMsgqHandle);
		GS_COMM_ErrSys("msgget error");
	}

	lBuf.msg_perm.uid = getuid();
	lBuf.msg_perm.gid = getgid();
	lBuf.msg_perm.cuid = getuid();
	lBuf.msg_perm.cgid = getgid();
	lBuf.msg_perm.mode = 0666;
	lBuf.msg_stime = 0;
	lBuf.msg_rtime = 0;
	lBuf.msg_ctime = time(NULL);
	lBuf.msg_qnum = 0;
	lBuf.msg_qbytes = MaxQueueSize; /* ��Ϣ������󳤶� */
	lBuf.msg_lspid = 0;
	lBuf.msg_lrpid = 0;
	if (msgctl(lMsgqId, IPC_SET, &lBuf) == -1)
	{
		msgctl(lMsgqId, IPC_RMID, NULL); /* ɾ����Ϣ���� */
		free(plMsgqHandle);
		GS_COMM_ErrSys("msgctl error");
	}

	plMsgqHandle->m_MsgqId = lMsgqId;

	return plMsgqHandle;
}

/*
����:	������Ϣ����
����:	Handle	--	��Ϣ���о��
����ֵ:	0 ���ٳɹ� -1 ʧ��
˵��:	ʹ�øú���������Ϣ���о������Ҫ�������ֵ����ΪNULL
*/
GS_S32 GS_COMM_MsgQueueDestroy(GS_HANDLE Handle)
{
	GS_MSGQ_S *plMsgqHandle = (GS_MSGQ_S *)Handle;

	if (!plMsgqHandle)
		return GS_SUCCESS;

	if (msgctl(plMsgqHandle->m_MsgqId, IPC_RMID, NULL) == -1)
		GS_COMM_ErrSys("msgctl error");

	free(plMsgqHandle);
	plMsgqHandle = NULL;

	return GS_SUCCESS;
}

/*
����:	��Ϣ����
����:	Handle	--	��Ϣ���о��
		pMsg	--	��Ϣ����
		IsBlock	--	�����Ƿ�������TRUE ������FLASE ������
����ֵ:	0 �ɹ� -1 ʧ��
˵��:	��pMsg����Ϣ���ݷ��͵�Handleָ������Ϣ������,����Ϊ������ģʽ
*/
GS_S32 GS_COMM_MsgQueueSend(GS_HANDLE Handle, GS_MSGBUF_S *pMsg, GS_BOOL IsBlock)
{
	GS_MSGQ_S *plMsgqHandle = (GS_MSGQ_S *)Handle;
	GS_S32 lMsgFlag = (IsBlock ? 0 : IPC_NOWAIT);

	if (!plMsgqHandle || !pMsg)
		return GS_FAILURE;

	if (msgsnd(plMsgqHandle->m_MsgqId, pMsg, sizeof(pMsg->m_MsgText), lMsgFlag) == -1)
	{
		GS_COMM_ErrRet("msgsnd error");
		return GS_FAILURE;
	}

	return GS_SUCCESS;
}

/*
����:	��Ϣ����
����:	Handle	--	��Ϣ���о��
		MsgType	--	��Ϣ����
		IsBlock	--	�����Ƿ�������TRUE ������FLASE ������
����ֵ:	0 �ɹ�	-1 ʧ�� 
		pMsg	--	���յ�����Ϣ����
˵��:	ʹ�������ķ�ʽ������Ϣ�Ļ�ȡ����ȡʱ��Ҫ������Ϣ����
*/
GS_S32 GS_COMM_MsgQueueRecv(GS_HANDLE Handle, GS_MSGBUF_S *pMsg, GS_LONG MsgType, GS_BOOL IsBlock)
{
	GS_MSGQ_S *plMsgqHandle = (GS_MSGQ_S *)Handle;
	GS_S32 lMsgFlag = (IsBlock ? 0 : IPC_NOWAIT);
	
	if (!plMsgqHandle || !pMsg)
		return GS_FAILURE;

	if (msgrcv(plMsgqHandle->m_MsgqId, pMsg, sizeof(pMsg->m_MsgText), MsgType, lMsgFlag) == -1)
	{
		if (errno != ENOMSG)
			GS_COMM_ErrRet("msgrcv error");
		return GS_FAILURE;
	}

	return GS_SUCCESS;
}

/* ��Ϣ���в������ */

/* ���λ��������� */
/*
���� Buffer ����ƣ�
1) ��� Buffer ������������ǰҪ�洢�����ݣ�
2) ��� Buffer �ѿգ���ȡ���ݽ����ܻ�ȡ����
3) ��Ҫд������ݴ���ʣ��ռ䣬��ֻд�뵱ǰ��д���ʵ�����ݣ�
4) ��Ҫ��ȡ�����ݴ����Ѵ洢�����ݣ���ֻ��ȡʵ�ʴ洢����������
*/
typedef struct {
	GS_U8	*m_pBuffer;
	GS_S32	m_BuffSize;
	GS_S32	m_Writer; /* дλ������ */
	GS_S32	m_Reader; /* ��λ������ */

	GS_BOOL	m_IsFull; /* Buffer ����� */
} GS_COMM_RING_BUFFER_S;

GS_HANDLE GS_COMM_RingBufferCreate(GS_S32 BuffSize)
{
	GS_COMM_RING_BUFFER_S *plHandle;

	if ((plHandle = (GS_COMM_RING_BUFFER_S *)malloc(sizeof(GS_COMM_RING_BUFFER_S))) == NULL) {
		GS_COMM_ErrRet("malloc error");
		return GS_NULL;
	}

	if ((plHandle->m_pBuffer = (GS_U8 *)malloc(BuffSize)) == NULL) {
		GS_COMM_ErrRet("malloc error");
		free(plHandle);
		plHandle = NULL;
		return GS_NULL;
	}

	plHandle->m_Reader = plHandle->m_Writer = 0;
	plHandle->m_BuffSize = BuffSize;
	plHandle->m_IsFull = GS_FALSE;

	return plHandle;
}

GS_VOID GS_COMM_RingBufferDestroy(GS_HANDLE Handle)
{
	GS_COMM_RING_BUFFER_S *plHandle = (GS_COMM_RING_BUFFER_S *)Handle;

	if (plHandle) {
		free(plHandle->m_pBuffer);
		plHandle->m_pBuffer = NULL;
		free(plHandle);
		plHandle = NULL;
	}
}

GS_S32 GS_COMM_RingBufferGetUsedSize(GS_HANDLE Handle)
{
	GS_COMM_RING_BUFFER_S *plHandle = (GS_COMM_RING_BUFFER_S *)Handle;

	if (plHandle->m_IsFull)
		return plHandle->m_BuffSize;
	else
		return (plHandle->m_Writer + plHandle->m_BuffSize - plHandle->m_Reader) % plHandle->m_BuffSize;  
}

/* ����д���ʵ�����ݳ��� */
GS_S32 GS_COMM_RingBufferWrite(GS_HANDLE Handle, GS_U8 *pBuff, GS_S32 BuffSize)
{
	GS_COMM_RING_BUFFER_S *plHandle = (GS_COMM_RING_BUFFER_S *)Handle;
	GS_S32 lLen = GS_MIN(plHandle->m_BuffSize - GS_COMM_RingBufferGetUsedSize(Handle), BuffSize);
	GS_S32 lRemLen;

	if (pBuff && lLen) {
		lRemLen = (lLen - (plHandle->m_Writer + lLen) % plHandle->m_BuffSize > 0 ? lLen - (plHandle->m_Writer + lLen) % plHandle->m_BuffSize : lLen);
		memcpy(plHandle->m_pBuffer + plHandle->m_Writer, pBuff, lRemLen);
		if (lRemLen != lLen)
			memcpy(plHandle->m_pBuffer, pBuff + lRemLen, lLen - lRemLen);

		plHandle->m_Writer = (plHandle->m_Writer + lLen) % plHandle->m_BuffSize;
		if (plHandle->m_Writer == plHandle->m_Reader)
			plHandle->m_IsFull = GS_TRUE;

		return lLen;
	}

	return 0;
}

/* ���ض�ȡ��ʵ�����ݳ��� */
GS_S32 GS_COMM_RingBufferRead(GS_HANDLE Handle, GS_U8 *pBuff, GS_S32 BuffSize)
{
	GS_COMM_RING_BUFFER_S *plHandle = (GS_COMM_RING_BUFFER_S *)Handle;
	GS_S32 lLen = GS_MIN(GS_COMM_RingBufferGetUsedSize(Handle), BuffSize);
	GS_S32 lRemLen;

	if (pBuff && lLen) {
		lRemLen = (lLen - (plHandle->m_Reader + lLen) % plHandle->m_BuffSize > 0 ? lLen - (plHandle->m_Reader + lLen) % plHandle->m_BuffSize : lLen);
		memcpy(pBuff, plHandle->m_pBuffer + plHandle->m_Reader, lRemLen);
		if (lRemLen != lLen) 
			memcpy(pBuff + lRemLen, plHandle->m_pBuffer, lLen - lRemLen);

		plHandle->m_IsFull = GS_FALSE;
		plHandle->m_Reader = (plHandle->m_Reader + lLen) % plHandle->m_BuffSize;

		return lLen;
	}

	return 0;
}

GS_VOID GS_COMM_RingBufferInfoPrint(GS_HANDLE Handle)
{
	GS_COMM_RING_BUFFER_S *plHandle = (GS_COMM_RING_BUFFER_S *)Handle;
	GS_S32 i;
	
	if (plHandle) {
		GS_COMM_Printf(ANSI_COLOR_PURPLE"RingBuffer Data(%d):"ANSI_COLOR_NONE, plHandle->m_BuffSize);
		for (i = 0; i < plHandle->m_BuffSize; i++) {
			if (i == plHandle->m_Reader && i == plHandle->m_Writer)
				GS_COMM_Printf(ANSI_COLOR_RED" %02X"ANSI_COLOR_NONE, plHandle->m_pBuffer[i]);
			else if (i == plHandle->m_Writer)
				GS_COMM_Printf(ANSI_COLOR_YELLOW" %02X"ANSI_COLOR_NONE, plHandle->m_pBuffer[i]);
			else if (i == plHandle->m_Reader)
				GS_COMM_Printf(ANSI_COLOR_LIGHT_BLUE" %02X"ANSI_COLOR_NONE, plHandle->m_pBuffer[i]);
			else
				GS_COMM_Printf(" %02X", plHandle->m_pBuffer[i]);
		}
		GS_COMM_Printf("\n");
	}
}

/* ���λ������������ */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
