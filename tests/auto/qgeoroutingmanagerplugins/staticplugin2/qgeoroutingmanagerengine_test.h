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

#ifndef QGEOROUTINGMANAGERENGINE_TEST_H
#define QGEOROUTINGMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeoroutingmanagerengine.h>
#include <qgeocoordinate.h>
#include <qgeoroutereply.h>
#include <qgeorouterequest.h>

QTM_USE_NAMESPACE

class QGeoRoutingManagerEngineTest: public QGeoRoutingManagerEngine

{
Q_OBJECT
public:
    QGeoRoutingManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoRoutingManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setSupportsRouteUpdates(true);
        setSupportsAlternativeRoutes(true);
        setSupportsExcludeAreas(true);
        setSupportedTravelModes(QGeoRouteRequest::PedestrianTravel);
        setSupportedFeatureTypes(QGeoRouteRequest::TollFeature);
        setSupportedFeatureWeights(QGeoRouteRequest::PreferFeatureWeight);
        setSupportedRouteOptimizations(QGeoRouteRequest::FastestRoute);
        setSupportedSegmentDetails(QGeoRouteRequest::BasicSegmentData);
        setSupportedManeuverDetails(QGeoRouteRequest::BasicManeuvers);
    }

    QGeoRouteReply* calculateRoute(const QGeoRouteRequest& request)
    {
        return new QGeoRouteReply(QGeoRouteReply::NoError,"no error");
    }

    QGeoRouteReply* updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
    {
        return new QGeoRouteReply(QGeoRouteReply::CommunicationError,"no error");

    }


};

#endif
