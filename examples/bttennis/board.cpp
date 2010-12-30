#include "board.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QTextDocument>
#include <QFontMetrics>
#include <QPropertyAnimation>

Board::Board(QObject *parent) :
    QObject(parent)
{
    scene = new QGraphicsScene(QRect(0, 0, 640, 360), this);

    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    ball = scene->addRect(0, 0, 12, 12, QPen(Qt::SolidLine), QBrush(Qt::black));
    ball->setPos(Width/2-6, Height/2-6);

    topWall = scene->addRect(0, 0, Width, 12, QPen(Qt::SolidLine), QBrush(Qt::black));
    bottomWall = scene->addRect(0, Height-12, Width, 12, QPen(Qt::SolidLine), QBrush(Qt::black));

    leftPaddle = scene->addRect(0, 12, 12, Paddle, QPen(Qt::SolidLine), QBrush(Qt::black));
    rightPaddle = scene->addRect(Width-12, 12, 12, Paddle, QPen(Qt::SolidLine), QBrush(Qt::black));

    QPen p;
    p.setWidth(2);
    p.setStyle(Qt::DotLine);
    scene->addLine(Width/2, 0, Width/2, Height, p);

    QFont f;
    f.setStyleHint(QFont::OldEnglish);
    f.setPixelSize(50);
    f.setBold(true);   
    leftScore = scene->addText(QString("0"), f);
//    leftScore->setPos(120, 50);

    rightScore = scene->addText(QString("0"), f);
//    rightScore->setPos(Width-140, 50);
    setScore(0, 0);

    f.setPixelSize(25);
    status = scene->addText(QString(), f);

    ball->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    leftPaddle->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    rightPaddle->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

//    connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

}
void Board::setBall(int x, int y)
{
    ball->setPos(x, y);
    if(x > 640)
        emit scored(Right);
    if(x < 0)
        emit scored(Left);
    checkBall();
}

void Board::setLeftPaddle(int y)
{
    leftPaddle->setPos(0, y);
}

void Board::setRightPaddle(int y)
{
    rightPaddle->setPos(0, y);
}

void Board::sceneChanged(const QList<QRectF> &region)
{
    QList<QGraphicsItem *>items = scene->collidingItems(ball);
    while(!items.empty()) {
        QGraphicsItem *i = items.takeFirst();
        if(i == topWall)
            emit ballCollision(Top);
        if(i == bottomWall)
            emit ballCollision(Bottom);
        if(i == leftPaddle)
            emit ballCollision(Left);
        if(i == rightPaddle)
            emit ballCollision(Right);
    }

}

void Board::checkBall()
{
    QList<QGraphicsItem *>items = scene->collidingItems(ball);
    while(!items.empty()) {
        QGraphicsItem *i = items.takeFirst();
        if(i == topWall)
            emit ballCollision(Top);
        if(i == bottomWall)
            emit ballCollision(Bottom);
        if(i == leftPaddle)
            emit ballCollision(Left);
        if(i == rightPaddle)
            emit ballCollision(Right);
    }

}

void Board::setScore(int l, int r)
{
    QString left = QString("%1").arg(l);
    QString right = QString("%1").arg(r);
    leftScore->document()->setPlainText(left);
    rightScore->document()->setPlainText(right);
    QFontMetrics fm(leftScore->font());
    leftScore->setPos(Width/4 - fm.width(left)/2, 50);
    rightScore->setPos(3*Width/4 - fm.width(right)/2, 50);

}

void Board::setStatus(QString text, int opacity_start, int opacity_end)
{

    status->document()->setPlainText(text);
    status->setPos(24, Height-25-25);
    QPropertyAnimation *a = new QPropertyAnimation(status, "opacity");
    a->setDuration(2000);
    a->setStartValue(opacity_start/100.0);
    a->setEndValue(opacity_end/100.0);
    a->start(QAbstractAnimation::DeleteWhenStopped);
}

