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

#include "qgeotiledmappolylineobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappolylineobject_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPolylineObjectInfo::QGeoTiledMapPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
        : QGeoTiledMapObjectInfo(mapObjectPrivate)
        , pathItem(0)
{
    polyline = static_cast<const QGeoMapPolylineObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapPolylineObjectInfo::~QGeoTiledMapPolylineObjectInfo() {}

void QGeoTiledMapPolylineObjectInfo::objectUpdate()
{
    QList<QGeoCoordinate> path = polyline->path;

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        points.append(mapData->q_ptr->coordinateToWorldPixel(coord));
    }

    if (points.size() < 2)
        return;

    QPainterPath painterPath(points.at(0));
    for (int i = 1; i < points.size(); ++i)
        painterPath.lineTo(points.at(i));

    if (!pathItem)
        pathItem = new QGraphicsPathItem();

    pathItem->setPath(painterPath);
    mapUpdate();

    graphicsItem1 = pathItem;
    graphicsItem2 = 0;
}

void QGeoTiledMapPolylineObjectInfo::mapUpdate()
{
    if (pathItem) {
        QPen pen = polyline->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        pathItem->setPen(pen);
    }
}

QTM_END_NAMESPACE

