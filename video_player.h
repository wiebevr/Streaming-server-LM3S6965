#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H
#include <QObject>
#include <QTimer>
#include <highgui.h>
#include <cxcore.h>
#include <cv.h>


class VideoPlayer
    : public QObject
{
    Q_OBJECT

    public:
        VideoPlayer(QString videoFile, QObject *parent = 0);
        VideoPlayer(QObject *parent = 0);
        ~VideoPlayer();
        int getFrameCount();
        int getPosition();
        double getCurrentTime();
        double getTotalTime();

    public slots:
        void setPosition(int position);
        void pause();
        void resume();
        void play(QString fileName);
        void stop();

    protected slots:
        void newFrame();

    signals:
        void updateSourceFrame(IplImage *frame);
        void updateDestFrame(IplImage *frame);
        void done();
        void newVideo();

    private:
        CvCapture *_stream;
        IplImage *_sourceFrame;
        IplImage *_destFrame;
        
        QTimer *_frameRateTimer;


};
#endif // VIDEO_PLAYER_H
