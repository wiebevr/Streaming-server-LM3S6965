#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMap>
#include <QString>

#include "ui_main_window.h"
#include "video_viewer.h"
#include "video_player.h"
#include "video_streamer.h"
#include "playlist_model.h"

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

    public slots:
//        void playlistChanged();
    
    protected:
        void timerEvent(QTimerEvent *event);

    private slots:
        void handleToggleButton();
        void handleStopButton();
        void handleFileButton();
        void handleRemoveButtonClicked();
        void handlePlay(QString path);
        void playNextButton();
        void playPrevButton();
        void handleCameraButton(bool state);

        void actionAboutClicked();
        
        void handleToggleCommand();
        void handleStopCommand();
        void handleRemoveCommand(QString name);
        void playNextCommand();
        void playPrevCommand();
        void handleCameraCommand();

        void stateChanged();
        void videoFinished();
        void handlePlaylistViewDoubleClick(QModelIndex modelIndex);
        void handleNewVideo();

    private:
        Ui::MainWindow ui;

        VideoPlayer *_videoPlayer;
        VideoViewer *_inputVideoWidget;
        VideoViewer *_outputVideoWidget;
        VideoStreamer *_videoStreamer;

        PlayerState _playerState;
        PlaylistModel *_playlistModel;

        int _itemPlaying;
};
#endif //MAIN_WINDOW_H
