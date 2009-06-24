#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtGui/QMainWindow>

#include "qmediaplaylist.h"
#include "qmediaplaylistiterator.h"
#include "qmediaplaylistmodel.h"

namespace Ui
{
    class ImageViewer;
}

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

public slots:
    void addImage();
    void advance();
    void back();
    void shuffle();

    void display(const QMediaSource &);

    void jump(const QModelIndex &index);

    void loadPlaylist();
    void savePlaylist();

    void setPlaybackMode(int mode);
    void updatePlaybackMode(QMediaPlaylistIterator::PlaybackMode mode);

private:
    Ui::ImageViewer *ui;

    QMediaPlaylist *playlist;
    QMediaPlaylistModel *model;
    QMediaPlaylistIterator *playlistIterator;
};

#endif // IMAGEVIEWER_H
