#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

class QMediaPlayer;
class QMediaMetadata;

class QSlider;

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
    void metaDataChanged();

private:
    QMediaPlayer *player;
    QMediaMetadata *metaData;
    QSlider *slider;
};

#endif
