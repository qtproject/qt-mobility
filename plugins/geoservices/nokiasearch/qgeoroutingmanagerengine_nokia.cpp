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

#include <QBuffer>
#include <QStringList>
#include <QNetworkProxy>
#include <QGeoBoundingBox>

const quint8 ID_REQUEST_TYPE=0x1;
const quint8 ID_REQUEST_UNICODE=0x63;
const quint8 ID_REQUEST_ZLIB=0x59;
const quint8 ID_REQUEST_BASE64=0x58;
const quint8 ID_REQUEST_MANEUVER_INFO=0x9c;
const quint8 ID_REQUEST_JUNCTION_INFO=0x47;
const quint8 ID_REQUEST_ROUTE_START_POSITION=0x1f;
const quint8 ID_REQUEST_ROUTE_END_POSITION=0x20;
const quint8 ID_REQUEST_ROUTE_FLAGS=0x23;
const quint8 ID_REQUEST_ROUTE_GUIDANCE_LANGUAGE=0x78;
const quint16 ID_REQUEST_ROUTE=0x2;
const quint8 ID_REQUEST_TRUE=0xff;
const quint8 ID_REQUEST_FALSE=0x0;


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
    avoidFeatures |= QGeoRouteRequest::AvoidTolls;
    avoidFeatures |= QGeoRouteRequest::AvoidFerries;
    avoidFeatures |= QGeoRouteRequest::AvoidTunnels;
    setSupportedAvoidFeatureTypes(avoidFeatures);

    QGeoRouteRequest::InstructionDetails instructionDetails;
    instructionDetails |= QGeoRouteRequest::BasicInstructions;
    setSupportedInstructionDetails(instructionDetails);

    QGeoRouteRequest::RouteOptimizations optimizations;
    optimizations |= QGeoRouteRequest::ShortestRoute;
    optimizations |= QGeoRouteRequest::FastestRoute;
    setSupportedRouteOptimizations(optimizations);

    QGeoRouteRequest::TravelModes travelModes;
    travelModes |= QGeoRouteRequest::CarTravel;
    travelModes |= QGeoRouteRequest::PedestrianTravel;
    setSupportedTravelModes(travelModes);

    QGeoRouteRequest::SegmentDetails segmentDetails;
    segmentDetails |= QGeoRouteRequest::BasicSegmentData;
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
        QGeoRouteReply *reply = new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError, "The given route request options are not supported by this service provider.", this);
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

    int numWaypoints = request.waypoints().size();
    if (numWaypoints < 2)
        supported = false;


    if (!supported)
        return "";

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/rt?tlv=";

    requestString += encodeTLV(ID_REQUEST_TYPE, ID_REQUEST_ROUTE);
    requestString += encodeTLV(ID_REQUEST_UNICODE, ID_REQUEST_FALSE);
    requestString += encodeTLV(ID_REQUEST_ZLIB, ID_REQUEST_FALSE);
    requestString += encodeTLV(ID_REQUEST_BASE64, ID_REQUEST_FALSE);
    requestString += encodeTLV(ID_REQUEST_MANEUVER_INFO, ID_REQUEST_TRUE);
    requestString += encodeTLV(ID_REQUEST_JUNCTION_INFO, ID_REQUEST_TRUE);

    requestString += encodeTLV(ID_REQUEST_ROUTE_START_POSITION,coordinateToByteArray(request.waypoints().at(0)));
    requestString += encodeTLV(ID_REQUEST_ROUTE_END_POSITION,coordinateToByteArray(request.waypoints().at(numWaypoints-1)));

    quint32 flags = Default;
    if ((request.routeOptimization() & QGeoRouteRequest::ShortestRoute) != 0)
        flags |= Shortest;
    if ((request.travelModes() & QGeoRouteRequest::PedestrianTravel) != 0)
        flags |= Pedestrian;
    if ((request.avoidFeatureTypes() & QGeoRouteRequest::AvoidTolls) != 0)
        flags |= AvoidTollRoads;
    if ((request.avoidFeatureTypes() & QGeoRouteRequest::AvoidHighways) != 0)
        flags |= AvoidHighways;
    if ((request.avoidFeatureTypes() & QGeoRouteRequest::AvoidFerries) != 0)
        flags |= AvoidFerries;
    if ((request.avoidFeatureTypes() & QGeoRouteRequest::AvoidTunnels) != 0)
        flags |= AvoidTunnels;

    requestString += encodeTLV(ID_REQUEST_ROUTE_FLAGS,flags);
    requestString += encodeTLV(ID_REQUEST_ROUTE_GUIDANCE_LANGUAGE,QByteArray::fromRawData("DEF\x0",4));

    return requestString;
}

QString QGeoRoutingManagerEngineNokia::updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position)
{
    return "";
}

QString QGeoRoutingManagerEngineNokia::encodeTLV(quint8 id, QByteArray data)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray,QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream << id << quint16(data.length());
    dataArray.append(data);

    return dataArray.toHex().data();
}

QString QGeoRoutingManagerEngineNokia::encodeTLV(quint8 id, quint8 data)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray,QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream << id << quint16(1) << data;  //data length 1 bytes

    return dataArray.toHex().data();
}

QString QGeoRoutingManagerEngineNokia::encodeTLV(quint8 id, quint16 data)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray,QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream << id << quint16(2) << data;  //data length 2 bytes

    return dataArray.toHex().data();
}

QString QGeoRoutingManagerEngineNokia::encodeTLV(quint8 id, quint32 data)
{
    QByteArray dataArray;
    QDataStream dataStream(&dataArray,QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream << id << quint16(4) << data; //data length 4 bytes

    return dataArray.toHex().data();
}

QByteArray QGeoRoutingManagerEngineNokia::coordinateToByteArray(QGeoCoordinate coord)
{
    QByteArray array;
    QDataStream dataStream(&array,QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream << toInt32(coord.latitude()+90) << toInt32(coord.longitude()+180);
    return array;
}

quint32 QGeoRoutingManagerEngineNokia::toInt32(double value)
{
    value *= 0xB60B60; // 0x100000000 / 360;
    value -= (value > 0x7fffffff ? 0x100000000 : 0);
    return quint32(value);
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
