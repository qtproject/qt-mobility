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

#ifndef QGEOROUTEREQUEST_H
#define QGEOROUTEREQUEST_H

#include "qmobilityglobal.h"

#include <QList>
#include <QSharedDataPointer>
#include <QDateTime>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;

class QGeoRouteRequestPrivate;

class Q_LOCATION_EXPORT QGeoRouteRequest
{
public:
    enum TravelMode {
        CarTravel = 0x0001,
        PedestrianTravel = 0x0002,
        BicycleTravel = 0x0004,
        PublicTransitTravel = 0x0008,
        TruckTravel = 0x0010
    };
    Q_DECLARE_FLAGS(TravelModes, TravelMode)

    enum AvoidFeatureType {
        AvoidNothing = 0x00000000,
        AvoidTolls = 0x00000001,
        AvoidHighways = 0x00000002,
        AvoidPublicTransit = 0x00000004,
        AvoidFerries = 0x00000008,
        AvoidTunnels = 0x00000010,
        AvoidDirtRoads = 0x00000020,
        AvoidPark = 0x00000040,
        AvoidMotorPoolLanes = 0x00000080
    };
    Q_DECLARE_FLAGS(AvoidFeatureTypes, AvoidFeatureType)

    enum RouteOptimization {
        ShortestRoute = 0x0001,
        FastestRoute = 0x0002,
        MostEconomicRoute = 0x0004,
        MostScenicRoute = 0x0008
    };
    Q_DECLARE_FLAGS(RouteOptimizations, RouteOptimization)

    enum SegmentDetail {
        NoSegmentData = 0x0000,
        BasicSegmentData = 0x0001,
        DetailedSegmentData = 0x0002
    };
    Q_DECLARE_FLAGS(SegmentDetails, SegmentDetail)

    enum InstructionDetail {
        NoInstructions = 0x0000,
        BasicInstructions = 0x0001,
        DetailedInstructions = 0x0002
    };
    Q_DECLARE_FLAGS(InstructionDetails, InstructionDetail)

    QGeoRouteRequest(const QList<QGeoCoordinate> &waypoints = QList<QGeoCoordinate>());
    QGeoRouteRequest(const QGeoCoordinate &origin,
                     const QGeoCoordinate &destination);
    QGeoRouteRequest(const QGeoRouteRequest &other);

    ~QGeoRouteRequest();

    QGeoRouteRequest& operator= (const QGeoRouteRequest &other);

    void setWaypoints(const QList<QGeoCoordinate> &waypoints);
    QList<QGeoCoordinate> waypoints() const;

    // default to 0
    void setNumberAlternativeRoutes(int alternatives);
    int numberAlternativeRoutes() const;

    // default to TravelByCar
    void setTravelModes(TravelModes travelModes);
    TravelModes travelModes() const;

    // defaults to NoAvoidance
    void setAvoidFeatureTypes(AvoidFeatureTypes avoidFeatureTypes);
    AvoidFeatureTypes avoidFeatureTypes() const;

    // defaults to OptimizeFastest
    void setRouteOptimization(RouteOptimizations optimization);
    RouteOptimizations routeOptimization() const;

    // defaults to BasicSegmentData
    void setSegmentDetail(SegmentDetail segmentDetail);
    SegmentDetail segmentDetail() const;

    // default to BasicInstructions
    void setInstructionDetail(InstructionDetail instructionDetail);
    InstructionDetail instructionDetail() const;

    // defaults to empty - no subclass required yet
    // this is how we handle private / public / truck attributes
    //void setTransitOptions(const QList<const QGeoRouteTransitOptions *> &transitOptions);
    //QList<const QGeoRouteTransitOptions *> transitOptions() const;

private:
    QSharedDataPointer<QGeoRouteRequestPrivate> d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRouteRequest::TravelModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRouteRequest::AvoidFeatureTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRouteRequest::RouteOptimizations)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRouteRequest::SegmentDetails)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoRouteRequest::InstructionDetails)

QTM_END_NAMESPACE

QT_END_HEADER

#endif
