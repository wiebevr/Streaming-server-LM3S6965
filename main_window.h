#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QStandardItemModel>

#include "ui_main_window.h"
#include "video_viewer.h"
#include "video_player.h"
#include "video_streamer.h"

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

    enum PlayerState
    {
        PLAYING,
        STOPPED,
        PAUSED
    };

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
    
    protected:
        void timerEvent(QTimerEvent *event);

    private slots:
        void handleToggleButton();
        void handleStopButton();
        void handleFileButton();
        void stateChanged();
        void videoFinished();
        void handlePlaylistViewDoubleClick(QModelIndex modelIndex);
        void handleRemoveButtonClicked();
        void playNext();

    private:
        Ui::MainWindow ui;

        VideoPlayer *_videoPlayer;
        VideoViewer *_inputVideoWidget;
        VideoViewer *_outputVideoWidget;
        VideoStreamer *_videoStreamer;

        PlayerState _playerState;
        QStandardItemModel *_playlistModel;
        int _itemPlaying;
};
#endif //MAIN_WINDOW_H
