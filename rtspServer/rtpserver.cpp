#include "rtpserver.h"

rtpServer::rtpServer(QObject *parent) :
    rtpClass(parent)
{
    //sess=new RTPSession;

}

int rtpServer::prosess_pack(int timestamp,int len,char*buf,int index)
{
    static int tmp=0;
    static int cnt=0;
    static int length;
    FILE *file;

    QString img_name;
    img_name=QString("%1.jpg").arg(index+1);


    picData=(struct pic_data *)buf;
    qDebug("offset=%d,size=%d",picData->offset,picData->length);
    memcpy(&buffer[picData->offset]+819200*index,picData->buf,1024);
    if(picData->length-picData->offset<1024)
    {
        //ֻ�е�һ֡���ݽ����������ʱ��д����Ļ


        if((file = fopen(img_name.toUtf8().data(), "wb")) < 0)
        {
            qDebug("Unable to create y frame recording file\n");
            exit(-1);
        }

        qDebug("receive %d,lentgth=%d,index=%d\n",picData->offset,picData->length,index);
        //printf("buffer write %d bytes into file.\n",length);
        fwrite(buffer+index*819200, picData->length, 1, file);
        fclose(file);
        //exit(0);
        QImage img;
        if(img.load(img_name))
        {
            emit display(QImageToIplImage(&img),1);
        }
        //draw(img_name,index+1);

    }
    return 0;

}
void rtpServer::run()
{
    unsigned int list[10],i=0,index,SSRC;

    do {
        // ����RTP����
        Poll();
        BeginDataAccess();
        // ����RTP����Դ
        if (GotoFirstSourceWithData())
        {


            do {
                RTPPacket* packet;
                // ��ȡRTP���ݱ�
                RTPSourceData *ssrc=GetCurrentSourceInfo();

                SSRC=ssrc->GetSSRC();



                if((packet = GetNextPacket()) != NULL) {
                    qDebug()<<packet->GetPayloadType();
                  //  prosess_pack(packet->GetTimestamp(),packet->GetPayloadLength(),(char *)packet->GetPayloadData(),i);
                    qDebug()<<packet->GetPacketData();

                    DeletePacket(packet);
                    // ɾ��RTP���ݱ�

                }
            } while (GotoNextSourceWithData());


        }


        EndDataAccess();


    } while(!stopped);
    qDebug()<<"recv thread is exit";
    return;
}
rtpServer::~rtpServer()
{
    Destroy();

}
void rtpServer::rtp_recv_init(int port)
{

    int status;
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    sessparams.SetOwnTimestampUnit(1.0/10.0);
    sessparams.SetAcceptOwnPackets(true);

    transparams.SetPortbase(port);

    status = Create(sessparams,&transparams);
    checkerror(status,"Create server");



}
void rtpServer::OnNewSource(RTPSourceData *dat)
{
    qDebug()<<dat->GetRTCPDataAddress()->IPv4Address;
}
void rtpServer::OnBYEPacket(RTPSourceData *dat)
{

}
void rtpServer::OnRemoveSource(RTPSourceData *dat)
{

}
void rtpServer::OnRTCPCompoundPacket(RTCPCompoundPacket *pack,
                          const RTPTime &receivetime,
                          const RTPAddress *senderaddress)
{
    qDebug()<<"get tcp";
    RTCPPacket *rtcppack;
    pack->GotoFirstPacket();
    while ((rtcppack = pack->GetNextPacket()) != 0)
    {
    if (rtcppack->IsKnownFormat())
    {
    switch (rtcppack->GetPacketType())
    {
    case RTCPPacket::SR:
    {
    RTCPSRPacket *p = (RTCPSRPacket *)rtcppack;

    uint32_t senderssrc = p->GetSenderSSRC();

    std::cout << "    SR Info:" << std::endl;
    std::cout << "        NTP timestamp:    " << p->GetNTPTimestamp().GetMSW() << ":" << p->GetNTPTimestamp().GetLSW() << std::endl;
    std::cout << "        RTP timestamp:    " << p->GetRTPTimestamp() << std::endl;
    std::cout << "        Packet count:     " <<  p->GetSenderPacketCount() << std::endl;
    std::cout << "        Octet count:      " << p->GetSenderOctetCount() << std::endl;
    //	std::cout << "        Receive time:     " << p->GetReceiveTime().GetSeconds() << std::endl;

    }
    break;


    }
    }
    }
}

