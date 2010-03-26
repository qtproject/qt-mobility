
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

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoute
    \brief The QGeoRoute class is a representation of a route.
    \ingroup location

    This class represents a route as contained in a QGeoRouteReply.
    A QGeoRoute contains a collection of QManeuver objects.
*/

/*!
    The default constructor.
*/
QGeoRoute::QGeoRoute()
    : d_ptr(new QGeoRoutePrivate())
{}


/*!
    The copy constructor.
*/
QGeoRoute::QGeoRoute(const QGeoRoute& route)
    : d_ptr(new QGeoRoutePrivate(*(route.d_ptr)))
{
}

/*!
    The assignment operator.
*/
QGeoRoute& QGeoRoute::operator=(const QGeoRoute & route)
{
    *d_ptr = *(route.d_ptr);
    return *this;
}

/*!
    The destructor.
*/
QGeoRoute::~QGeoRoute() {
    Q_D(QGeoRoute);
    delete d;
}

/*!
    Returns the departure time of this route.
*/
QDateTime QGeoRoute::timeOfDeparture() const
{
    Q_D(const QGeoRoute);
    return d->timeOfDeparture;
}

/*!
    Sets the time of departure for this route to \a timeOfDeparture
*/
void QGeoRoute::setTimeOfDeparture(const QDateTime &timeOfDeparture)
{
    Q_D(QGeoRoute);
    d->timeOfDeparture = timeOfDeparture;
}

/*!
    Returns the arrival time of this route.
*/
QDateTime QGeoRoute::timeOfArrival() const
{
    Q_D(const QGeoRoute);
    return d->timeOfArrival;
}

/*!
    Sets the time of arrival for this route to \a timeOfArrival
*/
void QGeoRoute::setTimeOfArrival(const QDateTime &timeOfArrival)
{
    Q_D(QGeoRoute);
    d->timeOfArrival = timeOfArrival;
}

/*!
    Returns the distance covered by this route in meters.
*/
quint32 QGeoRoute::distance() const
{
    Q_D(const QGeoRoute);
    return d->distance;
}

/*!
  Sets the distance covered by this route to \a distance metres.
*/
void QGeoRoute::setDistance(quint32 distance)
{
    Q_D(QGeoRoute);
    d->distance = distance;
}

/*!
    Returns the bounding box that completely encloses the route.

    The x coordinates of the corner points represent longitudes
    and the y coordinates represent latitudes.
*/
const QRectF& QGeoRoute::boundingBox() const
{
    Q_D(const QGeoRoute);
    return d->boundingBox;
}

/*!
    Sets the bounding box which completely encloses the route to \a boundingBox

    The x coordinates of the corner points represent longitudes
    and the y coordinates represent latitudes.
*/
void QGeoRoute::setBoundingBox(const QRectF &boundingBox)
{
    Q_D(QGeoRoute);
    d->boundingBox = boundingBox;
}

/*!
    Returns the list of all maneuvers comprising the route.
*/
QList<QManeuver> QGeoRoute::maneuvers() const
{
    Q_D(const QGeoRoute);
    return d->maneuvers;
}

/*!
    Sets the list of maneuvers which define the route to \a maneuvers.
*/
void QGeoRoute::setManeuvers(const QList<QManeuver> &maneuvers)
{
    Q_D(QGeoRoute);
    d->maneuvers = maneuvers;
}

/*******************************************************************************
*******************************************************************************/

QGeoRoutePrivate::QGeoRoutePrivate() {}

QGeoRoutePrivate::QGeoRoutePrivate(const QGeoRoutePrivate &rp)
    : timeOfArrival(rp.timeOfArrival),
    timeOfDeparture(rp.timeOfDeparture),
    distance(rp.distance),
    boundingBox(rp.boundingBox),
    maneuvers(rp.maneuvers)
{}

QGeoRoutePrivate& QGeoRoutePrivate::operator= (const QGeoRoutePrivate &rp) {
    timeOfArrival = rp.timeOfArrival;
    timeOfDeparture = rp.timeOfDeparture;
    distance = rp.distance;
    boundingBox = rp.boundingBox;
    maneuvers = rp.maneuvers;
    return *this;
}

QTM_END_NAMESPACE

