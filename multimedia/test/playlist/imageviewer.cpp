#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QtGui>

ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ImageViewer)
{
    ui->setupUi(this);
    playlist = new QMediaPlaylist(0, this);
    model = new QMediaPlaylistModel(playlist,this);
    playlistIterator = new QMediaPlaylistIterator(playlist,this);

    connect(playlistIterator, SIGNAL(activated(QMediaSource)), SLOT(display(QMediaSource)));
    connect(playlistIterator, SIGNAL(playbackModeChanged(QMediaPlaylistIterator::PlaybackMode)), SLOT(updatePlaybackMode(QMediaPlaylistIterator::PlaybackMode)));
    connect(ui->listView, SIGNAL(activated(QModelIndex)), SLOT(jump(QModelIndex)));

    playlistIterator->setPlaybackMode(QMediaPlaylistIterator::Linear);

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

        playlistIterator->jump(firstPos);
    }
}

void ImageViewer::advance()
{
    playlistIterator->advance();
}

void ImageViewer::back()
{
    playlistIterator->back();
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
        ui->listView->setCurrentIndex(model->index(playlistIterator->currentPosition()));
        ui->label->setPixmap(src.dataLocation().toString());
    }
}

void ImageViewer::jump(const QModelIndex &index)
{
    playlistIterator->jump(index.row());
}

void ImageViewer::setPlaybackMode(int mode)
{
    playlistIterator->setPlaybackMode(QMediaPlaylistIterator::PlaybackMode(mode));
}

void ImageViewer::updatePlaybackMode(QMediaPlaylistIterator::PlaybackMode mode)
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
