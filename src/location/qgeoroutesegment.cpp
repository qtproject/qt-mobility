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

#include "qgeoroutesegment.h"
#include "qgeoroutesegment_p.h"

#include "qgeocoordinate.h"
#include <QDateTime>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRouteSegment
    \brief The QGeoRouteSegment class represents a segment of a route.

    \inmodule QtLocation
    
    \ingroup maps-routing

    A QGeoRouteSegment instance has information about the physcial layout
    of the route segment, the length of the route and the estimated time and
    navigation instructions required to traverse the route segment.
*/

/*!
    Constructs a route segment object.
*/
QGeoRouteSegment::QGeoRouteSegment()
        : d_ptr(new QGeoRouteSegmentPrivate()) {}

/*!
    Constructs a route segment object from the contents of \a other.
*/
QGeoRouteSegment::QGeoRouteSegment(const QGeoRouteSegment &other)
    : d_ptr(other.d_ptr) {}

/*!
    Destroys this route segment object.
*/
QGeoRouteSegment::~QGeoRouteSegment() {}


/*!
    Assigns \a other to this route segment object and then returns a
    reference to this route segment object.
*/
QGeoRouteSegment& QGeoRouteSegment::operator= (const QGeoRouteSegment &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Sets the estimated amount of time it will take to traverse this segment of
    the route, in seconds, to \a secs.
*/
void QGeoRouteSegment::setTravelTime(int secs)
{
    d_ptr->travelTime = secs;
}

/*!
    Returns the estimated amount of time it will take to traverse this segment
    of the route, in seconds.
*/
int QGeoRouteSegment::travelTime() const
{
    return d_ptr->travelTime;
}

/*!
    Sets the distance covered by this segment of the route, in metres, to \a distance.
*/
void QGeoRouteSegment::setDistance(qreal distance)
{
    d_ptr->distance = distance;
}

/*!
    Returns the distance covered by this segment of the route, in metres.
*/
qreal QGeoRouteSegment::distance() const
{
    return d_ptr->distance;
}

/*!
    Sets the geometric shape of this segment of the route to \a path.

    The coordinates in \a path should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/
void QGeoRouteSegment::setPath(const QList<QGeoCoordinate> &path)
{
    d_ptr->path = path;
}

/*!
    Returns the geometric shape of this route segment of the route.

    The coordinates should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/

QList<QGeoCoordinate> QGeoRouteSegment::path() const
{
    return d_ptr->path;
}
/*!
    Sets the instruction for this route segement to \a instruction.
*/
void QGeoRouteSegment::setInstruction(const QGeoNavigationInstruction &instruction)
{
    d_ptr->instruction = instruction;
}

/*!
    Returns the instruction for this route segment.
*/
QGeoNavigationInstruction QGeoRouteSegment::instruction() const
{
    return d_ptr->instruction;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate()
    : travelTime(0),
    distance(0.0) {}

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate(const QGeoRouteSegmentPrivate &other)
        : QSharedData(other),
        travelTime(other.travelTime),
        distance(other.distance),
        path(other.path),
        instruction(other.instruction) {}

QGeoRouteSegmentPrivate::~QGeoRouteSegmentPrivate() {}

QTM_END_NAMESPACE

