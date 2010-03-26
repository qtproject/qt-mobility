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

RoutePresenter::RoutePresenter(QTreeWidget* treeWidget, const QGeoRouteReply* routeReply)
        : GeoPresenter(treeWidget), routeReply(routeReply)
{
}

void RoutePresenter::show()
{
    treeWidget->clear();
    QTreeWidgetItem* top = showRoutes();
    const QList<QGeoRoute> routes = routeReply->routes();

    for (int i = 0; i < routes.length(); i++) {
        showRoute(top, routes[i]);
    }
}

QTreeWidgetItem* RoutePresenter::showRoutes()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(treeWidget);
    top->setText(0, "routes");

    /*
    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "result");
    prop->setText(1, QString().setNum((quint16) routeReply->resultCode()));
    */

    QTreeWidgetItem* prop = new QTreeWidgetItem(top);
    prop->setText(0, "description");
    prop->setText(1, routeReply->description());

    /*
    QString lang = routeReply->language();

    if (!lang.isEmpty()) {
        prop = new QTreeWidgetItem(top);
        prop->setText(0, "language");
        prop->setText(1, lang);
    }
    */

    prop = new QTreeWidgetItem(top);
    prop->setText(0, "count");
    prop->setText(1, QString().setNum(routeReply->routes().size()));

    return top;
}

void RoutePresenter::showRoute(QTreeWidgetItem* top, const QGeoRoute& route)
{
    QTreeWidgetItem* routeItem = new QTreeWidgetItem(top);
    routeItem->setText(0, "route");

    QTreeWidgetItem* prop = new QTreeWidgetItem(routeItem);
    prop->setText(0, "distance");
    prop->setText(1, QString().setNum(route.distance()));

    showBoundingBox(routeItem, route.boundingBox());

    const QList<QManeuver> maneuvers = route.maneuvers();

    for (int i = 0; i < maneuvers.length(); i++) {
        showManeuver(routeItem, maneuvers[i]);
    }
}

void RoutePresenter::showManeuver(QTreeWidgetItem* routeItem, const QManeuver& maneuver)
{
    QTreeWidgetItem* maneuverItem = new QTreeWidgetItem(routeItem);
    maneuverItem->setText(0, "maneuver");

    QTreeWidgetItem* propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "duration");
    propItem->setText(1, QString().setNum(maneuver.duration()));

    propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "description");
    propItem->setText(1, maneuver.description());

    propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "action");
    propItem->setText(1, maneuver.action());

    propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "distance");
    propItem->setText(1, QString().setNum(maneuver.distance()));

    QString s = maneuver.turn();

    if (!s.isEmpty()) {
        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "turn");
        propItem->setText(1, s);
    }

    s = maneuver.streetName();

    if (!s.isEmpty()) {
        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "street name");
        propItem->setText(1, s);
    }

    s = maneuver.routeName();

    if (!s.isEmpty()) {
        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "route name");
        propItem->setText(1, s);
    }

    s = maneuver.nextStreetName();

    if (!s.isEmpty()) {
        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "next street name");
        propItem->setText(1, s);
    }

    s = maneuver.signPost();

    if (!s.isEmpty()) {
        propItem = new QTreeWidgetItem(maneuverItem);
        propItem->setText(0, "sign post");
        propItem->setText(1, s);
    }

    propItem = new QTreeWidgetItem(maneuverItem);
    propItem->setText(0, "traffic direction");
    propItem->setText(1, QString().setNum(maneuver.trafficDirection()));

    QTreeWidgetItem* wayPointsItem = new QTreeWidgetItem(maneuverItem);
    wayPointsItem->setText(0, "way points");
    showPoints(wayPointsItem, maneuver.wayPoints());

    QTreeWidgetItem* maneuverPointsItem = new QTreeWidgetItem(maneuverItem);
    maneuverPointsItem->setText(0, "maneuver points");
    showPoints(maneuverPointsItem, maneuver.maneuverPoints());
}

void RoutePresenter::showPoints(QTreeWidgetItem* pointsItem, const QList<QGeoCoordinate>& points)
{
    for (int i = 0; i < points.length(); i++) {
        QTreeWidgetItem* point = new QTreeWidgetItem(pointsItem);
        point->setText(0, QString().setNum(i + 1));
        point->setText(1, formatGeoCoordinate(points[i]));
    }
}

//-------------------------------------------------------------------------------------------------
void GeoPresenter::showBoundingBox(QTreeWidgetItem* routeItem, const QRectF& box)
{
    QTreeWidgetItem* boxItem = new QTreeWidgetItem(routeItem);
    boxItem->setText(0, "bounding box");

    QGeoCoordinate topLeft = QGeoCoordinate(box.topLeft().y(), box.topLeft().x());

    QTreeWidgetItem* nwItem = new QTreeWidgetItem(boxItem);
    nwItem->setText(0, "NW");
    nwItem->setText(1, formatGeoCoordinate(topLeft));

    QGeoCoordinate bottomRight = QGeoCoordinate(box.bottomRight().y(), box.bottomRight().x());

    QTreeWidgetItem* seItem = new QTreeWidgetItem(boxItem);
    seItem->setText(0, "SE");
    seItem->setText(1, formatGeoCoordinate(bottomRight));
}

QString GeoPresenter::formatGeoCoordinate(const QGeoCoordinate& coord)
{
    return QString().setNum(coord.longitude()) %
           ", " %
           QString().setNum(coord.latitude());
}


