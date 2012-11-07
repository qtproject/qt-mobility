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

#include "qgeotiledmaprectangleobjectinfo_p.h"

#include <QGraphicsPolygonItem>

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeoboundingbox.h"

#include "qgeomaprectangleobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
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

    polygonItem = new QGraphicsPolygonItem();
    graphicsItem = polygonItem;

    topLeftChanged(rectangle->topLeft());
    bottomRightChanged(rectangle->bottomRight());
    penChanged(rectangle->pen());
    brushChanged(rectangle->brush());
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

void QGeoTiledMapRectangleObjectInfo::topLeftChanged(const QGeoCoordinate &/*topLeft*/)
{
    regenPolygon();
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::bottomRightChanged(const QGeoCoordinate &/*bottomRight*/)
{
    regenPolygon();
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::penChanged(const QPen &/*pen*/)
{
    polygonItem->setPen(rectangle->pen());
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::brushChanged(const QBrush &/*brush*/)
{
    polygonItem->setBrush(rectangle->brush());
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::regenPolygon()
{
    QPolygonF poly;

    if (!rectangle->bounds().isValid())
        return;

    const QGeoCoordinate tl = rectangle->bounds().topLeft();
    if (!tl.isValid())
        return;

    const QGeoCoordinate br = rectangle->bounds().bottomRight();
    if (!br.isValid())
        return;

    double left = tl.longitude() * 3600.0;
    double right = br.longitude() * 3600.0;
    double top = tl.latitude() * 3600.0;
    double bottom = br.latitude() * 3600.0;

    if (left > right)
        right += 360.0 * 3600.0;

    poly << QPointF(left, top);
    poly << QPointF(right, top);
    poly << QPointF(right, bottom);
    poly << QPointF(left, bottom);

    polygonItem->setPolygon(poly);
}

#include "moc_qgeotiledmaprectangleobjectinfo_p.cpp"

QTM_END_NAMESPACE


