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

#include "qgeotiledmappolylineobjectinfo_p.h"
//#include "makepoly_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomappolylineobject.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapPolylineObjectInfo::QGeoTiledMapPolylineObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    polyline = static_cast<QGeoMapPolylineObject*>(mapObject);

    connect(polyline,
            SIGNAL(pathChanged(QList<QGeoCoordinate>)),
            this,
            SLOT(pathChanged(QList<QGeoCoordinate>)));
    connect(polyline,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;

    penChanged(polyline->pen());
    pathChanged(polyline->path());
}

QGeoTiledMapPolylineObjectInfo::~QGeoTiledMapPolylineObjectInfo() {}

void QGeoTiledMapPolylineObjectInfo::pathChanged(const QList<QGeoCoordinate> &/*path*/)
{
    genPath();
    updateItem();
}

void QGeoTiledMapPolylineObjectInfo::penChanged(const QPen &/*pen*/)
{
    pathItem->setPen(polyline->pen());
    updateItem();
}

void QGeoTiledMapPolylineObjectInfo::genPath()
{
    QPainterPath p;

    QList<QGeoCoordinate> path = polyline->path();

    if (path.size() > 0) {
        QGeoCoordinate origin = path.at(0);
        double ox = origin.longitude() * 3600.0;
        double oy = origin.latitude() * 3600.0;

        double oldx = ox;

        p.moveTo(0, 0);
        for (int i = 1; i < path.size(); ++i) {
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

            p.lineTo(x - ox, y - oy);

            oldx = x;
        }
    }

    pathItem->setPath(p);
}

#include "moc_qgeotiledmappolylineobjectinfo_p.cpp"

QTM_END_NAMESPACE

