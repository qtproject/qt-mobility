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

#ifndef QGEOMAPPINGMANAGER_H
#define QGEOMAPPINGMANAGER_H

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomappingreply.h"

#include <QObject>
#include <QPixmap>

class QSize;
class QNetworkProxy;
class QPainter;
class QStyleOptionGraphicsItem;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

//class QGeoMapWidget;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class Q_LOCATION_EXPORT QGeoMappingManager : public QObject
{
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

    virtual ~QGeoMappingManager();

    virtual QGeoMappingReply* requestMap(const QGeoCoordinate &center,
                                        int zoomLevel,
                                        const QSize &size,
                                        const QGeoMapRequestOptions &requestOptions) = 0;

    virtual QGeoMappingReply* requestTile(int row, int col, int zoomLevel,
                                          const QSize &size,
                                          const QGeoMapRequestOptions &requestOptions) = 0;

    QList<MapType> supportedMapTypes() const;
    QList<QString> supportedImageFormats() const;

    QSize minimumImageSize() const;
    QSize maximumImageSize() const;

    int minimumZoomLevel() const;
    int maximumZoomLevel() const;

    QString host() const;
    QNetworkProxy proxy() const;
    void setHost(const QString &host);
    void setProxy(const QNetworkProxy &proxy);

    //internal map state

//    void setMapWidget(QGeoMapWidget *widget);

    int zoomLevel() const;
    virtual void setZoomLevel(int zoomLevel) = 0;

    virtual void setViewPortSize(const QSize &size);
    QSize viewPortSize() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option) = 0;

    virtual void setCenter(const QGeoCoordinate &center) = 0;
    virtual QGeoCoordinate center() const = 0;

    virtual void pan(int startX, int startY, int endX, int endY) = 0;

    virtual QGeoBoundingBox viewBounds() const = 0;

    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const = 0;
    virtual QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const = 0;

signals:
    void finished(QGeoMappingReply* reply);
    void error(QGeoMappingReply* reply, QGeoMappingReply::Error error, QString errorString = QString());

protected:
    QGeoMappingManager(QGeoMappingManagerPrivate &dd);
    QGeoMappingManager();

    void setSupportedMapTypes(const QList<MapType> &mapTypes);
    void setSupportedImageFormats(const QList<QString> &imageFormats);

    void setMinimumZoomLevel(int minimumZoom);
    void setMaximumZoomLevel(int maximumZoom);

    void setMinimumSize(const QSize &minimumSize);
    void setMaximumSize(const QSize &maximumSize);

    QGeoMappingManagerPrivate* d_ptr;

private:
    Q_DISABLE_COPY(QGeoMappingManager)
    Q_DECLARE_PRIVATE(QGeoMappingManager)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
