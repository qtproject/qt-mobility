/*
* ==============================================================================
*  Name        : RoutePresenter.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef ROUTEPRESENTER_H
#define ROUTEPRESENTER_H

#include <QTreeWidget>

#include "geopresenter.h"
#include "qroutereply.h"
#include "qgeocoordinatemaps.h"

QTM_USE_NAMESPACE

class RoutePresenter : GeoPresenter
{
public:
    RoutePresenter(QTreeWidget* treeWidget, const QRouteReply* routeReply);
    virtual void show();

private:
    QTreeWidgetItem* showRoutes();
    void showRoute(QTreeWidgetItem* top, const QRoute& route);
    void showManeuver(QTreeWidgetItem* routeItem, const QManeuver& maneuver);
    void showPoints(QTreeWidgetItem* pointsItem, const QList<QGeoCoordinateMaps>& points);

private:
    const QRouteReply* routeReply;
};

#endif
