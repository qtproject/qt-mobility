/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "qgeotiledmapcircleobjectinfo_p.h"

#include "qgeotiledmapobjectinfo_p.h"
#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapcircleobject.h"

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>

QTM_BEGIN_NAMESPACE

QGeoTiledMapCircleObjectInfo::QGeoTiledMapCircleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    circle = static_cast<QGeoMapCircleObject*>(mapObject);

    connect(circle,
            SIGNAL(radiusChanged(qreal)),
            this,
            SLOT(radiusChanged(qreal)));
    connect(circle,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(circle,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    ellipseItem = new QGraphicsEllipseItem;
    ellipseItem->setPos(0, 0);

    graphicsItem = ellipseItem;

    radiusChanged(circle->radius());
    brushChanged(circle->brush());
    penChanged(circle->pen());
}

QGeoTiledMapCircleObjectInfo::~QGeoTiledMapCircleObjectInfo() {}

void QGeoTiledMapCircleObjectInfo::radiusChanged(qreal /*radius*/)
{
    qreal radius = circle->radius();
    ellipseItem->setRect(-1*radius, -1*radius, 2*radius, 2*radius);
    updateItem();
}

void QGeoTiledMapCircleObjectInfo::penChanged(const QPen &pen)
{
    ellipseItem->setPen(pen);
    updateItem();
}

void QGeoTiledMapCircleObjectInfo::brushChanged(const QBrush &brush)
{
    ellipseItem->setBrush(brush);
    updateItem();
}

#include "moc_qgeotiledmapcircleobjectinfo_p.cpp"

QTM_END_NAMESPACE
