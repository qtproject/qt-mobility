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

#include "qmaneuver.h"
#include "qmaneuver_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QManeuver
    \brief The QManeuver class represents the maneuver component of a QRouteReply.
    \ingroup location

    This class represents a maneuver as part of a route reply.

    \sa QRouteReply
*/

/*!
    Default constructor.
*/
QManeuver::QManeuver()
    : d_ptr(new QManeuverPrivate()) {}

/*!
    Constructs a QManeuver from \a maneuver.
*/
QManeuver::QManeuver(const QManeuver& maneuver)
        : d_ptr(new QManeuverPrivate(*(maneuver.d_ptr))) {}

/*!
    Assignment operator.
*/
QManeuver& QManeuver::operator=(const QManeuver & maneuver)
{
    *d_ptr = *(maneuver.d_ptr);
    return *this;
}

/*!
    Destructor
*/
QManeuver::~QManeuver()
{
    Q_D(QManeuver);
    delete d;
}

/*!
    Returns a textual representation of the maneuver that has to be performed to follow the route.
*/
QString QManeuver::description() const
{
    Q_D(const QManeuver);
    return d->description;
}

/*!
    Sets the textual representation of the maneuver to be performed to \a description.
*/
void QManeuver::setDescription(const QString &description)
{
    Q_D(QManeuver);
    d->description = description;
}

/*!
    Returns a string constant that describes the essential action that has to be taken on this maneuver.
*/

QString QManeuver::action() const
{
    Q_D(const QManeuver);
    return d->action;
}

/*!
    Sets the essential action which has to be taken to \a action.
*/
void QManeuver::setAction(const QString &action)
{
    Q_D(QManeuver);
    d->action = action;
}

/*!
    Returns the distance that is covered by this single maneuver in meters.
*/
quint32 QManeuver::distance() const
{
    Q_D(const QManeuver);
    return d->distance;
}

/*!
    Sets the distance that is covered by this maneuver to \a distance metres.
*/
void QManeuver::setDistance(quint32 distance)
{
    Q_D(QManeuver);
    d->distance = distance;
}

/*!
    Returns the estimated duration that performing the whole maneuver will take.
    The duration will be in the xsd:duration format.
*/
qint32 QManeuver::duration() const
{
    Q_D(const QManeuver);
    return d->duration;
}

/*!
    Sets the estimated duration of the maneuver to \a duration.
*/
void QManeuver::setDuration(qint32 duration)
{
    Q_D(QManeuver);
    d->duration = duration;
}

/*!
    Returns the turn to be taken in this maneuver.
*/
QString QManeuver::turn() const
{
    Q_D(const QManeuver);
    return d->turn;
}

/*!
    Sets the turn to be taken to \a turn;
*/
void QManeuver::setTurn(const QString &turn)
{
    Q_D(QManeuver);
    d->turn = turn;
}

/*!
    Returns the name of the street involved.
*/
QString QManeuver::streetName() const
{
    Q_D(const QManeuver);
    return d->streetName;
}

/*!
    Sets the name of the street involved in this maneuver to \a streetName.
*/
void QManeuver::setStreetName(const QString &streetName)
{
    Q_D(QManeuver);
    d->streetName = streetName;
}

/*!
    Returns the name of the route.
*/
QString QManeuver::routeName() const
{
    Q_D(const QManeuver);
    return d->routeName;
}

/*!
    Sets the name of the route to \a routeName;
*/
void QManeuver::setRouteName(const QString &routeName)
{
    Q_D(QManeuver);
    d->routeName = routeName;
}

/*!
    Returns the name of the next street.
*/
QString QManeuver::nextStreetName() const
{
    Q_D(const QManeuver);
    return d->nextStreetName;
}

/*!
    Sets the name of the next street to \a nextStreetName.
*/
void QManeuver::setNextStreetName(const QString &nextStreetName)
{
    Q_D(QManeuver);
    d->nextStreetName = nextStreetName;
}

/*!
    Returns a sign post.
*/
QString QManeuver::signPost() const
{
    Q_D(const QManeuver);
    return d->signPost;
}

/*!
    Sets the sign post to \a signPost.
*/
void QManeuver::setSignPost(const QString &signPost)
{
    Q_D(QManeuver);
    d->signPost = signPost;
}

/*!
    Returns a value representing the traffic direction.
*/
qint64 QManeuver::trafficDirection() const
{
    Q_D(const QManeuver);
    return d->trafficDirection;
}

/*!
    Sets the traffic direction to \a trafficDirection.
*/
void QManeuver::setTrafficDirection(qint64 trafficDirection)
{
    Q_D(QManeuver);
    d->trafficDirection = trafficDirection;
}

/*!
    Returns a value representing an icon.
*/
qint64 QManeuver::icon() const
{
    Q_D(const QManeuver);
    return d->icon;
}

/*!
    Sets the icon to \a icon.
*/
void QManeuver::setIcon(qint64 icon)
{
    Q_D(QManeuver);
    d->icon = icon;
}

/*!
    Returns the geographical coordinates that define this part of the route in form of a 2D-polyline.

    The geo coordinates in the way points are less detailed then those returned by maneuverPoints(),
    and are primarily designed for displaying on screen.

    \sa maneuverPoints()
*/
const QList<QGeoCoordinate> QManeuver::wayPoints() const
{
    Q_D(const QManeuver);
    return d->wayPoints;
}

/*!
  Sets the geographical coordinates that define this part of the route in form of a 2D-polyline to \a wayPoints.

  \sa wayPoints();
*/
void QManeuver::setWaypoints(const QList<QGeoCoordinate> &wayPoints)
{
    Q_D(QManeuver);
    d->wayPoints = wayPoints;
}

/*!
    Returns the geographical coordinates that define this part of the route in form of a 2D-polyline.

    Maneuver points describe the actual maneuver to take with a much higher detail than those returned by wayPoints().

    \sa wayPoints()
*/
const QList<QGeoCoordinate> QManeuver::maneuverPoints() const
{
    Q_D(const QManeuver);
    return d->maneuverPoints;
}

/*!
  Sets the geographical coordinates that define this part of the route in form of a 2D-polyline to \a maneuverPoints.

  \sa maneuverPoints()
*/
void QManeuver::setManeuverPoints(const QList<QGeoCoordinate> &maneuverPoints)
{
    Q_D(QManeuver);
    d->maneuverPoints = maneuverPoints;
}

/*******************************************************************************
*******************************************************************************/

QManeuverPrivate::QManeuverPrivate() {}

QManeuverPrivate::QManeuverPrivate(const QManeuverPrivate &mp)
    : description(mp.description),
    action(mp.action),
    duration(mp.duration),
    distance(mp.distance),
    turn(mp.turn),
    streetName(mp.streetName),
    routeName(mp.routeName),
    nextStreetName(mp.nextStreetName),
    signPost(mp.signPost),
    trafficDirection(mp.trafficDirection),
    icon(mp.icon),
    wayPoints(mp.wayPoints),
    maneuverPoints(mp.maneuverPoints)
{}

QManeuverPrivate& QManeuverPrivate::operator =(const QManeuverPrivate &mp)
{
    description = mp.description;
    action = mp.action;
    duration = mp.duration;
    distance = mp.distance;
    turn = mp.turn;
    streetName = mp.streetName;
    routeName = mp.routeName;
    nextStreetName = mp.nextStreetName;
    signPost = mp.signPost;
    trafficDirection = mp.trafficDirection;
    icon = mp.icon;
    wayPoints = mp.wayPoints;
    maneuverPoints = mp.maneuverPoints;

    return *this;
}

QTM_END_NAMESPACE

