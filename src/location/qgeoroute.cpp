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

#include "qgeoboundingbox.h"
#include "qgeoroutesegment.h"

#include <QDateTime>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoute
    \brief The QGeoRoute class represents a route between two points.

    \inmodule QtLocation
    
    \ingroup maps-routing

    A QGeoRoute object contains high level information about a route, such
    as the length the route, the estimated travel time for the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of QGeoRouteSegment objecs, or
    objects derived from QGeoRouteSegment, which describe subsections of the
    route in greater detail.  The subclasses of QGeoRouteSegment are used to
    provide specific information for the subsection of the route for particular
    cases, as may be needed when the segment is to be traversed via public
    transport or in a truck.

    Routing information is normally requested using
    QGeoRoutingManager::calculateRoute(), which returns a QGeoRouteReply
    instance. If the operation is completed successfully the routing
    information can be accessed with QGeoRouteReply::routes()

    \sa QGeoRoutingManager
*/

/*!
    Constructs a route object.
*/
QGeoRoute::QGeoRoute()
        : d_ptr(new QGeoRoutePrivate()) {}

/*!
    Constructs a route object from the contents of \a other.
*/
QGeoRoute::QGeoRoute(const QGeoRoute &other)
        : d_ptr(other.d_ptr) {}

/*!
    Destroys this route object.
*/
QGeoRoute::~QGeoRoute()
{
}

/*!
    Assigns the contents of \a other to this route and returns a reference to
    this route.
*/
QGeoRoute& QGeoRoute::operator= (const QGeoRoute & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Sets the ID of this route to \a id.

    Service providers which support the updating of routes commonly assign
    IDs to routes.  If this route came from such a service provider changing
    the ID will probably cause route updates to stop working.
*/
void QGeoRoute::setRouteId(const QString &id)
{
    d_ptr->id = id;
}

/*!
    Returns the ID of this route.

    Service providers which support the updating of routes commonly assign
    IDs to routes.  If this route did not come from such a service provider
    the function will return an empty string.
*/
QString QGeoRoute::routeId() const
{
    return d_ptr->id;
}

/*!
    Sets the route request which describes the criteria used in the
    calculcation of this route to \a request.
*/
void QGeoRoute::setRequest(const QGeoRouteRequest &request)
{
    d_ptr->request = request;
}

/*!
    Returns the route request which describes the criteria used in
    the calculation of this route.
*/
QGeoRouteRequest QGeoRoute::request() const
{
    return d_ptr->request;
}

/*!
    Sets the bounding box which encompasses the entire route to \a bounds.
*/
void QGeoRoute::setBounds(const QGeoBoundingBox &bounds)
{
    d_ptr->bounds = bounds;
}

/*!
    Returns a bounding box which encompasses the entire route.
*/
QGeoBoundingBox QGeoRoute::bounds() const
{
    return d_ptr->bounds;
}

/*!
    Sets the list of route segements to \a routeSegments.

    QGeoRouteSegment contains more detailed information than QGeoRoute
    and can be subclasses to provide more specialized information, such as
    for the description of segments of the journey for those who are travelling
    by public transport, or by truck.
*/
void QGeoRoute::setRouteSegments(const QList<QGeoRouteSegment> &routeSegments)
{
    d_ptr->routeSegments = routeSegments;
}

/*!
    Returns the route segments.

    QGeoRouteSegment contains more detailed information than QGeoRoute
    and can be subclasses to provide more specialized information, such as
    for the description of segments of the journey for those who are travelling
    by public transport, or by truck.
*/
QList<QGeoRouteSegment> QGeoRoute::routeSegments() const
{
    return d_ptr->routeSegments;
}

/*
void QGeoRoute::appendRouteSegment(const QGeoRouteSegment* routeSegment)
{
    d_ptr->routeSegments.append(routeSegment);
}
*/

/*!
    Sets the estimated amount of time it will take to traverse this route,
    in seconds, to \a secs.
*/
void QGeoRoute::setTravelTime(int secs)
{
    d_ptr->travelTime = secs;
}

/*!
    Returns the estimated amount of time it will take to traverse this route,
    in seconds.
*/
int QGeoRoute::travelTime() const
{
    return d_ptr->travelTime;
}

/*!
    Sets the distance covered by this route, in metres, to \a distance.
*/
void QGeoRoute::setDistance(qreal distance)
{
    d_ptr->distance = distance;
}

/*!
    Returns the distance covered by this route, in metres.
*/
qreal QGeoRoute::distance() const
{
    return d_ptr->distance;
}

/*!
    Sets the travel mode for this route.

    This should be one of the travel modes returned by request().travelModes().
*/
void QGeoRoute::setTravelMode(QGeoRouteRequest::TravelMode mode)
{
    d_ptr->travelMode = mode;
}

/*!
    Returns the travel mode for the this route.

    This should be one of the travel modes returned by request().travelModes().
*/
QGeoRouteRequest::TravelMode QGeoRoute::travelMode() const
{
    return d_ptr->travelMode;
}

//!
//    Sets the geometric shape of the route to \a path.

//    The coordinates in \a path should be listed in the order in which they
//    would be traversed by someone travelling along this segment of the route.

/*!
    API REVIEW NOTE

    This may not be in the API, or it may replace setPathSummary().

    It's possible that both will be removed, provided that its acceptable to
    only expose the detailed route geometry via QGeoRouteSegment, since the
    "summary" path should probably be recomputed based on how the route is
    being viewed (mostly based on the zoom level).

    This is being discussed with the people who added it.
*/
void QGeoRoute::setPath(const QList<QGeoCoordinate> &path)
{
    // TODO not handled in private impl copy constructor / assignment operator
    d_ptr->path = path;
}

//!
//    Returns the geometric shape of the route.

//    The coordinates should be listed in the order in which they
//    would be traversed by someone travelling along this segment of the route.

/*!
    API REVIEW NOTE

    This may not be in the API, or it may replace pathSummary().

    It's possible that both will be removed, provided that its acceptable to
    only expose the detailed route geometry via QGeoRouteSegment, since the
    "summary" path should probably be recomputed based on how the route is
    being viewed (mostly based on the zoom level).

    This is being discussed with the people who added it.
*/
QList<QGeoCoordinate> QGeoRoute::path() const
{
    // TODO not handled in private impl copy constructor / assignment operator
    return d_ptr->path;
}

/*******************************************************************************
*******************************************************************************/

QGeoRoutePrivate::QGeoRoutePrivate()
    : travelTime(0),
    distance(0.0){}

QGeoRoutePrivate::QGeoRoutePrivate(const QGeoRoutePrivate &other)
        : QSharedData(other),
        id(other.id),
        request(other.request),
        bounds(other.bounds),
        routeSegments(other.routeSegments),
        travelTime(other.travelTime),
        distance(other.distance),
        travelMode(other.travelMode),
        path(other.path) {}

QGeoRoutePrivate::~QGeoRoutePrivate() {}


QTM_END_NAMESPACE
