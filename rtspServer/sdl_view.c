
/* Standard Include Files */
#include <errno.h>
    
/* Verification Test Environment Include Files */
#include <sys/types.h>	
#include <sys/stat.h>	
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>    
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_audio.h"
#include "SDL/SDL_timer.h"
#include "SDL/SDL_image.h"
static int SCREEN_WIDTH = 640;  
static int SCREEN_HEIGHT = 480;  
static  int SCREEN_BPP = 32;   

SDL_Surface *screen = NULL;  
SDL_Overlay *poverlay=NULL;
int fd_v4l,ret,i;
SDL_Rect offset;
void set_screen(int width,int height,int bpp)
{
	SCREEN_WIDTH=width;
	SCREEN_HEIGHT=height;
	SCREEN_BPP=bpp;
}

int draw_init( void)
{
        
    	/** 
     	* ��ʼ��SDL���������������SDL�ĵ��������ʼ�����л��� 
     	* ��ʼ�����л����ĺô��ǿ�ʼʱ���еĻ����������ʼ���ã�����Ͳ��õ�����Ӧ�Ļ����Ƿ񱻳�ʼ�� 
     	* ��ȱ������ЩSDL�Ĺ���������ò��ţ������������ʼ����������Դ�˷� 
     	* ������ѧϰ������Ϊ�˷��㣬��ʼ�����л��� 
     	*/  
    	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )  
    	{  
    		printf("SDL_init fail\n");
		perror("init fail\n");
        	return 1;  
   		} 
   		/**���ó��򴰿����ԣ��ú�������ݴ���Ĳ����������ڣ�����һ��screen 
     	* screen���Կ��ɺʹ��ڶ�Ӧ��һ���ڴ����򣬳���������ڴ�������ճ��ͼƬ�����任 
     	* ����ٽ�������������������Ļ��ȥ�������͵õ�����Ϸ�е�һ�����棬��һ֡ 
     	* SCREEN_WIDTH     ���ڿ� 
     	* SCREEN_HEIGHT    ���ڸ� 
     	* SCREEN_BPP       �洢һ�������õ�λ������������ƽʱ������ʾ��ʱ��"��ɫ����"��������32λ 
     	* SDL_SWSURFACE    �ò������������Ƿ�ȫ�����������ݴ洢λ��֮��Ķ�������ϸ��Ϣ��SDL�ĵ�����������Ϊ��ȫ���Ҵ������ݴ洢���ڴ��� 
     	*/  
    	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );  
    	if( screen == NULL )  
    	{  
        	return 1;  
    	}          
	poverlay = SDL_CreateYUVOverlay(SCREEN_WIDTH,SCREEN_HEIGHT,SDL_YV12_OVERLAY,screen);  
    	//���ô��ڱ����ͼ�꣬������ʱ������ͼ��  
    	SDL_WM_SetCaption( "JPEG_SDLVIDEO", NULL ); 


}

void draw(char *img_path,int index)
{
                SDL_Surface *img = NULL;  
    		//����Ҫ��ʾ��ͼƬ��������Ի�������Ҫ��ʾ���κ�ͼƬ
    		img = IMG_Load(img_path);   
             
    		//��Ҫ��ʾ��ͼƬճ����screen�ϵ����Ͻ�ȥ
    		//ԴͼƬ��Ҫճ����λ�ã����λ��ָĿ�������ϵ�λ��  
    		//SDL������ԭ�������Ͻǣ�����ΪX����������ΪY������     
    		//�ڶ���������ʾ��ԴͼƬ���Ĳ���ճ����ȥ�����ΪNULL����ʾȫ��  
    		//�������ճ�������ڵڶ���������ָ��ȡԴͼƬ���Ĳ���  
    		// SDL_LockSurface(screen);
    		switch (index){
                case 1:
                             offset.x = 0;  
    		                offset.y = 0; 
    		                SDL_BlitSurface(img, NULL, screen, &offset); 
                             break;

                case 2:
                         offset.x = 320;  
    		             offset.y = 0; 
    		             SDL_BlitSurface(img, NULL, screen, &offset);
                           break;

                case 3:
                    offset.x = 0;  
    		        offset.y = 240; 
    		        SDL_BlitSurface(img, NULL, screen, &offset); 
                          break;

                case 4:
                     offset.x = 320;  
    		         offset.y = 240; 
    		        SDL_BlitSurface(img, NULL, screen, &offset); 
                          break;


            }
    		//������screen���������в����������ڴ��н��У������ǽ�screen�ڴ��е�����������Ļ������  
    		
    	         
		 //  SDL_UpdateRect(screen, 0, 0, 0, 0);
               // SDL_UnlockSurface(screen);
    		//SDL_UpdateRects(screen,1,&offset);
   		if( SDL_Flip( screen ) == -1 )
    	        {
    			printf("SDL_Flip error\n");
			exit(0);
                }
    		//SDL_Delay(500);   
    		SDL_FreeSurface( img );   
}      
void draw_destroy()
{
		//�ͷż��ص�ͼƬ  
    	     
    	//�ͷŴ���  
    	SDL_FreeSurface( screen );        
    	//�˳�SDL����  
    	SDL_Quit();   	   		
		close(fd_v4l);
		
}

int flip( unsigned char * src, int width, int height)   
{   
    unsigned char  *outy,*outu,*outv,*out_buffer,*op[3];  
	int y;
    SDL_Rect rect;  
    rect.w = width;  
    rect.h = height;  
    rect.x = 0;  
    rect.y = 0;  
    out_buffer = src;  
    SDL_LockSurface(screen);  
    SDL_LockYUVOverlay(poverlay);  
    outy = out_buffer;  
    outu = out_buffer+width*height*5/4;  
    outv = out_buffer+width*height;  
    for(y=0;y<screen->h && y<poverlay->h;y++)  
    {  
        op[0]=poverlay->pixels[0]+poverlay->pitches[0]*y;  
        op[1]=poverlay->pixels[1]+poverlay->pitches[1]*(y/2);  
        op[2]=poverlay->pixels[2]+poverlay->pitches[2]*(y/2);     
        memcpy(op[0],outy+y*width,width);  
        if(y%2 == 0)  
        {  
            memcpy(op[1],outu+width/2*y/2,width/2);  
            memcpy(op[2],outv+width/2*y/2,width/2);     
        }  
    }  
    SDL_UnlockYUVOverlay(poverlay);  
    SDL_UnlockSurface(screen);         
    SDL_DisplayYUVOverlay(poverlay, &rect);  
    return 0;  
} 
