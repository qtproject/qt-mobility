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

#include "routepresenter.h"

#include <QStringBuilder>
#include <qgeoboundingbox.h>
#include <qgeonavigationinstruction.h>

RoutePresenter::RoutePresenter(QTreeWidget* treeWidget, const QGeoRouteReply* routeReply)
        : GeoPresenter(treeWidget), routeReply(routeReply)
{
}

void RoutePresenter::show()
{
    treeWidget->clear();

    QTreeWidgetItem* requestTop = new QTreeWidgetItem(treeWidget);
    requestTop->setText(0, "request");

    QTreeWidgetItem* prop = new QTreeWidgetItem(requestTop);
    prop->setText(0, "modes");
    showModes(prop,routeReply->request().routeOptimization(),routeReply->request().travelModes(),routeReply->request().avoidFeatureTypes());

    prop = new QTreeWidgetItem(requestTop);
    prop->setText(0, "waypoints");
    prop->setText(1, QString().setNum(routeReply->request().waypoints().length()));
    showPoints(prop, routeReply->request().waypoints());

    prop = new QTreeWidgetItem(requestTop);
    prop->setText(0, "exclude areas");
    prop->setText(1, QString().setNum(routeReply->request().excludeAreas().length()));
    for (int i = 0; i < routeReply->request().excludeAreas().length(); ++i)
        showBoundingBox(prop, routeReply->request().excludeAreas().at(i));

    prop = new QTreeWidgetItem(requestTop);
    prop->setText(0, "alternatives");
    prop->setText(1, QString().setNum(routeReply->request().numberAlternativeRoutes()));


    QTreeWidgetItem* top = showRoutes();
    const QList<QGeoRoute> routes = routeReply->routes();

    for (int i = 0; i < routes.length(); ++i) {
        showRoute(top, routes[i]);
    }
}

QTreeWidgetItem* RoutePresenter::showRoutes()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(treeWidget);
    top->setText(0, "routes");

    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "count");
    prop->setText(1, QString().setNum(routeReply->routes().size()));

    return top;
}

void RoutePresenter::showRoute(QTreeWidgetItem* top, const QGeoRoute& route)
{
    QTreeWidgetItem* routeItem = new QTreeWidgetItem(top);
    routeItem->setText(0, "route");

    QTreeWidgetItem* prop=0;
    if(!route.routeId().isEmpty()) {
        prop = new QTreeWidgetItem(routeItem);
        prop->setText(0, "id");
        prop->setText(1, route.routeId());
    }

    prop = new QTreeWidgetItem(routeItem);
    prop->setText(0, "mode");
    showModes(prop,route.request().routeOptimization(),route.travelMode(),route.request().avoidFeatureTypes());

    prop = new QTreeWidgetItem(routeItem);
    prop->setText(0, "distance");
    prop->setText(1, QString().setNum(route.distance()));

    showBoundingBox(routeItem, route.bounds());

    QTreeWidgetItem* wayPointsItem = new QTreeWidgetItem(routeItem);
    QString overviewLabel = "route overview";
    if(route.path().count()>100)
        overviewLabel += " (First 100)";
    wayPointsItem->setText(0, overviewLabel);
    showPoints(wayPointsItem, route.path());

    QList<QGeoRouteSegment> segments = route.routeSegments();
    QTreeWidgetItem* segmentsItem = new QTreeWidgetItem(routeItem);
    QString segmentsLabel = "segments";
    if(segments.length()>100)
        segmentsLabel += " (First 100)";

    segmentsItem->setText(0, segmentsLabel);

    segmentsItem->setText(1, QString().setNum(segments.length()));
    for (int i = 0; i < segments.length() && i<100; ++i) {
        showRouteSegment(segmentsItem, segments[i]);
    }
}

void RoutePresenter::showRouteSegment(QTreeWidgetItem* routeItem, const QGeoRouteSegment &segment)
{
    QTreeWidgetItem* maneuverItem = new QTreeWidgetItem(routeItem);
    maneuverItem->setText(0, "segment");

    QTreeWidgetItem* propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "duration");
    propItem->setText(1, QString().setNum(segment.travelTime()));

    propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "distance");
    propItem->setText(1, QString().setNum(segment.distance()));

    // add back in when more qgeonavigationinstruction classes are made available
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
    QTreeWidgetItem* maneuverPointsItem = new QTreeWidgetItem(maneuverItem);
    maneuverPointsItem->setText(0, "segment points");
    showPoints(maneuverPointsItem, segment.path());

    if(!segment.instruction().instructionText().isEmpty()) {
        QTreeWidgetItem* instructionItem = new QTreeWidgetItem(maneuverItem);
        instructionItem->setText(0, "instruction");

        QTreeWidgetItem* positionItem = new QTreeWidgetItem(instructionItem);
        positionItem->setText(0, "position");
        QList<QGeoCoordinate> points;
        points.append(segment.instruction().position());
        showPoints(positionItem, points);

        propItem = new QTreeWidgetItem(instructionItem);
        propItem->setText(0, "text");
        propItem->setText(1, segment.instruction().instructionText());
    }
}

void RoutePresenter::showPoints(QTreeWidgetItem* pointsItem, const QList<QGeoCoordinate>& points)
{
    for (int i = 0; i < points.count() && i<100; ++i) {
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
    return QString().setNum(coord.longitude()) %
           ", " %
           QString().setNum(coord.latitude());
}

void RoutePresenter::showModes(QTreeWidgetItem* top, QGeoRouteRequest::RouteOptimizations optimization,
                                    QGeoRouteRequest::TravelModes travelModes,
                                    QGeoRouteRequest::AvoidFeatureTypes avoid )
{
    QTreeWidgetItem* item = new QTreeWidgetItem(top);
    item->setText(0, "optimization");

    if ((optimization & QGeoRouteRequest::ShortestRoute) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "ShortestRoute");
    }
    if ((optimization & QGeoRouteRequest::FastestRoute) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "FastestRoute");
    }
    if ((optimization & QGeoRouteRequest::MostEconomicRoute) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "MostEconomicRoute");
    }
    if ((optimization & QGeoRouteRequest::MostScenicRoute) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "MostScenicRoute");
    }

    item = new QTreeWidgetItem(top);
    item->setText(0, "travel mode");

    if ((travelModes & QGeoRouteRequest::CarTravel) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "CarTravel");
    }
    if ((travelModes & QGeoRouteRequest::PedestrianTravel) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "PedestrianTravel");
    }
    if ((travelModes & QGeoRouteRequest::PublicTransitTravel) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "PublicTransitTravel");
    }
    if ((travelModes & QGeoRouteRequest::BicycleTravel) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "BicycleTravel");
    }
    if ((travelModes & QGeoRouteRequest::TruckTravel) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "TruckTravel");
    }

    item = new QTreeWidgetItem(top);
    item->setText(0, "avoid");

    if (avoid == QGeoRouteRequest::AvoidNothing) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidNothing");
    }
    if ((avoid & QGeoRouteRequest::AvoidTolls) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidTolls");
    }
    if ((avoid & QGeoRouteRequest::AvoidHighways) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidHighways");
    }
    if ((avoid & QGeoRouteRequest::AvoidFerries) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidFerries");
    }
    if ((avoid & QGeoRouteRequest::AvoidTunnels) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidTunnels");
    }
    if ((avoid & QGeoRouteRequest::AvoidDirtRoads) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidDirtRoads");
    }
    if ((avoid & QGeoRouteRequest::AvoidPublicTransit) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidPublicTransit");
    }
    if ((avoid & QGeoRouteRequest::AvoidPark) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidPark");
    }
    if ((avoid & QGeoRouteRequest::AvoidMotorPoolLanes) != 0) {
        QTreeWidgetItem* prop = new QTreeWidgetItem(item);
        prop->setText(1, "AvoidMotorPoolLanes");
    }
}


