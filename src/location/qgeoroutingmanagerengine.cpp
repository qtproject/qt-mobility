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

#include "qgeoroutingmanagerengine.h"
#include "qgeoroutingmanagerengine_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoutingManagerEngine

    \brief The QGeoRoutingManagerEngine class provides an interface and
    convenience methods to implementers of QGeoServiceProvider plugins who want
    to provide access to geographic routing information.

    \ingroup maps-impl

    The QGeoRoutingManagerEngine class is used to define the service specific
    behaviour and metadata required by QGeoRoutingManager.

    Subclasses of QGeoRoutingManagerEngine need to provide an implementation of
    calculateRoute().

    In the default implementation, supportsRouteUpdates() returns false and
    updateRoute() returns a QGeoRouteReply object containing a
    QGeoRouteReply::UnsupportedOptionError.

    If the routing service supports updating routes as they are being
    travelled, the subclass should provide an implementation of updateRoute()
    and call setSupportsRouteUpdates(true) at some point in time before
    updateRoute() is called.

    The function setSeupportsRouteUpdates() is one of several functions which
    configure the reported capabilities of the engine.  If the capabilities
    of an engine differ from the default values these functions should be
    used so that the reported capabilies are accurate.

    It is important that this is done before calculateRoute(), updateRoute()
    or any of the capability reporting functions are used to prevent
    incorrect or inconsistent behaviour.

    \sa QGeoRoutingManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
*/
QGeoRoutingManagerEngine::QGeoRoutingManagerEngine(const QMap<QString, QString> &parameters, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoRoutingManagerEnginePrivate(parameters)) {}

/*!
    Destroys this engine.
*/
QGeoRoutingManagerEngine::~QGeoRoutingManagerEngine()
{
    delete d_ptr;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoRoutingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
QString QGeoRoutingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Returns the parameters used in the creation of this engine object.
*/
QMap<QString, QString> QGeoRoutingManagerEngine::managerParameters() const
{
    return d_ptr->managerParameters;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QGeoRoutingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
int QGeoRoutingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
\fn QGeoRouteReply* QGeoRoutingManagerEngine::calculateRoute(const QGeoRouteRequest& request)

    Begins the calculation of the route specified by \a request.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This engine and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the calculated route or routes.

    If \a request includes features which are not supported by this engine, as
    reported by the methods in this engine, then a
    QGeoRouteReply::UnsupportedOptionError will occur.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManagerEngine::finished(),
    QGeoRoutingManagerEngine::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
*/

/*!
    Begins the process of updating \a route based on the current position \a
    position.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This engine and the returned QGeoRouteReply object will emit signals
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

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManagerEngine::finished(),
    QGeoRoutingManagerEngine::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
*/
QGeoRouteReply* QGeoRoutingManagerEngine::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    Q_UNUSED(route)
    Q_UNUSED(position)
    return new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError,
                              "The updating of routes is not supported by this service provider.", this);
}

/*!
    Sets whether this engine supports updating routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support updating routes.
*/
void QGeoRoutingManagerEngine::setSupportsRouteUpdates(bool supported)
{
    d_ptr->supportsRouteUpdates = supported;
}

/*!
    Returns whether this engine supports updating routes.
*/
bool QGeoRoutingManagerEngine::supportsRouteUpdates() const
{
    return d_ptr->supportsRouteUpdates;
}

/*!
    Sets whether this engine supports request for alternative routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support alternative routes.
*/
void QGeoRoutingManagerEngine::setSupportsAlternativeRoutes(bool supported)
{
    d_ptr->supportsAlternativeRoutes = supported;
}

/*!
    Returns whether this engine supports request for alternative routes.
*/
bool QGeoRoutingManagerEngine::supportsAlternativeRoutes() const
{
    return d_ptr->supportsAlternativeRoutes;
}

/*!
    Sets whether this engine supports request for excluding areas from routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support excluding areas.
*/
void QGeoRoutingManagerEngine::setSupportsExcludeAreas(bool supported)
{
    d_ptr->supportsExcludeAreas = supported;
}

/*!
    Returns whether this engine supports request for excluding areas from routes.
*/
bool QGeoRoutingManagerEngine::supportsExcludeAreas() const
{
    return d_ptr->supportsExcludeAreas;
}

/*!
    Sets the travel modes supported by this engine to \a travelModes.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no travel modes at all.
*/
void QGeoRoutingManagerEngine::setSupportedTravelModes(QGeoRouteRequest::TravelModes travelModes)
{
    d_ptr->supportedTravelModes = travelModes;
}

/*!
    Returns the travel modes supported by this engine.
*/
QGeoRouteRequest::TravelModes QGeoRoutingManagerEngine::supportedTravelModes() const
{
    return d_ptr->supportedTravelModes;
}

/*!
    Sets the types of features that this engine can avoid during route planning to \a avoidFeatureTypes.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support avoiding features.
*/
void QGeoRoutingManagerEngine::setSupportedAvoidFeatureTypes(QGeoRouteRequest::AvoidFeatureTypes avoidFeatureTypes)
{
    d_ptr->supportedAvoidFeatureTypes = avoidFeatureTypes;
}

/*!
    Returns the types of features that this engine can avoid during route planning.
*/
QGeoRouteRequest::AvoidFeatureTypes QGeoRoutingManagerEngine::supportedAvoidFeatureTypes() const
{
    return d_ptr->supportedAvoidFeatureTypes;
}

/*!
    Sets the route optimizations supported by this engine to \a optimizations.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no route optimizations at all.
*/
void QGeoRoutingManagerEngine::setSupportedRouteOptimizations(QGeoRouteRequest::RouteOptimizations optimizations)
{
    d_ptr->supportedRouteOptimizations = optimizations;
}

/*!
    Returns the route optimizations supported by this engine.
*/
QGeoRouteRequest::RouteOptimizations QGeoRoutingManagerEngine::supportedRouteOptimizations() const
{
    return d_ptr->supportedRouteOptimizations;
}

/*!
    Sets the levels of detail for routing segments which can be
    requested by this engine to \a segmentDetails.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no segment detail at all.
*/
void QGeoRoutingManagerEngine::setSupportedSegmentDetails(QGeoRouteRequest::SegmentDetails segmentDetails)
{
    d_ptr->supportedSegmentDetails = segmentDetails;
}

/*!
    Returns the levels of detail for routing segments which can be
    requested by this engine.
*/
QGeoRouteRequest::SegmentDetails QGeoRoutingManagerEngine::supportedSegmentDetails() const
{
    return d_ptr->supportedSegmentDetails;
}

/*!
    Sets the levels of detail for navigation instructions which can be
    requested by this engine to \a instructionDetails.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no instruction detail at all.
*/
void QGeoRoutingManagerEngine::setSupportedInstructionDetails(QGeoRouteRequest::InstructionDetails instructionDetails)
{
    d_ptr->supportedInstructionDetails = instructionDetails;
}

/*!
    Returns the levels of detail for navigation instructions which can be
    requested by this engine.
*/
QGeoRouteRequest::InstructionDetails QGeoRoutingManagerEngine::supportedInstructionDetails() const
{
    return d_ptr->supportedInstructionDetails;
}

/*!
\fn void QGeoRoutingManagerEngine::finished(QGeoRouteReply* reply)

This signal is emitted when \a reply has finished processing.

If reply::error() equals QGeoRouteReply::NoError then the processing
finished successfully.

This signal and QGeoRouteReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*!
\fn void QGeoRoutingManagerEngine::error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString)

This signal is emitted when an error has been detected in the processing of
\a reply.  The QGeoRoutingManagerEngine::finished() signal will probably follow.

The error will be described by the error code \a error.  If \a errorString is
not empty it will contain a textual description of the error.

This signal and QGeoRouteReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingManagerEnginePrivate::QGeoRoutingManagerEnginePrivate(const QMap<QString, QString> &parameters)
        : managerParameters(parameters),
        managerVersion(-1),
        supportsRouteUpdates(false),
        supportsAlternativeRoutes(false),
        supportsExcludeAreas(false) {}

QGeoRoutingManagerEnginePrivate::QGeoRoutingManagerEnginePrivate(const QGeoRoutingManagerEnginePrivate &other)
        : managerName(other.managerName),
        managerParameters(other.managerParameters),
        managerVersion(other.managerVersion),
        supportsRouteUpdates(other.supportsRouteUpdates),
        supportsAlternativeRoutes(other.supportsAlternativeRoutes),
        supportsExcludeAreas(other.supportsExcludeAreas),
        supportedTravelModes(other.supportedTravelModes),
        supportedAvoidFeatureTypes(other.supportedAvoidFeatureTypes),
        supportedRouteOptimizations(other.supportedRouteOptimizations),
        supportedSegmentDetails(other.supportedSegmentDetails),
        supportedInstructionDetails(other.supportedInstructionDetails) {}

QGeoRoutingManagerEnginePrivate::~QGeoRoutingManagerEnginePrivate() {}

QGeoRoutingManagerEnginePrivate& QGeoRoutingManagerEnginePrivate::operator= (const QGeoRoutingManagerEnginePrivate & other)
{
    managerName = other.managerName;
    managerParameters = other.managerParameters;
    managerVersion = other.managerVersion;
    supportsRouteUpdates = other.supportsRouteUpdates;
    supportsAlternativeRoutes = other.supportsAlternativeRoutes;
    supportsExcludeAreas = other.supportsExcludeAreas;
    supportedTravelModes = other.supportedTravelModes;
    supportedAvoidFeatureTypes = other.supportedAvoidFeatureTypes;
    supportedRouteOptimizations = other.supportedRouteOptimizations;
    supportedSegmentDetails = other.supportedSegmentDetails;
    supportedInstructionDetails = other.supportedInstructionDetails;

    return *this;
}

#include "moc_qgeoroutingmanagerengine.cpp"

QTM_END_NAMESPACE
