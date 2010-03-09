
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
QRoute::QRoute() {}


/*!
    The copy constructor.
*/
QRoute::QRoute(const QRoute& route)
        : dist(route.dist), tod(route.tod), toa(route.toa), box(route.box), man(route.man)
{
}

/*!
    The assignment operator.
*/
QRoute& QRoute::operator=(const QRoute & route)
{
    dist = route.dist;
    tod = route.tod;
    toa = route.toa;
    box = route.box;
    man = route.man;

    return *this;
}

/*!
    Returns the departure time of this route.
*/
QDateTime QRoute::departure() const
{
    return tod;
}

/*!
    Returns the arrival time of this route.
*/
QDateTime QRoute::arrival() const
{
    return toa;
}

/*!
    Returns the distance covered by this route in meters.
*/
quint32 QRoute::distance() const
{
    return dist;
}

/*!
    Returns the bounding box that completely encloses the route.

    The x coordinates of the corner points represent longitudes
    and the y coordinates represent latitudes.
*/
const QRectF& QRoute::boundingBox() const
{
    return box;
}

/*!
    Returns the list of all maneuvers comprising the route.
*/
QList<QManeuver> QRoute::maneuvers() const
{
    return man;
}

QTM_END_NAMESPACE

