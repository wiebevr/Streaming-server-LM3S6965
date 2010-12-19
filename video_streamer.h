#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QStandardItemModel>
#include <highgui.h>

#include "playlist_model.h"

class VideoStreamer
    : public QObject
{
    Q_OBJECT

    public:
        VideoStreamer(PlaylistModel *playlistModel, QObject *parent = 0);
        ~VideoStreamer();

    signals:
        void toggle();
        void play(QString name);
        void stop();
        void next();
        void prev();
        void remove(QString name);

    public slots:
        void sendFrame(IplImage *frame); 
        void setControlPort(int port);
        void setDataPort(int port);
        void controlResponse(bool success);

    private slots:
        void newDataConnection();
        void newControlConnection();
        void readNewData();

    private:
        QTcpServer *_controlServer;
        QTcpServer *_dataServer;

        QTcpSocket *_controlSocket;
        QTcpSocket *_dataSocket;

        int _controlPort;
        int _dataPort;

        PlaylistModel *_playlistModel;

};
#endif // VIDEO_STREAMER_H
