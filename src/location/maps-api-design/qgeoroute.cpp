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

#include "qgeodistance.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

#include <QDateTime>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoute
    \brief The QGeoRoute class represents a route between two points.
    \ingroup maps

    A QGeoRoute object contains high level information about a route, such
    as the length the route, the estimated travel time for the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of QGeoRouteSegment objecs, or
    objects derived from QGeoRouteSegment, which describe subsections of the
    route in greater detail.  The subclasses of QGeoRouteSegment are used to
    provide specific information for the subsection of the route for particular
    cases, as may be needed when the segment is to be traversed via public
    transport or in a truck.

    An instance QGeoRoutingService of will normally be responsible for the
    creation and setup of QGeoRoute objects.

    \sa QGeoRoutingService
*/

/*!
    \enum QGeoRoute::TravelMode

    Defines modes of travel to be used for a route.

    \value CarTravel
        The route will be optimized for someone who is driving a car.
    \value PedestrianTravel
        The route will be optimized for someone who is walking.
    \value BicycleTravel
        The route will be optimized for someone who is riding a bicycle.
    \value PublicTransitTravel
        The route will be optimized for someone who is making use of public transit.
    \value TruckTravel
        The route will be optimized for someone who is driving a truck.
*/

/*!
    \enum QGeoRoute::AvoidFeatureType

    Defines features to be avoided while planning a route.

    \value AvoidNothing
        The route can be planned without considering features to be avoided.
    \value AvoidTolls
        Avoid routes that require the use of tollways.
    \value AvoidHighways
        Avoid routes that require the use of highways.
    \value AvoidPublicTransit
        Avoid routes that require the use of public transit.
    \value AvoidFerries
        Avoid routes that require the use of ferries.
    \value AvoidTunnels
        Avoid routes that require the use of tunnels.
    \value AvoidDirtRoads
        Avoid routes that require the use of dirt roads.
    \value AvoidPark
        Avoid routes that require the travel through parks.
    \value AvoidMotorPoolLanes
        Avoid routes that require the use of motor pool lanes.
*/

// TODO improve description of MostScenicRoute
/*!
    \enum QGeoRoute::RouteOptimization

    Defines the type of optimization which is applied to the planning of the route.

    \value ShortestRoute
        Minimize the length of the journey.
    \value FastestRoute
        Minimize the travelling time for the journey.
    \value MostEconomicRoute
        Minimize the cost of the journey.
    \value MostScenicRoute
        Maximize the scenic potential of the journey.
*/

/*!
    \enum QGeoRoute::DirectionsDetail

    Defines the amount of direction information that should be included with the
    route.

    \value NoDirections
        No directions should be included with the route.
    \value BasicDirections
        Basic directions will be included with the route.  This will typically
        include QGeoNavigationInstruction::instructionText().
    \value DetailedDirections
        Detailed directions will be included with the route.  This will typically
        mean that subclasses of QNavigationInstruction are used to provide
        data structures describing the directions.  See QGeoNavigationInstruction
        and its subclasses for more details.

    \sa QGeoNavigationInstruction
*/

/*!
    Constructs a route object.
*/
QGeoRoute::QGeoRoute()
        : d_ptr(new QGeoRoutePrivate()) {}

/*!
    Construcst a route object from the contents of \a other.
*/
QGeoRoute::QGeoRoute(const QGeoRoute &other)
        : d_ptr(new QGeoRoutePrivate(*(other.d_ptr))) {}

/*!
    Destroys a route object.
*/
QGeoRoute::~QGeoRoute()
{
    Q_D(QGeoRoute);
    delete d;
}

/*!
    Assigns the contents of \a other to this route and returns a reference to
    this route.
*/
QGeoRoute& QGeoRoute::operator= (const QGeoRoute & other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
    Sets the level of the detail to use when representing routing directions
    to \a directionsDetail.
*/
void QGeoRoute::setDirectionsDetail(QGeoRoute::DirectionsDetail directionsDetail)
{
    Q_D(QGeoRoute);
    d->directionsDetail = directionsDetail;
}

/*!
    Returns the level of detail which is used in the representation of routing
    directions.
*/
QGeoRoute::DirectionsDetail directionsDetail() const
{
    Q_D(const QGeoRoute);
    return d->directionsDetail;
}

// TODO
// this comes from the various specs
// - won't this be highly dependent on zoom levels?
// - could probably use the level of detail trick (min pixels distance)
//   in order to generate a list of points per zoom level
// - could then cache them in the route
// - these methods would then me modified to have a zoom level parameter

/*!
    Sets the list of coordinates suitable for providing a graphical
    overview of the route to \a routeOverview.

    Acutal routing information may describe the geographic path of the route
    in much higher detail than is necessary to draw the route at most scales,

    \sa QGeoRoute::routeOverview()
*/
void QGeoRoute::setRouteOverview(const QList<QGeoCoordinate> &routeOverview)
{
    Q_D(QGeoRoute);
    d->routeOverview = routeOverview;
}

/*!
    Returns a list of coordinates suitable for providing a graphical
    overview of the route.

    \sa QGeoRoute::setRouteOverview()
*/
QList<QGeoCoordinate> QGeoRoute::routeOverview() const
{
    Q_D(const QGeoRoute);
    return d->routeOverview;
}

/*!
    Sets the bounding box which encompasses the entire route to \a bounds.

    \sa QGeoRoute::bounds()
*/
void QGeoRoute::setBounds(const QGeoBoundingBox &bounds)
{
    Q_D(QGeoRoute);
    d->bounds = bounds;
}

/*!
    Returns a bounding box which encompasses the entire route.

    \sa QGeoRoute::setBounds()
*/
QGeoBoundingBox QGeoRoute::bounds() const
{
    Q_D(const QGeoRoute);
    return d->bounds;
}

/*!
    Sets the list of route segements to \a routeSegments.

    QGeoRouteSegment contains more detailed information than QGeoRoute
    and can be subclasses to provide more specialized information, such as
    for the description of segments of the journey for those who are travelling
    by public transport, or by truck.

    \sa QGeoRouteSegment
    \sa QGeoRoute::routeSegments()
*/
void QGeoRoute::setRouteSegments(const QList<const QGeoRouteSegment *> &routeSegments)
{
    Q_D(QGeoRoute);
    d->routeSegments = routeSegments;
}

/*!
    Returns the list of route segments.

    QGeoRouteSegment contains more detailed information than QGeoRoute
    and can be subclasses to provide more specialized information, such as
    for the description of segments of the journey for those who are travelling
    by public transport, or by truck.

    \sa QGeoRouteSegment
    \sa QGeoRoute::setRouteSegments()
*/
QList<const QGeoRouteSegment *> QGeoRoute::routeSegments() const
{
    Q_D(const QGeoRoute);
    return d->routeSegments;
}

/*!
    Sets an estimate of how long it will take to traverse the route to \a
    travelTime.

    \sa QGeoRoute::estimatedTravelTime()
*/
void QGeoRoute::setEstimatedTravelTime(const QDateTime &travelTime)
{
    Q_D(QGeoRoute);
    d->estimatedTravelTime = travelTime;
}

/*!
    Returns an estimate of how long it will take to travel the route.

    \sa QGeoRoute::setEstimatedTravelTime()
*/
QDateTime QGeoRoute::estimatedTravelTime() const
{
    Q_D(const QGeoRoute);
    return d->estimatedTravelTime;
}

/*!
    Sets the length of this route to \a length.

    \sa QGeoRoute::length()
*/
void QGeoRoute::setLength(const QGeoDistance &length)
{
    Q_D(QGeoRoute);
    d->length = length;
}

/*!
    Returns the length of this route.

    \sa QGeoRoute::setLength()
*/
QGeoDistance QGeoRoute::length() const
{
    Q_D(const QGeoRoute);
    return d->length;
}

/*!
    Returns the point on the route which is closest to \a position.

    If several points are equally close to \a position, the point that occurs
    first in the ordering of the points on the route will be used.
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
        :
        directionsDetail(other.directionsDetail),
        routeOverview(other.routeOverview),
        bounds(other.bounds),
        routeSegments(other.routeSegments),
        estimatedTravelTime(other.estimatedTravelTime),
        length(other.length) {}

QGeoRoutePrivate::~QGeoRoutePrivate() {}

QGeoRoutePrivate& QGeoRoutePrivate::operator= (const QGeoRoutePrivate & other)
{
    directionsDetail = other.directionsDetail;
    routeOverview = other.routeOverview;
    bounds = other.bounds;
    routeSegments = other.routeSegments;
    estimatedTravelTime = other.estimatedTravelTime;
    length = other.length;
    return *this;
}

QTM_END_NAMESPACE
