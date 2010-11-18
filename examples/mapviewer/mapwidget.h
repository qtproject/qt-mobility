/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <qgraphicsgeomap.h>
#include <qgeomapcircleobject.h>
#include <qgeomappingmanager.h>
#include <qgeocoordinate.h>

#include <QGraphicsSceneMouseEvent>
#include <QTime>

QTM_USE_NAMESPACE

class MapWidget : public QGraphicsGeoMap
{
    Q_OBJECT
public:
    MapWidget(QGeoMappingManager *manager);
    ~MapWidget();

public slots:
    void setMouseClickCoordQuery(bool state);

signals:
    void coordQueryResult(const QGeoCoordinate &coord);

private slots:
    void kineticTimerEvent();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

private:
    bool coordQueryState;
    bool panActive;
    bool panDecellerate;

    // Fractional pan, used by panFloatWrapper
    QPointF remainingPan;

    // current kinetic panning speed, in pixel/msec
    QPointF kineticPanSpeed;
    QPoint panDir;
    QTimer *kineticTimer;
    QTime lastMoveTime;

    // An entry in the mouse history. first=speed, second=time
    typedef QPair<QPointF, QTime> MouseHistoryEntry;
    // A history of the last (currently 5) mouse move events is stored in order to smooth out movement detection for kinetic panning
    QList<MouseHistoryEntry> mouseHistory;

    void panFloatWrapper(const QPointF& delta);
    void applyPan(const Qt::KeyboardModifiers& modifiers);

public:
    QGeoMapCircleObject *lastCircle;
};

#endif // MAPWIDGET_H
