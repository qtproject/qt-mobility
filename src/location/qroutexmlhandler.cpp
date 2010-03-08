/*
* ==============================================================================
*  Name        : QRouteXmlHandler.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include "qroutexmlhandler.h"

#define ROUTE_CODE_OK "OK"
#define ROUTE_CODE_FAILED "FAILED"
#define ROUTE_CODE_FAILED_WITH_ALTERNATIVE "FAILED WITH ALTERNATIVE"

QTM_BEGIN_NAMESPACE

QRouteXmlHandler::QRouteXmlHandler(QRouteReply* routeReply) :
        reply(routeReply)
{
}

bool QRouteXmlHandler::startBoundingBox()
{
    if (parseStates.count() == 0)
        return false;
    if (parseStates[0] != Route)
        return false;

    currBox = &currRoute.box;
    return true;
}

bool QRouteXmlHandler::startElement(const QString& namespaceURI,
                                    const QString& localName,
                                    const QString& qName,
                                    const QXmlAttributes& atts)
{
    if (QBaseXmlHandler::startElement(namespaceURI, localName, qName, atts))
        return true;

    if (qName == "routes") {
        parseRouteReply(atts);
        parseStates.push_front(Root);
    } else if (qName == "route") {
        parseRoute(atts);
        parseStates.push_front(Route);
    } else if (qName == "maneuver") {
        if (parseStates.count() == 0 || parseStates[0] != Route)
            return false;

        parseManeuver(atts);
        parseStates.push_front(Maneuver);
    } else if (qName == "wayPoints") {
        if (parseStates.count() == 0 || parseStates[0] != Maneuver)
            return false;

        parseStates.push_front(WayPoints);
    } else if (qName == "maneuverPoints") {
        if (parseStates.count() == 0 || parseStates[0] != Maneuver)
            return false;

        parseStates.push_front(ManeuverPoints);
    }

    return true;
}

bool QRouteXmlHandler::parseGeoPoints(const QString& strPoints, QList<QGeoCoordinateMaps>& geoPoints)
{
    QStringList rawPoints = strPoints.split(' ');

    for (int i = 0; i < rawPoints.length(); i++) {
        QStringList coords = rawPoints[i].split(',');

        if (coords.length() != 2)
            return false;

        QGeoCoordinateMaps geoPoint(coords[1].toDouble(), coords[0].toDouble());
        geoPoints.append(geoPoint);
        int x = geoPoints.size();
        x++;
    }

    return true;
}

void QRouteXmlHandler::parseRouteReply(const QXmlAttributes& atts)
{
    QString code = atts.value("resultCode");

    if (!code.isEmpty()) {
        if (code == ROUTE_CODE_OK)
            reply->rCode = QRouteReply::OK;
        else if (code == ROUTE_CODE_FAILED_WITH_ALTERNATIVE)
            reply->rCode = QRouteReply::FailedWithAlternative;
        else
            reply->rCode = QRouteReply::Failed;
    }

    QString descr = atts.value("resultDescription");

    if (!descr.isEmpty()) {
        reply->descr = descr;
    }

    QString lang = atts.value("language");

    if (!lang.isEmpty()) {
        reply->lang = lang;
    }
}

void QRouteXmlHandler::parseRoute(const QXmlAttributes& atts)
{
    currRoute = QRoute(); //empty route

    QString descr = atts.value("distance");

    if (!descr.isEmpty()) {
        currRoute.dist = descr.toUInt();
    }

    QString tod = atts.value("timeOfDepature");

    if (!tod.isEmpty()) {
        //TODO: time of departure
    }

    QString toa = atts.value("timeOfArrival");

    if (!toa.isEmpty()) {
        //TODO: time of arrival;
    }
}

void QRouteXmlHandler::parseManeuver(const QXmlAttributes& atts)
{
    currManeuver = QManeuver(); //empty maneuver;
    QString s = atts.value("description");

    if (!s.isEmpty())
        currManeuver.descr = s;

    s = atts.value("action");

    if (!s.isEmpty())
        currManeuver.act = s;

    s = atts.value("distance");

    if (!s.isEmpty())
        currManeuver.dist = s.toUInt();

    s = atts.value("duration");

    if (!s.isEmpty()) {
        //TODO: duration
    }

    s = atts.value("turn");

    if (!s.isEmpty())
        currManeuver.trn = s;

    s = atts.value("streetName");

    if (!s.isEmpty())
        currManeuver.stName = s;

    s = atts.value("routeName");

    if (!s.isEmpty())
        currManeuver.rtName = s;

    s = atts.value("nextStreetName");

    if (!s.isEmpty())
        currManeuver.nxtStreetName = s;

    s = atts.value("signPost");

    if (!s.isEmpty())
        currManeuver.sgnPost = s;

    s = atts.value("trafficDirection");

    if (!s.isEmpty())
        currManeuver.traffDir = s.toLong();

    s = atts.value("icon");

    if (!s.isEmpty()) {
        currManeuver.icn = s.toLong();
    }
}

bool QRouteXmlHandler::characters(const QString& ch)
{
    if (parseStates.count() == 0)
        return true;
    if (QBaseXmlHandler::characters(ch))
        return true;

    if (parseStates[0] == WayPoints) {
        return parseGeoPoints(ch, currManeuver.wPoints);
    } else if (parseStates[0] == ManeuverPoints) {
        return parseGeoPoints(ch, currManeuver.mPoints);
    }

    return true;
}

bool QRouteXmlHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    if (parseStates.count() == 0)
        return false;
    if (QBaseXmlHandler::endElement(namespaceURI, localName, qName))
        return true;

    if (qName == "routes") {
        if (parseStates[0] != Root)
            return false;
    } else if (qName == "route") {
        if (parseStates[0] != Route)
            return false;

        reply->rt.append(currRoute);
    } else if (qName == "wayPoints") {
        if (parseStates[0] != WayPoints)
            return false;
    } else if (qName == "maneuverPoints") {
        if (parseStates[0] != ManeuverPoints)
            return false;
    } else if (qName == "maneuver") {
        if (parseStates[0] != Maneuver)
            return false;

        currRoute.man.append(currManeuver);
    } else
        return false;

    parseStates.removeFirst();
    return true;
}

QTM_END_NAMESPACE

