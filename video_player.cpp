#include "video_player.h"
#include <QDebug>

VideoPlayer::VideoPlayer(QString videoFile, QObject *parent)
    : QObject(parent)
{
    _stream = cvCreateFileCapture(videoFile.toAscii());
    _destFrame = cvCreateImage(cvSize(128, 96), IPL_DEPTH_8U, 1);

    _frameRateTimer = new QTimer(this);
    connect(_frameRateTimer, SIGNAL(timeout()), this, SLOT(newFrame()));

    double frameRate = cvGetCaptureProperty(_stream, CV_CAP_PROP_FPS);
    _frameRateTimer->start(qRound((1.0 / frameRate) * 1000.0));
}

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent)
{
    _stream = NULL;
    _frameRateTimer = new QTimer(this);

    _destFrame = cvCreateImage(cvSize(128, 96), IPL_DEPTH_8U, 1);
    connect(_frameRateTimer, SIGNAL(timeout()), this, SLOT(newFrame()));
    qDebug() << "constr";
}

VideoPlayer::~VideoPlayer()
{
    cvReleaseCapture(&_stream);
}

void VideoPlayer::newFrame()
{
    _sourceFrame = cvQueryFrame(_stream);
    if (!_sourceFrame)
    {
        stop();
        emit done();
        qDebug() << "Done";
        return;
    }
    emit updateSourceFrame(_sourceFrame);

    // Processing
    IplImage *tmp = cvCreateImage(cvSize(128, 96), 
            _sourceFrame->depth, _sourceFrame->nChannels);

    cvResize(_sourceFrame, tmp, CV_INTER_LINEAR);
    cvConvertImage(tmp, _destFrame);
    emit updateDestFrame(_destFrame);
    cvReleaseImage(&tmp);
}

void VideoPlayer::pause()
{
    _frameRateTimer->stop();
}

void VideoPlayer::resume()
{
    double frameRate = cvGetCaptureProperty(_stream, CV_CAP_PROP_FPS);
    _frameRateTimer->start(qRound((1.0 / frameRate) * 1000.0));
}

void VideoPlayer::play(QString fileName)
{
    if (_stream)
        cvReleaseCapture(&_stream);
    _stream = cvCreateFileCapture(fileName.toAscii());
    emit newVideo();
    resume();
}

void VideoPlayer::stop()
{
    if (_stream)
        cvReleaseCapture(&_stream);
    _frameRateTimer->stop();
}

void VideoPlayer::setPosition(int position)
{
    if (_stream)
        cvSetCaptureProperty(_stream, CV_CAP_PROP_POS_FRAMES, (double)position);
}

int VideoPlayer::getPosition()
{
    if (!_stream)
        return 0;
    else
        return (int)cvGetCaptureProperty(_stream, CV_CAP_PROP_POS_FRAMES);
}

int VideoPlayer::getFrameCount()
{
    if (!_stream)
        return 0;
    else
        return (int)cvGetCaptureProperty(_stream, CV_CAP_PROP_FRAME_COUNT);
}
