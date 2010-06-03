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
    \ingroup maps

    A QGeoRouteSegment instance has information about the physcial layout
    of the route segment, the length of the route and the estimated time and
    navigation instructions required to traverse the route segment.

    Subclasses of QGeoRouteSegment will provide more detailed information about
    the segment.  The type() function can be used to determine the subclass
    that is being used.

    \sa QGeoRouteSegment::SegmentType
*/

/*!
    \enum QGeoRouteSegment::SegmentType

    This is used to distinguish between different subclasses of
    QGeoRouteSegment when they are accessed as QGeoRouteSegment objects.

    \value NormalSegment
        The route segment information is not specialized.
    \value PrivateTransportSegment
        The route segment information is specialized for travel by private transport.
    \value PublicTransportSegment
        The route segment information is specialized for travel by public transport.
    \value TruckSegment
        The route segment information is specialized for travel by truck.

    \sa QGeoRouteSegment::type()
*/

/*!
    Constructs a QGeoRouteSegment object.
*/
QGeoRouteSegment::QGeoRouteSegment()
        : d_ptr(new QGeoRouteSegmentPrivate()) {}

/*
    Internal use
*/
QGeoRouteSegment::QGeoRouteSegment(QGeoRouteSegmentPrivate *d_ptr)
        : d_ptr(d_ptr) {}

/*!
    Destroys this QGeoRouteSegment object.
*/
QGeoRouteSegment::~QGeoRouteSegment()
{
    Q_D(QGeoRouteSegment);
    delete d;
}

/*!
    Returns the type of this segment.

    If the type is something other than QGeoRouteSegment::NormalSegment then
    this QGeoRouteSegment object has been created as a subclass of
    QGeoRouteSegment which contains more detailed information about the segment.

    \sa QGeoRouteSegment::SegmentType
*/
QGeoRouteSegment::SegmentType QGeoRouteSegment::type() const
{
    Q_D(const QGeoRouteSegment);
    return d->type;
}

/*!
    Sets the estimated amount of time it will take to traverse this segment of
    the route, in seconds, to \a secs.
*/
void QGeoRouteSegment::setTravelTime(int secs)
{
    Q_D(QGeoRouteSegment);
    d->travelTime = secs;
}

/*!
    Returns the estimated amount of time it will take to traverse this segment
    of the route, in seconds.
*/
int QGeoRouteSegment::travelTime() const
{
    Q_D(const QGeoRouteSegment);
    return d->travelTime;
}

/*!
    Sets the \a distance covered by this segment of the route.
*/
void QGeoRouteSegment::setDistance(const QGeoDistance &distance)
{
    Q_D(QGeoRouteSegment);
    d->distance = distance;
}

/*!
    Returns the distance covered by this segment of the route.
*/
QGeoDistance QGeoRouteSegment::distance() const
{
    Q_D(const QGeoRouteSegment);
    return d->distance;
}

/*!
    Sets the geometric shape of this segment of the route to \a path.

    The coordinates in \a path should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/
void QGeoRouteSegment::setPath(const QList<QGeoCoordinate> &path)
{
    Q_D(QGeoRouteSegment);
    d->path = path;
}

/*!
    Returns the geometric shape of this route segment of the route.

    The coordinates should be listed in the order in which they
    would be traversed by someone travelling along this segment of the route.
*/

QList<QGeoCoordinate> QGeoRouteSegment::path() const
{
    Q_D(const QGeoRouteSegment);
    return d->path;
}
/*!
    Sets the instruction for this route segement to \a instruction.
*/
void QGeoRouteSegment::setInstruction(const QGeoNavigationInstruction *instruction)
{
    Q_D(QGeoRouteSegment);
    d->instruction = instruction;
}

/*!
    Returns the instruction for this route segment.
*/
const QGeoNavigationInstruction* QGeoRouteSegment::instruction() const
{
    Q_D(const QGeoRouteSegment);
    return d->instruction;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate()
    : instruction(NULL)
{
    type = QGeoRouteSegment::NormalSegment;
}

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate(const QGeoRouteSegmentPrivate &other)
        : type(other.type),
        travelTime(other.travelTime),
        distance(other.distance),
        path(other.path),
        instruction(other.instruction) {}

QGeoRouteSegmentPrivate::~QGeoRouteSegmentPrivate() {}

QGeoRouteSegmentPrivate& QGeoRouteSegmentPrivate::operator= (const QGeoRouteSegmentPrivate & other)
{
    type = other.type;
    travelTime = other.travelTime;
    distance = other.distance;
    path = other.path;
    instruction = other.instruction;
    return *this;
}

QTM_END_NAMESPACE

