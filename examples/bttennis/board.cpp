/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "board.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QTextDocument>
#include <QFontMetrics>
#include <QPropertyAnimation>

const QColor fg = Qt::white;
const QColor bg = Qt::black;

Board::Board(QObject *parent) :
    QObject(parent)
{
    scene = new QGraphicsScene(QRect(0, 0, 640, 360), this);

    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setBackgroundBrush(QBrush(bg));

    ball = scene->addRect(-6, -6, 12, 12, QPen(Qt::SolidLine), QBrush(fg));
    ball->setPos(Width/2-6, Height/2-6);

    // why is y -1...otherwise we have a gap...
    topWall = scene->addRect(0, -1, Width, 12, QPen(Qt::SolidLine), QBrush(fg));
    bottomWall = scene->addRect(0, Height-12, Width, 12, QPen(Qt::SolidLine), QBrush(fg));

    leftPaddle = scene->addRect(0, 12, 12, Paddle, QPen(Qt::SolidLine), QBrush(fg));
    rightPaddle = scene->addRect(Width-12, 12, 12, Paddle, QPen(Qt::SolidLine), QBrush(fg));

    QPen p;
    p.setWidth(2);
    p.setStyle(Qt::DotLine);
    p.setBrush(QBrush(fg));
    scene->addLine(Width/2, 0, Width/2, Height, p);

    QFont f;
    f.setStyleHint(QFont::OldEnglish);
    f.setPixelSize(50);
    f.setBold(true);       
    leftScore = scene->addText(QString("0"), f);
    leftScore->setDefaultTextColor(fg);
//    leftScore->setPos(120, 50);

    rightScore = scene->addText(QString("0"), f);
//    rightScore->setPos(Width-140, 50);
    rightScore->setDefaultTextColor(fg);
    setScore(0, 0);

    f.setPixelSize(25);
    status = scene->addText(QString(), f);
    status->setDefaultTextColor(fg);

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

