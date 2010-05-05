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

#include "qgeoroutingservice.h"
#include "qgeoroutingservice_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoRoutingService
  \brief The QGeoRoutingService class anages requests to and replies from
  a geographical routing service.
  \ingroup maps

    The request functions return a QGeoRouteReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoRouteReply objects and the QGeoRoutingService instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoRouteReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

    \sa QGeoRouteReply
*/

/*!
  \enum QGeoRoutingService::ErrorCode

    Describes the type of error which occurred when a QGeoMappingService instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    Constructs a QGeoRoutingService object.
*/
QGeoRoutingService::QGeoRoutingService()
    : d_ptr(new QGeoRoutingServicePrivate()) {}

/*!
    Destroys this QGeoRoutingService object.
*/
QGeoRoutingService::~QGeoRoutingService()
{
    Q_D(QGeoRoutingService);
    delete d;
}

/*!
\fn QGeoRouteReply* QGeoRoutingService::requestRoute(const QGeoCoordinate &origin,
                                                     const QGeoCoordinate &destination,
                                                     QGeoRoute::DirectionsDetail detail,
                                                     const QGeoRouteRequestOptions &requestOptions)
*/

/*!
\fn QGeoRouteReply* QGeoRoutingService::requestRoute(const QList<QGeoCoordinate> &waypoints,
                                                     QGeoRoute::DirectionsDetail detail,
                                                     const QGeoRouteRequestOptions &requestOptions) = 0;
*/

/*!
\fn QGeoRouteReply* QGeoRoutingService::updateRoute(const QGeoRoute &route,
                                                    const QGeoCoordinate &currentPosition)
*/

/*!
    Returns the travel modes supported by this QGeoRoutingService instance.

    \sa QGeoRoute::TravelModes
    \sa QGeoRoutingService::setSupportedTravelModes()
*/
QGeoRoute::TravelModes QGeoRoutingService::supportedTravelModes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedTravelModes;
}

/*!
    Returns which features this QGeoRoutingService instance is able to plan
    to avoid when creating routes.

    \sa QGeoRoute::AvoidFeatureTypes
    \sa QGeoRoutingService::setSupportedAvoidFeatureTypes()
*/
QGeoRoute::AvoidFeatureTypes QGeoRoutingService::supportedAvoidFeatureTypes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedAvoidFeatureTypes;
}

/*!
    Returns which route optimizations are supported by this QGeoRoutingService
    instance.

    \sa QGeoRoute::RouteOptimizations
    \sa QGeoRoute::setSupportedRouteOptimizations()
*/
QGeoRoute::RouteOptimizations QGeoRoutingService::supportedRouteOptimizations() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedRouteOptimizations;
}


/*!
    Returns the level of details for directions which this QGeoRoutingService
    supports.

    \sa QGeoRoute::DirectionsDetails
    \sa QGeoRoute::setSupportedDirectionDetails()
*/
QGeoRoute::DirectionsDetails QGeoRoutingService::supportedDirectionDetails() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedDirectionDetails;
}

/*!
    Returns whether this QGeoRoutingService supports updating a previously
    calculated route by specifying a new position.

    This is typically supported so that routes can be replanned based on the
    position of a user after they have left

    \sa QGeoRoutingService::setSupportsUpdatingRoutes()
*/
bool QGeoRoutingService::supportsUpdatingRoutes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportsUpdatingRoutes;
}

/*!
    Sets the travel modes supported by this QGeoRoutingService instance to
    \a supportedTravelModes.

    Subclasses of QGeoRoutingService should use this function to ensure that
    supportedTravelModes() provides accurate information.

    \sa QGeoRoute::TravelModes
    \sa QGeoRoutingService::supportedTravelModes()
*/
void QGeoRoutingService::setSupportedTravelModes(QGeoRoute::TravelModes travelModes)
{
    Q_D(QGeoRoutingService);
    d->supportedTravelModes = travelModes;
}

/*!
    Sets the features that this QGeoRoutingService instance is able to avoid to
    \a avoidFeatureTypes.

    Subclasses of QGeoRoutingService should use this function to ensure that
    supportedAvoidFeatureTypes() provides accurate information.

    \sa QGeoRoute::AvoidFeatureTypes
    \sa QGeoRoutingService::supportedAvoidFeatureTypes()
*/
void QGeoRoutingService::setSupportedAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes)
{
    Q_D(QGeoRoutingService);
    d->supportedAvoidFeatureTypes = avoidFeatureTypes;
}

/*!
    Sets the route optimzations that this QGeoRoutingService instance supports
    to \a routeOptimizations.

    Subclasses of QGeoRoutingService should use this function to ensure that
    supportedRouteOptimizations() provides accurate information.

    \sa QGeoRoute::RouteOptimizations
    \sa QGeoRoutingService::supportedRouteOptimizations()
*/
void QGeoRoutingService::setSupportedRouteOptimizations(QGeoRoute::RouteOptimizations routeOptimizations)
{
    Q_D(QGeoRoutingService);
    d->supportedRouteOptimizations = routeOptimizations;
}

/*!
    Sets the levels of details for directions that this QGeoRoutingService
    instance supports to \a directionsDetails.

    Subclasses of QGeoRoutingService should use this function to ensure that
    supportedDirectionDetails() provides accurate information.

    \sa QGeoRoute::DirectionsDetails
    \sa QGeoRoutingService::supportedDirectionDetails()
*/
void QGeoRoutingService::setSupportedDirectionDetails(QGeoRoute::DirectionsDetails directionsDetails)
{
    Q_D(QGeoRoutingService);
    d->supportedDirectionDetails = directionsDetails;
}

/*!
    Sets whether or not this QGeoRoutingService instance supports updating
    of previously calculated routes, according to the value of \a updatingRoutes.

    Subclasses of QGeoRoutingService should use this function to ensure that
    supportsUpdatingRoutes() provides accurate information.

    \sa QGeoRoutingService::supportsUpdatingRoutes()
*/
void QGeoRoutingService::setSupportsUpdatingRoutes(bool updatingRoutes)
{
    Q_D(QGeoRoutingService);
    d->supportsUpdatingRoutes = updatingRoutes;
}

/*!
    \fn void QGeoRoutingService::replyFinished(QGeoRouteReply *reply)

    Indicates that a request handled by this QGeoRoutingService object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoRoutingService instance when the request was issued, and that the
    QGeoRouteReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoRouteReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoRouteReply::finished()
*/

/*!
    \fn void QGeoRoutingService::replyError(QGeoRouteReply *reply,
                                            QGeoRoutingService::ErrorCode errorCode,
                                            QString errorString)

    Indicates that a request handled by this QGeoeRoutingService object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoRouteReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoRoutingService instance when the request was issued, and that the
    QGeoRouteReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoRouteReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoRouteReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingServicePrivate::QGeoRoutingServicePrivate() {}

QGeoRoutingServicePrivate::QGeoRoutingServicePrivate(const QGeoRoutingServicePrivate &other)
    : supportedTravelModes(other.supportedTravelModes),
    supportedAvoidFeatureTypes(other.supportedAvoidFeatureTypes),
    supportedRouteOptimizations(other.supportedRouteOptimizations),
    //supportedTransitOptionTypes(other.supportedTransitOptionTypes),
    supportedDirectionDetails(other.supportedDirectionDetails),
    supportsUpdatingRoutes(other.supportsUpdatingRoutes) {}

QGeoRoutingServicePrivate::~QGeoRoutingServicePrivate() {}

QGeoRoutingServicePrivate& QGeoRoutingServicePrivate::operator= (const QGeoRoutingServicePrivate &other)
{
    supportedTravelModes = other.supportedTravelModes;
    supportedAvoidFeatureTypes = other.supportedAvoidFeatureTypes;
    supportedRouteOptimizations = other.supportedRouteOptimizations;
    //supportedTransitOptionTypes = other.supportedTransitOptionTypes;
    supportedDirectionDetails = other.supportedDirectionDetails;
    supportsUpdatingRoutes = other.supportsUpdatingRoutes;

    return *this;
}

#include "moc_qgeoroutingservice.cpp"

QTM_END_NAMESPACE
