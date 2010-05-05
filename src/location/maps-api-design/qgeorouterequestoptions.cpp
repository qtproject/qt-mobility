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
  \brief The QGeoRouteRequestOptions class
  \ingroup maps

*/

/*!
*/
QGeoRouteRequestOptions::QGeoRouteRequestOptions()
    : d_ptr(new QGeoRouteRequestOptionsPrivate()) {}

/*!
*/
QGeoRouteRequestOptions::QGeoRouteRequestOptions(const QGeoRouteRequestOptions &other)
    : d_ptr(new QGeoRouteRequestOptionsPrivate(*(other.d_ptr))) {}

/*!
*/
QGeoRouteRequestOptions::~QGeoRouteRequestOptions()
{
    Q_D(QGeoRouteRequestOptions);
    delete d;
}

/*!
*/
QGeoRouteRequestOptions& QGeoRouteRequestOptions::operator= (const QGeoRouteRequestOptions &other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
*/
void QGeoRouteRequestOptions::setNumberAlternativeRoutes(int alternatives)
{
    Q_D(QGeoRouteRequestOptions);
    d->alternativeRoutes = alternatives;
}

/*!
*/
int QGeoRouteRequestOptions::numberAlternativeRoutes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->alternativeRoutes;
}

/*!
*/
void QGeoRouteRequestOptions::setTravelModes(QGeoRoute::TravelModes travelModes)
{
    Q_D(QGeoRouteRequestOptions);
    d->travelModes = travelModes;
}

/*!
*/
QGeoRoute::TravelModes QGeoRouteRequestOptions::travelModes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->travelModes;
}

/*!
*/
void QGeoRouteRequestOptions::setAvoidFeatureTypes(QGeoRoute::AvoidFeatureTypes avoidFeatureTypes)
{
    Q_D(QGeoRouteRequestOptions);
    d->avoidFeatureTypes = avoidFeatureTypes;
}

/*!
*/
QGeoRoute::AvoidFeatureTypes QGeoRouteRequestOptions::avoidFeatureTypes() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->avoidFeatureTypes;
}

/*!
*/
void QGeoRouteRequestOptions::setRouteOptimization(QGeoRoute::RouteOptimization optimization)
{
    Q_D(QGeoRouteRequestOptions);
    d->routeOptimization = optimization;
}

/*!
*/
QGeoRoute::RouteOptimization QGeoRouteRequestOptions::routeOptimization() const
{
    Q_D(const QGeoRouteRequestOptions);
    return d->routeOptimization;
}

// defaults to empty - no subclass required yet
// this is how we handle private / public / truck attributes
///*!
//*/
//void QGeoRouteRequestOptions::setTransitOptions(const QList<const QGeoRouteTransitOptions *> &transitOptions)
//{
//    Q_D(QGeoRouteRequestOptions);
//    d->transitOptions = transitOptions;
//}

///*!
//*/
//QList<const QGeoRouteTransitOptions *> QGeoRouteRequestOptions::transitOptions() const
//{
//    Q_D(const QGeoRouteRequestOptions);
//    return d->transitOptions;
//}

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

QGeoRouteRequestOptionsPrivate& QGeoRouteRequestOptionsPrivate::operator= (const QGeoRouteRequestOptionsPrivate &other)
{
    alternativeRoutes = other.alternativeRoutes;
    travelModes = other.travelModes;
    //transitOptions = other.transitOptions;
    avoidFeatureTypes = other.avoidFeatureTypes;
    routeOptimization = other.routeOptimization;

    return *this;
}

QTM_END_NAMESPACE

