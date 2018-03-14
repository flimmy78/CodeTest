#include <stdio.h>
#include <string.h>

#include "rtpsession.h"
#include "rtppacket.h"

// ��������
void checkerror(int err)
{
  if (err < 0) {
    char* errstr = RTPGetErrorString(err);
    printf("Error:%s\\n", errstr);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  RTPSession sess;
  int localport;
  int status;
  int timestamp,lengh = 1025,length = 0;
  char buffer[32768];
  unsigned char *RawData;
  
  if (argc != 2) {
    printf("Usage: ./recieve localport\\n");
    return -1;
  }
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(timestampunit);		
	
	sessparams.SetAcceptOwnPackets(true);
	if(portbase!=0)
		transparams.SetPortbase(atoi(argv[2]));
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);
   // ����û�ָ���Ķ˿ں�
 
  // ����RTP�Ự
  checkerror(status);
  FILE *file = 0; 	
  int i = 0;
  int j = 1;
        

  do {
    // ����RTP����
    status = sess.PollData();
 	// ����RTP����Դ

    if (sess.GotoFirstSourceWithData())
    {

      do {
        RTPPacket* packet;
        // ��ȡRTP���ݱ�
        
        while ((packet = sess.GetNextPacket()) != NULL) {
        	
           	printf("Got packet %d! of pic %d!\n",i,j);
	   	   	timestamp = packet->GetTimeStamp();
           	lengh=packet->GetPayloadLength();
           	length += lengh;
           	RawData=packet->GetPayload();   //�õ�ʵ����Ч����
           	printf(" timestamp:%d;lengh=%d\n",timestamp,lengh);
           	memcpy(&buffer[1024*i],RawData,lengh); 
           	i ++;   		 
            delete packet;
          	// ɾ��RTP���ݱ�
        }
      } while (sess.GotoNextSourceWithData());
    }    
    if(lengh < 1024)
    {
        if((file = fopen("rtp_rev.jpg", "wb")) < 0)
    	{
           	printf("Unable to create y frame recording file\n");
            return -1;
    	}
    	else
    	{
    		fwrite(buffer, length, 1, file);
    		printf("buffer write %d bytes into file.\n",length);
    		fclose(file);
    		i = 0;//write over so reset i
    		j ++ ;//write pic and j++
    		lengh = 1025;
    	}
    }  
  } while(1);

  return 0;
}
