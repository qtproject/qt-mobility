#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QtGui>

ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ImageViewer)
{
    ui->setupUi(this);
    playlist = new QMediaPlaylist(0, this);
    model = new QMediaPlaylistModel(playlist,this);
    navigator = new QMediaPlaylistNavigator(playlist,this);

    connect(navigator, SIGNAL(activated(QMediaSource)), SLOT(display(QMediaSource)));
    connect(navigator, SIGNAL(playbackModeChanged(QMediaPlaylistNavigator::PlaybackMode)), SLOT(updatePlaybackMode(QMediaPlaylistNavigator::PlaybackMode)));
    connect(ui->listView, SIGNAL(activated(QModelIndex)), SLOT(jump(QModelIndex)));

    navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);

    ui->listView->setModel(model);

}

ImageViewer::~ImageViewer()
{
    delete ui;
}

void ImageViewer::addImage()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Add Image", "/usr/share/doc/packages/libqt4/src/images", "*.png");
    if (1 || !fileNames.isEmpty()) {
        int firstPos = playlist->size();
        foreach(const QString &fileName, fileNames) {
            playlist->append(QMediaSource("image/png", fileName));
        }

        navigator->jump(firstPos);
    }
}

void ImageViewer::advance()
{
    navigator->advance();
}

void ImageViewer::back()
{
    navigator->back();
}

void ImageViewer::shuffle()
{
    playlist->shuffle();
}

void ImageViewer::display(const QMediaSource &src)
{
    if (src.isNull()) {
        ui->listView->clearSelection();
        ui->label->clear();
    } else {
        ui->listView->setCurrentIndex(model->index(navigator->currentPosition()));
        ui->label->setPixmap(src.dataLocation().toString());
    }
}

void ImageViewer::jump(const QModelIndex &index)
{
    navigator->jump(index.row());
}

void ImageViewer::setPlaybackMode(int mode)
{
    navigator->setPlaybackMode(QMediaPlaylistNavigator::PlaybackMode(mode));
}

void ImageViewer::updatePlaybackMode(QMediaPlaylistNavigator::PlaybackMode mode)
{
    ui->playbackMode->setCurrentIndex(int(mode));    
}

void ImageViewer::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load playlist", ".", "*.m3u");
    if (!fileName.isEmpty()) {
        if (!playlist->load(fileName, "m3u")) {
            QMessageBox::warning(this, "Playlist loading failed", "Playlist loading failed");
        }
    }
}

void ImageViewer::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save playlist", ".", "*.m3u");
    if (!fileName.isEmpty()) {
        if (!playlist->save(fileName, "m3u")) {
            QMessageBox::warning(this, "Playlist saving failed", "Playlist saving failed");
        }
    }
}
