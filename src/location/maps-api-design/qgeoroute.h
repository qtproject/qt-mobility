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

#ifndef QGEOROUTE_H
#define QGEOROUTE_H

#include "qmobilityglobal.h"

#include <QList>

class QDateTime;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoBoundingBox;
class QGeoCoordinate;
class QGeoDistance;
class QGeoRouteSegment;
class QGeoNavigationInstruction;

class QGeoRoutePrivate;

class Q_LOCATION_EXPORT QGeoRoute {

public:
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

    enum DirectionsDetail {
        NoDirections = 0x00001,
        BasicDirections = 0x0002,
        DetailedDirections = 0x0004
    };
    Q_DECLARE_FLAGS(DirectionsDetails, DirectionsDetail)

    QGeoRoute();
    QGeoRoute(const QGeoRoute &other);
    ~QGeoRoute();

    QGeoRoute& operator = (const QGeoRoute &other);

    void setDirectionsDetail(DirectionsDetail directionsDetail);
    DirectionsDetail directionsDetail() const;

    void setRouteOverview(const QList<QGeoCoordinate> &routeOverview);
    QList<QGeoCoordinate> routeOverview() const;

    void setBounds(const QGeoBoundingBox &bounds);
    QGeoBoundingBox bounds() const;

    void setRouteSegments(const QList<const QGeoRouteSegment *> &routeSegments);
    QList<const QGeoRouteSegment *> routeSegments() const;

    void setEstimatedTravelTime(const QDateTime &travelTime);
    QDateTime estimatedTravelTime() const;

    void setLength(const QGeoDistance &length);
    QGeoDistance length() const;

    QGeoCoordinate closestPointOnRoute(const QGeoCoordinate &position) const;

private:
    QGeoRoutePrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGeoRoute)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoute::TravelModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoute::AvoidFeatureTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoute::RouteOptimizations)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRoute::DirectionsDetails)

QTM_END_NAMESPACE

QT_END_HEADER

#endif
