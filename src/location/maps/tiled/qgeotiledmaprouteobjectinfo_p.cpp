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

#include "qgeotiledmaprouteobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomaprouteobject.h"

#include "qgeoroutesegment.h"

#include <QPointF>

QTM_BEGIN_NAMESPACE

QGeoTiledMapRouteObjectInfo::QGeoTiledMapRouteObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    route = static_cast<QGeoMapRouteObject*>(mapObject);

    connect(route,
            SIGNAL(routeChanged(QGeoRoute)),
            this,
            SLOT(routeChanged(QGeoRoute)));
    connect(route,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(route,
            SIGNAL(detailLevelChanged(quint32)),
            this,
            SLOT(detailLevelChanged(quint32)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;

    penChanged(route->pen());
    routeChanged(route->route());
}

QGeoTiledMapRouteObjectInfo::~QGeoTiledMapRouteObjectInfo() {}

void QGeoTiledMapRouteObjectInfo::routeChanged(const QGeoRoute &route)
{
    //QListIterator<QGeoRouteSegment> segIt(this->route->route().routeSegments());
    //while (segIt.hasNext()) {
    //    QListIterator<QGeoCoordinate> coordIt(segIt.next().path());
    QGeoRouteSegment segment = this->route->route().firstRouteSegment();
    while (segment.isValid()) {
        QListIterator<QGeoCoordinate> coordIt(segment.path());
        while (coordIt.hasNext()) {
            QGeoCoordinate coord = coordIt.next();

            if (!coord.isValid())
                continue;

            points.append(tiledMapData->coordinateToWorldReferencePosition(coord));
        }
        segment = segment.nextRouteSegment();
    }

    updateData();
}

void QGeoTiledMapRouteObjectInfo::penChanged(const QPen &pen)
{
    pathItem->setPen(route->pen());
    updateItem();
}

void QGeoTiledMapRouteObjectInfo::detailLevelChanged(quint32 detailLevel)
{
    updateData();
}

void QGeoTiledMapRouteObjectInfo::zoomLevelChanged(qreal zoomLevel)
{
    updateData();
}

void QGeoTiledMapRouteObjectInfo::updateData()
{
    distanceFilteredPoints.clear();

    if (!points.isEmpty()) {
        QPointF lastPoint = points.at(0);
        distanceFilteredPoints.append(points.at(0));
        for (int i = 1; i < points.size() - 1; ++i) {
            if ((lastPoint - points.at(i)).manhattanLength() >= route->detailLevel() * tiledMapData->zoomFactor()) {
                distanceFilteredPoints.append(points.at(i));
                lastPoint = points.at(i);
            }
        }

        distanceFilteredPoints.append(points.at(points.size() - 1));
    }
    setValid((distanceFilteredPoints.size() >= 2));

    if (valid())
        updateVisible();
}

void QGeoTiledMapRouteObjectInfo::windowSizeChanged(const QSizeF &windowSize)
{
    if (valid())
        updateVisible();
}

void QGeoTiledMapRouteObjectInfo::centerChanged(const QGeoCoordinate &coordinate)
{
    if (valid())
        updateVisible();
}

void QGeoTiledMapRouteObjectInfo::updateVisible()
{
    QPainterPath painterPath;

    bool offScreen = true;

    for (int i = 0; i < distanceFilteredPoints.size(); ++i) {
        if (!offScreen)
            painterPath.lineTo(distanceFilteredPoints.at(i));

        bool wasOffScreen = offScreen;

        QPointF point1 = distanceFilteredPoints.at(i);
        QPointF point2 = distanceFilteredPoints.at(i + 1 < distanceFilteredPoints.size() ? i + 1 : i);
        QPointF midpoint = (point1 + point2) / 2.0;

        QRect maxZoomScreenRect = tiledMapData->worldReferenceViewportRect();

        offScreen = !(maxZoomScreenRect.contains(point1.toPoint())
                      || maxZoomScreenRect.contains(point2.toPoint())
                      || maxZoomScreenRect.contains(midpoint.toPoint()));

        if (wasOffScreen && !offScreen)
            painterPath.moveTo(distanceFilteredPoints.at(i));
    }

    pathItem->setPath(painterPath);
    updateItem();
}

#include "moc_qgeotiledmaprouteobjectinfo_p.cpp"

QTM_END_NAMESPACE

