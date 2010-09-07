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

QGeoTiledMapRouteObjectInfo::QGeoTiledMapRouteObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject)
        : QGeoTiledMapObjectInfo(mapData, mapObject),
        pathItem(0),
        //groupItem(0),
        oldZoom(-1.0)
{
    route = static_cast<QGeoMapRouteObject*>(mapObject);
}

QGeoTiledMapRouteObjectInfo::~QGeoTiledMapRouteObjectInfo() {}

void QGeoTiledMapRouteObjectInfo::objectUpdate()
{
    QListIterator<QGeoRouteSegment> segIt(route->route().routeSegments());

    while (segIt.hasNext()) {
        QListIterator<QGeoCoordinate> coordIt(segIt.next().path());
        while (coordIt.hasNext()) {
            QGeoCoordinate coord = coordIt.next();

            if (!coord.isValid())
                continue;

            points.append(tiledMapData->coordinateToWorldPixel(coord));
        }
    }

    if (!pathItem)
        pathItem = new QGraphicsPathItem();

    mapUpdate();

    graphicsItem = pathItem;

    updateItem();
}

void QGeoTiledMapRouteObjectInfo::mapUpdate()
{
    if (!pathItem)
        return;

    if (tiledMapData->zoomLevel() != oldZoom) {
        oldZoom = tiledMapData->zoomLevel();

        distanceFilteredPoints.clear();

        QPointF lastPoint = points.at(0);
        distanceFilteredPoints.append(points.at(0));
        for (int i = 1; i < points.size() - 1; ++i) {
            if ((lastPoint - points.at(i)).manhattanLength() >= route->detailLevel() * tiledMapData->zoomFactor()) {
                distanceFilteredPoints.append(points.at(i));
                lastPoint = points.at(i);
            }
        }

        distanceFilteredPoints.append(points.at(points.size() - 1));

        pathItem->setPen(route->pen());
    }

    QPainterPath painterPath;

    if (distanceFilteredPoints.size() < 2) {
        pathItem->setPath(painterPath);
        return;
    }

    bool offScreen = true;

    for (int i = 0; i < distanceFilteredPoints.size() - 1; ++i) {
        if (!offScreen)
            painterPath.lineTo(distanceFilteredPoints.at(i));

        bool wasOffScreen = offScreen;

        QPointF point1 = distanceFilteredPoints.at(i);
        QPointF point2 = distanceFilteredPoints.at(i + 1);
        QPointF midpoint = (point1 + point2) / 2.0;

        QRect maxZoomScreenRect = tiledMapData->maxZoomScreenRect();

        offScreen = !(maxZoomScreenRect.contains(point1.toPoint())
                      || maxZoomScreenRect.contains(point2.toPoint())
                      || maxZoomScreenRect.contains(midpoint.toPoint()));

        if (wasOffScreen && !offScreen)
            painterPath.moveTo(distanceFilteredPoints.at(i));
    }

    pathItem->setPath(painterPath);
}

//QLineF QGeoTiledMapRouteObjectInfo::connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const
//{
//    //order from west to east
//    QGeoCoordinate pt1;
//    QGeoCoordinate pt2;

//    if (point1.longitude() < point2.longitude()) {
//        pt1 = point1;
//        pt2 = point2;
//    } else {
//        pt1 = point2;
//        pt2 = point1;
//    }

//    qulonglong x;
//    qulonglong y;
//    mapData->q_ptr->coordinateToWorldPixel(pt1, &x, &y);
//    QPointF mpt1(x, y);
//    mapData->q_ptr->coordinateToWorldPixel(pt2, &x, &y);
//    QPointF mpt2(x, y);

//    if (pt2.longitude() - pt1.longitude() > 180.0) {
//        mpt1.rx() += mapData->maxZoomSize.width();
//        return QLineF(mpt2, mpt1);
//    }

//    return QLineF(mpt1, mpt2);
//}

QTM_END_NAMESPACE

