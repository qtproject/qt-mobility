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
        : pt1(QGeoCoordinate()), pt2(QGeoCoordinate()),
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
QPen QMapLine::pen() const
{
    Q_D(const QMapLine);
    return d->p;
}

/*!
   \fn QGeoCoordinate QMapLine::point1() const

   Returns the first end point (as a geo coordinate) of the line.
*/
QGeoCoordinate QMapLine::point1() const
{
    Q_D(const QMapLine);
    return d->pt1;
}

/*!
   \fn QGeoCoordinate QMapLine::point2() const

   Returns the second end point (as a geo coordinate) of the line.
*/
QGeoCoordinate QMapLine::point2() const
{
    Q_D(const QMapLine);
    return d->pt2;
}

void QMapLine::compMapCoords()
{
    Q_D(QMapLine);

    if (!d->mapView)
        return;

    QPointF p1 = d->mapView->geoToMap(d->pt1);
    QPointF p2 = d->mapView->geoToMap(d->pt2);
    d->line = QLineF(p1, p2);

    //lines are drawn from west to east
    if (d->pt2.longitude() < d->pt1.longitude()) {
        p2.rx() += d->mapView->mapWidth();
        d->line.setP2(p2);
    }

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

    qint64 mapWidth = static_cast<qint64>(d->mapView->mapWidth());
    QPen oldPen = painter->pen();
    painter->setPen(d->p);
    d->line.translate(-viewPort.left(), -viewPort.top());
    painter->drawLine(d->line);
    d->line.translate(viewPort.left(), viewPort.top());

    //Is view port wrapping around date line?
    qreal right = viewPort.right();
    for (int i = 1;right >= mapWidth;++i, right -= mapWidth) {
        qint64 width = mapWidth * i;
        d->line.translate(-viewPort.left(), -viewPort.top());
        d->line.translate(width, 0);
        painter->drawLine(d->line);
        d->line.translate(-width, 0);
        d->line.translate(viewPort.left(), viewPort.top());
    }
    //Is line crossing date line?
    if (d->line.x2() >= mapWidth) {
        d->line.translate(-viewPort.left(), -viewPort.top());
        d->line.translate(-mapWidth, 0);
        painter->drawLine(d->line);
        d->line.translate(mapWidth, 0);
        d->line.translate(viewPort.left(), viewPort.top());
    }

    painter->setPen(oldPen);
}

bool QMapLine::intersects(const QRectF& rect) const
{
    Q_D(const QMapLine);
    return QMapObject::intersect(rect, d->line);
}

QTM_END_NAMESPACE
