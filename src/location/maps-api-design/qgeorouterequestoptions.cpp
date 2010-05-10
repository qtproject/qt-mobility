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

/*!
  \class QGeoRouteRequestOptions
  \brief The QGeoRouteRequestOptions class represents various options which
  may be specified when requesting geographical routes.
  \ingroup maps

    The most common uses of the request methods in QGeoMappingService will
    specific waypoints and, optionally, the level of detail required for
    directions.

    The QGeoRouteRequestOptions class represents the options corresponding to
    the more advanced (or less common) options which can be passed to the
    QGeoRoutingService request methods.
*/

/*!
    Constructs a QGeoRouteRequestOptions object.
*/
QGeoRouteRequestOptions::QGeoRouteRequestOptions()
        : d_ptr(new QGeoRouteRequestOptionsPrivate()) {}

/*!
    Constructs a QGeoRouteRequestOptions object from the contents of \a other.
*/
QGeoRouteRequestOptions::QGeoRouteRequestOptions(const QGeoRouteRequestOptions &other)
        : d_ptr(new QGeoRouteRequestOptionsPrivate(*(other.d_ptr))) {}

/*!
    Destroys this QGeoRouteRequestOptions object.
*/
QGeoRouteRequestOptions::~QGeoRouteRequestOptions()
{
    Q_D(QGeoRouteRequestOptions);
    delete d;
}

/*!
    Assigns \a other to this QGeoRouteRequestOptions object and returns a
    reference to this QGeoRouteRequestOptions object.
*/
QGeoRouteRequestOptions& QGeoRouteRequestOptions::operator= (const QGeoRouteRequestOptions & other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
    Sets the number of alternative routes to request to \a alternatives.

    The default value if none is set is 0.

    \sa QGeoRouteRequestOptions::numberAlternativeRoutes()
*/
void QGeoRouteRequestOptions::setNumberAlternativeRoutes(int alternatives)
{
    Q_D(QGeoRouteRequestOptions);
    d->alternativeRoutes = alternatives;
}

/*!
    Returns the number of alternative routes which will be requested.

    \sa QGeoRouteRequestOptions::setNumberAlternativeRoutes()
*/
int QGeoRouteRequestOptions::numberAlternativeRoutes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->alternativeRoutes;
}

/*!
    Sets the travel modes which should be considered during the planning of the
    route to \a travelModes.

    The default value if none is set is QGeoRoute::CarTravel.

    \sa QGeoRoute::TravelModes
    \sa QGeoRouteRequestOptions::travelModes()
*/
void QGeoRouteRequestOptions::setTravelModes(QGeoRoute::TravelModes travelModes)
{
    Q_D(QGeoRouteRequestOptions);
    d->travelModes = travelModes;
}

/*!
    Returns the travel modes which this request specifies should be considered
    during the planning of the route.

    \sa QGeoRoute::TravelModes
    \sa QGeoRouteRequestOptions::setTravelModes()
*/
QGeoRoute::TravelModes QGeoRouteRequestOptions::travelModes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->travelModes;
}

/*!
    Sets the features to be avoided during the planning of the route to
    \a avoidFeatureTypes.

    The default value if none is set is QGeoRoute::AvoidNothing.

    \sa QGeoRoute::AvoidFeatureTypes
    \sa QGeoRouteRequestOptions::avoidFeatureTypes()
*/
void QGeoRouteRequestOptions::setAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes)
{
    Q_D(QGeoRouteRequestOptions);
    d->avoidFeatureTypes = avoidFeatureTypes;
}

/*!
    Returns the features which this request specifies should be avoided during
    the planning of the route.

    \sa QGeoRoute::AvoidFeatureTypes
    \sa QGeoRouteRequestOptions::setAvoidFeatureTypes()
*/
QGeoRoute::AvoidFeatureTypes QGeoRouteRequestOptions::avoidFeatureTypes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->avoidFeatureTypes;
}

/*!
    Sets the optimization criteria to use while planning the route to
    \a optimization.

    The default value if none is set is QGeoRoute::FastestRoute.

    \sa QGeoRoute::RouteOptimization
    \sa QGeoRouteRequestOptions::routeOptimization()
*/
void QGeoRouteRequestOptions::setRouteOptimization(QGeoRoute::RouteOptimization optimization)
{
    Q_D(QGeoRouteRequestOptions);
    d->routeOptimization = optimization;
}

/*!
    Returns the optimization criteria which this request specifies should be
    used while planning the route.

    \sa QGeoRoute::RouteOptimization
    \sa QGeoRouteRequestOptions::setRouteOptimization()
*/
QGeoRoute::RouteOptimization QGeoRouteRequestOptions::routeOptimization() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->routeOptimization;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteRequestOptionsPrivate::QGeoRouteRequestOptionsPrivate()
{
    alternativeRoutes = 0;
    travelModes = QGeoRoute::CarTravel;
    avoidFeatureTypes = QGeoRoute::AvoidNothing;
    routeOptimization = QGeoRoute::FastestRoute;
}

QGeoRouteRequestOptionsPrivate::QGeoRouteRequestOptionsPrivate(const QGeoRouteRequestOptionsPrivate &other)
        : alternativeRoutes(other.alternativeRoutes),
        travelModes(other.travelModes),
        //transitOptions(other.transitOptions),
        avoidFeatureTypes(other.avoidFeatureTypes),
        routeOptimization(other.routeOptimization) {}

QGeoRouteRequestOptionsPrivate::~QGeoRouteRequestOptionsPrivate() {}

QGeoRouteRequestOptionsPrivate& QGeoRouteRequestOptionsPrivate::operator= (const QGeoRouteRequestOptionsPrivate & other)
{
    alternativeRoutes = other.alternativeRoutes;
    travelModes = other.travelModes;
    //transitOptions = other.transitOptions;
    avoidFeatureTypes = other.avoidFeatureTypes;
    routeOptimization = other.routeOptimization;

    return *this;
}

QTM_END_NAMESPACE

