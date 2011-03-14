/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QString>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include "testhelper.h"
#include <qgraphicsgeomap.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

TestHelper::TestHelper() :
    m_serviceProvider("geomapobject.test.plugin"),
    m_map(0),
    m_view(&m_scene)
{
    if (m_serviceProvider.error() != QGeoServiceProvider::NoError) {
        qDebug("Unable to find geoservices plugin.");
        qDebug() << m_serviceProvider.errorString();
        return;
    }

    QGeoMappingManager* mapManager = m_serviceProvider.mappingManager();
    if (mapManager)
        m_map = new QGraphicsGeoMap(mapManager);

    if (m_map == 0) {
        qDebug("Unable to create QGraphicsGeoMap.");
        return;
    }

    m_view.scene()->addItem(m_map);
    m_view.setGeometry(QRect(0, 0, 600, 600));
    m_map->setGeometry(QRect(0, 0, 600, 600));
    m_view.show();

    //set dummy data
    m_map->setCenter(QGeoCoordinate(52.5, 13.4, 0));
    m_map->setZoomLevel(10);
}

TestHelper::~TestHelper()
{
    if (m_map)
        delete m_map;
}

QGraphicsGeoMap* TestHelper::map()
{
    return m_map;
}

