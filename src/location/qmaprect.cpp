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

#include "qmaprect.h"
#include "qmaprect_p.h"
#include <QPainter>

QTM_BEGIN_NAMESPACE

QMapRectPrivate::QMapRectPrivate()
    :geoTopLeft(QGeoCoordinate()),
     geoBottomRight(QGeoCoordinate()),
     p(QPen()), b(QBrush()), rect(QRect())
{
}

/*!
    \class QMapRect
    \brief The QMapRect class represents a rectangle that has been added to a map
    \group location
*/

/*!
 * Constructs a rectangle with corners defined by its \a topLeft and \a bottomRight
   coordinates.  The rectangle outline is drawn with the given \a pen and the filled
   with the given \a brush.  The rectangle is displayed at the layer specified by
   \a layerIndex.
 */
QMapRect::QMapRect(const QGeoCoordinate& topLeft, const QGeoCoordinate& bottomRight,
                   const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(*new QMapRectPrivate, QMapObject::RectObject, layerIndex)
{
    Q_D(QMapRect);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p = pen;
    d->b = brush;
}

/*!
    \internal
*/
QMapRect::QMapRect(QMapRectPrivate &dd, const QGeoCoordinate& topLeft, const QGeoCoordinate& bottomRight,
                   const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(dd, QMapObject::RectObject, layerIndex)
{
    Q_D(QMapRect);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p = pen;
    d->b = brush;
}

/*!
     Returns the top left geo coordinate of the rectangle.
*/
QGeoCoordinate QMapRect::topLeft() const {
    Q_D(const QMapRect);
    return d->geoTopLeft;
}

/*!
    Returns the bottom right geo coordinate of the left rectangle.
*/
QGeoCoordinate QMapRect::bottomRight() const {
    Q_D(const QMapRect);
    return d->geoBottomRight;
}

/*!
    Returns the pen used for drawing the rectangle outline.
*/
QPen QMapRect::pen() const {
    Q_D(const QMapRect);
    return d->p;
}

/*!
    Returns the brush used for filling the rectangle.
*/
QBrush QMapRect::brush() const {
    Q_D(const QMapRect);
    return d->b;
}

void QMapRect::compMapCoords()
{
    Q_D(QMapRect);

    if (!d->mapView)
        return;

    QPointF p1 = d->mapView->geoToMap(d->geoTopLeft);
    QPointF p2 = d->mapView->geoToMap(d->geoBottomRight);

    if (p2.y() < p1.y()) {
        qreal y = p2.y();
        p2.setY(p1.y());
        p1.setY(y);
    }

    //Lines will always connect points from west to east.
    //If we cross the date line, we need to split.
    if (d->geoTopLeft.longitude() > d->geoBottomRight.longitude())
        p2.rx() += d->mapView->mapWidth();

    d->rect = QRectF(p1, p2);
    //compute intersecting map tiles now
    d->intersectingTiles.clear();
    compIntersectingTiles(d->rect);
}

void QMapRect::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapRect);

    if (!d->mapView)
        return;

    painter->save();

    painter->setPen(d->p);
    painter->setBrush(d->b);
    painter->translate(-viewPort.left(), -viewPort.top());
    painter->drawRect(d->rect);

    painter->restore();
}

bool QMapRect::intersects(const QRectF& rect) const
{
    Q_D(const QMapRect);
    return d->rect.intersects(rect);
}

QTM_END_NAMESPACE
