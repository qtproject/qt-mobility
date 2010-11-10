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

#ifndef VIEW_H
#define VIEW_H

#include <QPixmap>
#include <QGraphicsView>
#include <QMenu>
#include <QTimer>
#include <QTime>
#include "inputcontroller.h"




class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QGraphicsScene *scene);
    virtual ~View();
    static int m_imageWidth;


protected:

    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);




private slots:
    void startAccelerometer();
    void startTapSensor();
    void startMagnetometer();
    void startRotationSensor();
    void startOrientationSensor();
    void startCompass();
    void startKeys();
    void update();

private:
    int checkX(int x);
    int checkY(int y);
    void createActions();
    void handleAction(QString oldSensor, QString newSensor);
    void setupWindow();
    void switchController(QString sensor);

    static int m_imageHeight;

    static QString m_currentSensor;
    QList<QString> m_sensors;
    QTimer m_timer;
    InputController *m_controller;
    InputController *m_exController;
    int m_delay;
    QTime m_mousePressTime;
    QMenu* m_menu;
    bool m_mouseMode;
    int m_eventX, m_eventY;
    int m_dx, m_dy;
    QTime m_zoomTime;
    QPixmap m_pix;
    static bool m_isStarted;


};


#endif // VIEW_H
