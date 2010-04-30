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

#include "qgeoroute.h"
#include "qgeoroute_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

#include <QDateTime>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoute
    \brief The QGeoRoute class represents a route between two points.
    \ingroup maps

    A QGeoRoute object contains high level information about a route, such
    as the distance covered by the route, the estimated duration of the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of QGeoRouteSegment objecs, or
    objects derived from QGeoRouteSegment, which describe subsections of the
    route in greater detail.  The subclasses of QGeoRouteSegment are used to
    provide specific information for the subsection of the route for particular
    cases, as may be needed when the segment is to be traversed via public
    transport or in a truck.
*/

/*!
*/
QGeoRoute::QGeoRoute()
    : d_ptr(new QGeoRoutePrivate()) {}

/*!
*/
QGeoRoute::QGeoRoute(const QGeoRoute &other)
    : d_ptr(new QGeoRoutePrivate(*(other.d_ptr))) {}

/*!
*/
QGeoRoute::~QGeoRoute()
{
    Q_D(QGeoRoute);
    delete d;
}

/*!
*/
QGeoRoute& QGeoRoute::operator= (const QGeoRoute &other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
*/
void QGeoRoute::setRouteOverview(const QList<QGeoCoordinate> &routeOverview)
{
    Q_D(QGeoRoute);
    d->routeOverview = routeOverview;
}

/*!
*/
QList<QGeoCoordinate> QGeoRoute::routeOverview() const
{
    Q_D(const QGeoRoute);
    return d->routeOverview;
}

/*!
*/
void QGeoRoute::setBounds(const QGeoBoundingBox &bounds)
{
    Q_D(QGeoRoute);
    d->bounds = bounds;
}

/*!
*/
QGeoBoundingBox QGeoRoute::bounds() const
{
    Q_D(const QGeoRoute);
    return d->bounds;
}

/*!
*/
void QGeoRoute::setRouteSegments(const QList<const QGeoRouteSegment *> &routeSegments)
{
    Q_D(QGeoRoute);
    d->routeSegments = routeSegments;
}

/*!
*/
QList<const QGeoRouteSegment *> QGeoRoute::routeSegments() const
{
    Q_D(const QGeoRoute);
    return d->routeSegments;
}

/*!
*/
void QGeoRoute::setDuration(const QDateTime &duration)
{
    Q_D(QGeoRoute);
    d->duration = duration;
}

/*!
*/
QDateTime QGeoRoute::duration() const
{
    Q_D(const QGeoRoute);
    return d->duration;
}

/*!
*/
void QGeoRoute::setDistance(double value, QGeoMapWidget::DistanceUnits units)
{
    Q_D(QGeoRoute);
    d->distanceValue = value;
    d->distanceUnits = units;
}

/*!
*/
double QGeoRoute::distance(QGeoMapWidget::DistanceUnits units) const
{
    Q_D(const QGeoRoute);

    return QGeoMapWidget::convertDistance(d->distanceValue,
                                          d->distanceUnits,
                                          units);
}

/*!
*/
QGeoCoordinate QGeoRoute::closestPointOnRoute(const QGeoCoordinate &position) const
{
    // TODO implement
    Q_UNUSED(position);
    return QGeoCoordinate();
}

/*******************************************************************************
*******************************************************************************/

QGeoRoutePrivate::QGeoRoutePrivate() {}

QGeoRoutePrivate::QGeoRoutePrivate(const QGeoRoutePrivate &other)
    : routeOverview(other.routeOverview),
    bounds(other.bounds),
    routeSegments(other.routeSegments),
    duration(other.duration),
    distanceValue(other.distanceValue),
    distanceUnits(other.distanceUnits) {}

QGeoRoutePrivate::~QGeoRoutePrivate() {}

QGeoRoutePrivate& QGeoRoutePrivate::operator= (const QGeoRoutePrivate &other)
{
    routeOverview = other.routeOverview;
    bounds = other.bounds;
    routeSegments = other.routeSegments;
    duration = other.duration;
    distanceValue = other.distanceValue;
    distanceUnits = other.distanceUnits;
    return *this;
}

QTM_END_NAMESPACE
