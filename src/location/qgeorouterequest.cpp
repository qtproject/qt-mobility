
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

#include "qgeorouterequest.h"
#include "qgeorouterequest_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRouteRequest
    \brief The QGeoRouteRequest class represents a request for a route between two points.
    \ingroup location

    This is the base class representing a route request from a given source to a given destination.
*/

/*!
    \enum QGeoRouteRequest::RouteType

    \value Fastest
        fastest route (default)
    \value Shortest
        shortest route
    \value Economic
        economic route
*/

/*!
    \enum QGeoRouteRequest::RouteMode

    \value Car
        car route (default)
    \value Pedestrian
        pedestrian route
    \value PublicTransport
        public transport route
*/

/*!
    \enum QGeoRouteRequest::RouteAvoid

    Values that specify which components the route will not contain.

    \value Highways
        avoid highways
    \value Tollroads
        avoid tollroads
    \value Ferries
        avoid ferries
    \value Tunnels
        avoid tunnels
    \value Dirtroads
        avoid dirt roads
    \value RailFerries
        avoid rail ferries
*/

/*!
    The default constructor.
*/
QGeoRouteRequest::QGeoRouteRequest()
        : d_ptr(new QGeoRouteRequestPrivate)
{
}

/*!
    Destroys the route request.
*/
QGeoRouteRequest::~QGeoRouteRequest()
{
    Q_D(QGeoRouteRequest);
    delete d;
}

/*!
    Returns the service version.

    Currently the only supported version is 1.0.
*/
QString QGeoRouteRequest::version() const
{
    Q_D(const QGeoRouteRequest);
    return d->version;
}

/*!
    Sets the source geo coordinate for this request to \a source.
*/
void QGeoRouteRequest::setSource(const QGeoCoordinate& source)
{
    Q_D(QGeoRouteRequest);
    d->src = source;
}

/*!
    Returns the source geo coordinate for this request.
*/
QGeoCoordinate QGeoRouteRequest::source() const
{
    Q_D(const QGeoRouteRequest);
    return d->src;
}

/*!
    Sets the destination geo coordinate for this request to \a destination.
*/
void QGeoRouteRequest::setDestination(const QGeoCoordinate& destination)
{
    Q_D(QGeoRouteRequest);
    d->dst = destination;
}

/*!
    Returns the destination geo coordinate for this request.
*/
QGeoCoordinate QGeoRouteRequest::destination() const
{
    Q_D(const QGeoRouteRequest);
    return d->dst;
}

/*!
    Sets the maximum number of response results.

    If set to 0 all possible results will be returned.
*/
void QGeoRouteRequest::setTotalResults(quint32 totalResults)
{
    Q_D(QGeoRouteRequest);
    d->nTotal = totalResults;
}

/*!
    Returns the maximum number of response results.
*/
quint32 QGeoRouteRequest::totalResults() const
{
    Q_D(const QGeoRouteRequest);
    return d->nTotal;
}

/*!
    Sets the maximum number of alternative routes the server will try to generate.

    If set to 0 the result will contain no alternative routes (only one result is returned if any).
*/
void QGeoRouteRequest::setAlternatives(quint16 nAlternatives)
{
    Q_D(QGeoRouteRequest);
    d->nAlternatives = nAlternatives;
}

/*!
    Returns the maximum number of alternatives to be returned.
*/
quint16 QGeoRouteRequest::alternatives() const
{
    Q_D(const QGeoRouteRequest);
    return d->nAlternatives;
}

/*!
    Sets he RFC 3066 language code of the response to \a code.

    The language code should look like en-US or de-DE.
*/
void QGeoRouteRequest::setLanguage(const QString& code)
{
    Q_D(QGeoRouteRequest);
    d->languageCode = code;
}

/*!
    Returns the RFC 3066 language code of the response.
*/
QString QGeoRouteRequest::language() const
{
    Q_D(const QGeoRouteRequest);
    return d->languageCode;
}

/*!
    Sets the planned time of departure to \a tod.
*/
void QGeoRouteRequest::setDepartureTime(const QDateTime& departureTime)
{
    Q_D(QGeoRouteRequest);
    d->departureTime = departureTime;
}

/*!
    Returns the planned time of departure.
*/
QDateTime QGeoRouteRequest::departureTime() const
{
    Q_D(const QGeoRouteRequest);
    return d->departureTime;
}

/*!
    Sets the planned time of arrival to \a toa.
*/
void QGeoRouteRequest::setArrivalTime(const QDateTime& arrivalTime)
{
    Q_D(QGeoRouteRequest);
    d->arrivalTime = arrivalTime;
}

/*!
    Returns the planned time of departure.
*/
QDateTime QGeoRouteRequest::arrivalTime() const
{
    Q_D(const QGeoRouteRequest);
    return d->arrivalTime;
}

/*!
    Sets the type of the route the server will calculate to \a type.

    \sa RouteType
*/
void QGeoRouteRequest::setType(RouteType type)
{
    Q_D(QGeoRouteRequest);
    d->routeType = type;
}

/*!
    Returns the route type.
*/
QGeoRouteRequest::RouteType QGeoRouteRequest::type() const
{
    Q_D(const QGeoRouteRequest);
    return d->routeType;
}

/*!
    Sets the movement mode of the route to \a mode.

    \sa RouteMode
*/
void QGeoRouteRequest::setMode(RouteMode mode)
{
    Q_D(QGeoRouteRequest);
    d->routeMode = mode;
}

/*!
    Returns the route mode.
*/
QGeoRouteRequest::RouteMode QGeoRouteRequest::mode() const
{
    Q_D(const QGeoRouteRequest);
    return d->routeMode;
}

/*!
    Specifies which route components should not be part of the calculated route to \a avoid.

    The components are avoided on a best effort basis.

    \sa RouteAvoid
*/
void QGeoRouteRequest::setAvoid(QList<RouteAvoid> avoid)
{
    Q_D(QGeoRouteRequest);
    d->avoid = avoid;
}

/*!
    Returns a list containing all routes components to be avoided.
*/
QList<QGeoRouteRequest::RouteAvoid> QGeoRouteRequest::avoid() const
{
    Q_D(const QGeoRouteRequest);
    return d->avoid;
}

/*!
    Adds the stop over \a stopOver to the requested route.

    The server calculates a route that contains these waypoints (in the given order)
    as part of the route. These waypoints must be between the destination and the source.
*/
void QGeoRouteRequest::addStopOver(const QGeoCoordinate& stopOver)
{
    Q_D(QGeoRouteRequest);
    d->stopOvers += stopOver;
}

/*!
    Removes the stop over \a stopOver from the requested route.

    \sa addStopOver()
*/
void QGeoRouteRequest::removeStopOver(const QGeoCoordinate& stopOver)
{
    Q_D(QGeoRouteRequest);
    int i = 0;

    while (i < d->stopOvers.length()) {
        if (d->stopOvers[i] == stopOver)
            d->stopOvers.removeAt(i);
        else
            i++;
    }
}

/*!
    Returns all added stop overs.
*/
const QList<QGeoCoordinate>& QGeoRouteRequest::stopOvers() const
{
    Q_D(const QGeoRouteRequest);
    return d->stopOvers;
}

/*!
    Returns the request string for this request and a given \a host.
*/
QString QGeoRouteRequest::requestString(const QString &host) const
{
    Q_D(const QGeoRouteRequest);
    return d->requestString(host);
}
/*****************************************************************************
 *****************************************************************************/

QGeoRouteRequestPrivate::QGeoRouteRequestPrivate()
        : version("1.0"), nTotal(0),
        nAlternatives(0),
        languageCode(""),
        routeType(QGeoRouteRequest::Fastest),
        routeMode(QGeoRouteRequest::Car)
{}

QString QGeoRouteRequestPrivate::typeToString() const
{
    if (routeType == QGeoRouteRequest::Fastest)
        return "fastest";
    else if (routeType == QGeoRouteRequest::Shortest)
        return "shortest";
    else if (routeType == QGeoRouteRequest::Economic)
        return "economic";

    return "";
}

QString QGeoRouteRequestPrivate::modeToString() const
{
    if (routeMode == QGeoRouteRequest::Car)
        return "car";
    else if (routeMode == QGeoRouteRequest::Pedestrian)
        return "pedestrian";
    else if (routeMode == QGeoRouteRequest::PublicTransport)
        return "public transport";

    return "";
}

QString QGeoRouteRequestPrivate::avoidToString() const
{
    QString s;

    for (int i = 0; i < avoid.count(); i++) {
        if (avoid[i] == QGeoRouteRequest::Highways)
            s += "highways,";
        else if (avoid[i] == QGeoRouteRequest::Tollroads)
            s += "tollroads,";
        else if (avoid[i] == QGeoRouteRequest::Ferries)
            s += "ferries,";
        else if (avoid[i] == QGeoRouteRequest::Tunnels)
            s += "tunnels,";
        else if (avoid[i] == QGeoRouteRequest::Dirtroads)
            s += "dirtroads,";
        else if (avoid[i] == QGeoRouteRequest::RailFerries)
            s += "rail ferries,";
    }

    if (s.endsWith(","))
        return s.left(s.length() - 1);

    return "";
}

QString QGeoRouteRequestPrivate::requestString(const QString &host) const
{
    QString request = "http://";
    request += host;
    request += "/routing/rt/";
    request += version;
    request += "?referer=localhost";
    request += "&slong=";
    request += trimDouble(src.longitude());
    request += "&slat=";
    request += trimDouble(src.latitude());
    request += "&dlong=";
    request += trimDouble(dst.longitude());
    request += "&dlat=";
    request += trimDouble(dst.latitude());

    if (nTotal > 0) {
        request += "&total=";
        request += QString::number(nTotal);
    }

    if (nAlternatives > 0) {
        request += "&alternatives=";
        request += QString::number(nAlternatives);
    }
    if (languageCode != "") {
        request += "&lg=";
        request += languageCode;
    }
    if (departureTime.isValid()) {
        request += "&tod=";
        request += departureTime.toUTC().toString();
    }
    if (arrivalTime.isValid()) {
        request += "&toa=";
        request += arrivalTime.toUTC().toString();
    }

    request += "&type=";
    request += typeToString();
    request += "&mode=";
    request += modeToString();

    if (avoid.count() > 0) {
        request += "&avoid=";
        request += avoidToString();
    }

    for (int i = 0; i < stopOvers.length(); i++) {
        request += QString::number(stopOvers[i].latitude(), 'f');
        request += ",";
        request += QString::number(stopOvers[i].longitude(), 'f');
        request += " ";
    }

    return request;
}

QString QGeoRouteRequestPrivate::trimDouble(qreal degree, int decimalDigits) const
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

QTM_END_NAMESPACE

