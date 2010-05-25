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

/*!
    \class QGeoRoutingManager

    \brief The QGeoRoutingManager class provides support for geographic routing
    operations.

    \ingroup maps

    Instances of QGeoRoutingManager primarily provide support for the
    calculation and updating of routes. The calculateRoute() and updateRoute()
    methods return QGeoRouteReply objects, which manage these operations and
    report on the result of the operations and any errors which may have
    occurred.

    The QGeoRoutingManager class also contains functions which provide
    information on the capabilities and features supported by
    QGeoRoutingManager instances. Those who write subclasses of
    QGeoRoutingManager should take care to make sure that this capability
    information is set up correctly, otherwise clients may be denied access to
    functionality they would otherwise expect.
*/

/*!
    Constructs a new manager with the specified \a parent.

    This should only ever be called from subclasses of QGeoRoutingManager.
*/
QGeoRoutingManager::QGeoRoutingManager(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoRoutingManagerPrivate()) {}

/*!
    Destroys this manager.
*/
QGeoRoutingManager::~QGeoRoutingManager()
{
    delete d_ptr;
}

/*!
\fn QGeoRouteReply* QGeoRoutingManager::calculateRoute(const QGeoRouteRequest& request)

    Begins the calculation of the route specified by \a request.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This manager and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the calculated route or routes.

    If \a request includes features which are not supported by this manager, as
    reported by the methods in this manager, then a
    QGeoRouteReply::UnsupportedOptionError will occur.
*/

/*!
\fn QGeoRouteReply* QGeoRoutingManager::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)

    Begins the process of updating \a route based on the current position \a
    position.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This manager and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsRouteUpdates() returns false an
    QGeoRouteReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the updated route.

    The returned route could be entirely different to the original route,
    especially if \a position is far enough away from the initial route.
    Otherwise the route will be similar, although the remaining time and
    distance will be updated and any segments of the original route which
    have been traversed will be removed.
*/

/*!
    Sets whether this manager supports updating routes to \a supported.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support updating routes.
*/
void QGeoRoutingManager::setSupportsRouteUpdates(bool supported)
{
    d_ptr->supportsRouteUpdates = supported;
}

/*!
    Returns whether this manager supports updating routes.
*/
bool QGeoRoutingManager::supportsRouteUpdates() const
{
    return d_ptr->supportsRouteUpdates;
}

/*!
    Sets whether this manager supports request for alternative routes to \a supported.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support alternative routes.
*/
void QGeoRoutingManager::setSupportsAlternativeRoutes(bool supported)
{
    d_ptr->supportsAlternativeRoutes = supported;
}

/*!
    Returns whether this manager supports request for alternative routes.
*/
bool QGeoRoutingManager::supportsAlternativeRoutes() const
{
    return d_ptr->supportsAlternativeRoutes;
}

/*!
    Sets the travel modes supported by this manager to \a travelModes.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it supports no travel modes at all.
*/
void QGeoRoutingManager::setSupportedTravelModes(QGeoRouteRequest::TravelModes travelModes)
{
    d_ptr->supportedTravelModes = travelModes;
}

/*!
    Returns the travel modes supported by this manager.
*/
QGeoRouteRequest::TravelModes QGeoRoutingManager::supportedTravelModes() const
{
    return d_ptr->supportedTravelModes;
}

/*!
    Sets the types of features that this manager can avoid during route planning to \a avoidFeatureTypes.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support avoiding features.
*/
void QGeoRoutingManager::setSupportedAvoidFeatureTypes(QGeoRouteRequest::AvoidFeatureTypes avoidFeatureTypes)
{
    d_ptr->supportedAvoidFeatureTypes = avoidFeatureTypes;
}

/*!
    Returns the types of features that this manager can avoid during route planning.
*/
QGeoRouteRequest::AvoidFeatureTypes QGeoRoutingManager::supportedAvoidFeatureTypes() const
{
    return d_ptr->supportedAvoidFeatureTypes;
}

/*!
    Sets the route optimizations supported by this manager to \a optimizations.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it supports no route optimizations at all.
*/
void QGeoRoutingManager::setSupportedRouteOptimizations(QGeoRouteRequest::RouteOptimizations optimizations)
{
    d_ptr->supportedRouteOptimizations = optimizations;
}

/*!
    Returns the route optimizations supported by this manager.
*/
QGeoRouteRequest::RouteOptimizations QGeoRoutingManager::supportedRouteOptimizations() const
{
    return d_ptr->supportedRouteOptimizations;
}

/*!
    Sets the levels of instruction details supported by this manager to \a intructionDetails.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it supports no instruction detail at all.
*/
void QGeoRoutingManager::setSupportedInstructionDetails(QGeoRouteRequest::InstructionDetails instructionDetails)
{
    d_ptr->supportedInstructionDetails = instructionDetails;
}

/*!
    Returns the levels of instruction details supported by this manager.
*/
QGeoRouteRequest::InstructionDetails QGeoRoutingManager::supportedInstructionDetails() const
{
    return d_ptr->supportedInstructionDetails;
}

/*!
\fn void QGeoRoutingService::finished(QGeoRouteReply* reply)

This signal is emitted when \a reply has finished processing.

If reply::error() equals QGeoRouteReply::NoError then the processing
finished successfully.

This signal and QGeoRouteReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*!
\fn void QGeoRoutingService::error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString

This signal is emitted when an error has been detected in the processing of
\a reply.  The QGeoRoutingManager::finished() signal will probably follow.

The error will be described by the error code \error.  If \a errorString is
not empty it will contain a textual description of the error.

This signal and QGeoRouteReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingManagerPrivate::QGeoRoutingManagerPrivate()
    : supportsRouteUpdates(false),
    supportsAlternativeRoutes(false) {}

QGeoRoutingManagerPrivate::QGeoRoutingManagerPrivate(const QGeoRoutingManagerPrivate &other)
    : supportsRouteUpdates(other.supportsRouteUpdates),
    supportsAlternativeRoutes(other.supportsAlternativeRoutes),
    supportedTravelModes(other.supportedTravelModes),
    supportedAvoidFeatureTypes(other.supportedAvoidFeatureTypes),
    supportedRouteOptimizations(other.supportedRouteOptimizations),
    supportedInstructionDetails(other.supportedInstructionDetails) {}

QGeoRoutingManagerPrivate::~QGeoRoutingManagerPrivate() {}

QGeoRoutingManagerPrivate& QGeoRoutingManagerPrivate::operator= (const QGeoRoutingManagerPrivate &other)
{
    supportsRouteUpdates = other.supportsRouteUpdates;
    supportsAlternativeRoutes = other.supportsAlternativeRoutes;
    supportedTravelModes = other.supportedTravelModes;
    supportedAvoidFeatureTypes = other.supportedAvoidFeatureTypes;
    supportedRouteOptimizations = other.supportedRouteOptimizations;
    supportedInstructionDetails = other.supportedInstructionDetails;

    return *this;
}

#include "moc_qgeoroutingmanager.cpp"

QTM_END_NAMESPACE
