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

#ifndef QGEOROUTEREQUESTOPTIONS_H
#define QGEOROUTEREQUESTOPTIONS_H

#include "qgeoroute.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoRouteTransitOptions;

class QGeoRouteRequestOptionsPrivate;

class Q_LOCATION_EXPORT QGeoRouteRequestOptions
{
public:
    QGeoRouteRequestOptions();
    QGeoRouteRequestOptions(const QGeoRouteRequestOptions &other);
    ~QGeoRouteRequestOptions();

    QGeoRouteRequestOptions& operator = (const QGeoRouteRequestOptions &other);

    // default to 0
    void setNumberAlternativeRoutes(int alternatives);
    int numberAlternativeRoutes() const;

    // default to QGeoRoutingService::TravelByCar
    void setTravelModes(QGeoRoute::TravelModes travelModes);
    QGeoRoute::TravelModes travelModes() const;

    // defaults to QGeoRoute::NoAvoidance
    void setAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes);
    QGeoRoute::AvoidFeatureTypes avoidFeatureTypes() const;

    // defaults to QGeoRoute::OptimizeFastest
    void setRouteOptimization(QGeoRoute::RouteOptimization optimization);
    QGeoRoute::RouteOptimization routeOptimization() const;

    // defaults to empty - no subclass required yet
    // this is how we handle private / public / truck attributes
    //void setTransitOptions(const QList<const QGeoRouteTransitOptions *> &transitOptions);
    //QList<const QGeoRouteTransitOptions *> transitOptions() const;

private:
    QGeoRouteRequestOptionsPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGeoRouteRequestOptions);
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif

