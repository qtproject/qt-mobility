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

#include "marker.h"
#include <QPixmap>

Marker::Marker(MarkerType type) :
    QGeoMapPixmapObject()
{
    setMarkerType(type);
}

void Marker::setMarkerType(MarkerType type)
{
    QString filename;
    QPoint offset;
    int scale;

    m_type = type;

    switch (m_type) {
    case MyLocationMarker:
        filename = ":/icons/mylocation.png";
        break;
    case SearchMarker:
        filename = ":/icons/searchmarker.png";
        break;
    case WaypointMarker:
        filename = ":/icons/waypointmarker.png";
        break;
    case StartMarker:
        filename = ":/icons/startmarker.png";
        break;
    case EndMarker:
        filename = ":/icons/endmarker.png";
        break;
    case PathMarker:
        filename = ":/icons/pathmarker.png";
        break;
    }

    if (m_type == MyLocationMarker) {
        offset = QPoint(-13,-13);
        scale = 25;
    } else {
        offset = QPoint(-15, -36);
        scale = 30;
    }

    setOffset(offset);
    setPixmap(QPixmap(filename).scaledToWidth(scale, Qt::SmoothTransformation));
}

MarkerManager::MarkerManager(QGeoSearchManager *searchManager, QObject *parent) :
    QObject(parent),
    m_searchManager(searchManager)
{
    m_myLocation = new Marker(Marker::MyLocationMarker);
}

MarkerManager::~MarkerManager()
{
    m_map->removeMapObject(m_myLocation);
    delete m_myLocation;
    removeSearchMarkers();
}

void MarkerManager::setMap(QGraphicsGeoMap *map)
{
    map->addMapObject(m_myLocation);
}

void MarkerManager::setMyLocation(QGeoCoordinate coord)
{
    m_myLocation->setCoordinate(coord);
}

void MarkerManager::search(QString query)
{
    QGeoSearchReply *reply = m_searchManager->search(query);
    if (reply->isFinished()) {
        replyFinished(reply);
    } else {
        sigMap.setMapping(reply, reply);
        connect(reply, SIGNAL(finished()),
                &sigMap, SLOT(map()));
        connect(&sigMap, SIGNAL(mapped(QObject*)),
                this, SLOT(replyFinished(QObject*)));

        connect(reply, SIGNAL(error(QGeoSearchReply::Error,QString)),
                this, SIGNAL(searchError(QGeoSearchReply::Error,QString)));
    }
}

void MarkerManager::removeSearchMarkers()
{
    foreach (Marker *m, searchMarkers) {
        m_map->removeMapObject(m);
        delete m;
    }
}

QGeoCoordinate MarkerManager::myLocation() const
{
    return m_myLocation->coordinate();
}

void MarkerManager::replyFinished(QObject *obj)
{
    QGeoSearchReply *reply = dynamic_cast<QGeoSearchReply*>(obj);
    Q_ASSERT(reply != NULL);

    // generate the markers and add them to the map
    foreach (QGeoPlace place, reply->places()) {
        Marker *m = new Marker(Marker::SearchMarker);
        m->setCoordinate(place.coordinate());
        searchMarkers.append(m);

        if (m_map) {
            m_map->addMapObject(m);
            // also zoom out until marker is visible
            while (!m_map->viewport().contains(place.coordinate()))
                m_map->setZoomLevel(m_map->zoomLevel()-1);
        }
    }

    reply->deleteLater();

    emit searchFinished();
}
