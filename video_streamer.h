#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <highgui.h>

class VideoStreamer
    : public QObject
{
    Q_OBJECT

    public:
        VideoStreamer(QObject *parent = 0);
        ~VideoStreamer();

    public slots:
        void sendFrame(IplImage *frame); 

    private slots:
        void newDataConnection();
        void newControlConnection();

    private:
        QTcpServer *_controlServer;
        QTcpServer *_dataServer;

        QTcpSocket *_controlSocket;
        QTcpSocket *_dataSocket;


};
#endif // VIDEO_STREAMER_H
