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

#include "qgeoroutingmanagerengine_nokia_p.h"
#include "qgeoroutereply_nokia_p.h"

#include <QStringList>
#include <QNetworkProxy>

QGeoRoutingManagerEngineNokia::QGeoRoutingManagerEngineNokia(const QMap<QString, QString> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoRoutingManagerEngine(parameters),
        m_host("route.desktop.maps.svc.ovi.com")
{
    m_networkManager = new QNetworkAccessManager(this);

    QList<QString> keys = parameters.keys();

    if (keys.contains("routing.proxy")) {
        QString proxy = parameters.value("routing.proxy");
        if (!proxy.isEmpty())
            m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("routing.host")) {
        QString host = parameters.value("routing.host");
        if (!host.isEmpty())
            m_host = host;
    }

    setSupportsRouteUpdates(true);
    setSupportsAlternativeRoutes(true);

    QGeoRouteRequest::AvoidFeatureTypes avoidFeatures;
    avoidFeatures |= QGeoRouteRequest::AvoidTolls;
    avoidFeatures |= QGeoRouteRequest::AvoidHighways;
    avoidFeatures |= QGeoRouteRequest::AvoidFerries;
    avoidFeatures |= QGeoRouteRequest::AvoidTunnels;
    avoidFeatures |= QGeoRouteRequest::AvoidDirtRoads;
    setSupportedAvoidFeatureTypes(avoidFeatures);

    QGeoRouteRequest::InstructionDetails instructionDetails;
    instructionDetails |= QGeoRouteRequest::BasicInstructions;
    instructionDetails |= QGeoRouteRequest::DetailedInstructions;
    setSupportedInstructionDetails(instructionDetails);

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

    QGeoRouteRequest::SegmentDetails segmentDetails;
    segmentDetails |= QGeoRouteRequest::BasicSegmentData;
    segmentDetails |= QGeoRouteRequest::DetailedSegmentData;
    setSupportedSegmentDetails(segmentDetails);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoRoutingManagerEngineNokia::~QGeoRoutingManagerEngineNokia() {}

QGeoRouteReply* QGeoRoutingManagerEngineNokia::calculateRoute(const QGeoRouteRequest& request)
{
    QString reqString = calculateRouteRequestString(request);

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
            SIGNAL(error(QGeoRouteReply::Error, QString)),
            this,
            SLOT(routeError(QGeoRouteReply::Error, QString)));

    return reply;
}

QGeoRouteReply* QGeoRoutingManagerEngineNokia::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    QString reqString = updateRouteRequestString(route,position);

    if (reqString.isEmpty()) {
        QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "The give route request options are not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(reqString)));
    //TODO: request for the reply
    QGeoRouteReplyNokia *reply = new QGeoRouteReplyNokia(QGeoRouteRequest(), networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(routeFinished()));

    connect(reply,
            SIGNAL(error(QGeoRouteReply::Error, QString)),
            this,
            SLOT(routeError(QGeoRouteReply::Error, QString)));

    return reply;
}

QString QGeoRoutingManagerEngineNokia::calculateRouteRequestString(const QGeoRouteRequest &request)
{
    bool supported = true;

    if ((request.avoidFeatureTypes() & supportedAvoidFeatureTypes()) != request.avoidFeatureTypes())
        supported = false;

    if ((request.instructionDetail() & supportedInstructionDetails()) != request.instructionDetail())
        supported = false;

    if ((request.segmentDetail() & supportedSegmentDetails()) != request.segmentDetail())
        supported = false;

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
    requestString += "/calculateroute/6.1/routes.xml";

    int numWaypoints = request.waypoints().size();
    if (numWaypoints < 2)
        return "";

    for(int i=0;i<numWaypoints;++i) {
        requestString += i==0?"?":"&";
        requestString += "waypoint";
        requestString += QString::number(i);
        requestString += "=";
        requestString += trimDouble(request.waypoints().at(i).longitude());
        requestString += ",";
        requestString += trimDouble(request.waypoints().at(i).latitude());
    }

    //TODO: Avoid areas
    //requestString += "&avoidareas=";

    requestString += modesRequestString(request.routeOptimization(),request.travelModes(),request.avoidFeatureTypes());

    requestString += "&alternatives=";
    requestString += QString::number(request.numberAlternativeRoutes());

    requestString += "&representation=";
     if( request.instructionDetail() & QGeoRouteRequest::BasicInstructions )
         requestString += "display";
     else if( request.instructionDetail() & QGeoRouteRequest::DetailedInstructions )
         requestString += "navigation";
     else
         requestString += "overview";

      if( request.instructionDetail() & QGeoRouteRequest::BasicSegmentData )
          requestString += "&linkattributes=nextLink";
      else if( request.instructionDetail() & QGeoRouteRequest::DetailedSegmentData )
          requestString += "&linkattributes=shape,length,nextLink";

    requestString += routeRequestString(request);

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position)
{
    bool supported = true;

    if ((route.optimization() & supportedRouteOptimizations()) != route.optimization())
        supported = false;

    if ((route.travelMode() & supportedTravelModes()) != route.travelMode())
        supported = false;

    if (!supported)
        return "";


    QString requestString = "http://";
    requestString += m_host;
    requestString += "/getroute/6.1/routes.xml";

    // TODO: Check that optimization, mode, avoidance etc keep the same as requested with
    // calculate route once data is available
    requestString += "&routeid=";
    requestString += route.routeId();

    requestString += "&pos=";
    requestString += QString::number(position.latitude());
    requestString += ",";
    requestString += QString::number(position.longitude());

    requestString += routeRequestString();

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::modesRequestString(QGeoRouteRequest::RouteOptimizations optimization,
                                                    QGeoRouteRequest::TravelModes travelModes,
                                                    QGeoRouteRequest::AvoidFeatureTypes avoid)
{
    QString requestString = "&modes=";

    QStringList types;
    if ((optimization & QGeoRouteRequest::ShortestRoute) != 0)
        types.append("directDrive");  // TODO: check difference to "shortest" once data available
    if ((optimization & QGeoRouteRequest::FastestRoute) != 0)
        types.append("fastestNow");
    if ((optimization & QGeoRouteRequest::MostEconomicRoute) != 0)
        types.append("economic");
    if ((optimization & QGeoRouteRequest::MostScenicRoute) != 0)
        types.append("scenic");

    QStringList tModes;
    if ((travelModes & QGeoRouteRequest::CarTravel) != 0)
        tModes.append("car");
    if ((travelModes & QGeoRouteRequest::PedestrianTravel) != 0)
        tModes.append("pedestrian");
    if ((travelModes & QGeoRouteRequest::PublicTransitTravel) != 0)
        tModes.append("publicTransport");
    if ((travelModes & QGeoRouteRequest::BicycleTravel) != 0)
        tModes.append("bicycle");
    if ((travelModes & QGeoRouteRequest::TruckTravel) != 0)
        tModes.append("truck");

    QStringList avoidTypes;
    if (avoid != QGeoRouteRequest::AvoidNothing) {
        if ((avoid & QGeoRouteRequest::AvoidTolls) != 0)
            avoidTypes.append("disallowTollroads");
        if ((avoid & QGeoRouteRequest::AvoidHighways) != 0)
            avoidTypes.append("disallowMotorways");
        if ((avoid & QGeoRouteRequest::AvoidFerries) != 0)
            avoidTypes.append("disallowFerries");
        if ((avoid & QGeoRouteRequest::AvoidTunnels) != 0)
            avoidTypes.append("disallowTunnels");
        if ((avoid & QGeoRouteRequest::AvoidDirtRoads) != 0)
            avoidTypes.append("disallowDirtRoads");
        if ((avoid & QGeoRouteRequest::AvoidPublicTransit) != 0)
            avoidTypes.append("disallowPublicTransport");
        if ((avoid & QGeoRouteRequest::AvoidPark) != 0)
            avoidTypes.append("disallowPark");
        if ((avoid & QGeoRouteRequest::AvoidMotorPoolLanes) == 0)
            avoidTypes.append("allowHOVLanes");
    }
    QString avoidStr = avoidTypes.join(",");

    QStringList modes;
    QString tMode;
    foreach(tMode, tModes) {
        QString type;
        foreach(type, types) {
            QString mode = type + ";" + tMode;
            if (!avoidStr.isEmpty())
                mode += ";" + avoidStr;
            modes.append(mode);
        }
    }
    requestString += modes.join("!");
    return requestString;
}

QString QGeoRoutingManagerEngineNokia::routeRequestString()
{
    QString requestString;

    requestString += "&departure=";
    requestString += QDateTime::currentDateTime().toUTC().toString("yyyy-MM-ddThh:mm:ssZ");

    requestString += "&language=ENG";  // TODO locale / language handling

    requestString += "&routeattributes=waypoints,summary,shape,maneuvers,links,boundingBox";
    requestString += "&maneuverattributes=position,link";

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

void QGeoRoutingManagerEngineNokia::routeFinished()
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(finished(QGeoRouteReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoRoutingManagerEngineNokia::routeError(QGeoRouteReply::Error error, const QString &errorString)
{
    QGeoRouteReply *reply = qobject_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}
