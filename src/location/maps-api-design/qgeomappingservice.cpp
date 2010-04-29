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

#include "qgeomappingservice.h"

QTM_BEGIN_NAMESPACE

QGeoMappingService* QGeoMappingService::createService(QGeoMappingService::ServiceProvider provider,
                                                                QString versionString,
                                                                QString token)
{
    Q_UNUSED(provider);
    Q_UNUSED(versionString);
    Q_UNUSED(token);
    return 0;
}

/*
        enum ServiceProvider {
            NokiaServices
        };

        // features supported (rotation, tilting, etc..)

        enum MapType {
            StreetMap,
            SatelliteMapDay,
            SatelliteMapNight,
            TerrainMap
        };

        enum ErrorCode {
            NoError
        };
*/

QGeoMappingService::QGeoMappingService(QObject *parent) : QObject(parent)
{
}

QGeoMappingService::~QGeoMappingService()
{
}

        /*
        virtual QList<MapType> supportedMapTypes() const = 0;
        virtual QList<QString> supportedImageFormats() const = 0;

        virtual int minimumZoomLevel() const = 0;
        virtual int maximumZoomLevel() const = 0;

        virtual QSize minimumSize() const = 0;
        virtual QSize maximumSize() const = 0;

    // Option 1 - simplest
        virtual QGeoMapReply* requestMap(const QGeoCoordinate &center,
                                 int zoomLevel,
                                 const QSize &size,
                                 MapType type,
                                 QString imageFormat) = 0;

        // Option 2 - a little more extensible
        //virtual QGeoMapReply* requestMap(const QGeoCoordinate &center,
        //                                 int zoomLevel,
        //                                 const QSize &size,
        //                                 const QGeoMapRequestOptions &requestOptions = QGeoMapRequestOptions()) = 0;

        // Option 3 - Could just use a request object straight up
        // - means things don't get as awkward if/when there's server support for tilting and rotation

        // Option 4 - Could pass the map in along with other options
        // - it knows the center, zoom level and size

    signals:
        void replyFinished(QGeoMapReply *reply);
        void replyError(QGeoMapReply *reply,
                        QGeoMappingService::ErrorCode errorCode,
                        QString errorString);
*/

#include "moc_qgeomappingservice.cpp"

QTM_END_NAMESPACE
