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

#include "qgeoroutingmanager.h"
#include "qgeoroutingmanager_p.h"

QTM_BEGIN_NAMESPACE

QGeoRoutingManager::QGeoRoutingManager(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoRoutingManagerPrivate()) {}

QGeoRoutingManager::~QGeoRoutingManager()
{
    delete d_ptr;
}

/*!
\fn QGeoRouteReply* QGeoRoutingService::calculateRoute(const QGeoRouteRequest& request)
*/

/*!
*/
void QGeoRoutingManager::setSupportsAlternativeRoutes(bool supported)
{
    d_ptr->supportsAlternativeRoutes = supported;
}

/*!
*/
bool QGeoRoutingManager::supportsAlternativeRoutes() const
{
    return d_ptr->supportsAlternativeRoutes;
}

/*!
*/
void QGeoRoutingManager::setSupportedTravelModes(QGeoRouteRequest::TravelModes travelModes)
{
    d_ptr->supportedTravelModes = travelModes;
}

/*!
*/
QGeoRouteRequest::TravelModes QGeoRoutingManager::supportedTravelModes() const
{
    return d_ptr->supportedTravelModes;
}

/*!
*/
void QGeoRoutingManager::setSupportedAvoidFeatureTypes(QGeoRouteRequest::AvoidFeatureTypes avoidFeatureTypes)
{
    d_ptr->supportedAvoidFeatureTypes = avoidFeatureTypes;
}

/*!
*/
QGeoRouteRequest::AvoidFeatureTypes QGeoRoutingManager::supportedAvoidFeatureTypes() const
{
    return d_ptr->supportedAvoidFeatureTypes;
}

/*!
*/
void QGeoRoutingManager::setSupportedRouteOptimizations(QGeoRouteRequest::RouteOptimizations optimizations)
{
    d_ptr->supportedRouteOptimizations = optimizations;
}

/*!
*/
QGeoRouteRequest::RouteOptimizations QGeoRoutingManager::supportedRouteOptimizations() const
{
    return d_ptr->supportedRouteOptimizations;
}

/*!
*/
void QGeoRoutingManager::setSupportedInstructionDetails(QGeoRouteRequest::InstructionDetails instructionDetails)
{
    d_ptr->supportedInstructionDetails = instructionDetails;
}

/*!
*/
QGeoRouteRequest::InstructionDetails QGeoRoutingManager::supportedInstructionDetails() const
{
    return d_ptr->supportedInstructionDetails;
}

/*!
\fn void QGeoRoutingService::finished(QGeoRouteReply* reply)
*/

/*!
\fn void QGeoRoutingService::error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingManagerPrivate::QGeoRoutingManagerPrivate()
    : supportsAlternativeRoutes(false) {}

QGeoRoutingManagerPrivate::QGeoRoutingManagerPrivate(const QGeoRoutingManagerPrivate &other)
    : supportsAlternativeRoutes(other.supportsAlternativeRoutes),
    supportedTravelModes(other.supportedTravelModes),
    supportedAvoidFeatureTypes(other.supportedAvoidFeatureTypes),
    supportedRouteOptimizations(other.supportedRouteOptimizations),
    supportedInstructionDetails(other.supportedInstructionDetails) {}

QGeoRoutingManagerPrivate::~QGeoRoutingManagerPrivate() {}

QGeoRoutingManagerPrivate& QGeoRoutingManagerPrivate::operator= (const QGeoRoutingManagerPrivate &other)
{
    supportsAlternativeRoutes = other.supportsAlternativeRoutes;
    supportedTravelModes = other.supportedTravelModes;
    supportedAvoidFeatureTypes = other.supportedAvoidFeatureTypes;
    supportedRouteOptimizations = other.supportedRouteOptimizations;
    supportedInstructionDetails = other.supportedInstructionDetails;

    return *this;
}

#include "moc_qgeoroutingmanager.cpp"

QTM_END_NAMESPACE
