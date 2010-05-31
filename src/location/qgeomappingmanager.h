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

#include "qgeomapwidget.h"
#include "qgeomapreply.h"

#include <QObject>
#include <QSize>
#include <QPair>

QTM_BEGIN_NAMESPACE

class QGeoBoundingBox;
class QGeoCoordinate;
class QGeoMapViewport;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class Q_LOCATION_EXPORT QGeoMappingManager : public QObject
{
    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

    virtual ~QGeoMappingManager();

    virtual QGeoMapViewport* createViewport(QGeoMapWidget *widget) = 0;
    virtual void removeViewport(QGeoMapViewport *viewport);

    virtual void updateMapImage(QGeoMapViewport *viewport) = 0;

    QList<QGeoMapWidget::MapType> supportedMapTypes() const;


    QSize minimumImageSize() const;
    QSize maximumImageSize() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

protected:
    QGeoMappingManager();
    QGeoMappingManager(QGeoMappingManagerPrivate *dd);

    void setSupportedMapTypes(const QList<QGeoMapWidget::MapType> &mapTypes);

    void setMinimumZoomLevel(qreal minimumZoom);
    void setMaximumZoomLevel(qreal maximumZoom);

    void setMinimumImageSize(const QSize &minimumSize);
    void setMaximumImageSize(const QSize &maximumSize);

    QGeoMappingManagerPrivate* d_ptr;

//    // Tile related functions

//    void setTileSize(const QSize &tileSize);
//    QSize tileSize() const;

//    // TODO better way of returning these results
//    virtual void coordinateToTileIndices(const QGeoCoordinate &coordinate, qint32 zoomLevel, qint32 *rowIndex, qint32 *columnIndex);
//    virtual QGeoCoordinate tileIndicesToUpperLeftCoordinate(qint32 zoomLevel, qint32 rowIndex, qint32 columIndex);
//    virtual QGeoBoundingBox getTileBounds(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex);
//    virtual QPoint screenPositionToTilePosition(const QGeoTiledMapViewport *viewport, const QPointF &screenPosition) const = 0;

//    // TODO create a QGeoMapTileRequest object to make this saner?
//    virtual QGeoMapReply* getTileImage(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex, QGeoMapWidget::MapType mapType, const QString &imageFormat);

//signals:
//    void finished(QGeoMapReply* reply);
//    void error(QGeoMapReply* reply, QGeoMapReply::Error error, QString errorString = QString());

private:
    Q_DECLARE_PRIVATE(QGeoMappingManager)
    Q_DISABLE_COPY(QGeoMappingManager)
};

QTM_END_NAMESPACE

#endif
