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

#include "qgeotiledmapobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include <QGraphicsScene>

QTM_BEGIN_NAMESPACE

QGeoTiledMapObjectInfo::QGeoTiledMapObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
        : QGeoMapObjectInfo(mapObjectPrivate),
        graphicsItem1(0),
        graphicsItem2(0)
{
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapObjectInfo::~QGeoTiledMapObjectInfo()
{
    if (graphicsItem1)
        delete graphicsItem1;
    if (graphicsItem2)
        delete graphicsItem2;
}

void QGeoTiledMapObjectInfo::addToParent()
{
    if (graphicsItem1) {
        mapData->scene->addItem(graphicsItem1);
        mapData->itemMap.insert(graphicsItem1, mapObjectPrivate->q_ptr);
    }
    if (graphicsItem2) {
        mapData->scene->addItem(graphicsItem2);
        mapData->itemMap.insert(graphicsItem2, mapObjectPrivate->q_ptr);
    }
}

void QGeoTiledMapObjectInfo::removeFromParent()
{
    if (graphicsItem1) {
        mapData->scene->removeItem(graphicsItem1);
        mapData->itemMap.remove(graphicsItem1);
    }
    if (graphicsItem2) {
        mapData->scene->removeItem(graphicsItem2);
        mapData->itemMap.remove(graphicsItem2);
    }
}

QGeoBoundingBox QGeoTiledMapObjectInfo::boundingBox() const
{
    if (!graphicsItem1)
        return QGeoBoundingBox();

    QRectF rect = graphicsItem1->boundingRect();
    QGeoCoordinate topLeft = mapData->q_ptr->worldPixelToCoordinate(rect.topLeft().toPoint());
    QGeoCoordinate bottomRight = mapData->q_ptr->worldPixelToCoordinate(rect.bottomRight().toPoint());

    return QGeoBoundingBox(topLeft, bottomRight);
}

bool QGeoTiledMapObjectInfo::contains(const QGeoCoordinate &coord) const
{
    QPoint point = mapData->q_ptr->coordinateToWorldPixel(coord);

    if (graphicsItem1 && graphicsItem1->contains(point))
        return true;

    if (graphicsItem2 && graphicsItem2->contains(point))
        return true;

    return false;
}

QTM_END_NAMESPACE

