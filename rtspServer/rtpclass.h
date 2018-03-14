#ifndef RTPCLASS_H
#define RTPCLASS_H

#include <QThread>
#include <QUdpSocket>
#include <errno.h>
#include <QFile>
#include <QTime>

#include "rtpsession.h"
#include "rtpsourcedata.h"
#include "rtpudpv4transmitter.h"
#include "rtcpcompoundpacket.h"
#include "rtcpsrpacket.h"
#include "rtpipv4address.h"
#include "rtppacket.h"

#include "rtpsessionparams.h"
#include <stdio.h>
#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "h264class.h"
#include <QDebug>
#include <QPixmap>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib")//���ؼ�;
using namespace std;
using namespace jrtplib;
typedef struct pic_data
{
    int index;
    int offset;
    int length;
    unsigned char payloadType;
    unsigned char *buf;
}pic_data;

Q_DECLARE_METATYPE(pic_data)


class rtpClass : public QThread,public RTPSession
{
    Q_OBJECT
public:
    rtpClass(QObject *parent = 0);
    ~rtpClass();
     void startThread();
     void stopThread();
    void checkerror(int rtperr,char *error);
  void delay(unsigned long time);
    void initInstance();
    IplImage* QImageToIplImage(const QImage * qImage);

protected:

    bool stopped;

signals:

public slots:

};

#endif // RTPCLASS_H
