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

#ifndef QLOCATION_MAPOBJECT_H
#define QLOCATION_MAPOBJECT_H

#include <QGraphicsItem>
#include <QRectF>
#include <QLineF>
#include <QList>
#include <QObject>

#include "qmapview.h"

QTM_BEGIN_NAMESPACE

class QMapObjectPrivate;
class Q_LOCATION_EXPORT QMapObject
{
    friend class QMapView;

public:
    enum MapObjectType {
        MarkerObject,
        LineObject,
        RectObject,
        RouteObject,
        EllipseObject,
        PolygonObject,
        PixmapObject,
        NullObject
    };

public:
    QMapObject(MapObjectType type, quint16 z = 0);

    quint16 zValue() const;
    MapObjectType type() const;

    static QRectF boundingRect(const QLineF& line);
    static bool intersect(const QRectF& rect, const QLineF& line);

protected:
    virtual bool intersects(const QRectF& rect) const = 0;
    virtual void compMapCoords() = 0;
    virtual void compIntersectingTiles(const QRectF& box);
    virtual void paint(QPainter* painter, const QRectF& viewPort) = 0;

    void setParentView(QMapView *mapView);

    QMapObject(QMapObjectPrivate &dd, MapObjectType type, quint16 z = 0);
    QMapObjectPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QMapObject)
};

QTM_END_NAMESPACE

#endif
