#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

class QMediaPlayerService;
class QSlider;
class QModelIndex;
class QListView;
class QMediaPlaylistModel;

class Player : public QWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void jump(const QModelIndex &index);
    void setCurrentItem(int);

private:
    QMediaPlayerService *service;
    QSlider *slider;
    QListView *playlistView;
    QMediaPlaylistModel *playlistModel;
};

#endif
