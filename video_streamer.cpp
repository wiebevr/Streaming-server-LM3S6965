#include "video_streamer.h"
#define CONTROL_SERVER_PORT 58001
#define DATA_SERVER_PORT 58002

VideoStreamer::VideoStreamer(QObject *parent)
    : QObject(parent)
{
    _controlSocket = NULL;
    _dataSocket = NULL;

    _controlServer = new QTcpServer(this);   
    _dataServer = new QTcpServer(this);   

    _controlServer->listen(QHostAddress::Any, CONTROL_SERVER_PORT);
    _dataServer->listen(QHostAddress::Any, DATA_SERVER_PORT);

    connect(_controlServer, SIGNAL(newConnection()), 
            this, SLOT(newControlConnection()));
    connect(_dataServer, SIGNAL(newConnection()),
            this, SLOT(newDataConnection()));
}

VideoStreamer::~VideoStreamer()
{
}

void VideoStreamer::newDataConnection()
{
    _dataSocket = _dataServer->nextPendingConnection();
}

void VideoStreamer::newControlConnection()
{

}

void VideoStreamer::sendFrame(IplImage *frame)
{
    if (!_dataSocket)
        return;
    char buffer[64*96];
    char *cvInput = (char *)frame->imageData;
    for (int i = 0; i < 96 * 128; i+=2)
    {
            buffer[i / 2] = cvInput[i] & 0xf0;
            buffer[i / 2] |= (cvInput[ i + 1] & 0xf0) >> 4;
    }
    qDebug() << _dataSocket->write(buffer, 64 * 96);
}

