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

#include "qgeotiledmapmarkerobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapmarkerobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapMarkerObjectInfo::QGeoTiledMapMarkerObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject)
        : QGeoTiledMapObjectInfo(mapData, mapObject),
        pixmapItem(0)

{
    marker = static_cast<QGeoMapMarkerObject*>(mapObject);
}

QGeoTiledMapMarkerObjectInfo::~QGeoTiledMapMarkerObjectInfo() {}

void QGeoTiledMapMarkerObjectInfo::objectUpdate()
{
    QPointF position = tiledMapData->coordinateToWorldPixel(marker->coordinate());

    if (!pixmapItem)
        pixmapItem = new QGraphicsPixmapItem();

    pixmapItem->setPixmap(marker->icon());
    pixmapItem->setOffset(position);
    pixmapItem->setTransformOriginPoint(position);

    mapUpdate();

    graphicsItem1 = pixmapItem;
    graphicsItem2 = 0;
}

void QGeoTiledMapMarkerObjectInfo::mapUpdate()
{
    if (pixmapItem) {
        pixmapItem->resetTransform();
        pixmapItem->setScale(tiledMapDataPrivate->zoomFactor);
        pixmapItem->translate(marker->anchor().x() * tiledMapDataPrivate->zoomFactor,
                              marker->anchor().y() * tiledMapDataPrivate->zoomFactor);
    }
}

QTM_END_NAMESPACE

