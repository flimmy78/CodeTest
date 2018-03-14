
/* Standard Include Files */
#include <errno.h>
#include "rtpsession.h" 
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
/* Verification Test Environment Include Files */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>




extern "C" {
	#include "v4l2.h"
	struct framebuffer *v4l_get_frame(int fd_v4l);
 	int v4l_capture_start(int fd);
	int v4l_capture_init(char *dev);
	
}



 
struct framebuffer *buffer;

char *dpath= "/dev/video0";
const int SCREEN_WIDTH = 640;  
const int SCREEN_HEIGHT = 480;  
const int SCREEN_BPP = 32;   
using namespace jrtplib;
using namespace std;
// ��������
void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}
void delay(unsigned long time){
	int i = 0;
	for(i=0;i<100*time;i++);
}
struct pic_data
{
	int index;
	int offset;
	int length;
	char buf[1024];
}pic_data;


int main(int argc, char **argv)
{
	int fd_v4l;
	RTPSession sess;
    	unsigned long destip;
    	int destport;
    	int portbase = 6000;
    	int status, index,ret,i,j,Bn;

    	//char buffer_char[10];

    	if (argc < 3) 
    	{
      		printf("Usage: ./sender dest_ip port\n");
      		printf("Please input like this:./sender 127.0.0.1 8080 /dev/video0\n");
      		return -1;
    	}

    // ��ý��ն˵�IP��ַ�Ͷ˿ں�
	destip = ntohl(inet_addr(argv[1]));
    if (destip == INADDR_NONE) {
      printf("Bad IP address specified\n");
      return -1;
    }
 	printf("ip=%s\n",argv[1]);
    destport = 5000;
//	destport=5000;
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(1.0/10.0);		
	sessparams.SetAcceptOwnPackets(true);
	
	
	RTPIPv4Address addr(destip,destport);
	
	transparams.SetPortbase(atoi(argv[2]));
    // ����RTP�Ự
    
 	status = sess.Create(sessparams,&transparams);	
	checkerror(status);
	
    // ָ��RTP���ݽ��ն�
  
	status = sess.AddDestination(addr);
		
    		
    	checkerror(status);

    index = 1;
	 if(argc>3)
			fd_v4l =v4l_capture_init(argv[3]);
	else
		fd_v4l = v4l_capture_init(dpath);
	printf("is open the camera\n");
	v4l_capture_start(fd_v4l);
	printf("now starting capture\n");
    while(1)
    {
		
		buffer=v4l_get_frame(fd_v4l);
	
		memset(&pic_data,0,sizeof(pic_data));
		pic_data.index=index;
		pic_data.length=buffer->length;
		pic_data.offset=0;
		Bn = 1024;
	
    		printf("Sending the %dst pic that Bn = %d,length = %d\n",index,Bn,buffer->length);

 
   		for(i=0;i<=buffer->length;i+=Bn)
    		{
    			j = buffer->length- i;
		
    			if(i == 0)
    			{	
			
				memcpy(pic_data.buf,(void *)(&buffer->start[pic_data.offset]),1024);
				sess.SendPacket(&pic_data,sizeof(pic_data),(unsigned char)0,false,100UL);
				pic_data.offset+=1024;
				continue;
    			}
    			if(j > Bn)
    			{
				memcpy(pic_data.buf,(void *)(&buffer->start[pic_data.offset]),1024);
		
				sess.SendPacket(&pic_data,sizeof(pic_data),(unsigned char)0,false,100UL);
    				pic_data.offset+=1024;
    				printf("%d packet send!\n",i/Bn);
    	
    			}
    			else 
    			{
				memcpy(pic_data.buf,(void *)(&buffer->start[pic_data.offset]),j);
    				sess.SendPacket(&pic_data,sizeof(pic_data),(unsigned char)0,false,100UL);
    				printf("%d packet send!\n",buffer->length/Bn);
				printf("i=%d,j=%d\n",pic_data.offset,j);
    		
    			}    
		
    		}
    		//checkerror(status);
   		printf("Packets of %d pic have been sent!\n",index);
    		index ++;
    

    };
    
    close(fd_v4l);
    return 0;
    	/*
        SDL_Surface *message = NULL;  
    	SDL_Surface *screen = NULL;  
    	SDL_Rect offset;
    	/** 
     	* ��ʼ��SDL���������������SDL�ĵ��������ʼ�����л��� 
     	* ��ʼ�����л����ĺô��ǿ�ʼʱ���еĻ����������ʼ���ã�����Ͳ��õ�����Ӧ�Ļ����Ƿ񱻳�ʼ�� 
     	* ��ȱ������ЩSDL�Ĺ���������ò��ţ������������ʼ����������Դ�˷� 
     	* ������ѧϰ������Ϊ�˷��㣬��ʼ�����л��� 
     	 
    	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )  
    	{  
        	return 1;  
   		} 
   		/**���ó��򴰿����ԣ��ú�������ݴ���Ĳ����������ڣ�����һ��screen 
     	* screen���Կ��ɺʹ��ڶ�Ӧ��һ���ڴ����򣬳���������ڴ�������ճ��ͼƬ�����任 
     	* ����ٽ�������������������Ļ��ȥ�������͵õ�����Ϸ�е�һ�����棬��һ֡ 
     	* SCREEN_WIDTH     ���ڿ� 
     	* SCREEN_HEIGHT    ���ڸ� 
     	* SCREEN_BPP       �洢һ�������õ�λ������������ƽʱ������ʾ��ʱ��"��ɫ����"��������32λ 
     	* SDL_SWSURFACE    �ò������������Ƿ�ȫ�����������ݴ洢λ��֮��Ķ�������ϸ��Ϣ��SDL�ĵ�����������Ϊ��ȫ���Ҵ������ݴ洢���ڴ��� 
     	  
    	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );  
    	if( screen == NULL )  
    	{  
        	return 1;  
    	}          
    	//���ô��ڱ����ͼ�꣬������ʱ������ͼ��  
    	SDL_WM_SetCaption( "JPEG_SDLVIDEO", NULL ); 
        
        fd_v4l = v4l_capture_setup();
        for(i=0;i<10;i++)
        {        
        	ret = v4l_capture_test(fd_v4l, "1.jpg");
			if(ret < 0)printf("capture error! \n");
		}
			
			
			/*
    		//����Ҫ��ʾ��ͼƬ��������Ի�������Ҫ��ʾ���κ�ͼƬ
    		message = IMG_Load( "1_rev.jpg" );   
    		//��Ҫ��ʾ��ͼƬճ����screen�ϵ����Ͻ�ȥ
    		//ԴͼƬ��Ҫճ����λ�ã����λ��ָĿ�������ϵ�λ��  
    		//SDL������ԭ�������Ͻǣ�����ΪX����������ΪY������     
    		//�ڶ���������ʾ��ԴͼƬ���Ĳ���ճ����ȥ�����ΪNULL����ʾȫ��  
    		//�������ճ�������ڵڶ���������ָ��ȡԴͼƬ���Ĳ���   
    		offset.x = 0;  
    		offset.y = 0; 
    		SDL_BlitSurface(message, NULL, screen, &offset);  
  
    		//������screen���������в����������ڴ��н��У������ǽ�screen�ڴ��е�����������Ļ������  
    		if( SDL_Flip( screen ) == -1 )return 1;  
    		SDL_Delay(500);   
    	}       
    	//�ͷż��ص�ͼƬ  
    	SDL_FreeSurface( message );        
    	//�ͷŴ���  
    	SDL_FreeSurface( screen );        
    	//�˳�SDL����  
    	SDL_Quit(); 
    	  	   		
		close(fd_v4l);
		return 0;*/
}


