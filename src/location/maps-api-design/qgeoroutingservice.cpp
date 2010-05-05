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
  \brief The QGeoRoutingService class
  \ingroup maps

*/

/*!
  \enum QGeoRoutingService::ErrorCode

description

  \value NoError description
*/

/*!
*/
QGeoRoutingService::QGeoRoutingService()
    : d_ptr(new QGeoRoutingServicePrivate()) {}

/*!
*/
QGeoRoutingService::~QGeoRoutingService()
{
    Q_D(QGeoRoutingService);
    delete d;
}

/*!
*/
QGeoRoute::TravelModes QGeoRoutingService::supportedTravelModes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedTravelModes;
}

/*!
*/
QGeoRoute::AvoidFeatureTypes QGeoRoutingService::supportedAvoidFeatureTypes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedAvoidFeatureTypes;
}

/*!
*/
QGeoRoute::RouteOptimizations QGeoRoutingService::supportedRouteOptimizations() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedRouteOptimizations;
}

///*!
//*/
//QGeoRoutingService::TransitOptionTypes QGeoRoutingService::supportedTransitOptionTypes() const
//{
//    Q_D(const QGeoRoutingService);
//    return d->supportedTravelOptionTypes;
//}

/*!
*/
QGeoRoute::DirectionsDetails QGeoRoutingService::supportedDirectionDetails() const
{
    Q_D(const QGeoRoutingService);
    return d->supportedDirectionDetails;
}

/*!
*/
bool QGeoRoutingService::supportsUpdatingRoutes() const
{
    Q_D(const QGeoRoutingService);
    return d->supportsUpdatingRoutes;
}


        // Option 1, current favourite
        // TODO - fix include order fiasco preventing requestOptions using default constructor
//        virtual QGeoRouteReply* requestRoute(const QGeoCoordinate &origin,
//                                    const QGeoCoordinate &destination,
//                                    DirectionsDetail detail = NoDirections,
//                                    const QGeoRouteRequestOptions &requestOptions = 0) = 0;
//        virtual QGeoRouteReply* requestRoute(const QList<QGeoCoordinate> &waypoints,
//                                     DirectionsDetail detail = NoDirections,
//                                    const QGeoRouteRequestOptions &requestOptions = 0) = 0;

        // Option 2 - roll pathing info and directions level-of-detail into request
        //virtual QGeoRouteReply* requestRoute(const QGeoRouteRequestOptions &requestOptions) = 0;

        // Present in both options
/*!
\fn QGeoRouteReply* QGeoRoutingService::updateRoute(const QGeoRoute &route,
                                                    const QGeoCoordinate &currentPosition)
*/

/*!
\fn void QGeoRoutingService::replyFinished(QGeoRouteReply *reply)
*/

/*!
\fn void QGeoRoutingService::replyError(QGeoRouteReply *reply,
                                        QGeoRoutingService::ErrorCode errorCode,
                                        QString errorString)
*/

/*!
*/
void QGeoRoutingService::setSupportedTravelModes(QGeoRoute::TravelModes supportedTravelModes)
{
    Q_D(QGeoRoutingService);
    d->supportedTravelModes = supportedTravelModes;
}

/*!
*/
void QGeoRoutingService::setSupportedAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes)
{
    Q_D(QGeoRoutingService);
    d->supportedAvoidFeatureTypes = avoidFeatureTypes;
}

/*!
*/
void QGeoRoutingService::setSupportedRouteOptimizations(QGeoRoute::RouteOptimizations routeOptimizations)
{
    Q_D(QGeoRoutingService);
    d->supportedRouteOptimizations = routeOptimizations;
}

///*!
//*/
// void QGeoRoutingService::setSupportedTransitOptionType(QGeoRoutingService::TransitOptionTypes transitOptionTypes)
//{
//    Q_D(QGeoRoutingService);
//    d->supportedTransitOptionTypes = transitOptionTypes;
//}

/*!
*/
void QGeoRoutingService::setSupportedDirectionDetails(QGeoRoute::DirectionsDetails directionsDetails)
{
    Q_D(QGeoRoutingService);
    d->supportedDirectionDetails = directionsDetails;
}

/*!
*/
void QGeoRoutingService::setSupportsUpdatingRoutes(bool updatingRoutes)
{
    Q_D(QGeoRoutingService);
    d->supportsUpdatingRoutes = updatingRoutes;
}

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
