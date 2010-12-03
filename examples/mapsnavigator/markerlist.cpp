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

#include "markerlist.h"

//TODO: get rid of debug when finalized
#include <QDebug>

void MarkerObject::normalizeIcon()
{
    m_state = Valid;

    QPoint offset;
    int scale;

    if (m_type == MyLocation) {
        offset = QPoint(-13,-13);
        scale = 25;
    } else {
        offset = QPoint(-15, -36);
        scale = 30;
    }

    setOffset(offset);
    setPixmap(QPixmap(m_filename).scaledToWidth(scale, Qt::SmoothTransformation));
}

void MarkerObject::expandIcon(const QGeoCoordinate& pos)
{
    m_state = Held;
    setCoordinate(pos);
    setOffset(QPoint(-22, -52));
    setPixmap(QPixmap(m_filename));
}

void MarkerObject::setSearchReply(QGeoSearchReply *reply)
{
    if (m_searchReply)
        m_searchReply->deleteLater();

    m_searchReply = reply;

    // Check if search reply is already finished
    if (m_searchReply->isFinished()) {
        if (m_searchReply->error() == QGeoSearchReply::NoError) {
            searchFinished();
        } else {
            searchError(m_searchReply->error(), m_searchReply->errorString());
        }
        return;
    }

    // Connect the search signals
    connect(m_searchReply, SIGNAL(finished()),
            this, SLOT(searchFinished()));

    connect(m_searchReply, SIGNAL(error(QGeoSearchReply::Error,QString)),
            this, SLOT(searchError(QGeoSearchReply::Error,QString)));

    m_state = Searching;
}

void MarkerObject::searchFinished()
{
    if (!m_searchReply)
        return;

    if (m_searchReply->places().length() != 0) {
        setPlace(m_searchReply->places().at(0));

        if (!coordinate().isValid())
            setCoordinate(m_place.coordinate());
    }

    m_searchReply->deleteLater();
    m_searchReply = 0;
    m_state = Valid;
}

void MarkerObject::searchError(QGeoSearchReply::Error error, const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);
    //emit searchReplyError(errorString);
    m_searchReply->deleteLater();
    m_searchReply = 0;
    m_state = Error;
}

MarkerList::MarkerList(QWidget *parent)
    : QWidget(parent), m_searchManager(0), selectedMarker(0), moved(false),
      waypointCount(0), markerCount(0), pathCount(0)
{
}

MarkerList::~MarkerList()
{
}

void MarkerList::initialize(QGeoSearchManager *searchManager)
{
    m_searchManager = searchManager;
}

void MarkerList::addMarker(MarkerObject *marker)
{
    if (!marker)
        return;

    QString markerName;
    switch (marker->markerType()) {
        case MarkerObject::MyLocation:
            markerName = "My Location";
            break;
        case MarkerObject::Search:
            markerName = "Marker " + QString::number(++markerCount);
            break;
        case MarkerObject::Waypoint:
            markerName = "Way Point " + QString::number(++waypointCount);
            break;
        case MarkerObject::Startpoint:
            markerName = "Start Point";
            break;
        case MarkerObject::Endpoint:
            markerName = "End Point";
            break;
        case MarkerObject::Path:
            markerName = "Path " + QString::number(++pathCount);
            break;
        default:
            break;
    }

    // searches based on coordinates or address string
    getMarkerLocation(marker);

    marker->setName(markerName);
    qDebug() << "ADDING MARKER" << markerName;
}

void MarkerList::removeMarker(MarkerObject *marker)
{
    switch (marker->markerType()) {
        case MarkerObject::Search:
            markerCount--;
            break;
        case MarkerObject::Waypoint:
            //TODO: will have to rearrange the waypoint numbers
            waypointCount--;
            break;
        case MarkerObject::Path:
            pathCount--;
            break;
        default:
            break;
    }
}

void MarkerList::markerPressed(MarkerObject* marker)
{
    selectedMarker = marker;
    moved = false;
}

void MarkerList::markerReleased()
{
    if (!selectedMarker)
        return;

    qDebug() << "MARKER RELEASED";

    if (selectedMarker->markerState() == MarkerObject::Held) {
        selectedMarker->normalizeIcon();
        getMarkerLocation(selectedMarker);
    } else if (!moved)
        emit showMarkerInfo(selectedMarker);

    selectedMarker = 0;
}

void MarkerList::moveMarker(const QGeoCoordinate& last, const QGeoCoordinate& curr)
{
    //TODO: bug - marker disappears when dragged to edge of screen

    if (!selectedMarker)
        return;

    moved = true;

    double dlat = curr.latitude() - last.latitude();
    double dlon = curr.longitude() - last.longitude();
    if (qAbs(dlon) >= 180.0)
        dlon = 360.0 - dlon;

    QGeoCoordinate markerCoordinate = selectedMarker->coordinate();

    double mlat = markerCoordinate.latitude() + dlat;
    if (mlat < -90.0)
        mlat = -90.0;
    if (mlat > 90.0)
        mlat = 90.0;

    double mlon = markerCoordinate.longitude() + dlon;
    if (mlon > 180.0)
        mlon -= 360.0;
    if (mlon < -180.0)
        mlon += 360.0;

    selectedMarker->setCoordinate(QGeoCoordinate(mlat, mlon));
}

void MarkerList::mapMoved()
{
    moved = true;
}

void MarkerList::getMarkerLocation(MarkerObject *marker)
{
    if (!marker)
        return;

    QGeoSearchReply *reply;
    if (marker->markerState() == MarkerObject::Address)
        reply = m_searchManager->search(marker->name());
    else
        reply = m_searchManager->reverseGeocode(marker->coordinate());

    marker->setSearchReply(reply);
}
