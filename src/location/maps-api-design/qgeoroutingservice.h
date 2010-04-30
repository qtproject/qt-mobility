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

#ifndef QGEOROUTINGSERVICE_H
#define QGEOROUTINGSERVICE_H

#include "qmobilityglobal.h"

#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoRoute;
class QGeoRouteReply;
class QGeoRouteRequestOptions;

class QGeoRoutingServicePrivate;

class Q_LOCATION_EXPORT QGeoRoutingService : public QObject {

    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

    enum TravelMode {
        CarTravel = 0x0001,
        PedestrianTravel = 0x0002,
        BicycleTravel = 0x0004,
        PublicTransitTravel = 0x0008,
        TruckTravel = 0x000F
    };
    Q_DECLARE_FLAGS(TravelModes, TravelMode)

    enum AvoidFeatureType {
        AvoidNothing = 0x00000000,
        AvoidTolls = 0x00000001,
        AvoidHighways = 0x00000002,
        AvoidPublicTransit = 0x00000004,
        AvoidFerries = 0x00000008,
        AvoidTunnels = 0x0000000F,
        AvoidDirtRoads = 0x00000010,
        AvoidPark = 0x00000020,
        AvoidMotorPoolLanes = 0x00000040
    };
    Q_DECLARE_FLAGS(AvoidFeatureTypes, AvoidFeatureType)

    enum RouteOptimization {
        ShortestRoute = 0x0001,
        FastestRoute = 0x0002,
        MostEconomicRoute = 0x0004,
        MostScenicRoute = 0x0008
    };
    Q_DECLARE_FLAGS(RouteOptimizations, RouteOptimization)

    // TODO
    //- move to QGeoRouteTransitOption base class once when we require it
    //- used to determine subclass type from instances of the base class
    /*
    enum TransitOptionType {
        CarTravel = 0x0001,
        PedestrianTravel = 0x0002,
        BicycleTravel = 0x0004,
        PublicTransitTravel = 0x0008,
        TruckTravel = 0x000F
    };
    Q_DECLARE_FLAGS(TransitOptionTypes, TransitOptionType)
    */

    enum DirectionsDetail {
        NoDirections = 0x00001,
        BasicDirections = 0x0002,
        DetailedDirections = 0x0004
    };
    Q_DECLARE_FLAGS(DirectionsDetails, DirectionsDetail)

    QGeoRoutingService();
    virtual ~QGeoRoutingService();

    // Option 1, current favourite
    // TODO - fix include order fiasco preventing requestOptions using default constructor
    /*
    virtual QGeoRouteReply* requestRoute(const QGeoCoordinate &origin,
                                const QGeoCoordinate &destination,
                                DirectionsDetail detail = NoDirections,
                                const QGeoRouteRequestOptions &requestOptions = 0) = 0;
    virtual QGeoRouteReply* requestRoute(const QList<QGeoCoordinate> &waypoints,
                                 DirectionsDetail detail = NoDirections,
                                const QGeoRouteRequestOptions &requestOptions = 0) = 0;
    */

    // Option 2 - roll pathing info and directions level-of-detail into request
    virtual QGeoRouteReply* requestRoute(const QGeoRouteRequestOptions &requestOptions) = 0;

    // Present in both options
    virtual QGeoRouteReply* updateRoute(const QGeoRoute &route,
                                const QGeoCoordinate &currentPosition) = 0;

    TravelModes supportedTravelModes() const;
    AvoidFeatureTypes supportedAvoidFeatureTypes() const;
    RouteOptimizations supportedRouteOptimizations() const;

    // see above
    //TransitOptionTypes supportedTransitOptionTypes() const;

    DirectionsDetails supportedDirectionDetails() const;
    bool supportsUpdatingRoutes() const;

protected:
    void setSupportedTravelModes(TravelModes supportedTravelModes);
    void setSupportedAvoidFeatureTypes(AvoidFeatureTypes avoidFeatureTypes);
    void setSupportedRouteOptimizations(RouteOptimizations routeOptimizations);
    // void setSupportedTransitOptionType(TransitOptionTypes transitOptionTypes);
    void setSupportedDirectionDetails(DirectionsDetails directionsDetails);
    void setSupportsUpdatingRoutes(bool updatingRoutes);

    QGeoRoutingServicePrivate* d_ptr;

signals:
    void replyFinished(QGeoRouteReply *reply);
    void replyError(QGeoRouteReply *reply, 
                    QGeoRoutingService::ErrorCode errorCode,
                    QString errorString);

private:
    Q_DISABLE_COPY(QGeoRoutingService)
    Q_DECLARE_PRIVATE(QGeoRoutingService)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoutingService::TravelModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoutingService::AvoidFeatureTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoutingService::RouteOptimizations)
// see above
//Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoutingService::TransitOptionTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoutingService::DirectionsDetails)

QTM_END_NAMESPACE

QT_END_HEADER

#endif
