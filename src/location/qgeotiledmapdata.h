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

#ifndef QGEOTILEDMAPDATA_H
#define QGEOTILEDMAPDATA_H

#include "qgeomapdata.h"

QTM_BEGIN_NAMESPACE

class QGeoTiledMapDataPrivate;

class Q_LOCATION_EXPORT QGeoTiledMapData : public QGeoMapData
{
    friend class QGeoTiledMapDataPrivate;

public:
    QGeoTiledMapData(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget);
    virtual ~QGeoTiledMapData();

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const;

    virtual QPoint screenPositionToTileIndices(const QPointF &screenPosition) const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void addMapObject(QGeoMapObject *mapObject);

    void setZoomLevel(qreal zoomLevel);
    void setViewportSize(const QSizeF &size);
    void pan(int dx, int dy);

    QRectF screenRect() const;

    virtual QList<QGeoMapObject*> visibleMapObjects();
    virtual QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius = 0);
    virtual QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect);

    virtual QPixmap mapObjectsOverlay() const;

    QRectF protectedRegion() const;
    void clearProtectedRegion();

protected:
    virtual void coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const;
    virtual QGeoCoordinate worldPixelToCoordinate(qulonglong x, qulonglong y) const;

private:
    QGeoTiledMapDataPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoTiledMapData)
};

QTM_END_NAMESPACE

#endif
