/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

QGeoRouteSegment::QGeoRouteSegment(QExplicitlySharedDataPointer<QGeoRouteSegmentPrivate> &d_ptr)
    : d_ptr(d_ptr) {}

/*!
    Destroys this route segment object.
*/
QGeoRouteSegment::~QGeoRouteSegment() {}


/*!
    Assigns \a other to this route segment object and then returns a
    reference to this route segment object.
*/
QGeoRouteSegment& QGeoRouteSegment::operator= (const QGeoRouteSegment & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this route segment and \a other are equal.
*/
bool QGeoRouteSegment::operator ==(const QGeoRouteSegment &other) const
{
    return (d_ptr.constData() == other.d_ptr.constData());
}

/*!
    Returns whether this route segment and \a other are not equal.
*/
bool QGeoRouteSegment::operator !=(const QGeoRouteSegment &other) const
{
    return !(operator==(other));
}

/*!
*/
bool QGeoRouteSegment::isValid() const
{
    return d_ptr->valid;
}

/*!
*/
void QGeoRouteSegment::setNextRouteSegment(const QGeoRouteSegment &routeSegment)
{
    d_ptr->nextSegment = routeSegment.d_ptr;
}

/*!
*/
QGeoRouteSegment QGeoRouteSegment::nextRouteSegment() const
{
    if (d_ptr->nextSegment)
        return QGeoRouteSegment(d_ptr->nextSegment);

    QGeoRouteSegment segment;
    segment.d_ptr->valid = false;
    return segment;
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
    Sets the maneuver for this route segement to \a maneuver.
*/
void QGeoRouteSegment::setManeuver(const QGeoManeuver &maneuver)
{
    d_ptr->maneuver = maneuver;
}

/*!
    Returns the instruction for this route segment.
*/
QGeoManeuver QGeoRouteSegment::maneuver() const
{
    return d_ptr->maneuver;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate()
        : valid(true),
        travelTime(0),
        distance(0.0) {}

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate(const QGeoRouteSegmentPrivate &other)
        : QSharedData(other),
        valid(other.valid),
        travelTime(other.travelTime),
        distance(other.distance),
        path(other.path),
        maneuver(other.maneuver),
        nextSegment(other.nextSegment) {}

QGeoRouteSegmentPrivate::~QGeoRouteSegmentPrivate()
{
    nextSegment.reset();
}

bool QGeoRouteSegmentPrivate::operator ==(const QGeoRouteSegmentPrivate &other) const
{
    return ((valid == other.valid)
            && (travelTime == other.travelTime)
            && (distance == other.distance)
            && (path == other.path)
            && (maneuver == other.maneuver));
}

/*******************************************************************************
*******************************************************************************/

QTM_END_NAMESPACE

