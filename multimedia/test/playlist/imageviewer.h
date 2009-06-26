#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtGui/QMainWindow>

#include "qmediaplaylist.h"
#include "qmediaplaylistnavigator.h"
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

private slots:
    void updatePlaybackMode(QMediaPlaylistNavigator::PlaybackMode mode);
    void updateNavigationActions();

private:
    Ui::ImageViewer *ui;

    QMediaPlaylist *playlist;
    QMediaPlaylistModel *model;
    QMediaPlaylistNavigator *navigator;
};

#endif // IMAGEVIEWER_H
