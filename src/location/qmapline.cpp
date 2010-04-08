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

#include "qmapline.h"
#include "qmapline_p.h"

QTM_BEGIN_NAMESPACE

QMapLinePrivate::QMapLinePrivate()
    :pt1(QGeoCoordinate()), pt2(QGeoCoordinate()),
    p(QPen()), line(QLineF())
{
}

/*!
    \class QMapLine
    \brief The QMapLine class represents a line on a map.
*/

/*!
* Constructs a map line with endpoints defined by \a point1
* and \a point2.  The line is drawn using the \a pen and at the layer
* specified by \a layerIndex.
*/
QMapLine::QMapLine(const QGeoCoordinate& point1,
                   const QGeoCoordinate& point2, const QPen& pen, quint16 layerIndex)
        : QMapObject(*new QMapLinePrivate, QMapObject::LineObject, layerIndex)
{
    Q_D(QMapLine);
    d->pt1 = point1;
    d->pt2 = point2;
    d->p = pen;
}

/*!
    \internal
*/
QMapLine::QMapLine(QMapLinePrivate &dd, const QGeoCoordinate& point1,
                   const QGeoCoordinate& point2, const QPen& pen, quint16 layerIndex)
        : QMapObject(dd, QMapObject::LineObject, layerIndex)
{
    Q_D(QMapLine);
    d->pt1 = point1;
    d->pt2 = point2;
    d->p = pen;
}

/*!
    \fn QPen::QMapLine() const

    Returns the pen used for drawing this line.
*/
QPen QMapLine::pen() const {
    Q_D(const QMapLine);
    return d->p;
}

/*!
   \fn QGeoCoordinate QMapLine::point1() const

   Returns the first end point (as a geo coordinate) of the line.
*/
QGeoCoordinate QMapLine::point1() const {
    Q_D(const QMapLine);
    return d->pt1;
}

/*!
   \fn QGeoCoordinate QMapLine::point2() const

   Returns the second end point (as a geo coordinate) of the line.
*/
QGeoCoordinate QMapLine::point2() const {
    Q_D(const QMapLine);
    return d->pt2;
}

void QMapLine::compMapCoords()
{
    Q_D(QMapLine);

    if (!d->mapView)
        return;

    d->line = connectShortest(*(d->mapView), d->pt1, d->pt2);

    //compute intersecting map tiles now
    d->intersectingTiles.clear();
    QRectF box = QMapObject::boundingRect(d->line);
    compIntersectingTiles(box);
}

void QMapLine::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapLine);

    if (!d->mapView)
        return;

    painter->save();

    painter->setPen(d->p);
    painter->translate(-viewPort.left(), -viewPort.top());
    painter->drawLine(d->line);

    painter->restore();
}

bool QMapLine::intersects(const QRectF& rect) const
{
    Q_D(const QMapLine);
    return QMapObject::intersect(rect, d->line);
}

QLineF QMapLine::connectShortest(const QMapView &mapView, const QGeoCoordinate& point1, const QGeoCoordinate& point2)
{
    //order from west to east
    QGeoCoordinate pt1;
    QGeoCoordinate pt2;

    if (point1.longitude() < point2.longitude()) {
        pt1 = point1;
        pt2 = point2;
    } else {
        pt1 = point2;
        pt2 = point1;
    }

    QPointF mpt1 = mapView.geoToMap(pt1);
    QPointF mpt2 = mapView.geoToMap(pt2);

    if (pt2.longitude() - pt1.longitude() > 180.0) {
        mpt1.rx() += mapView.mapWidth();
        return QLineF(mpt2, mpt1);
    }

    return QLineF(mpt1, mpt2);
}

QTM_END_NAMESPACE
