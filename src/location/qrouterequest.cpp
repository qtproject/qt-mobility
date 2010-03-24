
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
#include "qrouterequest_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QRouteRequest
    \brief The QRouteRequest class represents a request for a route between two points.
    \ingroup location

    This is the base class representing a route request from a given source to a given destination.
*/

/*!
    \enum QRouteRequest::RouteType

    \value Fastest
        fastest route (default)
    \value Shortest
        shortest route
    \value Economic
        economic route
*/

/*!
    \enum QRouteRequest::RouteMode

    \value Car
        car route (default)
    \value Pedestrian
        pedestrian route
    \value PublicTransport
        public transport route
*/

/*!
    \enum QRouteRequest::RouteAvoid

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
QRouteRequest::QRouteRequest()
    : d_ptr(new QRouteRequestPrivate)
{
}

/*!
    Destroys the route request.
*/
QRouteRequest::~QRouteRequest()
{
    Q_D(QRouteRequest);
    delete d;
}

/*!
    Returns the service version.

    Currently the only supported version is 1.0.
*/
QString QRouteRequest::version() const
{
    Q_D(const QRouteRequest);
    return d->version;
}

/*!
    Sets the source geo coordinate for this request to \a source.
*/
void QRouteRequest::setSource(const QGeoCoordinate& source)
{
    Q_D(QRouteRequest);
    d->src = source;
}

/*!
    Returns the source geo coordinate for this request.
*/
QGeoCoordinate QRouteRequest::source() const
{
    Q_D(const QRouteRequest);
    return d->src;
}

/*!
    Sets the destination geo coordinate for this request to \a destination.
*/
void QRouteRequest::setDestination(const QGeoCoordinate& destination)
{
    Q_D(QRouteRequest);
    d->dst = destination;
}

/*!
    Returns the destination geo coordinate for this request.
*/
QGeoCoordinate QRouteRequest::destination() const
{
    Q_D(const QRouteRequest);
    return d->dst;
}

/*!
    Sets the maximum number of response results.

    If set to 0 all possible results will be returned.
*/
void QRouteRequest::setTotalResults(quint32 totalResults)
{
    Q_D(QRouteRequest);
    d->nTotal = totalResults;
}

/*!
    Returns the maximum number of response results.
*/
quint32 QRouteRequest::totalResults() const
{
    Q_D(const QRouteRequest);
    return d->nTotal;
}

/*!
    Sets the maximum number of alternative routes the server will try to generate.

    If set to 0 the result will contain no alternative routes (only one result is returned if any).
*/
void QRouteRequest::setAlternatives(quint16 nAlternatives)
{
    Q_D(QRouteRequest);
    d->nAlternatives = nAlternatives;
}

/*!
    Returns the maximum number of alternatives to be returned.
*/
quint16 QRouteRequest::alternatives() const
{
    Q_D(const QRouteRequest);
    return d->nAlternatives;
}

/*!
    Sets he RFC 3066 language code of the response to \a code.

    The language code should look like en-US or de-DE.
*/
void QRouteRequest::setLanguage(const QString& code)
{
    Q_D(QRouteRequest);
    d->languageCode = code;
}

/*!
    Returns the RFC 3066 language code of the response.
*/
QString QRouteRequest::language() const
{
    Q_D(const QRouteRequest);
    return d->languageCode;
}

/*!
    Sets the planned time of departure to \a tod.
*/
void QRouteRequest::setDepartureTime(const QDateTime& departureTime)
{
    Q_D(QRouteRequest);
    d->departureTime = departureTime;
}

/*!
    Returns the planned time of departure.
*/
QDateTime QRouteRequest::departureTime() const
{
    Q_D(const QRouteRequest);
    return d->departureTime;
}

/*!
    Sets the planned time of arrival to \a toa.
*/
void QRouteRequest::setArrivalTime(const QDateTime& arrivalTime)
{
    Q_D(QRouteRequest);
    d->arrivalTime = arrivalTime;
}

/*!
    Returns the planned time of departure.
*/
QDateTime QRouteRequest::arrivalTime() const
{
    Q_D(const QRouteRequest);
    return d->arrivalTime;
}

/*!
    Sets the type of the route the server will calculate to \a type.

    \sa RouteType
*/
void QRouteRequest::setType(RouteType type)
{
    Q_D(QRouteRequest);
    d->routeType = type;
}

/*!
    Returns the route type.
*/
QRouteRequest::RouteType QRouteRequest::type() const
{
    Q_D(const QRouteRequest);
    return d->routeType;
}

/*!
    Sets the movement mode of the route to \a mode.

    \sa RouteMode
*/
void QRouteRequest::setMode(RouteMode mode)
{
    Q_D(QRouteRequest);
    d->routeMode = mode;
}

/*!
    Returns the route mode.
*/
QRouteRequest::RouteMode QRouteRequest::mode() const
{
    Q_D(const QRouteRequest);
    return d->routeMode;
}

/*!
    Specifies which route components should not be part of the calculated route to \a avoid.

    The components are avoided on a best effort basis.

    \sa RouteAvoid
*/
void QRouteRequest::setAvoid(QList<RouteAvoid> avoid)
{
    Q_D(QRouteRequest);
    d->avoid = avoid;
}

/*!
    Returns a list containing all routes components to be avoided.
*/
QList<QRouteRequest::RouteAvoid> QRouteRequest::avoid() const
{
    Q_D(const QRouteRequest);
    return d->avoid;
}

/*!
    Adds the stop over \a stopOver to the requested route.

    The server calculates a route that contains these waypoints (in the given order)
    as part of the route. These waypoints must be between the destination and the source.
*/
void QRouteRequest::addStopOver(const QGeoCoordinate& stopOver)
{
    Q_D(QRouteRequest);
    d->stopOvers += stopOver;
}

/*!
    Removes the stop over \a stopOver from the requested route.

    \sa addStopOver()
*/
void QRouteRequest::removeStopOver(const QGeoCoordinate& stopOver)
{
    Q_D(QRouteRequest);
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
const QList<QGeoCoordinate>& QRouteRequest::stopOvers() const
{
    Q_D(const QRouteRequest);
    return d->stopOvers;
}

/*!
    Returns the request string for this request and a given \a host.
*/
QString QRouteRequestPrivate::requestString(const QString &host) const
{
    Q_D(const QRouteRequest);
    return d->requestString(host);
}
/*****************************************************************************
 *****************************************************************************/

QRouteRequestPrivate::QRouteRequestPrivate()
    : version("1.0"), nTotal(0),
    nAlternatives(0),
    languageCode(""),
    routeType(QRouteRequest::Fastest),
    routeMode(QRouteRequest::Car)
{}

QString QRouteRequestPrivate::typeToString() const
{
    if (routeType == QRouteRequest::Fastest)
        return "fastest";
    else if (routeType == QRouteRequest::Shortest)
        return "shortest";
    else if (routeType == QRouteRequest::Economic)
        return "economic";

    return "";
}

QString QRouteRequestPrivate::modeToString() const
{
    if (routeMode == QRouteRequest::Car)
        return "car";
    else if (routeMode == QRouteRequest::Pedestrian)
        return "pedestrian";
    else if (routeMode == QRouteRequest::PublicTransport)
        return "public transport";

    return "";
}

QString QRouteRequestPrivate::avoidToString() const
{
    QString s;

    for (int i = 0; i < avoid.count(); i++) {
        if (avoid[i] == QRouteRequest::Highways)
            s += "highways,";
        else if (avoid[i] == QRouteRequest::Tollroads)
            s += "tollroads,";
        else if (avoid[i] == QRouteRequest::Ferries)
            s += "ferries,";
        else if (avoid[i] == QRouteRequest::Tunnels)
            s += "tunnels,";
        else if (avoid[i] == QRouteRequest::Dirtroads)
            s += "dirtroads,";
        else if (avoid[i] == QRouteRequest::RailFerries)
            s += "rail ferries,";
    }

    if (s.endsWith(","))
        return s.left(s.length() - 1);

    return "";
}

QString QRouteRequestPrivate::requestString(const QString &host) const
{
    QString request = "http://";
    request += host;
    request += "/routing/rt/";
    request += vers;
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
    if (tod.isValid()) {
        request += "&tod=";
        request += tod.toUTC().toString();
    }
    if (toa.isValid()) {
        request += "&toa=";
        request += toa.toUTC().toString();
    }

    request += "&type=";
    request += typeToString();
    request +="&mode=";
    request += modeToString();

    if (rAvoid.count() > 0) {
        request += "&avoid=";
        request += avoidToString();
    }

    const QList<QGeoCoordinate>& stOvers = stopOvers();

    for (int i = 0; i < stOvers.length(); i++) {
        request += QString::number(stOvers[i].latitude(), 'f');
        request += ",";
        request += QString::number(stOvers[i].longitude(), 'f');
        request += " ";
    }

    return request;
}

QString QRouteRequestPrivate::trimDouble(qreal degree, int decimalDigits) const
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

QTM_END_NAMESPACE

