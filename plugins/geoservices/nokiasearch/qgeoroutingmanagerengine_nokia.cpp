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

#include "qgeoroutingmanagerengine_nokia.h"
#include "qgeoroutereply_nokia.h"

#include <QStringList>
#include <QNetworkProxy>
#include <QGeoBoundingBox>

QGeoRoutingManagerEngineNokia::QGeoRoutingManagerEngineNokia(const QMap<QString, QString> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoRoutingManagerEngine(parameters),
        m_host("cgu02.tst.nav.svc.ovi.com")
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

    setSupportsRouteUpdates(false);
    setSupportsAlternativeRoutes(false);
    setSupportsExcludeAreas(false);

    QGeoRouteRequest::AvoidFeatureTypes avoidFeatures;
    avoidFeatures |= QGeoRouteRequest::AvoidNothing;
    setSupportedAvoidFeatureTypes(avoidFeatures);

    QGeoRouteRequest::InstructionDetails instructionDetails;
    instructionDetails |= QGeoRouteRequest::NoInstructions;
    setSupportedInstructionDetails(instructionDetails);

    QGeoRouteRequest::RouteOptimizations optimizations;
    setSupportedRouteOptimizations(optimizations);

    QGeoRouteRequest::TravelModes travelModes;
    setSupportedTravelModes(travelModes);

    QGeoRouteRequest::SegmentDetails segmentDetails;
    segmentDetails |= QGeoRouteRequest::NoSegmentData;
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
    QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "Route updates are not supported by this service provider.", this);
    emit error(reply, reply->error(), reply->errorString());

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
    requestString += "rt?tlv=";

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position)
{
    return "";
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
