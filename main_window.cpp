#include "main_window.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    _playerState = STOPPED;
    _itemPlaying = -1;
    _playlistModel = new PlaylistModel(this);
    
    _inputVideoWidget = new VideoViewer(this);
    _outputVideoWidget = new VideoViewer(this);
    _videoPlayer = new VideoPlayer(this);
    _videoStreamer = new VideoStreamer(_playlistModel, this);

    ui.mediaPlayWidget->setLayout(new QHBoxLayout());
    ui.mediaPlayWidget->layout()->addWidget(_inputVideoWidget);
    ui.mediaPlayWidget->layout()->addWidget(_outputVideoWidget);

    ui.playlistView->setModel(_playlistModel);
    ui.playlistView->setEditTriggers(QAbstractItemView::SelectedClicked | 
            QAbstractItemView::AnyKeyPressed);

    connect(_videoPlayer, SIGNAL(updateSourceFrame(IplImage *)), 
            _inputVideoWidget, SLOT(updateTexture(IplImage *)));
    connect(_videoPlayer, SIGNAL(updateDestFrame(IplImage *)), 
            _outputVideoWidget, SLOT(updateTexture(IplImage *)));
    connect(_videoPlayer, SIGNAL(updateDestFrame(IplImage *)),
            _videoStreamer, SLOT(sendFrame(IplImage *)));

    connect(ui.progressSlider, SIGNAL(valueChanged(int)),
            _videoPlayer, SLOT(setPosition(int)));
    connect(ui.toggleButton, SIGNAL(clicked()),
            this, SLOT(handleToggleButton()));
    connect(ui.stopButton, SIGNAL(clicked()),
            this, SLOT(handleStopButton()));
    connect(ui.addFileButton, SIGNAL(clicked()),
            this, SLOT(handleFileButton()));
    connect(_videoPlayer, SIGNAL(done()),
            this, SLOT(videoFinished()));
    connect(ui.playlistView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(handlePlaylistViewDoubleClick(QModelIndex)));
    connect(ui.removeFileButton, SIGNAL(clicked()),
            this, SLOT(handleRemoveButtonClicked()));
    connect(ui.nextFileButton, SIGNAL(clicked()),
            this, SLOT(playNextButton()));
    connect(ui.prevFileButton, SIGNAL(clicked()),
            this, SLOT(playPrevButton()));
    connect(ui.actionAbout, SIGNAL(triggered()),
            this, SLOT(actionAboutClicked()));
    connect(ui.cameraButton, SIGNAL(clicked(bool)),
            this, SLOT(handleCameraButton(bool)));

    connect(_videoStreamer, SIGNAL(toggle()),
            this, SLOT(handleToggleCommand()));
    connect(_videoStreamer, SIGNAL(stop()),
            this, SLOT(handleStopCommand()));
    connect(_videoStreamer, SIGNAL(play(QString)),
            this, SLOT(handlePlay(QString))); 
    connect(_videoStreamer, SIGNAL(next()),
            this, SLOT(playNextCommand()));
    connect(_videoStreamer, SIGNAL(prev()),
            this, SLOT(playPrevCommand()));
    connect(_videoStreamer, SIGNAL(remove(QString)),
            this, SLOT(handleRemoveCommand(QString)));
    connect(_videoStreamer, SIGNAL(camera()),
            this, SLOT(handleCameraCommand()));

    connect(ui.controlPortBox, SIGNAL(valueChanged(int)),
            _videoStreamer, SLOT(setControlPort(int)));
    connect(ui.dataPortBox, SIGNAL(valueChanged(int)),
            _videoStreamer, SLOT(setDataPort(int)));

    stateChanged();
    startTimer(500);
}

MainWindow::~MainWindow()
{
}

void MainWindow::videoFinished()
{
    if (_itemPlaying < _playlistModel->rowCount() - 1) 
    {
        _itemPlaying++;
        _videoPlayer->play(
                _playlistModel->item(_itemPlaying, 1)->text());
        handleNewVideo();
    }
    else
    {
        _playerState = STOPPED;
        stateChanged();
    }
}

void MainWindow::stateChanged()
{
    switch(_playerState)
    {
        case PLAYING:
            ui.toggleButton->setText("Pause");
            break;
        case PAUSED:
            ui.toggleButton->setText("Resume");
            break;
        case STOPPED:
            ui.toggleButton->setText("Play File");
            break;
    }
}

void MainWindow::handleNewVideo()
{
    ui.playlistView->setCurrentIndex(
            _playlistModel->item(_itemPlaying, 0)->index());
    ui.progressSlider->setMaximum(_videoPlayer->getFrameCount());

}

void MainWindow::playNextButton()
{
    if (_playlistModel->rowCount() - 1 > _itemPlaying && _itemPlaying >= 0)
    {
        _videoPlayer->play(_playlistModel->getPathForId(++_itemPlaying));
        _playerState = PLAYING;
        stateChanged();
        handleNewVideo();
    }
}

void MainWindow::playNextCommand()
{
    if (_playlistModel->rowCount() - 1 > _itemPlaying && _itemPlaying >= 0)
    {
        _videoPlayer->play(_playlistModel->getPathForId(++_itemPlaying));
        _playerState = PLAYING;
        stateChanged();
        handleNewVideo();
        _videoStreamer->controlResponse(true);
    }
    else
    {
        _videoStreamer->controlResponse(false);
    }
}

void MainWindow::playPrevButton()
{
    if (_playlistModel->rowCount() > _itemPlaying && _itemPlaying > 0)
    {
        _videoPlayer->play(_playlistModel->getPathForId(--_itemPlaying));
        _playerState = PLAYING;
        stateChanged();
        handleNewVideo();
    }
}

void MainWindow::playPrevCommand()
{
    if (_playlistModel->rowCount() > _itemPlaying && _itemPlaying > 0)
    {
        _videoPlayer->play(_playlistModel->getPathForId(--_itemPlaying));
        _playerState = PLAYING;
        stateChanged();
        handleNewVideo();
        _videoStreamer->controlResponse(true);
    }
    else
    {
        _videoStreamer->controlResponse(false);
    }
}

void MainWindow::handlePlaylistViewDoubleClick(QModelIndex modelIndex)
{
    _videoPlayer->play(_playlistModel->item(modelIndex.row(), 1)->text());
    _itemPlaying = modelIndex.row();
    _playerState = PLAYING;
    stateChanged();
    handleNewVideo();
}

void MainWindow::handleRemoveButtonClicked()
{
    _playlistModel->removeRow(ui.playlistView->currentIndex().row());
}

void MainWindow::handleRemoveCommand(QString name)
{
    int id = _playlistModel->getIdForName(name);
    if (_itemPlaying == id)
    {
        handleStopButton();
    }
    if (_itemPlaying > id)
    {
        --_itemPlaying;
    }
    _videoStreamer->controlResponse(_playlistModel->removeByName(name));
}

void MainWindow::handleFileButton()
{
    QString path = QFileDialog::getOpenFileName(this);  
    QList<QStandardItem *> row;

    QRegExp nameRegEx("([^/]*)$");
    nameRegEx.indexIn(path);

    row.append(new QStandardItem(nameRegEx.cap()));
    row.append(new QStandardItem(path));
    row[0]->setToolTip(path);

    _playlistModel->appendRow(row);
}

void MainWindow::handleStopButton()
{
    if (_playerState == PAUSED || _playerState == PLAYING)
    {
        _videoPlayer->stop();
        _playerState = STOPPED;
        stateChanged();
    }
}

void MainWindow::handleStopCommand()
{
    if (_playerState == PAUSED || _playerState == PLAYING)
    {
        _videoPlayer->stop();
        _playerState = STOPPED;
        stateChanged();
        _videoStreamer->controlResponse(true);
    }
    else
    {
        _videoStreamer->controlResponse(false);
    }
}

void MainWindow::handleToggleButton()
{
    qDebug() << "toggleButton";
    switch (_playerState)
    {
        case PLAYING:
            _videoPlayer->pause();
            _playerState = PAUSED;
            break;
        case PAUSED:
            _videoPlayer->resume();
            _playerState = PLAYING;
            break;
        case STOPPED:
            _videoPlayer->play("test.mkv");
            handleNewVideo();
            _playerState = PLAYING;
            break;
    }    
    stateChanged();
}

void MainWindow::handleToggleCommand()
{
    qDebug() << "toggleButton";
    switch (_playerState)
    {
        case PLAYING:
            _videoPlayer->pause();
            _playerState = PAUSED;
            _videoStreamer->controlResponse(true);
            stateChanged();
            break;
        case PAUSED:
            _videoPlayer->resume();
            _playerState = PLAYING;
            _videoStreamer->controlResponse(true);
            stateChanged();
            break;
        default:
            _videoStreamer->controlResponse(false);
    }    
}

void MainWindow::handlePlay(QString path)
{
    _itemPlaying = _playlistModel->getPaths().indexOf(path);
    _videoPlayer->play(path);
    _playerState = PLAYING;
    stateChanged();
    handleNewVideo();
}

void MainWindow::timerEvent(QTimerEvent *)
{
    ui.progressSlider->blockSignals(true);
    ui.progressSlider->setValue(_videoPlayer->getPosition());
    ui.progressSlider->blockSignals(false);

    QTime current = QTime(0, 0);
    current = current.addMSecs((int) _videoPlayer->getCurrentTime());
    QTime total = QTime(0, 0);
    total = total.addMSecs((int) _videoPlayer->getTotalTime());
    if (_videoPlayer->getTotalTime() > 0)
        ui.progressLabel->setText(QString("%1 / %2").
                arg(current.toString("hh:mm:ss")).
                arg(total.toString("hh:mm:ss")));
    else
        ui.progressLabel->setText(current.toString("hh:mm:ss"));
}

void MainWindow::actionAboutClicked()
{
    QMessageBox::about(this, QString("About Video Streamer"), QString(
                "<h1>Video Streamer</h1>"
                "<p>This application is designed to first covert the video to 4 bit grey scale, with a resolution of 128x96. "
                "And then stream it to the LM3S6965 Ethernet Evaluation Board that will render the video on the screen.</p>"
                "<p>It uses OpenCL to convert the video, and Qt to draw the interface. The video in the application "
                "is rendered using OpenGL.</p>"
                "<p>This software is part of a project for an Embedded Software course at the De Nayer institute.</p>"
                "<p>Created by Wiebe Van Ranst - wiebevr@gmail.com</p>")); 
}

void MainWindow::handleCameraButton(bool)
{
    _videoPlayer->playCamera(-1);
    _playerState = PLAYING;
    stateChanged();
}

void MainWindow::handleCameraCommand()
{
    handleCameraButton(true);
    _videoStreamer->controlResponse(true);

}
