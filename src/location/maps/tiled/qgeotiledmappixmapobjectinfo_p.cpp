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

#include "qgeotiledmappixmapobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappixmapobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPixmapObjectInfo::QGeoTiledMapPixmapObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject),
      pixmapItem2(0)
{
    pixmap = static_cast<QGeoMapPixmapObject*>(mapObject);

    connect(pixmap,
            SIGNAL(coordinateChanged(QGeoCoordinate)),
            this,
            SLOT(coordinateChanged(QGeoCoordinate)));
    connect(pixmap,
            SIGNAL(pixmapChanged(QPixmap)),
            this,
            SLOT(pixmapChanged(QPixmap)));
    connect(pixmap,
            SIGNAL(offsetChanged(QPoint)),
            this,
            SLOT(offsetChanged(QPoint)));

    pixmapItem1 = new QGraphicsPixmapItem();
    graphicsItem = pixmapItem1;

    pixmapChanged(this->pixmap->pixmap());
    coordinateChanged(this->pixmap->coordinate());
}

QGeoTiledMapPixmapObjectInfo::~QGeoTiledMapPixmapObjectInfo() {}

QGeoBoundingBox QGeoTiledMapPixmapObjectInfo::boundingBox() const
{
    return boundingBox_;
}

void QGeoTiledMapPixmapObjectInfo::updateValidity()
{
    setValid((pixmap->coordinate().isValid() && !pixmap->pixmap().isNull()));
}

void QGeoTiledMapPixmapObjectInfo::coordinateChanged(const QGeoCoordinate &/*coordinate*/)
{
    updateValidity();
    if (valid())
        update();
}

void QGeoTiledMapPixmapObjectInfo::pixmapChanged(const QPixmap &/*pixmap*/)
{
    updateValidity();
    if (!this->pixmap->pixmap().isNull()) {
        pixmapItem1->setPixmap(this->pixmap->pixmap());
        if (pixmapItem2)
            pixmapItem2->setPixmap(this->pixmap->pixmap());
    }
    if (valid())
        updateItem();
}

void QGeoTiledMapPixmapObjectInfo::offsetChanged(const QPoint &/*offset*/)
{
    if (valid())
        update();
}

void QGeoTiledMapPixmapObjectInfo::zoomLevelChanged(qreal /*zoomLevel*/)
{
    if (valid())
        update();
}

void QGeoTiledMapPixmapObjectInfo::update()
{
    int zoomFactor = tiledMapData->zoomFactor();
    
    QPointF pos = tiledMapData->coordinateToWorldReferencePosition(pixmap->coordinate());

    QPointF offset = pixmap->offset();
    offset *= zoomFactor;
    pos += offset;

    int width = tiledMapData->worldReferenceSize().width();
    int x = pos.x();

    if (x > width)
        x -= width;
    if (x < 0)
        x += width;

    pos.setX(x);

    int rightBound = pos.x() + pixmap->pixmap().width() * zoomFactor;
    int bottomBound = pos.y() + pixmap->pixmap().height() * zoomFactor;

    boundingBox_ = QGeoBoundingBox(tiledMapData->worldReferencePositionToCoordinate(pos.toPoint()),
                                   tiledMapData->worldReferencePositionToCoordinate(QPoint(rightBound, bottomBound)));

    QPointF pos2 = QPointF(-1.0 * static_cast<qreal>(width / zoomFactor), 0);

    if (rightBound > width ) {
        if (!pixmapItem2) {
            pixmapItem2 = new QGraphicsPixmapItem(pixmapItem1);
            pixmapItem2->setPixmap(pixmap->pixmap());
        }
    } else {
        if (pixmapItem2) {
            delete pixmapItem2;
            pixmapItem2 = 0;
        }
    }

    pixmapItem1->setScale(zoomFactor);

    pixmapItem1->setPos(pos);
    if (pixmapItem2)
        pixmapItem2->setPos(pos2);

    updateItem();
}

#include "moc_qgeotiledmappixmapobjectinfo_p.cpp"

QTM_END_NAMESPACE

