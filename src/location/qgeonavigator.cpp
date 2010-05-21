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

QGeoNavigator& QGeoNavigator::operator = (const QGeoNavigator &other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

void QGeoNavigator::setPositionSource(QGeoPositionInfoSource *positionSource)
{
    d_ptr->positionSource = positionSource;
}

QGeoPositionInfoSource* QGeoNavigator::positionSource() const
{
    return d_ptr->positionSource;
}

void QGeoNavigator::setRoutingManager(QGeoRoutingManager *routingManager)
{
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
}

void QGeoNavigator::depart()
{
}

void QGeoNavigator::updatePosition(const QGeoPositionInfo &positionInfo)
{
}

void QGeoNavigator::routeResultsAvailable()
{
}

/*******************************************************************************
*******************************************************************************/

QGeoNavigatorPrivate::QGeoNavigatorPrivate() {}

QGeoNavigatorPrivate::QGeoNavigatorPrivate(const QGeoNavigatorPrivate &other) {}

QGeoNavigatorPrivate::~QGeoNavigatorPrivate() {}

QGeoNavigatorPrivate& QGeoNavigatorPrivate::operator= (const QGeoNavigatorPrivate &other)
{
    return *this;
}

#include "moc_qgeonavigator.cpp"

QTM_END_NAMESPACE
