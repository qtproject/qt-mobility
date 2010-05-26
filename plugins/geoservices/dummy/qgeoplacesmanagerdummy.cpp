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

#include "qgeoplacesmanagerdummy.h"
#include <QTimer>
#include <QGeoCoordinate>
#include <QGeoAddress>
#include <QGeoPlacesReply>

QGeoPlacesManagerDummy::QGeoPlacesManagerDummy(QObject *parent)
        : QGeoPlacesManager(parent)
{
}

QGeoPlacesManagerDummy::~QGeoPlacesManagerDummy()
{
}


QGeoPlacesReply* QGeoPlacesManagerDummy::geocode(const QGeoCoordinate &coordinate,
                         const QGeoBoundingBox &bounds)
{
    QList<QGeoPlace> places;
    QGeoPlace place;
    QGeoAddress addr;
    addr.setCountry("QGeoServiceProviderPluginDummy");
    addr.setState("QGeoPlacesManagerDummy");
    addr.setCounty("geocode");
    addr.setCity("QGeoCoordinate");
    addr.setDistrict("coordinate");
    addr.setThoroughfareName("HardCoded");
    addr.setThoroughfareNumber("00");
    addr.setPostCode("000000");
    addr.setPostOfficeBox("00");
    place.setAddress(addr);
    place.setCoordinate(coordinate);
    places.append(place);
    QGeoPlacesReplyDummy* reply = new QGeoPlacesReplyDummy(places);
    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    return reply;
}

QGeoPlacesReply* QGeoPlacesManagerDummy::geocode(const QGeoAddress &address,
                         const QGeoBoundingBox &bounds)
{
    QList<QGeoPlace> places;
    QGeoPlace place;
    QGeoAddress addr;
    addr.setCountry("QGeoServiceProviderPluginDummy");
    addr.setState("QGeoPlacesManagerDummy");
    addr.setCounty("geocode");
    addr.setCity("QGeoAddress");
    addr.setDistrict("address");
    addr.setThoroughfareName("HardCoded");
    addr.setThoroughfareNumber("00");
    addr.setPostCode("000000");
    addr.setPostOfficeBox("00");
    place.setAddress(addr);
    place.setCoordinate(QGeoCoordinate(0.0,0.0));
    places.append(place);
    QGeoPlacesReplyDummy* reply = new QGeoPlacesReplyDummy(places);
    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    return reply;
}

QGeoPlacesReply* QGeoPlacesManagerDummy::placesSearch(const QString &searchString,
                              SearchTypes searchTypes,
                              const QGeoBoundingBox &bounds)
{
    QList<QGeoPlace> places;
    QGeoPlace place;
    QGeoAddress addr;
    addr.setCountry("QGeoServiceDummy");
    addr.setState("QGeocodingServiceDummy");
    addr.setCounty("geocode");
    addr.setCity("QString");
    addr.setDistrict("locationString");
    addr.setThoroughfareName("HardCoded");
    addr.setThoroughfareNumber("00");
    addr.setPostCode("000000");
    addr.setPostOfficeBox("00");
    place.setAddress(addr);
    place.setCoordinate(QGeoCoordinate(0.0,0.0));
    places.append(place);
    QGeoPlacesReplyDummy* reply = new QGeoPlacesReplyDummy(places);
    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    return reply;
}

void QGeoPlacesManagerDummy::placesFinished()
{
    QGeoPlacesReply *reply = qobject_cast<QGeoPlacesReply*>(sender());
    if (!reply)
        return;

    if(receivers(SIGNAL(finished(QGeoPlacesReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoPlacesManagerDummy::placesError(QGeoPlacesReply::Error error, const QString &errorString)
{
    QGeoPlacesReply *reply = qobject_cast<QGeoPlacesReply*>(sender());
    if (!reply)
        return;
    
    if(receivers(SIGNAL(finished(QGeoPlacesReply*, QGeoPlacesReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

