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

#include "qgeorouterequestoptions.h"
#include "qgeorouterequestoptions_p.h"

QTM_BEGIN_NAMESPACE

QGeoRouteRequestOptions::QGeoRouteRequestOptions()
{
}

QGeoRouteRequestOptions::QGeoRouteRequestOptions(const QGeoRouteRequestOptions &other)
{
    Q_UNUSED(other);
}

QGeoRouteRequestOptions::~QGeoRouteRequestOptions()
{
}

QGeoRouteRequestOptions& QGeoRouteRequestOptions::operator= (const QGeoRouteRequestOptions &other)
{
    Q_UNUSED(other);
    return *this;
}

// default to 0
void QGeoRouteRequestOptions::setNumberAlternativeRoutes(int alternatives)
{
    Q_UNUSED(alternatives);
}

int QGeoRouteRequestOptions::numberAlternativeRoutes() const
{
    return 0;
}

// default to QGeoRoutingService::TravelByCar
void QGeoRouteRequestOptions::setTravelModes(QGeoRoutingService::TravelModes travelModes)
{
    Q_UNUSED(travelModes);
}

QGeoRoutingService::TravelModes QGeoRouteRequestOptions::travelModes() const
{
    return QGeoRoutingService::CarTravel;
}

// defaults to QGeoRoutingService::NoAvoidance
void QGeoRouteRequestOptions::setAvoidFeatureTypes(QGeoRoutingService::AvoidFeatureTypes avoidFeatureTypes)
{
    Q_UNUSED(avoidFeatureTypes);
}

QGeoRoutingService::AvoidFeatureTypes QGeoRouteRequestOptions::avoidFeatureTypes() const
{
    return QGeoRoutingService::AvoidNothing;
}

// defaults to QGeoRoutingService::OptimizeFastest
void QGeoRouteRequestOptions::setRouteOptimization(QGeoRoutingService::RouteOptimization optimization)
{
    Q_UNUSED(optimization);
}

QGeoRoutingService::RouteOptimization QGeoRouteRequestOptions::routeOptimization() const
{
    return QGeoRoutingService::FastestRoute;
}

// defaults to empty - no subclass required yet
// this is how we handle private / public / truck attributes
//void QGeoRouteRequestOptions::setTransitOptions(const QList<const QGeoRouteTransitOptions *> &transitOptions);
//QList<const QGeoRouteTransitOptions *> QGeoRouteRequestOptions::transitOptions() const;

QTM_END_NAMESPACE

