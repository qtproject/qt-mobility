
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

#include "qroute.h"
#include "qroute_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QRoute
    \brief The QRoute class is a representation of a route.
    \ingroup location

    This class represents a route as contained in a QRouteReply.
    A QRoute contains a collection of QManeuver objects.
*/

/*!
    The default constructor.
*/
QRoute::QRoute()
    : d_ptr(new QRoutePrivate())
{}


/*!
    The copy constructor.
*/
QRoute::QRoute(const QRoute& route)
    : d_ptr(new QRoutePrivate(*(route.d_ptr)))
{
}

/*!
    The assignment operator.
*/
QRoute& QRoute::operator=(const QRoute & route)
{
    *d_ptr = *(route.d_ptr);
    return *this;
}

/*!
    The destructor.
*/
QRoute::~QRoute() {
    Q_D(QRoute);
    delete d;
}

/*!
    Returns the departure time of this route.
*/
QDateTime QRoute::timeOfDeparture() const
{
    Q_D(const QRoute);
    return d->timeOfDeparture;
}

/*!
    Sets the time of departure for this route to \a timeOfDeparture
*/
void QRoute::setTimeOfDeparture(const QDateTime &timeOfDeparture)
{
    Q_D(QRoute);
    d->timeOfDeparture = timeOfDeparture;
}

/*!
    Returns the arrival time of this route.
*/
QDateTime QRoute::timeOfArrival() const
{
    Q_D(const QRoute);
    return d->timeOfArrival;
}

/*!
    Sets the time of arrival for this route to \a timeOfArrival
*/
void QRoute::setTimeOfArrival(const QDateTime &timeOfArrival)
{
    Q_D(QRoute);
    d->timeOfArrival = timeOfArrival;
}

/*!
    Returns the distance covered by this route in meters.
*/
quint32 QRoute::distance() const
{
    Q_D(const QRoute);
    return d->distance;
}

/*!
  Sets the distance covered by this route to \a distance metres.
*/
void QRoute::setDistance(quint32 distance)
{
    Q_D(QRoute);
    d->distance = distance;
}

/*!
    Returns the bounding box that completely encloses the route.

    The x coordinates of the corner points represent longitudes
    and the y coordinates represent latitudes.
*/
const QRectF& QRoute::boundingBox() const
{
    Q_D(const QRoute);
    return d->boundingBox;
}

/*!
    Sets the bounding box which completely encloses the route to \a boundingBox

    The x coordinates of the corner points represent longitudes
    and the y coordinates represent latitudes.
*/
void QRoute::setBoundingBox(const QRectF &boundingBox)
{
    Q_D(QRoute);
    d->boundingBox = boundingBox;
}

/*!
    Returns the list of all maneuvers comprising the route.
*/
QList<QManeuver> QRoute::maneuvers() const
{
    Q_D(const QRoute);
    return d->maneuvers;
}

/*!
    Sets the list of maneuvers which define the route to \a maneuvers.
*/
void QRoute::setManeuvers(const QList<QManeuver> &maneuvers)
{
    Q_D(QRoute);
    d->maneuvers = maneuvers;
}

/*******************************************************************************
*******************************************************************************/

QRoutePrivate::QRoutePrivate() {}

QRoutePrivate::QRoutePrivate(const QRoutePrivate &rp)
    : timeOfArrival(rp.timeOfArrival),
    timeOfDeparture(rp.timeOfDeparture),
    distance(rp.distance),
    boundingBox(rp.boundingBox),
    maneuvers(rp.maneuvers)
{}

QRoutePrivate& QRoutePrivate::operator= (const QRoutePrivate &rp) {
    timeOfArrival = rp.timeOfArrival;
    timeOfDeparture = rp.timeOfDeparture;
    distance = rp.distance;
    boundingBox = rp.boundingBox;
    maneuvers = rp.maneuvers;
    return *this;
}

QTM_END_NAMESPACE

