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

#include "qmapellipse.h"
#include "qmapellipse_p.h"
#include <QPainter>

QTM_BEGIN_NAMESPACE

QMapEllipsePrivate::QMapEllipsePrivate()
        : geoTopLeft(QGeoCoordinate()),
        geoBottomRight(QGeoCoordinate()),
        p(QPen()), b(QBrush())
{
}

/*!
*   \class QMapEllipse
*   \brief The QMapEllipse class represents an ellipse that has been added to the
           map.
*   \ingroup location
*/

/*!
    Constructs an ellipse with bounding box corners specified by \a topLeft and
    \a bottomRight.  The ellipse outline is drawn with the given \a pen and filled
    with the given \a brush.  The ellipse is displayed at the layer specified by
    \a layerIndex.
*/
QMapEllipse::QMapEllipse(const QGeoCoordinate& topLeft, const QGeoCoordinate& bottomRight,
                         const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(*new QMapEllipsePrivate, QMapObject::EllipseObject, layerIndex)
{
    Q_D(QMapEllipse);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p =  pen;
    d->b = brush;
}

/*!
    \internal
*/
QMapEllipse::QMapEllipse(QMapEllipsePrivate &dd, const QGeoCoordinate& topLeft, const QGeoCoordinate& bottomRight,
                         const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(dd, QMapObject::EllipseObject, layerIndex)
{
    Q_D(QMapEllipse);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p =  pen;
    d->b = brush;
}

/*!
    Returns the top left geo coordinate of the bounding box of the ellipse.
*/
QGeoCoordinate QMapEllipse::topLeft() const
{
    Q_D(const QMapEllipse);
    return d->geoTopLeft;
}

/*!
    Returns the bottom right geo coordinate of the bounding box of the ellipse.
*/
QGeoCoordinate QMapEllipse::bottomRight() const
{
    Q_D(const QMapEllipse);
    return d->geoBottomRight;
}

/*!
    Returns the pen used for drawing the ellipse.
*/
QPen QMapEllipse::pen() const
{
    Q_D(const QMapEllipse);
    return d->p;
}
/*!
*   Returns the brush used for drawing the ellipse.
*/
QBrush QMapEllipse::brush() const
{
    Q_D(const QMapEllipse);
    return d->b;
}

void QMapEllipse::compMapCoords()
{
    Q_D(QMapEllipse);
    if (!d->mapView)
        return;

    QPointF p1 = d->mapView->translateToViewport(d->geoTopLeft);
    QPointF p2 = d->mapView->translateToViewport(d->geoBottomRight);

    if (p2.y() < p1.y()) {
        qreal y = p2.y();
        p2.setY(p1.y());
        p1.setY(y);
    }

    //Lines will always connect points from west to east.
    //If we cross the date line, we need to split.
    if (d->geoTopLeft.longitude() > d->geoBottomRight.longitude())
        p2.rx() += d->mapView->mapWidth();

    QRectF rect(p1, p2);
    d->path = QPainterPath();
    d->path.addEllipse(rect);
    //compute intersecting map tiles now
    d->intersectingTiles.clear();
    compIntersectingTiles(rect);
}

void QMapEllipse::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapEllipse);
    if (!d->mapView)
        return;

    painter->save();

    painter->setPen(d->p);
    painter->setBrush(d->b);
    painter->translate(-viewPort.left(), -viewPort.top());
    painter->drawPath(d->path);

    painter->restore();
}

bool QMapEllipse::intersects(const QRectF& rect) const
{
    Q_D(const QMapEllipse);
    return d->path.intersects(rect);
}

QTM_END_NAMESPACE
