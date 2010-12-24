#include "controller.h"

#include <QDateTime>
#include <QDebug>

#define FRAME_RATE 60

Controller::Controller(QObject *parent) :
    QObject(parent), timer(new QTimer), elapsed(new QTime), score_left(0), score_right(0),
    rightPaddleForce(0), leftPaddleForce(0),
    rightPaddleLast(0), leftPaddleLast(0),
    rightPowerUp(0), leftPowerUp(0)
{

    qsrand(QDateTime::currentDateTime().time().msec());

    timer->setInterval(1000/FRAME_RATE);
    connect(timer, SIGNAL(timeout()), SLOT(tick()));
    srand(QDateTime::currentDateTime().time().msec());

    timer->start();
    restart_ball();
}

void Controller::tick()
{
    static int i;
    static int ttf = 0;
    static int dev = 0;

    int msec = elapsed->elapsed();
    elapsed->restart();

    ball_x += speed_x*msec/1000;
    ball_y += speed_y*msec/1000;


    const int decay = 5;

    if(leftPaddleForce > decay)
        leftPaddleForce-=decay;
    else if(leftPaddleForce < -decay)
        leftPaddleForce+=decay;
    if(rightPaddleForce > decay)
        rightPaddleForce-=decay;
    else if(rightPaddleForce < -decay)
        rightPaddleForce+=decay;

    if(rightPaddleForce <= decay)
        rightPowerUp++;

    if(leftPaddleForce <= decay)
        leftPowerUp++;


//    ttf++;
//    if(msec > 1000/FRAME_RATE+2 || msec < 1000/FRAME_RATE-2)
//        dev++;

//    if(!(i++%120)) {
//        qDebug() << "powerUp: " << leftPowerUp << rightPowerUp << leftPaddleForce << rightPaddleForce << speed_x*msec/1000 << speed_y*msec/1000 << msec << dev;
//        ttf = dev =0;
//    }

    emit moveBall(ball_x, ball_y);
}


static inline int paddle_boost(int force){
    if(force > 30)
        return -3*FRAME_RATE;
    else if(force > 20)
        return -2*FRAME_RATE;
    else if(force > 6)
        return -1*FRAME_RATE;
    else if(force < -30)
        return 3*FRAME_RATE;
    else if(force < -20)
        return 2*FRAME_RATE;
    else if(force < -6)
        return 1*FRAME_RATE;
    return 0;
}

void Controller::ballCollision(Board::Edge pos)
{
    if(pos == Board::Top || pos == Board::Bottom)
        speed_y *= -1;
    if(pos == Board::Left) {
        speed_x *= -1;
        ball_x = 13;
        speed_y += paddle_boost(leftPaddleForce);

        if(leftPowerUp > 75 && speed_x < 8*FRAME_RATE){
//            qDebug() << "Boost:" << leftPowerUp;
            speed_x *= 2;
            leftPowerUp = 0;
        }
    }
    else if(pos == Board::Right) {
        speed_x *= -1;
        ball_x = Board::Width-24;
        speed_y += paddle_boost(rightPaddleForce);

        if(rightPowerUp > 75 && speed_x > -8*FRAME_RATE){
//            qDebug() << "Boost:" << rightPowerUp;
            speed_x *= 2;
            rightPowerUp = 0;
        }

    }
//    tick();
//    QMetaObject::invokeMethod(this, "moveBall", Qt::QueuedConnection, Q_ARG(int, ball_x), Q_ARG(int, ball_y));
//    emit moveBall(ball_x, ball_y);

}

void Controller::scored(Board::Edge pos)
{
    if(!timer->isActive())
        return;

    if(pos == Board::Left)
        emit score(score_left, ++score_right);
    else if(pos == Board::Right)
        emit score(++score_left, score_right);

    restart_ball();
}

void Controller::restart_ball()
{
    if(!timer->isActive())
        return;

    elapsed->start();

    ball_x = Board::Width/2;
    ball_y = Board::Height/2;

//    ball_y = (qrand()%(Board::Height/2))+Board::Height/4;
    ball_y = (qrand()%(Board::Height-48))+24;

    // Speed in in pixels/second

    const int max = 4;
    const int min_x = 2*FRAME_RATE;
    const int min_y = 1.5*FRAME_RATE;


    speed_y = 0;
    while((speed_y <= 0 && speed_y > -1*min_y) ||
          (speed_y > 0 && speed_y < min_y))
        speed_y = qrand()%(2*max*FRAME_RATE)-max*FRAME_RATE;


    speed_x = 0;
    while((speed_x <= 0 && speed_x > -1*min_x) ||
          (speed_x > 0 && speed_x < min_x))
        speed_x = qrand()%(2*max*FRAME_RATE)-max*FRAME_RATE;

    leftPowerUp = rightPowerUp = 0;

    emit moveBall(ball_x, ball_y);
}


void Controller::resetBoard()
{
    if(!timer->isActive())
        return;

    score_left = score_right = 0;
    restart_ball();
}

void Controller::stop()
{
    timer->stop();
}

void Controller::start()
{
    timer->start();
}

void Controller::moveLeftPaddle(int y)
{
    leftPaddleForce += leftPaddleLast-y;
    leftPaddleLast = y;
    leftPowerUp = 0;
}

void Controller::moveRightPaddle(int y)
{
    rightPaddleForce += rightPaddleLast-y;
    rightPaddleLast =y;
    rightPowerUp = 0;
}

void Controller::refresh()
{
    emit moveBall(ball_x, ball_y);
    emit score(score_left, score_right);
}
