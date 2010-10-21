/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmaprectangleobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeoboundingbox.h"

#include "qgeomaprectangleobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject),
      rectangleItem2(0)
{
    rectangle = static_cast<QGeoMapRectangleObject*>(mapObject);

    connect(rectangle,
            SIGNAL(topLeftChanged(QGeoCoordinate)),
            this,
            SLOT(topLeftChanged(QGeoCoordinate)));
    connect(rectangle,
            SIGNAL(bottomRightChanged(QGeoCoordinate)),
            this,
            SLOT(bottomRightChanged(QGeoCoordinate)));
    connect(rectangle,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(rectangle,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    rectangleItem1 = new QGraphicsRectItem();
    graphicsItem = rectangleItem1;

    penChanged(rectangle->pen());
    brushChanged(rectangle->brush());

    updateValidity();
    if (valid())
        update();
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

void QGeoTiledMapRectangleObjectInfo::updateValidity()
{
    setValid((rectangle->topLeft().isValid() && rectangle->bottomRight().isValid()));
}

void QGeoTiledMapRectangleObjectInfo::topLeftChanged(const QGeoCoordinate &topLeft)
{
    updateValidity();
    if (valid())
        update();
}

void QGeoTiledMapRectangleObjectInfo::bottomRightChanged(const QGeoCoordinate &bottomRight)
{
    updateValidity();
    if (valid())
        update();
}

void QGeoTiledMapRectangleObjectInfo::penChanged(const QPen &pen)
{
    rectangleItem1->setPen(pen);
    if (rectangleItem2)
        rectangleItem2->setPen(pen);
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::brushChanged(const QBrush &brush)
{
    rectangleItem1->setBrush(brush);
    if (rectangleItem2)
        rectangleItem2->setBrush(brush);
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::update()
{
#if 1
    QGeoCoordinate coord1 = rectangle->bounds().topLeft();
    QGeoCoordinate coord2 = rectangle->bounds().bottomRight();

    const qreal lng1 = coord1.longitude();
    const qreal lng2 = coord2.longitude();

    // is the dateline crossed = different sign AND gap is large enough
    const bool crossesDateline = lng1 * lng2 < 0 && abs(lng1 - lng2) > 180;
    // is the shortest route east = dateline crossed XOR longitude is east by simple comparison
    const bool goesEast = crossesDateline != (lng2 > lng1);

    // calculate base points
    QPointF point1 = tiledMapData->coordinateToWorldReferencePosition(coord1);
    QPointF point2 = tiledMapData->coordinateToWorldReferencePosition(coord2);

    QRectF bounds1 = QRectF(point1, point2).normalized();
    QRectF bounds2;

    // if the dateline is crossed, draw "around" the map over the chosen pole
    if (crossesDateline && goesEast) {
        // direction = positive if east, negative otherwise
        const qreal dir = goesEast ? 1 : -1;

        int width = tiledMapData->worldReferenceSize().width();

        // lastPoint on the other side
        QPointF point1_ = point1 - QPointF(width * dir, 0);

        // point on this side
        QPointF point2_ = point2 + QPointF(width * dir, 0);

        bounds1 = QRectF(point1_, point2).normalized();
        bounds2 = QRectF(point1, point2_).normalized();
    }

#else
    // Old code, to be removed.
    QPoint topLeft = mapData->q_ptr->coordinateToWorldPixel(rectangle->bounds.topLeft());
    QPoint bottomRight = mapData->q_ptr->coordinateToWorldPixel(rectangle->bounds.bottomRight());

    bounds = QRectF(topLeft, bottomRight);

    QRectF bounds1 = bounds;
    QRectF bounds2;

    if (bounds1.right() < bounds1.left()) {
        bounds1.setRight(bounds1.right() + tiledMapDataPrivate->maxZoomSize.width());
        bounds2 = bounds1.translated(-tiledMapDataPrivate->maxZoomSize.width(), 0);
    }
#endif

    if (bounds2.isValid()) {
        if (!rectangleItem2)
            rectangleItem2 = new QGraphicsRectItem(rectangleItem1);
            rectangleItem2->setPen(rectangle->pen());
            rectangleItem2->setBrush(rectangle->brush());
    } else {
        if (rectangleItem2) {
            delete rectangleItem2;
            rectangleItem2 = 0;
        }
    }

    rectangleItem1->setRect(bounds1);
    if (rectangleItem2)
        rectangleItem2->setRect(bounds2);

    updateItem();
}

#include "moc_qgeotiledmaprectangleobjectinfo_p.cpp"

QTM_END_NAMESPACE


