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

#include "qgeotiledmaprectangleobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomaprectangleobject_p.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
        : QGeoTiledMapObjectInfo(mapObjectPrivate),
        rectangleItem1(0),
        rectangleItem2(0)
{
    rectangle = static_cast<const QGeoMapRectangleObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

void QGeoTiledMapRectangleObjectInfo::objectUpdate()
{
    QPoint topLeft = mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->bounds.topLeft());
    QPoint bottomRight = mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->bounds.bottomRight());

    bounds = QRectF(topLeft, bottomRight);

    QRectF bounds1 = bounds;
    QRectF bounds2;

    if (bounds1.right() < bounds1.left()) {
        bounds1.setRight(bounds1.right() + mapData->maxZoomSize.width());
        bounds2 = bounds1.translated(-mapData->maxZoomSize.width(), 0);
    }

    if (!rectangleItem1)
        rectangleItem1 = new QGraphicsRectItem();

    if (bounds2.isValid()) {
        if (!rectangleItem2)
            rectangleItem2 = new QGraphicsRectItem(rectangleItem1);
    } else {
        if (rectangleItem2) {
            delete rectangleItem2;
            rectangleItem2 = 0;
        }
    }

    rectangleItem1->setRect(bounds1);
    if (rectangleItem2)
        rectangleItem2->setRect(bounds2);

    rectangleItem1->setBrush(rectangle->brush);
    if (rectangleItem2)
        rectangleItem2->setBrush(rectangle->brush);

    mapUpdate();

    graphicsItem1 = rectangleItem1;
    graphicsItem2 = rectangleItem2;
}

void QGeoTiledMapRectangleObjectInfo::mapUpdate()
{
    if (rectangleItem1) {
        QPen pen = rectangle->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        rectangleItem1->setPen(pen);
        if (rectangleItem2)
            rectangleItem2->setPen(pen);
    }
}

QTM_END_NAMESPACE


