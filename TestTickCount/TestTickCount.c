#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>		//va_start va_end va_arg va_list
#include <string.h>		//strlen strcpy ....
#include <malloc.h>		//malloc calloc relloc free
#include <memory.h>		//memset memcmp memcpy
#include <setjmp.h>		//JpegLib �����������ɴ�������׼C֧�֡�
#include <time.h>		//ʱ��ʱ������ time
#include <unistd.h>		//close()
#include <errno.h>
#include <fcntl.h>		//socket block set (fcntl)
#include <pthread.h>	//pthread
#include <signal.h>		//linux �ź�
#include <semaphore.h>
#include <sys/socket.h>	//socket
#include <sys/types.h>	
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <linux/if.h>
#include <linux/if_tun.h>// for tun/tap
#include <net/if_arp.h> //*for ARPHRD_ETHER*/

unsigned int PL_GetTickCount(void)
{
	int lRetCode = 0;
	unsigned int lTickCount = 0;
	struct timespec ts;
	lRetCode = clock_gettime(CLOCK_REALTIME, &ts );/*��������ʵʱʱ�ӿ� -lrt ����Ч����������Ч�����������ø߾��ȶ�ʱ������ɣ�*/
	if (lRetCode != 0)
	{
		printf("Error = %d, Msg = %s\n", errno, strerror(errno));
	}
	lTickCount = ts.tv_sec * 1000 + ts.tv_nsec / 1000 / 1000;
	return lTickCount;
}

int main(int argc, char argv[])
{
	int lTimeOut = 3000; /* ms */
	unsigned int lPreTickCount = 0;

	while (1)
	{
		lPreTickCount = PL_GetTickCount();
		while (PL_GetTickCount() - lPreTickCount < lTimeOut)
		{
			
		}
		printf ("Test Time Out: %u\n", lTimeOut);
	}
}
