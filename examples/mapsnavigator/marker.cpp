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
#include "mapswidget.h"

#include <QPixmap>

#include "qlandmark.h"
#include "qgeoboundingcircle.h"

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

void Marker::setAddress(QGeoAddress addr)
{
    if (m_address != addr) {
        m_address = addr;
        emit addressChanged(m_address);
    }
}

void Marker::setName(QString name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(m_name);
    }
}

void Marker::setMoveable(bool moveable)
{
    if (m_moveable != moveable) {
        m_moveable = moveable;
        emit moveableChanged(m_moveable);
    }
}

MarkerManager::MarkerManager(QGeoSearchManager *searchManager, QObject *parent) :
    QObject(parent),
    m_searchManager(searchManager),
    m_status(0),
    revGeocodeRunning(false),
    myLocHasMoved(false)
{
    m_myLocation = new Marker(Marker::MyLocationMarker);
    m_myLocation->setName("Me");

    // hook the coordinateChanged() signal for reverse geocoding
    connect(m_myLocation, SIGNAL(coordinateChanged(QGeoCoordinate)),
            this, SLOT(myLocationChanged(QGeoCoordinate)));
}

MarkerManager::~MarkerManager()
{
    m_map->removeMapObject(m_myLocation);
    delete m_myLocation;
    removeSearchMarkers();
}

void MarkerManager::setMap(QGraphicsGeoMap *map)
{
    m_map = map;
    map->addMapObject(m_myLocation);
}

void MarkerManager::setMyLocation(QGeoCoordinate coord)
{
    m_myLocation->setCoordinate(coord);
}

void MarkerManager::search(QString query, qreal radius)
{
    QGeoSearchReply *reply;
    if (radius > 0) {
        QGeoBoundingCircle *boundingCircle = new QGeoBoundingCircle(
                    m_myLocation->coordinate(), radius);
        reply = m_searchManager->search(query,
                                        QGeoSearchManager::SearchAll,
                                        -1, 0,
                                        boundingCircle);
    } else {
        reply = m_searchManager->search(query);
    }

    if (m_status) {
        m_status->setText("Searching...");
        m_status->show();
    }

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

void MarkerManager::myLocationChanged(QGeoCoordinate location)
{
    if (revGeocodeRunning) {
        myLocHasMoved = true;
    } else {
        QGeoSearchReply *reply = m_searchManager->reverseGeocode(location);
        myLocHasMoved = false;

        if (reply->isFinished()) {
            revGeocodeRunning = false;
            reverseReplyFinished(reply);
        } else {
            revGeocodeRunning = true;

            revSigMap.setMapping(reply, reply);
            connect(reply, SIGNAL(finished()),
                    &revSigMap, SLOT(map()));
            connect(&revSigMap, SIGNAL(mapped(QObject*)),
                    this, SLOT(reverseReplyFinished(QObject*)));
        }
    }
}

void MarkerManager::reverseReplyFinished(QObject *obj)
{
    QGeoSearchReply *reply = dynamic_cast<QGeoSearchReply*>(obj);
    Q_ASSERT(reply != NULL);

    if (reply->places().size() > 0) {
        QGeoPlace place = reply->places().first();
        m_myLocation->setAddress(place.address());
    }

    revGeocodeRunning = false;
    if (myLocHasMoved)
        myLocationChanged(m_myLocation->coordinate());

    reply->deleteLater();
}

void MarkerManager::replyFinished(QObject *obj)
{
    QGeoSearchReply *reply = dynamic_cast<QGeoSearchReply*>(obj);
    Q_ASSERT(reply != NULL);

    // generate the markers and add them to the map
    foreach (QGeoPlace place, reply->places()) {
        Marker *m = new Marker(Marker::SearchMarker);
        m->setCoordinate(place.coordinate());

        if (place.isLandmark()) {
            QLandmark lm(place);
            m->setName(lm.name());
        } else {
            m->setName(QString("%1, %2").arg(place.address().street())
                                        .arg(place.address().city()));
        }
        m->setAddress(place.address());
        m->setMoveable(false);

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

    if (m_status)
        m_status->hide();
}
