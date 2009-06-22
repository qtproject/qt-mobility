#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

class QMediaPlayerService;

class Player : public QWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

private slots:
    void open();

private:
    QMediaPlayerService *service;
};

#endif
