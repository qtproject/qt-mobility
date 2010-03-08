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

#ifndef QLOCATION_GEOENGINE_H
#define QLOCATION_GEOENGINE_H

#include <QNetworkAccessManager>
#include <QHash>
#include <QNetworkReply>
#include <QNetworkProxy>

#include "qrouterequest.h"
#include "qroutereply.h"
#include "qgeocodingrequest.h"
#include "qreversegeocodingrequest.h"
#include "qgeocodingreply.h"
#include "qmaptilerequest.h"
#include "qmaptilereply.h"
#include "qmaptile.h"

QTM_BEGIN_NAMESPACE

/*! \mainpage
* \section sec_brief_overview Brief Overview
* The qlocation package provides wrapper classes for (reverse) geocoding requests/replies
* (QGeocodingRequest, QReverseGeocodingRequest, QGeocodingReply),
* geo routing requests/replies (QRouteRequest, QRouteReply), and map tile requests/replies
* (QMapTileRequest, QMapTileReply). QMapView is a Qt::QGraphicsView based widget that displays
* a tile based map.
*
* \section sec_geo_engine The QGeoEngine
* All requests and replies are handled via a QGeoEngine. The design of QGeoEngine follows roughly
* the same "look and feel" as Qt:QNetworkAccessManager. Via the QGeoEngine::get() methods,
* asynchronous requests are submitted to the geo engine, and a pointer to the corresponding
* QGeoReply object is returned. Once the reply is avalaible, the QGeoEngine emits a
* QGeoEngine::finished() signal. When an error occurs, a QGeoEngine::error() signal is emitted.
*
* \subsection subsec_geo_network_manager The QGeoNetworkManager
* Currently, QGeoNetworkManager is the only concrete implementation of QGeoEngine. The QGeoNetworkManager
* forwards the geo requests to their corresponding Nokia REST servers and handles all network communication
* that goes on "behind the scenes".

* \section sec_geocoding Geocoding Requests and Replies
* A QGeocodingRequest tries to map an address to its geographical location (longitude/latitude).
* An address can either be specified by a "one box location" description, or by its individual
* country, state, city, post code, street name and street number fields.<br>
* Depending on the address input, the corresponding QGeocodingReply can contain several QGeoLocation<i></i>s, each
* containing a QGeoCoordinateMaps, a QAddress and possibly a QAlternativeAddress.

* \section sec_reverse_geocoding Reverse Geocoding Requests and Replies
* A QReverseGeocodingRequests tries to map a QGeoCoordinateMaps to an address.<br>
* It too returns a QGeocodingReply.

* \section sec_routing Routing Requests and Replies
* A QRouteRequest requests a route from a source QGeoCoordinateMaps to a destination QGeoCoordinateMaps.<br>
* The return QRouteReply can contain several QRoute<i></i>s, each containing numerous QManeuver<i></i>s.
* A QManeuver is defined by a collection of QManeuver::wayPoints() and QManeuver::maneuverPoints().
*
* \section sec_map_tile Map Tile Requests and Replies
* A QMapTileRequest is specified by a column and row index and various format parameters.
* The returned QMapTileReply contains the raw byte array of the Qt::QPixmap of the tile.
*
* \section sec_map_view The QMapView Widget
* The QMapView displays a tile based map. It supports panning and zooming, different map formats,
* and provides overlay layers for QMapObject<i></i>s and QRoute<i></i>s.
* A QMapView is always associated with a QGeoEngine, through which it handles all the required
* map tile management.
*/

/*!
* This is the abstract base class for all classes that handle
* (reverse) geocoding requests/replies, map tile requests/replies,
* and routing requests/replies. Currently, the only class that
* extends QGeoEngine is \ref QGeoNetworkManager, that handles
* all communication with the REST servers. In the future, it is
* conceivable that another class could be implemented that handles
* all that functionality locally. That class would simply have to
* adhere to the interface defined here and could then be easily
* swapped into place for QGeoNetworkManager.
*/
class Q_LOCATION_EXPORT QGeoEngine : public QObject
{
    Q_OBJECT

public:
    /*!
    * Default constructor.
    */
    QGeoEngine() {}
    /*!
    * Virtual destructor.
    */
    virtual ~QGeoEngine() {};

    /*!
    * Submits a route request.
    * @param request The route request.
    * @return The corresponding route reply.
    * @note Due to the potentially asynchronous nature of
    *       requests, you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QRouteReply*) signal.
    */
    virtual QRouteReply* get(const QRouteRequest& request) = 0;
    /*!
    * Submits a geocoding request.
    * @param request The geocoding request.
    * @return The corresponding geocoding repy.
    * @note Due to the potentially asynchronous nature of
    *       requests, you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QGeocodingReply*) signal.
    */
    virtual QGeocodingReply* get(const QGeocodingRequest& request) = 0;
    /*!
    * Submits a reverse geocoding request.
    * @param request The route request.
    * @return The corresponding geocoding reply.
    * @note Due to the potentially asynchronous nature of
    *       requests, you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QGeocodingReply*) signal.
    */
    virtual QGeocodingReply* get(const QReverseGeocodingRequest& request) = 0;
    /*!
    * Submits a map tile request.
    * @param request The map tile request.
    * @return The corresponding map tile reply.
    * @note Due to the potentially asynchronous nature of
    *       requests, you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QMapTileReply*) signal.
    */
    virtual QMapTileReply* get(const QMapTileRequest& request) = 0;

    /*!
    * When a \ref QGeoReply is no longer needed, it should never be manually
    * deleted. Instead this method should be used to properly remove the reply
    * from corresponding internal maps.
    * @param reply The reply to be released.
    */
    virtual void release(QGeoReply* reply) = 0;
    /*!
    * Cancels a (pending) reply.
    * @param reply The reply to abort.
    */
    virtual void cancel(QGeoReply* reply) = 0;

    /*!
    * @return The maximum zoom level supported by this geo engine.
    */
    virtual quint16 maxZoomLevel() const = 0;
    /*!
    * @return A list of all available \ref MapVersion<i>s</i>.
    */
    virtual QList<MapVersion> versions() const = 0;
    /*!
    * @return A list of all available \ref MapResolution<i>s</i>.
    */
    virtual QList<MapResolution> resolutions() const = 0;
    /*!
    * @return A list of all available \ref MapFormat<i>s</i>.
    */
    virtual QList<MapFormat> formats() const = 0;
    /*!
    * @return A list of all available \ref MapScheme<i>s</i>.
    */
    virtual QList<MapScheme> schemes() const = 0;

private:
    Q_DISABLE_COPY(QGeoEngine)

signals:
    /*!
    * This signal is emitted when a \ref QRouteReply is available in
    * response to a previously submitted \ref QRouteRequest.
    */
    void finished(QRouteReply* reply);
    /*!
    * This signal is emitted when a \ref QGeocodingReply is available in
    * response to a previously submitted \ref QGeocodingRequest
    * or \ref QReverseGeocodingRequest.
    */
    void finished(QGeocodingReply* reply);
    /*!
    * This signal is emitted when a \ref QMapTileReply is available in
    * response to a previously submitted \ref QMapTileRequest.
    */
    void finished(QMapTileReply* reply);

    /*!
    * This signal is emitted when a \ref QRouteReply encountered an error
    * in response to a previously submitted \ref QRouteRequest.
    * @note For \ref QGeoNetworkManager, the error code will be identical
    *       to Qt::QNetworkReply::NetworkError.
    */
    void error(QRouteReply* reply, QGeoReply::ErrorCode code);
    /*!
    * This signal is emitted when a \ref QGeocodingReply encountered an error
    * in response to a previously submitted \ref QGeocodingRequest
    * or \ref QReverseGeocodingRequest.
    * @note For \ref QGeoNetworkManager, the error code will be identical
    *       to Qt::QNetworkReply::NetworkError.
    */
    void error(QGeocodingReply* reply, QGeoReply::ErrorCode code);
    /*!
    * This signal is emitted when a \ref QMapTileReply encountered an error
    * in response to a previously submitted \ref QMapTileRequest.
    * @note For \ref QGeoNetworkManager, the error code will be identical
    *       to Qt::QNetworkReply::NetworkError.
    */
    void error(QMapTileReply* reply, QGeoReply::ErrorCode code);

public:
    /*!
    * The map tile server uses a Mercator projection to map geo coordinates into a 2D map.
    * For each zoom level, that map is then split into 2^zoomLevel x 2^zoomLevel map tiles.
    * This method calculates the respective col and row index of the map tile that the
    * given geo coordinate lies on.
    * @note This does not mean that the coordinate lies in the center of the calculated tile.
    * @param coordinate The geo coordinate for which the corresponding tile index is to be determined.
    * @param zoomLevel The zoom level for which the tile index is to be determined.
    * @param col A pointer to the buffer that will take the calculated col index.
    * @param row A pointer to the buffer that will take the calculated row index.
    */
    static void getMercatorTileIndex(const QGeoCoordinateMaps& coordinate, quint16 zoomLevel, quint32* col, quint32* row);
};

QTM_END_NAMESPACE

#endif
