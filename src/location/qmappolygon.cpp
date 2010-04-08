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

#include "qmappolygon.h"
#include "qmappolygon_p.h"
#include <QPainter>

QTM_BEGIN_NAMESPACE

QMapPolygonPrivate::QMapPolygonPrivate()
    :poly(QList<QGeoCoordinate>()),p(QPen()), br(QBrush()),
     path(QPainterPath())
{
}
/*!
*   \class QMapPolygon
*   \brief The QMapPolygon class represents a polygon that has been added to the
           map.
*   \ingroup location
*/

/*!
    Constructs a polygon with its verticies defined in \a polygon.
    The polygon oulined is drawn with the given \a pen and filled with the given \a brush.
    The polygon is displayed in the layer specified by \a layerIndex.
*/
QMapPolygon::QMapPolygon(const QList<QGeoCoordinate>& polygon,
                         const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(*new QMapPolygonPrivate, QMapObject::PolygonObject, layerIndex)
{
    Q_D(QMapPolygon);
    d->poly = polygon;
    d->p = pen;
    d->br = brush;
}

/*!
    \internal
*/
QMapPolygon::QMapPolygon(QMapPolygonPrivate &dd, const QList<QGeoCoordinate>& polygon,
                         const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(dd, QMapObject::PolygonObject, layerIndex)
{
    Q_D(QMapPolygon);
    d->poly = polygon;
    d->p = pen;
    d->br = brush;
}

/*!
    \fn QList<QGeoCoordinate> QMapPolygon::polygon() const

    Returns the polygon as a list of geo coordinates.
*/
QList<QGeoCoordinate> QMapPolygon::polygon() const {
    Q_D(const QMapPolygon);
    return d->poly;
}

/*!
    \fn QPen QMapPolygon::pen() const

    Returns the pen used for drawing the polygon outline.
*/
QPen QMapPolygon::pen() const {
    Q_D(const QMapPolygon);
    return d->p;
}

/*!
    \fn QBrush QMapPolygon::brush() const

    The brush used for filling in the polygon.
*/
QBrush QMapPolygon::brush() const {
    Q_D(const QMapPolygon);
    return d->br;
}

void QMapPolygon::compMapCoords()
{
    Q_D(QMapPolygon);

    if (!d->mapView)
        return;

    if (d->poly.count() < 2)
        return;

    QPolygonF pgn;

    for (int i = 0; i < d->poly.count(); i++) {
        pgn << d->mapView->geoToMap(d->poly[i]);
    }

    pgn << d->mapView->geoToMap(d->poly[0]);
    d->path = QPainterPath();
    d->path.addPolygon(pgn);
    //compute intersecting map tiles now
    d->intersectingTiles.clear();
    compIntersectingTiles(d->path.boundingRect());
}

void QMapPolygon::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapPolygon);

    if (!d->mapView)
        return;

    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();
    painter->setPen(d->p);
    painter->setBrush(d->br);
    d->path.translate(-viewPort.topLeft());
    painter->drawPath(d->path);
    qint64 mapWidth = static_cast<qint64>(d->mapView->mapWidth());

    //Is view port wrapping around date line?
    qreal right = viewPort.right();
    for(int i=1;right>=mapWidth;++i,right -= mapWidth) {
        qint64 width = mapWidth*i;
        d->path.translate(width, 0);
        painter->drawPath(d->path);
        d->path.translate(-width, 0);
    }

    d->path.translate(viewPort.topLeft());
    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

bool QMapPolygon::intersects(const QRectF& rect) const
{
    Q_D(const QMapPolygon);
    return d->path.intersects(rect);
}

QTM_END_NAMESPACE
