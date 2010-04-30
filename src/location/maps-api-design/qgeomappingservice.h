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

#ifndef QGEOMAPPINGSERVICE_H
#define QGEOMAPPINGSERVICE_H

#include "qgeocoordinate.h"

#include <QObject>

class QSize;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoMapReply;

class QGeoMappingServicePrivate;

class Q_LOCATION_EXPORT QGeoMappingService : public QObject{

    Q_OBJECT

public:

    enum ErrorCode {
        NoError
    };

    enum MapType {
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap
    };


    QGeoMappingService();
    virtual ~QGeoMappingService();

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

    QList<MapType> supportedMapTypes() const;
    QList<QString> supportedImageFormats() const;

    int minimumZoomLevel() const;
    int maximumZoomLevel() const;

    QSize minimumSize() const;
    QSize maximumSize() const;

signals:
    void replyFinished(QGeoMapReply *reply);
    void replyError(QGeoMapReply *reply, 
                    QGeoMappingService::ErrorCode errorCode,
                    QString errorString);

protected:
    void setSupportedMapTypes(const QList<MapType> &mapTypes);
    void setSupportedImageFormats(const QList<QString> &imageFormats);

    void setMinimumZoomLevel(int minimumZoom);
    void setMaximumZoomLevel(int maximumZoom);

    void setMinimumSize(const QSize &minimumSize);
    void setMaximumSize(const QSize &maximumSize);

    QGeoMappingServicePrivate* d_ptr;

private:
    Q_DISABLE_COPY(QGeoMappingService)
    Q_DECLARE_PRIVATE(QGeoMappingService)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
