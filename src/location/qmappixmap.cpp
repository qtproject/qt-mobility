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

#include "qmappixmap.h"
#include "qmappixmap_p.h"

#include <QPainter>

QTM_BEGIN_NAMESPACE

QMapPixmapPrivate::QMapPixmapPrivate()
    :pic(QPixmap()), geoTopLeft(QGeoCoordinate())
{
}

/*!
    \class QMapPixmap
    \brief The QMapPixmap class represents a pixmap on a map.
    \ingroup location
*/

/*!
    Constructs the \a pixmap.  It's top left corner is position according to \a topLeft
    corner.  The pixmap is displayed in the layer specified by \a layerIndex.
*/
QMapPixmap::QMapPixmap(const QGeoCoordinate& topLeft, const QPixmap& pixmap, quint16 layerIndex)
    :QMapObject(*new QMapPixmapPrivate, QMapObject::PixmapObject, layerIndex)
{
    Q_D(QMapPixmap);
    d->geoTopLeft = topLeft;
    d->pic = pixmap;
}

QMapPixmap::QMapPixmap(QMapPixmapPrivate &dd, const QGeoCoordinate& topLeft, const QPixmap& pixmap, quint16 layerIndex)
    :QMapObject(dd, QMapObject::PixmapObject, layerIndex)
{
    Q_D(QMapPixmap);
    d->geoTopLeft = topLeft;
    d->pic = pixmap;
}

/*!
    Returns the pixmap.
*/
QPixmap QMapPixmap::pixmap() const {
    Q_D(const QMapPixmap);
    return d->pic;
}

/*!
    Returns the top left corner (as a geo coordinate) of this pixmap.
*/
QGeoCoordinate QMapPixmap::topLeft() const {
    Q_D(const QMapPixmap);
    return d->geoTopLeft;
}

void QMapPixmap::compMapCoords()
{
    Q_D(QMapPixmap);
    if (!d->mapView)
        return;

    QPointF mapTopLeft(d->mapView->geoToMap(d->geoTopLeft));
    d->pixRect = d->pic.rect();
    d->pixRect.moveTopLeft(mapTopLeft);
    //compute intersecting map tiles now
    d->intersectingTiles.clear();
    compIntersectingTiles(d->pixRect);
}

void QMapPixmap::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapPixmap);
    if (!d->mapView)
        return;

    d->pixRect.translate(-viewPort.left(), -viewPort.top());
    painter->drawRect(d->pixRect);
    d->pixRect.translate(viewPort.left(), viewPort.top());
    qint64 mapWidth = static_cast<qint64>(d->mapView->mapWidth());

    //Is view port wrapping around date line?
    if (viewPort.right() >= mapWidth) {
        d->pixRect.translate(-viewPort.left(), -viewPort.top());
        d->pixRect.translate(mapWidth, 0);
        painter->drawRect(d->pixRect);
        d->pixRect.translate(-mapWidth, 0);
        d->pixRect.translate(viewPort.left(), viewPort.top());
    }
    //Is rect crossing date line?
    if (d->pixRect.right() >= mapWidth) {
        d->pixRect.translate(-viewPort.left(), -viewPort.top());
        d->pixRect.translate(-mapWidth, 0);
        painter->drawRect(d->pixRect);
        d->pixRect.translate(mapWidth, 0);
        d->pixRect.translate(viewPort.left(), viewPort.top());
    }
}

bool QMapPixmap::intersects(const QRectF& rect) const
{
    Q_D(const QMapPixmap);
    return d->pixRect.intersects(rect);
}

QTM_END_NAMESPACE
