/*
* ==============================================================================
*  Name        : QRouteRequest.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ROUTEREQUEST_H
#define QLOCATION_ROUTEREQUEST_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* This is the base class representing a route request from a given source to
* a given destination.
*/
class Q_LOCATION_EXPORT QRouteRequest
{
    friend class QGeoNetworkManager;

public:

    //! Route types
    enum RouteType {
        Fastest, //!< fastest route, <b>default</b>
        Shortest, //!< shortest route
        Economic //!< economic route
    };

    //! Route modes
    enum RouteMode {
        Car, //!< car route, <b>default</b>
        Pedestrian, //!< pedestrian route
        PublicTransport //!< public transport route
    };

    //! Values that specify which components the route will not contain.
    enum RouteAvoid {
        Highways, //!< avoid highways
        Tollroads, //!< avoid tollroads
        Ferries, //!< avoid ferries
        Tunnels, //!< avoid tunnels
        Dirtroads, //!< avoid dirt roads
        RailFerries //!< avoid rail ferries
    };

public:
    /*!
    * The default constructor.
    */
    QRouteRequest();

    /*!
    * @return The service version; currently the only supported version is 1.0.
    */
    QString version() const;

    /*!
    * Sets the source geo coordinate for this request.
    * @param source The source geo coordinate.
    */
    void setSource(const QGeoCoordinateMaps& source);
    /*!
    * Gets the source geo coordinate for this request.
    * @return The source geo coordinate.
    */
    QGeoCoordinateMaps source() const;

    /*!
    * Sets the destination geo coordinate for this request.
    * @param destination The destination geo coordinate.
    */
    void setDestination(const QGeoCoordinateMaps& destination);
    /*!
    * Gets the destination geo coordinate for this request.
    * @param The destination geo coordinate.
    */
    QGeoCoordinateMaps destination() const;

    /*!
    * Sets the maximum number of response results. If set to <i>0</i>,
    * all possible results will be returned.
    * @param totalResults The maximum number of response results.
    */
    void setTotalResults(quint32 totalResults);
    /*!
    * @return The maximum number of response results.
    */
    quint32 totalResults() const;

    /*!
    * Sets the maximum number of alternative routes the server will try to generate.
    * If set to <i>0</i>, the result will contain no alternative routes
    * (only one result is returned if any.
    * @param nAlternatives The number of alternatives.
    */
    void setAlternatives(quint16 nAlternatives);
    /*!
    * @return The maximum number of alternatives to be returned.
    */
    quint16 alternatives() const;

    /*!
    * The RFC 3066 language code of the response (e.g. en-US or de-DE).
    * @param code The language code.
    */
    void setLanguage(const QString& code);
    /*!
    * @return The RFC 3066 language code of the response.
    */
    QString language() const;

    /*!
    * Sets the planned time of departure.
    * @param tod Time of departure.
    */
    void setDeparture(const QDateTime& tod);
    /*!
    * Gets the planned time of departure.
    * @return Time of departure.
    */
    QDateTime departure() const;

    /*!
    * Sets the planned time of arrival.
    * @param toa Time of arrival.
    */
    void setArrival(const QDateTime& toa);
    /*!
    * Gets the planned time of departure.
    * @return Time of departure.
    */
    QDateTime arrival() const;

    /*!
    * Sets the type of the route the server will calculate.
    * @see RouteType
    * @param type The route type.
    */
    void setType(RouteType type);
    /*!
    * @return The route type.
    */
    RouteType type() const;

    /*!
    * Sets the movement mode of the route.
    * @see RouteMode
    * @param mode The route mode.
    */
    void setMode(RouteMode mode);
    /*!
    * @return The route mode.
    */
    RouteMode mode() const;

    /*!
    * Specifies which route components should not be
    * part of the calculated route (best effort).
    * @see RouteAvoid
    * @param avoid A list containing all routes components to be avoided.
    */
    void setAvoid(QList<RouteAvoid> avoid);
    /*!
    * @return A list containing all routes components to be avoided.
    */
    QList<RouteAvoid> avoid() const;

    /*!
    * Adds a stop over to the requested route.
    * The server calculates a route that contains these waypoints (in the given order)
    * as part of the route. These waypoints must be between the destination and the source.
    * @param stopOver A stop over.
    */
    void addStopOver(const QGeoCoordinateMaps& stopOver);
    /*!
    * Removes a previously added stop.
    * @see addStopOver()
    * @param stopOver A stop over to be removed.
    */
    void removeStopOver(const QGeoCoordinateMaps& stopOver);
    /*!
    * @return All added stop overs.
    */
    const QList<QGeoCoordinateMaps>& stopOvers() const;

private:
    Q_DISABLE_COPY(QRouteRequest)

private:
    QString typeToString() const;
    QString modeToString() const;
    QString avoidToString() const;

    QString vers;
    QGeoCoordinateMaps src;
    QGeoCoordinateMaps dst;
    quint32 nTotal;
    quint16 nAlternatives;
    QString languageCode;
    QDateTime tod;
    QDateTime toa;
    RouteType rType;
    RouteMode rMode;
    QList<RouteAvoid> rAvoid;
    QList<QGeoCoordinateMaps> stpOvers;
};

QTM_END_NAMESPACE

#endif
