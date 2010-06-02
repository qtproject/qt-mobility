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
#include "qgeoboundingbox.h"
#include "qgeoroutesegment.h"

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
    delete d_ptr;
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
    Sets the list of coordinates suitable for providing a graphical
    overview of the route to \a pathSummary.

    Acutal routing information may describe the geographic path of the route
    in much higher detail than is necessary to draw the route at most scales,

    \sa QGeoRoute::pathSummary()
*/
void QGeoRoute::setPathSummary(const QList<QGeoCoordinate> &pathSummary)
{
    d_ptr->pathSummary = pathSummary;
}

/*!
    Returns a list of coordinates suitable for providing a graphical
    overview of the route.

    \sa QGeoRoute::setPathSummary()
*/
QList<QGeoCoordinate> QGeoRoute::pathSummary() const
{
    return d_ptr->pathSummary;
}

/*!
    Sets the bounding box which encompasses the entire route to \a bounds.

    \sa QGeoRoute::bounds()
*/
void QGeoRoute::setBounds(const QGeoBoundingBox &bounds)
{
    d_ptr->bounds = bounds;
}

/*!
    Returns a bounding box which encompasses the entire route.

    \sa QGeoRoute::setBounds()
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

    \sa QGeoRouteSegment
    \sa QGeoRoute::routeSegments()
*/
void QGeoRoute::setRouteSegments(const QHash<QString, const QGeoRouteSegment *> &routeSegments)
{
    d_ptr->routeSegments = routeSegments;
}

/*!
    Returns the route segments.

    QGeoRouteSegment contains more detailed information than QGeoRoute
    and can be subclasses to provide more specialized information, such as
    for the description of segments of the journey for those who are travelling
    by public transport, or by truck.

    \sa QGeoRouteSegment
    \sa QGeoRoute::setRouteSegments()
*/
QHash<QString, const QGeoRouteSegment *> QGeoRoute::routeSegments() const
{
    return d_ptr->routeSegments;
}

void QGeoRoute::appendRouteSegment(const QGeoRouteSegment* routeSegment)
{
    d_ptr->routeSegments.insert(routeSegment->id(), routeSegment);
}

void QGeoRoute::appendNavigationInstruction(const QGeoNavigationInstruction* instruction)
{
    d_ptr->navigationInstructions.append(instruction);
}


/*!
    Sets the list of navigation to \a navigationInstructions.

    \sa QGeoNavigationInstruction
    \sa QGeoRoute::navigationInstructions()
*/
void QGeoRoute::setNavigationInstructions(const QList<const QGeoNavigationInstruction *> &instruction)
{
    d_ptr->navigationInstructions = instruction;
}

/*!
    Returns the list of navigation instructions.

    \sa QGeoNavigationInstruction
    \sa QGeoRoute::setNavigationInstructions()
*/
QList<const QGeoNavigationInstruction *> QGeoRoute::navigationInstructions() const
{
    return d_ptr->navigationInstructions;
}

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
    Sets the distance covered by this route to \a distance.
*/
void QGeoRoute::setDistance(const QGeoDistance &distance)
{
    d_ptr->distance = distance;
}

/*!
    Returns the distance covered by this route.
*/
QGeoDistance QGeoRoute::distance() const
{
    return d_ptr->distance;
}
void QGeoRoute::setTravelMode(const QGeoRouteRequest::TravelMode mode)
{
    d_ptr->travelMode = mode;
}

QGeoRouteRequest::TravelMode QGeoRoute::travelMode() const
{
    return d_ptr->travelMode;
}

void QGeoRoute::setOptimization(const QGeoRouteRequest::RouteOptimization optimization)
{
    d_ptr->optimization = optimization;
}

QGeoRouteRequest::RouteOptimization QGeoRoute::optimization() const
{
    return d_ptr->optimization;
}

/*!
    Sets the geometric shape of the route to \a path.

    The coordinates in \a path should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/
void QGeoRoute::setPath(const QList<QGeoCoordinate> &path)
{
    d_ptr->path = path;
}

/*!
    Returns the geometric shape of the route.

    The coordinates should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/
QList<QGeoCoordinate> QGeoRoute::path() const
{
    return d_ptr->path;
}

/*******************************************************************************
*******************************************************************************/

QGeoRoutePrivate::QGeoRoutePrivate() {}

QGeoRoutePrivate::QGeoRoutePrivate(const QGeoRoutePrivate &other)
        : id(other.id),
        request(other.request),
        pathSummary(other.pathSummary),
        bounds(other.bounds),
        routeSegments(other.routeSegments),
        navigationInstructions(other.navigationInstructions),
        travelTime(other.travelTime),
        distance(other.distance) {}

QGeoRoutePrivate::~QGeoRoutePrivate() {}

QGeoRoutePrivate& QGeoRoutePrivate::operator= (const QGeoRoutePrivate & other)
{
    id = other.id;
    request = other.request;
    pathSummary = other.pathSummary;
    bounds = other.bounds;
    routeSegments = other.routeSegments;
    navigationInstructions = other.navigationInstructions;
    travelTime = other.travelTime;
    distance = other.distance;

    return *this;
}

QTM_END_NAMESPACE
