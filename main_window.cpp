#include "main_window.h"
#include <QHBoxLayout>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    _playerState = STOPPED;
    
    _inputVideoWidget = new VideoViewer(this);
    _outputVideoWidget = new VideoViewer(this);
    _videoStreamer = new VideoStreamer(this);
    _videoPlayer = new VideoPlayer(this);
    _playlistModel = new QStandardItemModel(this);

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
    connect(_videoPlayer, SIGNAL(newVideo()),
            _inputVideoWidget, SLOT(updateAspectRatio()));
    connect(_videoPlayer, SIGNAL(newVideo()),
            _outputVideoWidget, SLOT(updateAspectRatio()));
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
        ui.playlistView->setCurrentIndex(
                _playlistModel->item(_itemPlaying, 0)->index());
        ui.progressSlider->setMaximum(_videoPlayer->getFrameCount());
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

void MainWindow::playNext()
{
#if 0
    if (_itemPlayin < _playlistModel->rowCount() -1) 
        _videoPlayer->play(
                _playlistModel->item(itemPlaying, 1)
#endif
}

void MainWindow::handlePlaylistViewDoubleClick(QModelIndex modelIndex)
{
    _videoPlayer->play(_playlistModel->item(modelIndex.row(), 1)->text());
    _itemPlaying = modelIndex.row();
    _playerState = PLAYING;
    stateChanged();
    ui.progressSlider->setMaximum(_videoPlayer->getFrameCount());
}

void MainWindow::handleRemoveButtonClicked()
{
    _playlistModel->removeRow(ui.playlistView->currentIndex().row());
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
    _videoPlayer->stop();
    _playerState = STOPPED;
    stateChanged();
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
            ui.progressSlider->setMaximum(_videoPlayer->getFrameCount());
            _playerState = PLAYING;
            break;
    }    
    stateChanged();
}

void MainWindow::timerEvent(QTimerEvent *)
{
    ui.progressSlider->blockSignals(true);
    ui.progressSlider->setValue(_videoPlayer->getPosition());
    ui.progressSlider->blockSignals(false);
}