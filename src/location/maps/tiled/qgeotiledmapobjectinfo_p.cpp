/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmapobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"
#include "qgeoboundingbox.h"
#include "qgeomapobjectengine_p.h"

#include <QGraphicsScene>

#include <QPolygonF>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QGeoTiledMapObjectInfo::QGeoTiledMapObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoMapObjectInfo(mapData, mapObject),
      inited(false),
      updateAfterInit(false),
      graphicsItem(0)
{
    tiledMapData = mapData;
    tiledMapDataPrivate = static_cast<QGeoTiledMapDataPrivate*>(mapData->d_ptr);
}

QGeoTiledMapObjectInfo::~QGeoTiledMapObjectInfo()
{
    if (graphicsItem) {
        delete graphicsItem;
        graphicsItem = 0;
    }
}

void QGeoTiledMapObjectInfo::init()
{
    if (graphicsItem) {
        graphicsItem->setZValue(mapObject()->zValue());
        graphicsItem->setVisible(mapObject()->isVisible());
        //graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable);
    }
    inited = true;
    if (updateAfterInit) {
        tiledMapData->updateMapDisplay();
        updateAfterInit = false;
    }
}

QGeoMapObject* QGeoTiledMapObjectInfo::mapObject() const
{
    return QGeoMapObjectInfo::mapObject();
}

void QGeoTiledMapObjectInfo::zValueChanged(int zValue)
{
    if (graphicsItem) {
        graphicsItem->setZValue(zValue);
        updateItem();
        if (tiledMapDataPrivate && tiledMapDataPrivate->oe)
            tiledMapDataPrivate->oe->rebuildScenes();
    }
}

void QGeoTiledMapObjectInfo::visibleChanged(bool visible)
{
    if (graphicsItem) {
        graphicsItem->setVisible(visible);
        updateItem();
    }
}

void QGeoTiledMapObjectInfo::selectedChanged(bool /*selected*/)
{
    // don't want to draw the selection box
//    if (graphicsItem) {
//        graphicsItem->setSelected(selected);
//        updateItem();
//    }
}

void QGeoTiledMapObjectInfo::originChanged(const QGeoCoordinate &origin)
{
    if (graphicsItem)
        updateItem();
}

void QGeoTiledMapObjectInfo::unitsChanged(QGeoMapObject::CoordinateUnit units)
{
    if (graphicsItem)
        updateItem();
}

void QGeoTiledMapObjectInfo::transformTypeChanged(QGeoMapObject::TransformType transformType)
{
    if (graphicsItem)
        updateItem();
}

QGeoBoundingBox QGeoTiledMapObjectInfo::boundingBox() const
{
    if (!graphicsItem || !tiledMapData)
        return QGeoBoundingBox();

    QGeoMapObjectEngine *e = tiledMapDataPrivate->oe;

    QGeoMapObject *object = mapObject();

    e->updateTransforms();

    if (e->latLonExact.contains(object)) {
        QList<QGraphicsItem*> items = e->latLonExact.values(object);
        QGeoBoundingBox box;
        foreach (QGraphicsItem *item, items) {
            QRectF latLonBounds = item->boundingRect();
            QPointF topLeft = latLonBounds.bottomLeft();
            if (topLeft.x() >= 180.0 * 3600.0)
                topLeft.setX(topLeft.x() - 360.0 * 3600.0);
            if (topLeft.x() < -180.0 * 3600.0)
                topLeft.setX(topLeft.x() + 360.0 * 3600.0);

            QPointF bottomRight = latLonBounds.topRight();
            if (bottomRight.x() >= 180.0 * 3600.0)
                bottomRight.setX(bottomRight.x() - 360.0 * 3600.0);
            if (bottomRight.x() < -180.0 * 3600.0)
                bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);

            QGeoCoordinate tlc(topLeft.y() / 3600.0, topLeft.x() / 3600.0);
            QGeoCoordinate brc(bottomRight.y() / 3600.0, bottomRight.x() / 3600.0);

            return QGeoBoundingBox(tlc, brc);

            // it looks like the following is overkill
//            if (box.isValid()) {
//                box |= QGeoBoundingBox(tlc, brc);
//            } else {
//                box = QGeoBoundingBox(tlc, brc);
//            }
        }
        return box;
    } else {
        QTransform trans = e->latLonTrans.value(object);

        QRectF bounds = graphicsItem->boundingRect();
        QPolygonF poly = bounds * trans;

        QRectF latLonBounds = poly.boundingRect();
        QPointF topLeft = latLonBounds.bottomLeft();
        if (topLeft.x() >= 180.0 * 3600.0)
            topLeft.setX(topLeft.x() - 360.0 * 3600.0);
        if (topLeft.x() < -180.0 * 3600.0)
            topLeft.setX(topLeft.x() + 360.0 * 3600.0);

        QPointF bottomRight = latLonBounds.topRight();
        if (bottomRight.x() >= 180.0 * 3600.0)
            bottomRight.setX(bottomRight.x() - 360.0 * 3600.0);
        if (bottomRight.x() < -180.0 * 3600.0)
            bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);

        QGeoCoordinate tlc(topLeft.y() / 3600.0, topLeft.x() / 3600.0);
        QGeoCoordinate brc(bottomRight.y() / 3600.0, bottomRight.x() / 3600.0);

        return QGeoBoundingBox(tlc, brc);
    }
}

bool QGeoTiledMapObjectInfo::contains(const QGeoCoordinate &coordinate) const
{
    if (!graphicsItem || !tiledMapData)
        return false;

    QGeoMapObjectEngine *e = tiledMapDataPrivate->oe;

    e->updateTransforms();
    QPointF latLonPoint(coordinate.longitude()*3600.0, coordinate.latitude()*3600.0);

    QGeoMapObject *object = mapObject();

    if (e->latLonExact.contains(object)) {
        QList<QGraphicsItem*> items = e->latLonExact.values(object);
        foreach (QGraphicsItem *item, items) {
            if (item->contains(latLonPoint))
                return true;
        }
    } else {
        QList<QTransform> transList = e->latLonTrans.values(object);
        foreach (QTransform trans, transList) {
            bool ok;
            QTransform inv = trans.inverted(&ok);
            if (!ok)
                continue;

            QPointF localPoint = latLonPoint * inv;

            if (graphicsItem->contains(localPoint))
                return true;
        }
    }

    return false;
}

void QGeoTiledMapObjectInfo::updateItem(const QRectF& target)
{
    if (!inited) {
        updateAfterInit = true;
        return;
    }

    QGeoMapObject *object = mapObject();
    if (object)
        tiledMapDataPrivate->update(object);
    if (graphicsItem)
        tiledMapData->triggerUpdateMapDisplay(target);
}

#include "moc_qgeotiledmapobjectinfo_p.cpp"

QTM_END_NAMESPACE

