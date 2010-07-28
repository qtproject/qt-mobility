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

#ifndef QGEOMAPWIDGET_H
#define QGEOMAPWIDGET_H

#include "qmobilityglobal.h"
#include <QGraphicsWidget>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoMappingManager;
class QGeoBoundingBox;
class QGeoMapObject;
class QGeoMapWidgetPrivate;

class Q_LOCATION_EXPORT QGeoMapWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    enum MapType {
        NoMap,
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap
    };

    QGeoMapWidget(QGeoMappingManager *manager, QGraphicsItem *parent = 0);
    virtual ~QGeoMapWidget();

    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *parent);

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void startPanning();
    void stopPanning();
    void pan(int dx, int dy);

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    QList<MapType> supportedMapTypes() const;
    void setMapType(MapType mapType);
    MapType mapType() const;

    void addMapObject(QGeoMapObject *mapObject);
    void removeMapObject(QGeoMapObject *mapObject);
    QList<QGeoMapObject*> mapObjects();

    QList<QGeoMapObject*> visibleMapObjects();
    QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition);
    QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect);

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const;

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

signals:
    void zoomLevelChanged(qreal zoomLevel);
    void centered(const QGeoCoordinate &coordinate);
    void mapTypeChanged(MapType mapType);

private:
    QGeoMapWidgetPrivate *d_ptr;
};

QTM_END_NAMESPACE

#endif
