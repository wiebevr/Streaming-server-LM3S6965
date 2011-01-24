#include "video_streamer.h"
#include <QRegExp>
#include <QModelIndex>

VideoStreamer::VideoStreamer(PlaylistModel *playlistModel, QObject *parent)
    : QObject(parent)
{
    _controlSocket = NULL;
    _dataSocket = NULL;

    _controlServer = new QTcpServer(this);   
    _dataServer = new QTcpServer(this);   

    _playlistModel = playlistModel;

    _controlPort = 58001;
    _dataPort = 58002;

    _controlServer->listen(QHostAddress::Any, _controlPort);
    _dataServer->listen(QHostAddress::Any, _dataPort);

    connect(_controlServer, SIGNAL(newConnection()), 
            this, SLOT(newControlConnection()));
    connect(_dataServer, SIGNAL(newConnection()),
            this, SLOT(newDataConnection()));
    
}

VideoStreamer::~VideoStreamer()
{
}

void VideoStreamer::readNewData()
{
    QString recvData = _controlSocket->readLine();
    if (recvData.size() == 0)
    {
        qDebug() << "readyRead() emmitted but no data read.";
        return;
    }

    if (recvData.startsWith("toggle"))
    {
        emit toggle();
    }
    else if (recvData.startsWith("keep_alive"))
    {
        _controlSocket->write("OK\n");
    }
    else if (recvData.startsWith("stop"))
    {
        emit stop();
    }
    else if (recvData.startsWith("next"))
    {
        emit next();
    }
    else if (recvData.startsWith("prev"))
    {
        emit prev();
    }
    else if (recvData.startsWith("camera"))
    {
        emit camera();
    }
    else if (recvData.startsWith("play"))
    {
        QString path = _playlistModel->nameForPath(
                recvData.section(QRegExp("\\s+"), 1, 1));
        if (path.isEmpty())
        {
            _controlSocket->write("NOK\n");
        }
        else
        {
            emit play(path);
            _controlSocket->write("OK\n");
        }
    }
    else if (recvData.startsWith("remove"))
    {
        emit remove(recvData.section(QRegExp("\\s+"), 1, 1));
    }
    else if (recvData.startsWith("getplaylist"))
    {
        for (int i = 0; i < _playlistModel->getNames().count(); ++i)
        {
            _controlSocket->write(_playlistModel->getNames()[i].toAscii());
            _controlSocket->write("\n");
        }
        _controlSocket->write("OK\n");
    }
    else
    {
        _controlSocket->write("NOK\n");
    }
    qDebug() << "Recieved:" << recvData;
    
}

void VideoStreamer::newDataConnection()
{
    _dataSocket = _dataServer->nextPendingConnection();
}

void VideoStreamer::newControlConnection()
{
    _controlSocket = _controlServer->nextPendingConnection();
    connect(_controlSocket, SIGNAL(readyRead()),
            this, SLOT(readNewData()));
}

void VideoStreamer::controlResponse(bool success)
{
    if (_controlSocket && _controlSocket->isOpen())
    {
        _controlSocket->write( success ? "OK\n" : "NOK\n");
    }
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

void VideoStreamer::setControlPort(int port)
{
    _controlServer->close();
    _controlPort = port;
    _controlServer->listen(QHostAddress::Any, _controlPort);
}

void VideoStreamer::setDataPort(int port)
{
    _dataServer->close();
    _dataPort = port;
    _dataServer->listen(QHostAddress::Any, _dataPort);
}
