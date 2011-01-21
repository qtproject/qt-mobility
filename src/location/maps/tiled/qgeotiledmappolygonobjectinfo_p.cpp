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
//#include "makepoly_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappolygonobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPolygonObjectInfo::QGeoTiledMapPolygonObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    polygon = static_cast<QGeoMapPolygonObject*>(mapObject);

    connect(polygon,
            SIGNAL(pathChanged(QList<QGeoCoordinate>)),
            this,
            SLOT(pathChanged(QList<QGeoCoordinate>)));
    connect(polygon,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(polygon,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    polygonItem = new QGraphicsPolygonItem();
    graphicsItem = polygonItem;

    penChanged(polygon->pen());
    brushChanged(polygon->brush());
    pathChanged(polygon->path());
}

QGeoTiledMapPolygonObjectInfo::~QGeoTiledMapPolygonObjectInfo() {}

void QGeoTiledMapPolygonObjectInfo::pathChanged(const QList<QGeoCoordinate> &/*path*/)
{
    points = createPolygon(polygon->path(), tiledMapData, true);
    if (points.size() >= 3) {
        polygonItem->setPolygon(points);
        setValid(true);
    } else {
        setValid(false);
    }
    updateItem();
}

void QGeoTiledMapPolygonObjectInfo::penChanged(const QPen &/*pen*/)
{
    polygonItem->setPen(polygon->pen());
    updateItem();
}

void QGeoTiledMapPolygonObjectInfo::brushChanged(const QBrush &/*brush*/)
{
    polygonItem->setBrush(polygon->brush());
    updateItem();
}

#include "moc_qgeotiledmappolygonobjectinfo_p.cpp"

QTM_END_NAMESPACE

