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

#include "qgeotiledmappolygonobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappolygonobject_p.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPolygonObjectInfo::QGeoTiledMapPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
        : QGeoTiledMapObjectInfo(mapObjectPrivate),
        polygonItem1(0),
        polygonItem2(0)
{
    polygon = static_cast<const QGeoMapPolygonObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapPolygonObjectInfo::~QGeoTiledMapPolygonObjectInfo() {}

void QGeoTiledMapPolygonObjectInfo::objectUpdate()
{
    QList<QGeoCoordinate> path = polygon->path;

    points.clear();

    //TODO - handle when polygons are drawn across the dateline...
    // regular graphics item with polygon item children?
    QGeoCoordinate lastCoord = path.at(0);

    qreal xoffset = 0;
    bool polygonCrossesDateline = false;
    int width = mapData->maxZoomSize.width();

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        const qreal lng = coord.longitude();
        const qreal lastLng = lastCoord.longitude();

        // is the dateline crossed = different sign AND gap is large enough
        const bool crossesDateline = lastLng * lng < 0 && abs(lastLng - lng) > 180;

        polygonCrossesDateline |= crossesDateline;

        // is the shortest route east = dateline crossed XOR longitude is east by simple comparison
        const bool goesEast = crossesDateline != (lng > lastLng);
        // direction = positive if east, negative otherwise
        const qreal dir = goesEast ? 1 : -1;

        // if the dateline is crossed, advance the offset in the given direction
        if (crossesDateline)
            xoffset += width * dir;

        if (xoffset < 0) {
            xoffset += width;
            points.translate(width, 0);
        }

        // calculate base point
        QPointF point = mapData->q_ptr->coordinateToWorldPixel(coord);

        // apply offset
        point += QPointF(xoffset, 0);

        // add point to polygon
        points.append(point);

        lastCoord = coord;
    }

    if (!polygonItem1)
        polygonItem1 = new QGraphicsPolygonItem();

    polygonItem1->setPolygon(points);
    polygonItem1->setBrush(polygon->brush);

    if (polygonCrossesDateline) {
        if (!polygonItem2)
            polygonItem2 = new QGraphicsPolygonItem();
        polygonItem2->setPolygon(points.translated(-width, 0));
        polygonItem2->setBrush(polygon->brush);
    } else {
        delete polygonItem2;
        polygonItem2 = 0;
    }
    graphicsItem1 = polygonItem1;
    graphicsItem2 = polygonItem2;
}

void QGeoTiledMapPolygonObjectInfo::mapUpdate()
{
    if (polygonItem1) {
        QPen pen = polygon->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        polygonItem1->setPen(pen);
        if (polygonItem2) polygonItem2->setPen(pen);
    }
}

QTM_END_NAMESPACE

