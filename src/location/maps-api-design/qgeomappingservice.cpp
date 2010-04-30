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
#include "qgeomappingservice_p.h"

QTM_BEGIN_NAMESPACE

/*
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

/*!
*/
QGeoMappingService::QGeoMappingService()
    : d_ptr(new QGeoMappingServicePrivate()) {}

/*!
*/
QGeoMappingService::~QGeoMappingService()
{
    Q_D(QGeoMappingService);
    delete d;
}

/*!
*/
QList<QGeoMappingService::MapType> QGeoMappingService::supportedMapTypes() const
{
    Q_D(const QGeoMappingService);
    return d->supportedMapTypes;
}

/*!
*/
QList<QString> QGeoMappingService::supportedImageFormats() const
{
    Q_D(const QGeoMappingService);
    return d->supportedImageFormats;
}

/*!
*/
int QGeoMappingService::minimumZoomLevel() const
{
    Q_D(const QGeoMappingService);
    return d->minimumZoomLevel;
}

/*!
*/
int QGeoMappingService::maximumZoomLevel() const

{
    Q_D(const QGeoMappingService);
    return d->maximumZoomLevel;
}

/*!
*/
QSize QGeoMappingService::minimumSize() const
{
    Q_D(const QGeoMappingService);
    return d->minimumSize;
}

/*!
*/
QSize QGeoMappingService::maximumSize() const
{
    Q_D(const QGeoMappingService);
    return d->maximumSize;
}

/*!
\fn void QGeoMappingService::replyFinished(QGeoMapReply *reply)
*/

/*!
\fn void QGeoMappingService::replyError(QGeoMapReply *reply,
                                        QGeoMappingService::ErrorCode errorCode,
                                        QString errorString);
*/

/*!
*/
void QGeoMappingService::setSupportedMapTypes(const QList<QGeoMappingService::MapType> &mapTypes)
{
    Q_D(QGeoMappingService);
    d->supportedMapTypes = mapTypes;
}

/*!
*/
void QGeoMappingService::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    Q_D(QGeoMappingService);
    d->supportedImageFormats = imageFormats;
}

/*!
*/
void QGeoMappingService::setMinimumZoomLevel(int minimumZoom)
{
    Q_D(QGeoMappingService);
    d->minimumZoomLevel = minimumZoom;
}

/*!
*/
void QGeoMappingService::setMaximumZoomLevel(int maximumZoom)
{
    Q_D(QGeoMappingService);
    d->maximumZoomLevel = maximumZoom;
}

/*!
*/
void QGeoMappingService::setMinimumSize(const QSize &minimumSize)
{
    Q_D(QGeoMappingService);
    d->minimumSize = minimumSize;
}

/*!
*/
void QGeoMappingService::setMaximumSize(const QSize &maximumSize)
{
    Q_D(QGeoMappingService);
    d->maximumSize = maximumSize;
}

        /*

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
*/

/*******************************************************************************
*******************************************************************************/

QGeoMappingServicePrivate::QGeoMappingServicePrivate() {}

QGeoMappingServicePrivate::QGeoMappingServicePrivate(const QGeoMappingServicePrivate &other)
    : supportedMapTypes(other.supportedMapTypes),
    supportedImageFormats(other.supportedImageFormats),
    minimumZoomLevel(other.minimumZoomLevel),
    maximumZoomLevel(other.maximumZoomLevel),
    minimumSize(other.minimumSize),
    maximumSize(other.maximumSize) {}

QGeoMappingServicePrivate::~QGeoMappingServicePrivate() {}

QGeoMappingServicePrivate& QGeoMappingServicePrivate::operator= (const QGeoMappingServicePrivate &other)
{
    supportedMapTypes = other.supportedMapTypes;
    supportedImageFormats = other.supportedImageFormats;
    minimumZoomLevel = other.minimumZoomLevel;
    maximumZoomLevel = other.maximumZoomLevel;
    minimumSize = other.minimumSize;
    maximumSize = other.maximumSize;

    return *this;
}

#include "moc_qgeomappingservice.cpp"

QTM_END_NAMESPACE
