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

#include "qgeoroutingmanager_nokia_p.h"
#include "qgeoroutereply_nokia_p.h"

#include <QStringList>

#include <QNetworkProxy>

QGeoRoutingManagerNokia::QGeoRoutingManagerNokia(QObject *parent)
    : QGeoRoutingManager(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    
    setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    setHost("route.desktop.maps.svc.ovi.com");

    // TODO flip this over once we have support for updates
    setSupportsRouteUpdates(false);

    setSupportsAlternativeRoutes(true);

    QGeoRouteRequest::AvoidFeatureTypes avoidFeatures;
    avoidFeatures |= QGeoRouteRequest::AvoidTolls;
    avoidFeatures |= QGeoRouteRequest::AvoidHighways;
    avoidFeatures |= QGeoRouteRequest::AvoidFerries;
    avoidFeatures |= QGeoRouteRequest::AvoidTunnels;
    avoidFeatures |= QGeoRouteRequest::AvoidDirtRoads;
    setSupportedAvoidFeatureTypes(avoidFeatures);

    // TODO
    //setSupportedDirectionsDetails();

    QGeoRouteRequest::RouteOptimizations optimizations;
    optimizations |= QGeoRouteRequest::ShortestRoute;
    optimizations |= QGeoRouteRequest::FastestRoute;
    optimizations |= QGeoRouteRequest::MostEconomicRoute;
    setSupportedRouteOptimizations(optimizations);

    QGeoRouteRequest::TravelModes travelModes;
    travelModes |= QGeoRouteRequest::CarTravel;
    travelModes |= QGeoRouteRequest::PedestrianTravel;
    travelModes |= QGeoRouteRequest::PublicTransitTravel;
    setSupportedTravelModes(travelModes);
}

QGeoRoutingManagerNokia::~QGeoRoutingManagerNokia() {}

void QGeoRoutingManagerNokia::setProxy(const QNetworkProxy &proxy)
{
    m_networkManager->setProxy(proxy);
}

void QGeoRoutingManagerNokia::setHost(QString host)
{
    m_host = host;
}

QString QGeoRoutingManagerNokia::requestString(const QGeoRouteRequest &request)
{
    bool supported = true;

    if ((request.avoidFeatureTypes() & supportedAvoidFeatureTypes()) != request.avoidFeatureTypes())
        supported = false;

    // TODO
    //setSupportedDirectionsDetails();

    if ((request.routeOptimization() & supportedRouteOptimizations()) != request.routeOptimization())
        supported = false;

    if ((request.travelModes() & supportedTravelModes()) != request.travelModes())
        supported = false;

    if ((request.numberAlternativeRoutes() != 0) && !supportsAlternativeRoutes())
        supported = false;

    if (!supported)
        return "";

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/routing/rt/1.0?referer=localhost";

    int numWaypoints = request.waypoints().size();

    if (numWaypoints < 2)
        return "";

    requestString += "&slong=";
    requestString += trimDouble(request.waypoints().at(0).longitude());

    requestString += "&slat=";
    requestString += trimDouble(request.waypoints().at(0).latitude());

    requestString += "&dlong=";
    requestString += trimDouble(request.waypoints().at(numWaypoints - 1).longitude());

    requestString += "&dlat=";
    requestString += trimDouble(request.waypoints().at(numWaypoints - 1).latitude());

    requestString += "&alternatives=";
    requestString += QString::number(request.numberAlternativeRoutes());

    requestString += "&type=";
    switch (request.routeOptimization()) {
    case QGeoRouteRequest::ShortestRoute:
        requestString += "shortest";
    case QGeoRouteRequest::FastestRoute:
        requestString += "fastest";
    case QGeoRouteRequest::MostEconomicRoute:
        requestString += "economic";
    default:
        return "";
    }

    requestString += "&mode=";
    switch (request.travelModes()) {
    case QGeoRouteRequest::CarTravel:
        requestString += "car";
    case QGeoRouteRequest::PedestrianTravel:
        requestString += "pedestrian";
    case QGeoRouteRequest::PublicTransitTravel:
        requestString += "public transport";
    default:
        return "";
    }

    QGeoRouteRequest::AvoidFeatureTypes avoid = request.avoidFeatureTypes();
    if (avoid != QGeoRouteRequest::AvoidNothing) {
        requestString += "&avoid=";
        QStringList avoidTypes;
        if ((avoid & QGeoRouteRequest::AvoidTolls) != 0)
            avoidTypes.append("tollroads");
        if ((avoid & QGeoRouteRequest::AvoidHighways) != 0)
            avoidTypes.append("highways");
        if ((avoid & QGeoRouteRequest::AvoidFerries) != 0)
            avoidTypes.append("ferries");
        if ((avoid & QGeoRouteRequest::AvoidTunnels) != 0)
            avoidTypes.append("tunnels");
        if ((avoid & QGeoRouteRequest::AvoidDirtRoads) != 0)
            avoidTypes.append("dirtroads");
        requestString += avoidTypes.join(",");
    }

    if (numWaypoints > 2) {
        requestString += "&stopovers=";
        QStringList waypoints;
        for (int i = 1; i < numWaypoints - 2; ++i) {
            QString pair = trimDouble(request.waypoints().at(i).latitude());
            pair += ",";
            pair += trimDouble(request.waypoints().at(i).longitude());
            waypoints.append(pair);
        }
        requestString += waypoints.join(" ");
    }

    return requestString;
}

QString QGeoRoutingManagerNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

QGeoRouteReply* QGeoRoutingManagerNokia::calculateRoute(const QGeoRouteRequest& request)
{
    QString reqString = requestString(request);

    if (reqString.isEmpty()) {
        QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "The give route request options are not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(reqString)));
    QGeoRouteReplyNokia *reply = new QGeoRouteReplyNokia(request, networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(routeFinished()));

    connect(reply,
            SIGNAL(error(QGeoRouteReply::Error,QString)),
            this,
            SLOT(routeError(QGeoRouteReply::Error,QString)));

    return reply;
}

QGeoRouteReply* QGeoRoutingManagerNokia::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    // TODO flip this over once we have support for updates
    QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "Route updates are not supported by this service provider.", this);
    emit error(reply, reply->error(), reply->errorString());
    return reply;
}

void QGeoRoutingManagerNokia::routeFinished()
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if(receivers(SIGNAL(finished(QGeoRouteReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoRoutingManagerNokia::routeError(QGeoRouteReply::Error error, const QString &errorString)
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if(receivers(SIGNAL(finished(QGeoRouteReply*, QGeoRouteReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

