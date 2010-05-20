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

#include "qgeorouterequestwrapper_p.h"
#include "qgeoroutecalculatexmlparser_p.h"

#include <qgeoroutecalculaterequest.h>
#include <qgeorouteupdaterequest.h>
#include <qgeocoordinate.h>

#include <QNetworkProxy>
#include <QStringList>

QGeoRoutingManagerEngineNokia::QGeoRoutingManagerEngineNokia(QObject *parent)
    : QGeoRoutingManagerEngine(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                                             "172.16.42.137",
                                             8080));
    m_host = "route.desktop.maps.svc.ovi.com";
}

QGeoRoutingManagerEngineNokia::~QGeoRoutingManagerEngineNokia() {}

void QGeoRoutingManagerEngineNokia::requestDestroyed(QGeoRouteAbstractRequest *request)
{
    if (!request)
        return;

    if (!m_requests.contains(request))
        return;

    QNetworkReply *reply = m_requests[request];
    m_requests.remove(request);

    reply->abort();
    reply->deleteLater();
}

bool QGeoRoutingManagerEngineNokia::startRequest(QGeoRouteAbstractRequest *request)
{
    if (!request)
        return false;

    if (m_requests.contains(request))
       return false;

    QString rawRequest = "";

    switch (request->type()) {
        case QGeoRouteAbstractRequest::CalculateRequest:
            rawRequest = requestString(static_cast<QGeoRouteCalculateRequest*>(request));
            break;
        case QGeoRouteAbstractRequest::UpdateRequest:
            rawRequest = requestString(static_cast<QGeoRouteUpdateRequest*>(request));
            break;
        default:
            break;
    }

    if (rawRequest.isEmpty())
        return false;

    QNetworkRequest networkRequest = QNetworkRequest(QUrl(rawRequest));
    QNetworkReply *reply = m_networkManager->get(networkRequest);
    m_requests.insert(request, reply);

    QGeoRouteRequestWrapper *wrapper
            = new QGeoRouteRequestWrapper(this, request, reply);

    connect(wrapper,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(wrapper,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));

    return true;
}

void QGeoRoutingManagerEngineNokia::networkFinished()
{
    QGeoRouteRequestWrapper *wrapper
            = qobject_cast<QGeoRouteRequestWrapper *>(sender());

    QGeoRouteAbstractRequest *request = wrapper->request();
    QNetworkReply *reply = wrapper->reply();

    m_requests.remove(request);

    if (reply->error() != QNetworkReply::NoError) {
        updateRequestStatus(request,
                            QGeoRouteAbstractRequest::FinishedState,
                            QGeoRoutingManager::CommunicationError,
                            reply->errorString());
        reply->deleteLater();
        return;
    }

    switch (request->type()) {
        case QGeoRouteAbstractRequest::CalculateRequest:
            parseReply(reply, static_cast<QGeoRouteCalculateRequest*>(request));
            break;
        case QGeoRouteAbstractRequest::UpdateRequest:
            parseReply(reply, static_cast<QGeoRouteUpdateRequest*>(request));
            break;
        default:
            // TODO error and status update
            break;
    }

    reply->deleteLater();
}

void QGeoRoutingManagerEngineNokia::networkError(QNetworkReply::NetworkError error)
{
    QGeoRouteRequestWrapper *wrapper
            = qobject_cast<QGeoRouteRequestWrapper *>(sender());

    QGeoRouteAbstractRequest *request = wrapper->request();
    QNetworkReply *reply = wrapper->reply();

    m_requests.remove(request);

    updateRequestStatus(request,
                        QGeoRouteAbstractRequest::FinishedState,
                        QGeoRoutingManager::CommunicationError,
                        reply->errorString());

    reply->deleteLater();
}

bool QGeoRoutingManagerEngineNokia::cancelRequest(QGeoRouteAbstractRequest *request)
{
    if (!request)
        return false;

    if (!m_requests.contains(request))
       return false;

    QNetworkReply *reply = m_requests[request];
    m_requests.remove(request);

    reply->abort();
    reply->deleteLater();

    return true;
}

bool QGeoRoutingManagerEngineNokia::waitForRequestFinished(QGeoRouteAbstractRequest *request, int msecs)
{
    if (!request)
        return false;

    if (!m_requests.contains(request))
       return false;

    // TODO implement

    return true;
}

QString QGeoRoutingManagerEngineNokia::requestString(QGeoRouteCalculateRequest *request)
{
    QString requestString = "http://";
    requestString += m_host;
    requestString += "/routing/rt/1.0?referer=localhost";

    int numWaypoints = request->waypoints().size();

    if (numWaypoints < 2)
        return "";

    requestString += "&slong=";
    requestString += trimDouble(request->waypoints().at(0).longitude());

    requestString += "&slat=";
    requestString += trimDouble(request->waypoints().at(0).latitude());

    requestString += "&dlong=";
    requestString += trimDouble(request->waypoints().at(numWaypoints - 1).longitude());

    requestString += "&dlat=";
    requestString += trimDouble(request->waypoints().at(numWaypoints - 1).latitude());

    requestString += "&alternatives=";
    requestString += QString::number(request->numberAlternativeRoutes());

    requestString += "&type=";
    switch (request->routeOptimization()) {
    case QGeoRouteCalculateRequest::ShortestRoute:
        requestString += "shortest";
    case QGeoRouteCalculateRequest::FastestRoute:
        requestString += "fastest";
    case QGeoRouteCalculateRequest::MostEconomicRoute:
        requestString += "economic";
    default:
        return "";
    }

    requestString += "&mode=";
    switch (request->travelModes()) {
    case QGeoRouteCalculateRequest::CarTravel:
        requestString += "car";
    case QGeoRouteCalculateRequest::PedestrianTravel:
        requestString += "pedestrian";
    case QGeoRouteCalculateRequest::PublicTransitTravel:
        requestString += "public transport";
    default:
        return "";
    }

    QGeoRouteCalculateRequest::AvoidFeatureTypes avoid = request->avoidFeatureTypes();
    if (avoid != QGeoRouteCalculateRequest::AvoidNothing) {
        requestString += "&avoid=";
        QStringList avoidTypes;
        if ((avoid & QGeoRouteCalculateRequest::AvoidTolls) != 0)
            avoidTypes.append("tollroads");
        if ((avoid & QGeoRouteCalculateRequest::AvoidHighways) != 0)
            avoidTypes.append("highways");
        if ((avoid & QGeoRouteCalculateRequest::AvoidFerries) != 0)
            avoidTypes.append("ferries");
        if ((avoid & QGeoRouteCalculateRequest::AvoidTunnels) != 0)
            avoidTypes.append("tunnels");
        if ((avoid & QGeoRouteCalculateRequest::AvoidDirtRoads) != 0)
            avoidTypes.append("dirtroads");
        requestString += avoidTypes.join(",");
    }

    if (numWaypoints > 2) {
        requestString += "&stopovers=";
        QStringList waypoints;
        for (int i = 1; i < numWaypoints - 2; ++i) {
            QString pair = trimDouble(request->waypoints().at(i).latitude());
            pair += ",";
            pair += trimDouble(request->waypoints().at(i).longitude());
            waypoints.append(pair);
        }
        requestString += waypoints.join(" ");
    }

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

QString QGeoRoutingManagerEngineNokia::requestString(QGeoRouteUpdateRequest *request)
{
    return "";
}

void QGeoRoutingManagerEngineNokia::parseReply(QNetworkReply *reply, QGeoRouteCalculateRequest *request)
{
    QGeoRouteCalculateXmlParser parser;

    if (parser.parse(reply)) {
        updateCalculateRequestResults(request, parser.results());
        updateRequestStatus(request,
                            QGeoRouteAbstractRequest::FinishedState,
                            QGeoRoutingManager::NoError,
                            "");
    } else {
        updateRequestStatus(request,
                            QGeoRouteAbstractRequest::FinishedState,
                            QGeoRoutingManager::ParseError,
                            "The response to the requested service was not able to be parsed.");
    }
}

void QGeoRoutingManagerEngineNokia::parseReply(QNetworkReply *reply, QGeoRouteUpdateRequest *request)
{
}

QGeoRoutingManagerNokia::QGeoRoutingManagerNokia(QObject *parent)
    : QGeoRoutingManager(parent)
{
    setEngine(new QGeoRoutingManagerEngineNokia(this));
}

QGeoRoutingManagerNokia::~QGeoRoutingManagerNokia() {}

