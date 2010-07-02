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

#ifndef QGEONAVIGATOR_H
#define QGEONAVIGATOR_H

#include "qgeoroutereply.h"

#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoPositionInfo;
class QGeoRoute;
class QGeoCoordinate;
class QGeoNavigationInstruction;

class QGeoNavigatorPrivate;

class QGeoPositionInfoSource;
class QGeoRoutingManager;

class Q_LOCATION_EXPORT QGeoNavigator : public QObject
{

    Q_OBJECT

public:
    enum Error {
        NoError,
        RoutingError,
        PositionError
    };

    enum NavigationStatus {
        NavigationNotDeparted,
        NavigationOnRoute,
        NavigationOffRoute,
        NavigationCompleted
    };

    QGeoNavigator(const QGeoRoutingManager *routingManager);
    QGeoNavigator(const QGeoNavigator &other);
    ~QGeoNavigator();

    QGeoNavigator& operator = (const QGeoNavigator &other);

    QGeoRoutingManager* routingManager() const;

    void setRouteWidth(qreal routeWidth);
    qreal routeWidth() const;

    QGeoRoute routeTravelled() const;
    QGeoRoute routeRemaining() const;

    int timeToNextSegment() const;
    qreal distanceToNextSegment() const;


    /*
     waypoints passed is internal, used to update the request if we go
     off track

     probably good to keep track of segments traversed, instructions presented
     as well

     depart clears waypoints passed and starts the position updates

     at each position update we want to find the closest point on the route
     (in the order of route traversal / that we haven't passed yet) within
     positionTolerance of the route

      need to keep track of waypoints / segments / instructions / points? passed
      emit signals when waypoints or instruction points are passed

      signals also emitted if the person goes off course or reaches the
      destination

      tracking waypoints means we can submit a route update request even
      if the server doesn't support them - it's just a new route request with
      less waypoints and a new origin
    */

public slots:
    void calculate(const QGeoRouteRequest &request);
    void updateRoute(const QGeoRoute &route);

    void depart();
    void updateRoute();

private slots:
    void positionUpdated(const QGeoPositionInfo &positionInfo);
    void positionTimeout();
    void calculateRouteFinished();
    void calculateRouteError(QGeoRouteReply::Error error, const QString &errorString);

signals:
    void departed();
    void offCourse();
    void onCourse();
    void arrived();

    void waypointReached(const QGeoCoordinate &coordinate);
    void instructionReached(QGeoNavigationInstruction *instruction);
    void routeSegmentReached(QGeoRouteSegment *routeSegment);

    void routeUpdated(const QList<QGeoRoute> &routes);

    void error(QGeoNavigator::Error error, QString errorString);

private:
    QGeoNavigatorPrivate* d_ptr;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
