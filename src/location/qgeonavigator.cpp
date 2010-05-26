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

#include "qgeonavigator.h"
#include "qgeonavigator_p.h"

#include "qgeodistance.h"
#include "qgeoroute.h"
#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"
#include "qgeoroutingmanager.h"

QTM_BEGIN_NAMESPACE

/*
enum NavigationStatus {
    OnRouteStatus,
    OffRouteStatus,
    ArrivedStatus
};
*/

QGeoNavigator::QGeoNavigator()
        : d_ptr(new QGeoNavigatorPrivate()) {}

QGeoNavigator::QGeoNavigator(const QGeoNavigator &other)
        : d_ptr(new QGeoNavigatorPrivate(*(other.d_ptr))) {}

QGeoNavigator::~QGeoNavigator()
{
    delete d_ptr;
}

QGeoNavigator& QGeoNavigator::operator = (const QGeoNavigator & other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

void QGeoNavigator::setPositionSource(QGeoPositionInfoSource *positionSource)
{
    if (!positionSource)
        return;

    d_ptr->positionSource = positionSource;

    // TODO set update interval for the position source if none set so far?

    connect(d_ptr->positionSource,
            SIGNAL(positionUpdated(QGeoPositionInfo)),
            this,
            SLOT(positionUpdated(QGeoPositionInfo)));

    connect(d_ptr->positionSource,
            SIGNAL(updateTimeout()),
            this,
            SLOT(positionTimeout()));
}

QGeoPositionInfoSource* QGeoNavigator::positionSource() const
{
    return d_ptr->positionSource;
}

void QGeoNavigator::setRoutingManager(QGeoRoutingManager *routingManager)
{
    if (!routingManager)
        return;

    d_ptr->routingManager = routingManager;
}

QGeoRoutingManager* QGeoNavigator::routingManager() const
{
    return d_ptr->routingManager;
}

void QGeoNavigator::setPositionTolerance(const QGeoDistance &radius)
{
    d_ptr->radius = radius;
}

QGeoDistance QGeoNavigator::positionTolerance() const
{
    return d_ptr->radius;
}

int QGeoNavigator::elapsedTime() const
{
    return d_ptr->elapsedTime;
}

QGeoDistance QGeoNavigator::elapsedDistance() const
{
    return d_ptr->elapsedDistance;
}

int QGeoNavigator::remainingTime() const
{
    return d_ptr->remainingTime;
}

QGeoDistance QGeoNavigator::remainingDistance() const
{
    return d_ptr->remainingDistance;
}

int QGeoNavigator::timeToNextInstruction() const
{
    return d_ptr->timeNextInstruction;
}

QGeoDistance QGeoNavigator::distanceToNextInstruction(QGeoDistance) const
{
    return d_ptr->distanceNextInstruction;
}

void QGeoNavigator::calculate(const QGeoRouteRequest &request)
{
    if (!d_ptr->routingManager) {
        // TODO error of some kind
        return;
    }

    d_ptr->routeReply = d_ptr->routingManager->calculateRoute(request);
    connect(d_ptr->routeReply,
            SIGNAL(finished()),
            this,
            SLOT(calculateRouteFinished()));

    connect(d_ptr->routeReply,
            SIGNAL(error(QGeoRouteReply::Error, QString)),
            this,
            SLOT(calculateRouteError(QGeoRouteReply::Error, QString)));
}

void QGeoNavigator::depart()
{
    if (!d_ptr->routeReply) {
        // TODO error of some kind
        return;
    }

    if (!d_ptr->routeReply->isFinished()) {
        // TODO error of some kind
        return;
    }

    if (!d_ptr->positionSource) {
        // TODO error of some kind
        return;
    }

    d_ptr->positionSource->startUpdates();
}

void QGeoNavigator::positionUpdated(const QGeoPositionInfo &positionInfo)
{
    // if gps was out of range, notify the user that it's back

    // update distance travelled
    // check for
    // - waypoint (or closest point on route) passed
    // - instruction reached
    // - end of route segment (update times)
    //   - also check for end of route
    // - off route beyond tolerance
    //   - fire off an update request, update remaining time and distance on return
}

void QGeoNavigator::positionTimeout()
{
    // TODO notify the user somehow that they're out of gps range
}

void QGeoNavigator::calculateRouteFinished()
{
    // common to first request and to updates
    // - grab the first route and put it somewhere
    // - update the remaining time and distance
}

void QGeoNavigator::calculateRouteError(QGeoRouteReply::Error error, const QString &errorString)
{
    // TODO pass the error up
}

/*******************************************************************************
*******************************************************************************/

QGeoNavigatorPrivate::QGeoNavigatorPrivate()
        : positionSource(0),
        routingManager(0),
        routeReply(0) {}

QGeoNavigatorPrivate::QGeoNavigatorPrivate(const QGeoNavigatorPrivate &other)
        : positionSource(positionSource),
        routingManager(routingManager),
        routeReply(routeReply),
        radius(radius),
        elapsedTime(elapsedTime),
        elapsedDistance(elapsedDistance),
        remainingTime(remainingTime),
        remainingDistance(remainingDistance),
        timeNextInstruction(timeNextInstruction),
        distanceNextInstruction(distanceNextInstruction) {}

QGeoNavigatorPrivate::~QGeoNavigatorPrivate()
{
    if (routeReply)
        delete routeReply;
}

QGeoNavigatorPrivate& QGeoNavigatorPrivate::operator= (const QGeoNavigatorPrivate & other)
{
    positionSource = other.positionSource;
    routingManager = other.routingManager;
    routeReply = other.routeReply;
    radius = other.radius;
    elapsedTime = other.elapsedTime;
    elapsedDistance = other.elapsedDistance;
    remainingTime = other.remainingTime;
    remainingDistance = other.remainingDistance;
    timeNextInstruction = other.timeNextInstruction;
    distanceNextInstruction = other.distanceNextInstruction;

    return *this;
}

#include "moc_qgeonavigator.cpp"

QTM_END_NAMESPACE
