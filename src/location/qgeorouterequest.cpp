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

#include "qgeorouterequest.h"
#include "qgeorouterequest_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRouteRequest
    \brief The QGeoRouteRequest class represents the parameters and restrictions
    which define a request for routing information.
    \ingroup maps-routing

    The default state of a QGeoRouteRequest instance will result in a request
    for basic route segment and navigation instructions describing the fastest
    route by car which covers the given waypoints.

    There may be signifcant variation in the features supported by different
    providers of routing information, or even in the features supported by
    the same provider if different levels of authorization are used.

    There are several functions in QGeoRoutingManager which can be used to
    check which features are supported with the current provider and
    authorization level.
    \sa QGeoRoutingManager
*/

/*

    DESIGN NOTE FOR API REVIEW:

    There are plans to make this extensible by allowing the user to set a
    list of QGeoTransportOptions (or QGeoTransitOptions).  We don't have any
    subclasses for that just yet, otherwise they'd be present.

    A QGeoPublicTransportOption subclass would allow users to specify things
    like cost limits, the maximum number of changes of vehicle, the maximum
    walking time / distance between changes of vehicle.

    There's Nokia / Navteq support for specifying things like Truck attributes
    so you can route around various road restrictions and conditions which
    effect trucks.

    A QGeoTrafficAwareTransportOption is probably also a good place to put the
    inputs for the various traffic / time aware bits of information. A
    departure / arrrival time could be set, and the presence of this transport
    options subclass (and the fact that user auth said that the user had
    support) would mean we could provide better values for the estimated
    travel times etc...
*/

/*!
    \enum QGeoRouteRequest::TravelMode

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
    \enum QGeoRouteRequest::AvoidFeatureType

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
    \enum QGeoRouteRequest::RouteOptimization

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

        enum SegmentDetail {
            NoSegmentData = 0x0000,
            BasicSegmentData = 0x0001,
            DetailedSegmentData = 0x0002
        };

/*!
    \enum QGeoRouteRequest::SegmentDetail

    Defines the amount of route segment information that shoudl be included
    with the route.

    \value NoSegmentData
        No segment data should be included with the route.  A route requested
        with this level of segment detail will initialise
        QGeoRouteSegment::path() as a straight line between the positions of
        the previous and next QGeoNavigationInstructions.

    \value BasicSegmentData
        Basic segment data will be included with the route.  This will include
        QGeoRouteSegment::path().

    \value DetailedSegmentData
        Detailed segment data will be included with the route.  This will
        typcially meant that subclasses of QGeoRouteSegment are used to provide
        data structures describing the segment.  See QGeoRouteSegment and its
        subclasses for more details.
*/

/*!
    \enum QGeoRouteRequest::InstructionDetail

    Defines the amount of instruction information that should be included with
    the route.

    \value NoInstructions
        No instructions should be included with the route.

    \value BasicInstructions
        Basic instructions will be included with the route. This will
        include QGeoNavigationInstruction::instructionText().

    \value DetailedInstructions
        Detailed instructions will be included with the route. This will
        typically mean that subclasses of QNavigationInstruction are used to
        provide data structures describing the instructions. See
        QGeoNavigationInstruction and its subclasses for more details.

    \sa QGeoNavigationInstruction
*/

/*!
    Constructs a request to calculate a route through the coordinates \a waypoints.

    The route will traverse the elements of \a waypoints in order.
*/
QGeoRouteRequest::QGeoRouteRequest(const QList<QGeoCoordinate> &waypoints)
        : d_ptr(new QGeoRouteRequestPrivate())
{
    d_ptr->waypoints = waypoints;
}

/*!
    Constructs a request to calculate a route between \a origin and
    \a destination.
*/
QGeoRouteRequest::QGeoRouteRequest(const QGeoCoordinate &origin, const QGeoCoordinate &destination)
        : d_ptr(new QGeoRouteRequestPrivate())
{
    d_ptr->waypoints.append(origin);
    d_ptr->waypoints.append(destination);
}

/*!
    Constructs a route request object from the contents of \a other.
*/
QGeoRouteRequest::QGeoRouteRequest(const QGeoRouteRequest &other)
        : d_ptr(other.d_ptr) {}

/*!
    Destroys the request.
*/
QGeoRouteRequest::~QGeoRouteRequest()
{
}

/*!
    Assigns \a other to this route request object and then returns a reference
    to this route request object.
*/
QGeoRouteRequest& QGeoRouteRequest::operator= (const QGeoRouteRequest & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Sets \a waypoints as the waypoints that the route should pass through.

    The waypoints should be given in order from origin to destination.

    This request will be invalid until the waypoints have been set to a
    list containing two or more coordinates.
*/
void QGeoRouteRequest::setWaypoints(const QList<QGeoCoordinate> &waypoints)
{
    d_ptr->waypoints = waypoints;
}

/*!
    Returns the waypoints that the route will pass through.
*/
QList<QGeoCoordinate> QGeoRouteRequest::waypoints() const
{
    return d_ptr->waypoints;
}

/*!
    Sets \a areas as excluded areas that the route must not cross.
*/
void QGeoRouteRequest::setExcludeAreas(const QList<QGeoBoundingBox> &areas)
{
    d_ptr->excludeAreas = areas;
}

/*!
    Returns areas the route must not cross.
*/
QList<QGeoBoundingBox> QGeoRouteRequest::excludeAreas() const
{
    return d_ptr->excludeAreas;
}

/*!
    Sets the number of alternative routes to request to \a alternatives.

    The default value is 0.
*/
void QGeoRouteRequest::setNumberAlternativeRoutes(int alternatives)
{
    d_ptr->numberAlternativeRoutes = alternatives;
}

/*!
    Returns the number of alternative routes which will be requested.
*/
int QGeoRouteRequest::numberAlternativeRoutes() const
{
    return d_ptr->numberAlternativeRoutes;
}

/*!
    Sets the travel modes which should be considered during the planning of the
    route to \a travelModes.

    The default value is QGeoRouteRequest::CarTravel.
*/
void QGeoRouteRequest::setTravelModes(QGeoRouteRequest::TravelModes travelModes)
{
    d_ptr->travelModes = travelModes;
}

/*!
    Returns the travel modes which this request specifies should be considered
    during the planning of the route.
*/
QGeoRouteRequest::TravelModes QGeoRouteRequest::travelModes() const
{
    return d_ptr->travelModes;
}

/*!
    Sets the features to be avoided during the planning of the route to
    \a avoidFeatureTypes.

    The default value is QGeoRouteRequest::AvoidNothing.
*/
void QGeoRouteRequest::setAvoidFeatureTypes(QGeoRouteRequest::AvoidFeatureTypes avoidFeatureTypes)
{
    d_ptr->avoidFeatureTypes = avoidFeatureTypes;
}

/*!
    Returns the features which this request specifies should be avoided during
    the planning of the route.
*/
QGeoRouteRequest::AvoidFeatureTypes QGeoRouteRequest::avoidFeatureTypes() const
{
    return d_ptr->avoidFeatureTypes;
}

/*!
    Sets the optimization criteria to use while planning the route to
    \a optimization.

    The default value is QGeoRouteRequest::FastestRoute.
*/
void QGeoRouteRequest::setRouteOptimization(QGeoRouteRequest::RouteOptimizations optimization)
{
    d_ptr->routeOptimization = optimization;
}

/*!
    Returns the optimization criteria which this request specifies should be
    used while planning the route.
*/
QGeoRouteRequest::RouteOptimizations QGeoRouteRequest::routeOptimization() const
{
    return d_ptr->routeOptimization;
}

/*!
    Sets the level of detail to use when representing routing segments to
    \a segmentDetail.
*/
void QGeoRouteRequest::setSegmentDetail(QGeoRouteRequest::SegmentDetail segmentDetail)
{
    d_ptr->segmentDetail = segmentDetail;
}

/*!
    Returns the level of detail which will be used in the representation of
    routing segments.
*/
QGeoRouteRequest::SegmentDetail QGeoRouteRequest::segmentDetail() const
{
    return d_ptr->segmentDetail;
}

/*!
    Sets the level of detail to use when representing routing instructions to
    \a instructionDetail.

    The default value is QGeoRouteRequest::BasicInstructions.
*/

void QGeoRouteRequest::setInstructionDetail(QGeoRouteRequest::InstructionDetail instructionDetail)
{
    d_ptr->instructionDetail = instructionDetail;
}

/*!
    Returns the level of detail which will be used in the representation of
    routing instructions.
*/
QGeoRouteRequest::InstructionDetail QGeoRouteRequest::instructionDetail() const
{
    return d_ptr->instructionDetail;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteRequestPrivate::QGeoRouteRequestPrivate()
        : QSharedData(),
        numberAlternativeRoutes(0),
        travelModes(QGeoRouteRequest::CarTravel),
        avoidFeatureTypes(QGeoRouteRequest::AvoidNothing),
        routeOptimization(QGeoRouteRequest::FastestRoute),
        segmentDetail(QGeoRouteRequest::BasicSegmentData),
        instructionDetail(QGeoRouteRequest::BasicInstructions) {}

QGeoRouteRequestPrivate::QGeoRouteRequestPrivate(const QGeoRouteRequestPrivate &other)
        : QSharedData(other),
        waypoints(other.waypoints),
        excludeAreas(other.excludeAreas),
        numberAlternativeRoutes(other.numberAlternativeRoutes),
        travelModes(other.travelModes),
        avoidFeatureTypes(other.avoidFeatureTypes),
        routeOptimization(other.routeOptimization),
        segmentDetail(other.segmentDetail),
        instructionDetail(other.instructionDetail)
        //transitOptions(other.transitOptions)
{}

QGeoRouteRequestPrivate::~QGeoRouteRequestPrivate() {}

QGeoRouteRequestPrivate& QGeoRouteRequestPrivate::operator= (const QGeoRouteRequestPrivate & other)
{
    waypoints = other.waypoints;
    excludeAreas = other.excludeAreas;
    numberAlternativeRoutes = other.numberAlternativeRoutes;
    travelModes = other.travelModes;
    avoidFeatureTypes = other.avoidFeatureTypes;
    routeOptimization = other.routeOptimization;
    segmentDetail = other.segmentDetail;
    instructionDetail = other.instructionDetail;
    //transitOptions = other.transitOptions

    return *this;
}

QTM_END_NAMESPACE
