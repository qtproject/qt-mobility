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
//#include "makepoly_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappolylineobject.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPolylineObjectInfo::QGeoTiledMapPolylineObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    polyline = static_cast<QGeoMapPolylineObject*>(mapObject);

    connect(polyline,
            SIGNAL(pathChanged(QList<QGeoCoordinate>)),
            this,
            SLOT(pathChanged(QList<QGeoCoordinate>)));
    connect(polyline,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;

    penChanged(polyline->pen());
    pathChanged(polyline->path());
}

QGeoTiledMapPolylineObjectInfo::~QGeoTiledMapPolylineObjectInfo() {}

void QGeoTiledMapPolylineObjectInfo::pathChanged(const QList<QGeoCoordinate> &/*path*/)
{
    points = createPolygon(polyline->path(), tiledMapData, false);
    if (points.size() >= 2) {
        QPainterPath painterPath;
        painterPath.addPolygon(points);
        pathItem->setPath(painterPath);
        setValid(true);
    } else {
        setValid(false);
    }
    updateItem();
}

void QGeoTiledMapPolylineObjectInfo::zoomLevelChanged(qreal /*zoomLevel*/)
{
    QPen p = polyline->pen();
    p.setWidth(p.width() * tiledMapData->zoomFactor());
    pathItem->setPen(p);
    updateItem();
}

void QGeoTiledMapPolylineObjectInfo::penChanged(const QPen &/*pen*/)
{
    QPen p = polyline->pen();
    p.setWidth(p.width() * tiledMapData->zoomFactor());
    pathItem->setPen(p);
    updateItem();
}

#include "moc_qgeotiledmappolylineobjectinfo_p.cpp"

QTM_END_NAMESPACE

