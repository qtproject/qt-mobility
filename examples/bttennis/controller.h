#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include "board.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);

signals:
    void moveBall(int x, int y);
    void score(int left, int right);    

public slots:
    void ballCollision(Board::Edge pos);
    void scored(Board::Edge pos);
    void resetBoard();
    void refresh();

    void moveLeftPaddle(int y);
    void moveRightPaddle(int y);

    void tick();

    void start();
    void stop();

private:
    QTimer *timer;
    QTime *elapsed;
    int ball_x;
    int speed_x;
    int ball_y;
    int speed_y;
    int score_left;
    int score_right;

    int rightPaddleForce;
    int rightPaddleLast;
    int leftPaddleForce;
    int leftPaddleLast;
    int leftPowerUp;
    int rightPowerUp;

    void restart_ball();

};

#endif // CONTROLLER_H
