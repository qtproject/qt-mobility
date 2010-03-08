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

#include "qrouterequest.h"
#include "qroutereply.h"

QTM_BEGIN_NAMESPACE

//---------------------------------------------------------------------
//QRouteRequest

QRouteRequest::QRouteRequest()
{
    vers = "1.0";
    nTotal = 0;
    nAlternatives = 0;
    languageCode = "";
    rType = Fastest;
    rMode = Car;
}

QString QRouteRequest::version() const
{
    return vers;
}

void QRouteRequest::setSource(const QGeoCoordinateMaps& source)
{
    src = source;
}

QGeoCoordinateMaps QRouteRequest::source() const
{
    return src;
}

void QRouteRequest::setDestination(const QGeoCoordinateMaps& destination)
{
    dst = destination;
}

QGeoCoordinateMaps QRouteRequest::destination() const
{
    return dst;
}

void QRouteRequest::setTotalResults(quint32 totalResults)
{
    nTotal = totalResults;
}

quint32 QRouteRequest::totalResults() const
{
    return nTotal;
}

void QRouteRequest::setAlternatives(quint16 nAlternatives)
{
    this->nAlternatives = nAlternatives;
}

quint16 QRouteRequest::alternatives() const
{
    return nAlternatives;
}

void QRouteRequest::setLanguage(const QString& code)
{
    languageCode = code;
}

QString QRouteRequest::language() const
{
    return languageCode;
}

void QRouteRequest::setDeparture(const QDateTime& tod)
{
    this->tod = tod;
}

QDateTime QRouteRequest::departure() const
{
    return tod;
}

void QRouteRequest::setArrival(const QDateTime& toa)
{
    this->toa = toa;
}

QDateTime QRouteRequest::arrival() const
{
    return toa;
}

void QRouteRequest::setType(RouteType type)
{
    this->rType = type;
}

QRouteRequest::RouteType QRouteRequest::type() const
{
    return rType;
}

void QRouteRequest::setMode(RouteMode mode)
{
    this->rMode = mode;
}

QRouteRequest::RouteMode QRouteRequest::mode() const
{
    return rMode;
}

void QRouteRequest::setAvoid(QList<RouteAvoid> avoid)
{
    rAvoid = avoid;
}

QList<QRouteRequest::RouteAvoid> QRouteRequest::avoid() const
{
    return rAvoid;
}

void QRouteRequest::addStopOver(const QGeoCoordinateMaps& stopOver)
{
    stpOvers += stopOver;
}

void QRouteRequest::removeStopOver(const QGeoCoordinateMaps& stopOver)
{
    int i = 0;

    while (i < stpOvers.length()) {
        if (stpOvers[i] == stopOver)
            stpOvers.removeAt(i);
        else
            i++;
    }
}

const QList<QGeoCoordinateMaps>& QRouteRequest::stopOvers() const
{
    return stpOvers;
}

QString QRouteRequest::typeToString() const
{
    if (rType == Fastest)
        return "fastest";
    else if (rType == Shortest)
        return "shortest";
    else if (rType == Economic)
        return "economic";

    return "";
}

QString QRouteRequest::modeToString() const
{
    if (rMode == Car)
        return "car";
    else if (rMode == Pedestrian)
        return "pedestrian";
    else if (rMode == PublicTransport)
        return "public transport";

    return "";
}

QString QRouteRequest::avoidToString() const
{
    QString s;

    for (int i = 0; i < rAvoid.count(); i++) {
        if (rAvoid[i] == Highways)
            s += "highways,";
        else if (rAvoid[i] == Tollroads)
            s += "tollroads,";
        else if (rAvoid[i] == Ferries)
            s += "ferries,";
        else if (rAvoid[i] == Tunnels)
            s += "tunnels,";
        else if (rAvoid[i] == Dirtroads)
            s += "dirtroads,";
        else if (rAvoid[i] == RailFerries)
            s += "rail ferries,";
    }

    if (s.endsWith(","))
        return s.left(s.length() - 1);

    return "";
}

//---------------------------------------------------------------------
//QRouteReply
/*
QRouteReply::~QRouteReply()
{
    for (int i = 0; i < rt.length(); i++)
    {
        QRoute* route = rt[i];
        rt.pop_front();

        delete route;
    }
}
*/

//---------------------------------------------------------------------
//QRoute

QRoute::QRoute(const QRoute& route)
        : dist(route.dist), tod(route.tod), toa(route.toa), box(route.box), man(route.man)
{
}

QRoute& QRoute::operator=(const QRoute& route)
{
    dist = route.dist;
    tod = route.tod;
    toa = route.toa;
    box = route.box;
    man = route.man;

    return *this;
}

//---------------------------------------------------------------------
//QManeuver
QManeuver::QManeuver(const QManeuver& maneuver)
        : wPoints(maneuver.wPoints), mPoints(maneuver.mPoints)
{
    descr = maneuver.descr;
    act = maneuver.act;
    dist = maneuver.dist;
    dur = maneuver.dur;
    trn = maneuver.trn;
    stName = maneuver.stName;
    rtName = maneuver.rtName;
    nxtStreetName = maneuver.nxtStreetName;
    sgnPost = maneuver.sgnPost;
    traffDir = maneuver.traffDir;
    icn = maneuver.icn;
}

QManeuver& QManeuver::operator=(const QManeuver& maneuver)
{
    wPoints = maneuver.wPoints;
    mPoints = maneuver.mPoints;

    descr = maneuver.descr;
    act = maneuver.act;
    dist = maneuver.dist;
    dur = maneuver.dur;
    trn = maneuver.trn;
    stName = maneuver.stName;
    rtName = maneuver.rtName;
    nxtStreetName = maneuver.nxtStreetName;
    sgnPost = maneuver.sgnPost;
    traffDir = maneuver.traffDir;
    icn = maneuver.icn;

    return *this;
}

QTM_END_NAMESPACE

