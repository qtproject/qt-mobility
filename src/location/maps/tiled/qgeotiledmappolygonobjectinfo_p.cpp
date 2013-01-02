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

#include "qgeotiledmappolygonobjectinfo_p.h"

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
    genPoly();
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

void QGeoTiledMapPolygonObjectInfo::genPoly()
{
    QPolygonF poly;

    QList<QGeoCoordinate> path = polygon->path();

    if (path.size() > 0) {
        QGeoCoordinate origin = path.at(0);
        double ox = origin.longitude() * 3600.0;
        double oy = origin.latitude() * 3600.0;

        double oldx = ox;

        poly << QPointF(0,0);
        for (int i = 0; i < path.size(); ++i) {
            QGeoCoordinate pt = path.at(i);
            double x = pt.longitude() * 3600.0;
            double y = pt.latitude() * 3600.0;

            if (qAbs(x - oldx) > 180.0 * 3600.0) {
                if (x > oldx) {
                    x -= 360.0 * 3600.0;
                } else if (x < oldx) {
                    x += 360.0 * 3600.0;
                }
            }

            poly << QPointF(x - ox, y - oy);

            oldx = x;
        }
    }

    polygonItem->setPolygon(poly);
}

#include "moc_qgeotiledmappolygonobjectinfo_p.cpp"

QTM_END_NAMESPACE

