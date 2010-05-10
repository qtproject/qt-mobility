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

#include "qgeoroute.h"
#include "qgeorouterequestoptions.h"

#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoRouteReply;

class QGeoRoutingServicePrivate;

class Q_LOCATION_EXPORT QGeoRoutingService : public QObject
{

    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

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

    virtual ~QGeoRoutingService();

    virtual QGeoRouteReply* requestRoute(const QGeoCoordinate &origin,
                                         const QGeoCoordinate &destination,
                                         QGeoRoute::DirectionsDetail detail = QGeoRoute::NoDirections,
                                         const QGeoRouteRequestOptions &requestOptions = QGeoRouteRequestOptions()) = 0;
    virtual QGeoRouteReply* requestRoute(const QList<QGeoCoordinate> &waypoints,
                                         QGeoRoute::DirectionsDetail detail = QGeoRoute::NoDirections,
                                         const QGeoRouteRequestOptions &requestOptions = QGeoRouteRequestOptions()) = 0;

    virtual QGeoRouteReply* updateRoute(const QGeoRoute &route,
                                        const QGeoCoordinate &currentPosition) = 0;

    QGeoRoute::TravelModes supportedTravelModes() const;
    QGeoRoute::AvoidFeatureTypes supportedAvoidFeatureTypes() const;
    QGeoRoute::RouteOptimizations supportedRouteOptimizations() const;

    // see above
    //TransitOptionTypes supportedTransitOptionTypes() const;

    QGeoRoute::DirectionsDetails supportedDirectionDetails() const;
    bool supportsUpdatingRoutes() const;

protected:
    QGeoRoutingService();

    void setSupportedTravelModes(QGeoRoute::TravelModes travelModes);
    void setSupportedAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes);
    void setSupportedRouteOptimizations(QGeoRoute::RouteOptimizations routeOptimizations);
    // void setSupportedTransitOptionType(TransitOptionTypes transitOptionTypes);
    void setSupportedDirectionDetails(QGeoRoute::DirectionsDetails directionsDetails);
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

QTM_END_NAMESPACE

QT_END_HEADER

#endif
