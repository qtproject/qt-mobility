/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "routepresenter.h"

#include <QStringBuilder>
#include <qgeoboundingbox.h>
#include <qgeoinstruction.h>

RoutePresenter::RoutePresenter(QTreeWidget* treeWidget, const QGeoRouteReply* routeReply)
        : GeoPresenter(treeWidget), m_routeReply(routeReply)
{
}

void RoutePresenter::show()
{
    m_treeWidget->clear();

    QTreeWidgetItem* requestTop = new QTreeWidgetItem(m_treeWidget);
    requestTop->setText(0, "request");

    QTreeWidgetItem* modesItem = new QTreeWidgetItem(requestTop);
    modesItem->setText(0, "modes");
    showModes(modesItem, m_routeReply->request().routeOptimization(), m_routeReply->request().travelModes(),
        m_routeReply->request().avoidFeatureTypes());

    QTreeWidgetItem* waypointsItem = new QTreeWidgetItem(requestTop);
    waypointsItem->setText(0, "waypoints");
    waypointsItem->setText(1, QString().setNum(m_routeReply->request().waypoints().length()));
    showPoints(waypointsItem, m_routeReply->request().waypoints());

    QTreeWidgetItem* excludesItem = new QTreeWidgetItem(requestTop);
    excludesItem->setText(0, "exclude areas");
    excludesItem->setText(1, QString().setNum(m_routeReply->request().excludeAreas().length()));
    for (int i = 0; i < m_routeReply->request().excludeAreas().length(); ++i)
        showBoundingBox(excludesItem, m_routeReply->request().excludeAreas().at(i));

    QTreeWidgetItem* alternativesItem = new QTreeWidgetItem(requestTop);
    alternativesItem->setText(0, "alternatives");
    alternativesItem->setText(1, QString().setNum(m_routeReply->request().numberAlternativeRoutes()));

    showRoutes();
}

void RoutePresenter::showRoutes()
{
    QTreeWidgetItem* routeTop = new QTreeWidgetItem(m_treeWidget);
    routeTop->setText(0, "routes");

    QTreeWidgetItem* countItem = new QTreeWidgetItem(routeTop);
    countItem->setText(0, "count");
    countItem->setText(1, QString().setNum(m_routeReply->routes().size()));

    const QList<QGeoRoute> routes = m_routeReply->routes();
    for (int i = 0; i < routes.length(); ++i) {
        showRoute(routeTop, routes[i]);
    }
}

void RoutePresenter::showRoute(QTreeWidgetItem* top, const QGeoRoute& route)
{
    QTreeWidgetItem* routeItem = new QTreeWidgetItem(top);
    routeItem->setText(0, "route");

    QTreeWidgetItem* idItem = 0;
    if (!route.routeId().isEmpty()) {
        idItem = new QTreeWidgetItem(routeItem);
        idItem->setText(0, "id");
        idItem->setText(1, route.routeId());
    }

    QTreeWidgetItem* modeItem = new QTreeWidgetItem(routeItem);
    modeItem->setText(0, "mode");
    showModes(modeItem, route.request().routeOptimization(), route.travelMode(), route.request().avoidFeatureTypes());

    QTreeWidgetItem* distanceItem = new QTreeWidgetItem(routeItem);
    distanceItem->setText(0, "distance");
    distanceItem->setText(1, QString().setNum(route.distance()));

    showBoundingBox(routeItem, route.bounds());

    QTreeWidgetItem* wayPointsItem = new QTreeWidgetItem(routeItem);
    QString overviewLabel = "overview";
    if (route.path().count() > 100)
        overviewLabel += "(100)";
    wayPointsItem->setText(0, overviewLabel);
    showPoints(wayPointsItem, route.path());

    QList<QGeoRouteSegment> segments = route.routeSegments();
    QTreeWidgetItem* segmentsItem = new QTreeWidgetItem(routeItem);
    QString segmentsLabel = "segments";
    if (segments.length() > 100)
        segmentsLabel += "(100)";

    segmentsItem->setText(0, segmentsLabel);

    segmentsItem->setText(1, QString().setNum(segments.length()));
    for (int i = 0; i < segments.length() && i < 100; ++i) {
        showRouteSegment(segmentsItem, segments[i]);
    }
}

void RoutePresenter::showRouteSegment(QTreeWidgetItem* routeItem, const QGeoRouteSegment &segment)
{
    QTreeWidgetItem* segmentItem = new QTreeWidgetItem(routeItem);
    segmentItem->setText(0, "segment");

    QTreeWidgetItem* durationItem = new QTreeWidgetItem(segmentItem);
    durationItem->setText(0, "duration");
    durationItem->setText(1, QString().setNum(segment.travelTime()));

    QTreeWidgetItem* distanceItem = new QTreeWidgetItem(segmentItem);
    distanceItem->setText(0, "distance");
    distanceItem->setText(1, QString().setNum(segment.distance()));

    // add back in when more qgeoinstruction classes are made available
    /*
        QString s = segment->turn();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "turn");
            propItem->setText(1, s);
        }

        s = segment->streetName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "street name");
            propItem->setText(1, s);
        }

        s = segment->routeName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "route name");
            propItem->setText(1, s);
        }

        s = segment->nextStreetName();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "next street name");
            propItem->setText(1, s);
        }

        s = segment->signPost();

        if (!s.isEmpty()) {
            propItem = new QTreeWidgetItem(maneuverItem);
            propItem->setText(0, "sign post");
            propItem->setText(1, s);
        }

        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "traffic direction");
        propItem->setText(1, QString().setNum(segment->trafficDirection()));
    */
    QTreeWidgetItem* pathItem = new QTreeWidgetItem(segmentItem);
    pathItem->setText(0, "path");
    showPoints(pathItem, segment.path());

    if (!segment.instruction().instructionText().isEmpty()) {
        QTreeWidgetItem* instructionItem = new QTreeWidgetItem(segmentItem);
        instructionItem->setText(0, "instruction");

        QTreeWidgetItem* positionItem = new QTreeWidgetItem(instructionItem);
        positionItem->setText(0, "position");
        QList<QGeoCoordinate> points;
        points.append(segment.instruction().position());
        showPoints(positionItem, points);

        QTreeWidgetItem* instructionTextItem = new QTreeWidgetItem(instructionItem);
        instructionTextItem->setText(0, "text");
        instructionTextItem->setText(1, segment.instruction().instructionText());
    }
}

void RoutePresenter::showPoints(QTreeWidgetItem* pointsItem, const QList<QGeoCoordinate>& points)
{
    for (int i = 0; i < points.count() && i < 100; ++i) {
        QTreeWidgetItem* point = new QTreeWidgetItem(pointsItem);
        point->setText(0, QString().setNum(i + 1));
        point->setText(1, formatGeoCoordinate(points[i]));
    }
}

//-------------------------------------------------------------------------------------------------
void GeoPresenter::showBoundingBox(QTreeWidgetItem* routeItem, const QGeoBoundingBox& box)
{
    QTreeWidgetItem* boxItem = new QTreeWidgetItem(routeItem);
    boxItem->setText(0, "bounding box");

    QTreeWidgetItem* nwItem = new QTreeWidgetItem(boxItem);
    nwItem->setText(0, "NW");
    nwItem->setText(1, formatGeoCoordinate(box.topLeft()));

    QTreeWidgetItem* seItem = new QTreeWidgetItem(boxItem);
    seItem->setText(0, "SE");
    seItem->setText(1, formatGeoCoordinate(box.bottomRight()));
}

QString GeoPresenter::formatGeoCoordinate(const QGeoCoordinate& coord)
{
    return QString().setNum(coord.latitude()) %
           ", " %
           QString().setNum(coord.longitude());
}

void RoutePresenter::showModes(QTreeWidgetItem* top, QGeoRouteRequest::RouteOptimizations optimization,
                               QGeoRouteRequest::TravelModes travelModes,
                               QGeoRouteRequest::AvoidFeatureTypes avoid)
{
    QTreeWidgetItem* optimizationItem = new QTreeWidgetItem(top);
    optimizationItem->setText(0, "optimization");

    if ((optimization & QGeoRouteRequest::ShortestRoute) != 0) {
        QTreeWidgetItem* shortestRouteItem = new QTreeWidgetItem(optimizationItem);
        shortestRouteItem->setText(1, "ShortestRoute");
    }
    if ((optimization & QGeoRouteRequest::FastestRoute) != 0) {
        QTreeWidgetItem* fastestRouteItem = new QTreeWidgetItem(optimizationItem);
        fastestRouteItem->setText(1, "FastestRoute");
    }
    if ((optimization & QGeoRouteRequest::MostEconomicRoute) != 0) {
        QTreeWidgetItem* economicRouteItem = new QTreeWidgetItem(optimizationItem);
        economicRouteItem->setText(1, "MostEconomicRoute");
    }
    if ((optimization & QGeoRouteRequest::MostScenicRoute) != 0) {
        QTreeWidgetItem* scenicRouteItem = new QTreeWidgetItem(optimizationItem);
        scenicRouteItem->setText(1, "MostScenicRoute");
    }

    QTreeWidgetItem* travelModeItem = new QTreeWidgetItem(top);
    travelModeItem->setText(0, "travel mode");

    if ((travelModes & QGeoRouteRequest::CarTravel) != 0) {
        QTreeWidgetItem* carTravelItem = new QTreeWidgetItem(travelModeItem);
        carTravelItem->setText(1, "CarTravel");
    }
    if ((travelModes & QGeoRouteRequest::PedestrianTravel) != 0) {
        QTreeWidgetItem* pedestrianTravelItem = new QTreeWidgetItem(travelModeItem);
        pedestrianTravelItem->setText(1, "PedestrianTravel");
    }
    if ((travelModes & QGeoRouteRequest::PublicTransitTravel) != 0) {
        QTreeWidgetItem* transitTravelItem = new QTreeWidgetItem(travelModeItem);
        transitTravelItem->setText(1, "PublicTransitTravel");
    }
    if ((travelModes & QGeoRouteRequest::BicycleTravel) != 0) {
        QTreeWidgetItem* bicycleTravelItem = new QTreeWidgetItem(travelModeItem);
        bicycleTravelItem->setText(1, "BicycleTravel");
    }
    if ((travelModes & QGeoRouteRequest::TruckTravel) != 0) {
        QTreeWidgetItem* truckTravelItem = new QTreeWidgetItem(travelModeItem);
        truckTravelItem->setText(1, "TruckTravel");
    }

    QTreeWidgetItem* avoitItem = new QTreeWidgetItem(top);
    avoitItem->setText(0, "avoid");

    if (avoid == QGeoRouteRequest::AvoidNothing) {
        QTreeWidgetItem* nothingAvoidItem = new QTreeWidgetItem(avoitItem);
        nothingAvoidItem->setText(1, "AvoidNothing");
    }
    if ((avoid & QGeoRouteRequest::AvoidTolls) != 0) {
        QTreeWidgetItem* tollsAvoidItem = new QTreeWidgetItem(avoitItem);
        tollsAvoidItem->setText(1, "AvoidTolls");
    }
    if ((avoid & QGeoRouteRequest::AvoidHighways) != 0) {
        QTreeWidgetItem* highwaysAvoidItem = new QTreeWidgetItem(avoitItem);
        highwaysAvoidItem->setText(1, "AvoidHighways");
    }
    if ((avoid & QGeoRouteRequest::AvoidFerries) != 0) {
        QTreeWidgetItem* ferriesAvoidItem = new QTreeWidgetItem(avoitItem);
        ferriesAvoidItem->setText(1, "AvoidFerries");
    }
    if ((avoid & QGeoRouteRequest::AvoidTunnels) != 0) {
        QTreeWidgetItem* tunnelsAvoidItem = new QTreeWidgetItem(avoitItem);
        tunnelsAvoidItem->setText(1, "AvoidTunnels");
    }
    if ((avoid & QGeoRouteRequest::AvoidDirtRoads) != 0) {
        QTreeWidgetItem* dirtRoadsAvoidItem = new QTreeWidgetItem(avoitItem);
        dirtRoadsAvoidItem->setText(1, "AvoidDirtRoads");
    }
    if ((avoid & QGeoRouteRequest::AvoidPublicTransit) != 0) {
        QTreeWidgetItem* publicTransitAvoidItem = new QTreeWidgetItem(avoitItem);
        publicTransitAvoidItem->setText(1, "AvoidPublicTransit");
    }
    if ((avoid & QGeoRouteRequest::AvoidPark) != 0) {
        QTreeWidgetItem* parkAvoidItem = new QTreeWidgetItem(avoitItem);
        parkAvoidItem->setText(1, "AvoidPark");
    }
    if ((avoid & QGeoRouteRequest::AvoidMotorPoolLanes) != 0) {
        QTreeWidgetItem* motorPoolLanesAvoidItem = new QTreeWidgetItem(avoitItem);
        motorPoolLanesAvoidItem->setText(1, "AvoidMotorPoolLanes");
    }
}


