/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "mapbox.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomaprectangleobject.h>

#include <QGridLayout>
#include <QPushButton>

#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connect(MapBox::session(), SIGNAL(opened()), this, SLOT(networkSessionOpened()));

    setWindowTitle("MapBox test");

    resize(640, 480);

    m_cleanButton = new QPushButton("Clean cache");

    m_layout = new QGridLayout;
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(m_layout);
    startTimer(10);

    eventTime.start();
}

MainWindow::~MainWindow()
{

}

void MainWindow::addBox(MapBox * box)
{
    m_boxes << box;

    connect(m_cleanButton, SIGNAL(clicked()), box, SLOT(clearCache()));

    resizeEvent(0);
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QLayoutItem *child;
    while ((child = m_layout->takeAt(0)) != 0) ;

    int columns = floor(0.5+sqrt(static_cast<qreal>(m_boxes.size())));

    m_layout->addWidget(m_cleanButton, 0, 0, 1, columns);

    int i = 0;
    for (int row = 1; i < m_boxes.size(); ++row) {
        for (int col = 0; col < columns; ++col) {
            if (i+1 >= m_boxes.size()) {
                m_layout->addWidget(m_boxes[i], row, col, 1, columns - i);
                ++i;
                break;
            }
            else {
                m_layout->addWidget(m_boxes[i], row, col);
                ++i;
            }
        }
    }
}

void MainWindow::networkSessionOpened()
{
    for (int i = 0; i < 1; ++i) {
        MapBox * box = MapBox::createOnlineMap(this);
        box->addRectangle(55.1977, 5.87586, 47.5395, 15.2802);
        //box->addRoute(QGeoCoordinate(50, 5.87586), QGeoCoordinate(47.5395, 15.2802));

        addBox(box);
    }
}

void MainWindow::timerEvent(QTimerEvent * event)
{
    const qreal speed = 0.5;
    qreal currentMovement = speed * qMin(300, eventTime.elapsed());

    foreach(MapBox * box, m_boxes) {
        box->map()->pan(currentMovement, 0);
    }
    eventTime.restart();
}
