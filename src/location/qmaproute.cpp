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

#include "qmaproute.h"
#include "qmaproute_p.h"
#include <QPainter>

QTM_BEGIN_NAMESPACE

QMapRoutePrivate::QMapRoutePrivate()
    :rt(QRoute()), rPen(QPen())
{
}

/*!
    \class QMapRoute
    \brief The QMapRoute classes represets a route on a map.
    \ingroup location

    The representation of a route that has been added to the map.
    It keeps track of all indivdual QRouteSegments.
*/

/*!
    Constructor.
    The \a route represents the route that this map route represents.
*/
QMapRoute::QMapRoute(const QRoute& route, const QPen& pen, const QPixmap& /*endpointMarker*/, quint16 layerIndex)
        : QMapObject(*new QMapRoutePrivate, QMapObject::RouteObject, layerIndex)
{
    Q_D(QMapRoute);
    d->rt = route;
    d->rPen = pen;
    //TODO: endpoint marker for routes
}

QMapRoute::QMapRoute(QMapRoutePrivate &dd, const QRoute& route, const QPen& pen, const QPixmap& /*endpointMarker*/, quint16 layerIndex)
        : QMapObject(dd, QMapObject::RouteObject, layerIndex)
{
    Q_D(QMapRoute);
    d->rt = route;
    d->rPen = pen;
    //TODO: endpoint marker for routes
}

/*!
    \fn QPen QMapRoute::pen() const

    Retunrs the pen used for drawing this route.
*/
QPen QMapRoute::pen() const {
    Q_D(const QMapRoute);
    return d->rPen;
}

void QMapRoute::compMapCoords()
{
    Q_D(QMapRoute);
    d->segments.clear();

    if (!d->mapView)
        return;

    quint32 minDist = d->mapView->routeDetailLevel();
    QList<QManeuver> maneuvers = d->rt.maneuvers();
    QGeoCoordinate last;
    QGeoCoordinate here;

    for (int i = 0; i < maneuvers.size(); i++) {
        QList<QGeoCoordinate> wayPoints = maneuvers[i].wayPoints();

        for (int n = 0; n < wayPoints.size(); n++) {
            //make sure first waypoint is always shown
            if (last.isValid()) {
                last = wayPoints[n];
                continue;
            }

            here = wayPoints[n];
            QLineF line = d->mapView->connectShortest(here, last);

            //Moved the (line.p1() - line.p2()).manhattanLength() 
            //call out of the if(..). If used inside if-statement
            //this will cause internal compiler error with symbian compilers.
            qreal r = (line.p1() - line.p2()).manhattanLength();
            if (r >= minDist ||
                    d->mapView->zoomLevel() == d->mapView->maxZoomLevel()) {
                addSegment(line);
                last = here;
                here = QGeoCoordinate();
            }
        }
    }

    //make sure last waypoint is always shown
    if (!here.isValid()) {
        QLineF line = d->mapView->connectShortest(here, last);
        addSegment(line);
    }

    compIntersectingTiles(QRectF());
}

void QMapRoute::addSegment(const QLineF& line)
{
    Q_D(QMapRoute);

    if (!d->mapView)
        return;

    //TODO: refine: check whether tiles are actually being intersected
    QMapView::TileIterator it(*d->mapView, QMapObject::boundingRect(line));

    while (it.hasNext()) {
        it.next();

        if (it.isValid()) {
            quint64 tileIndex = d->mapView->getTileIndex(it.col(), it.row());

            if (!d->segments.contains(tileIndex))
                d->segments.insert(tileIndex, QList<QLineF>());

            d->segments[tileIndex].append(line);
        }
    }
}

void QMapRoute::compIntersectingTiles(const QRectF& /*box*/)
{
    Q_D(QMapRoute);
    d->intersectingTiles.clear();
    QMapIterator<quint64, QList<QLineF> > it(d->segments);

    while (it.hasNext()) {
        it.next();
        d->intersectingTiles.append(it.key());
    }
}

bool QMapRoute::intersects(const QRectF& rect) const
{
    Q_D(const QMapRoute);

    if (!d->mapView)
        return false;

    QMapView::TileIterator it(*d->mapView, rect);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 tileIndex = d->mapView->getTileIndex(it.col(), it.row());

        if (d->segments.contains(tileIndex)) {
            QRectF tile = it.tileRect();
            QListIterator<QLineF> lit(d->segments[tileIndex]);

            while (lit.hasNext()) {
                const QLineF& line = lit.next();

                if (QMapObject::intersect(tile, line))
                    return true;
            }
        }
    }

    return false;
}

void QMapRoute::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapRoute);

    if (!d->mapView)
        return;

    QPen oldPen = painter->pen();
    painter->setPen(d->rPen);
    QMapView::TileIterator it(*d->mapView, viewPort);
    int count = 0;
    QPainterPath path;

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 tileIndex = d->mapView->getTileIndex(it.col(), it.row());

        if (d->segments.contains(tileIndex)) {
            QListIterator<QLineF> lit(d->segments[tileIndex]);

            while (lit.hasNext()) {
                QLineF line = lit.next().translated(-viewPort.topLeft());
                //painter->drawLine(line);
                path.moveTo(line.p1());
                path.lineTo(line.p2());
                count++;
            }
        }
    }

    painter->drawPath(path);
    qint64 mapWidth = static_cast<qint64>(d->mapView->mapWidth());

    //Is view port wrapping around date line?
    //Moved the viewPort.right() function call out of the if(..)
    //If used inside if-statement this will cause internal compiler error
    //with symbian compilers.
    qreal r = viewPort.right();
    if (r >= mapWidth) {
        path.translate(mapWidth, 0);
        painter->drawPath(path);
        path.translate(-mapWidth, 0);
    }
    //Is path spanning date line?
    //Moved the path.boundingRect().right() function call out of the if(..)
    //If used inside if-statement this will cause internal compiler error
    //with symbian compilers.
    r = path.boundingRect().right();
    if (r >= mapWidth) {
        path.translate(-mapWidth, 0);
        painter->drawPath(path);
    }

    painter->setPen(oldPen);
}

QTM_END_NAMESPACE
