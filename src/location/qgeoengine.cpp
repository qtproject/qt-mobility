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

#include "qgeoengine.h"

#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

// TODO: merge with location overview
/* \mainpage
* \section sec_brief_overview Brief Overview
* The qlocation package provides wrapper classes for (reverse) geocoding requests/replies
* (QGeocodingRequest, QReverseGeocodingRequest, QGeocodingReply),
* geo routing requests/replies (QGeoRouteRequest, QGeoRouteReply), and map tile requests/replies
* (QGeoMapTileRequest, QGeoMapTileReply). QMapView is a Qt::QGraphicsView based widget that displays
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
* containing a QGeoCoordinate and a QGeoAddress.

* \section sec_reverse_geocoding Reverse Geocoding Requests and Replies
* A QReverseGeocodingRequests tries to map a QGeoCoordinate to an address.<br>
* It too returns a QGeocodingReply.

* \section sec_routing Routing Requests and Replies
* A QGeoRouteRequest requests a route from a source QGeoCoordinate to a destination QGeoCoordinate.<br>
* The return QGeoRouteReply can contain several QGeoRoute<i></i>s, each containing numerous QManeuver<i></i>s.
* A QManeuver is defined by a collection of QManeuver::wayPoints() and QManeuver::maneuverPoints().
*
* \section sec_map_tile Map Tile Requests and Replies
* A QGeoMapTileRequest is specified by a column and row index and various format parameters.
* The returned QGeoMapTileReply contains the raw byte array of the Qt::QPixmap of the tile.
*
* \section sec_map_view The QMapView Widget
* The QMapView displays a tile based map. It supports panning and zooming, different map formats,
* and provides overlay layers for QMapObject<i></i>s and QGeoRoute<i></i>s.
* A QMapView is always associated with a QGeoEngine, through which it handles all the required
* map tile management.
*/

/*!
    \class QGeoEngine
    \brief The QGeoEngine class is the base class for all maps and navigations requests and replies.
    \ingroup location

    This is the abstract base class for all classes that handle
    (reverse) geocoding requests/replies, map tile requests/replies,
    and routing requests/replies. Currently, the only class that
    extends QGeoEngine is QGeoNetworkManager, that handles
    all communication with the REST servers. In the future, it is
    conceivable that another class could be implemented that handles
    all that functionality locally. That class would simply have to
    adhere to the interface defined here and could then be easily
    swapped into place for QGeoNetworkManager.
*/

/*!
    Default constructor.
*/
QGeoEngine::QGeoEngine() {}
/*!
    Virtual destructor.
*/
QGeoEngine::~QGeoEngine() {}

/*!
    \fn QGeoRouteReply* QGeoEngine::get(const QGeoRouteRequest& request)

    Submits the route request \a request and returns the corresponding QGeoRouteReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeoRouteReply*) signal from this object or the
    QGeoRouteReply::finished() signal from the returned QGeoRouteReply object before
    working with the reply.
*/

/*!
    \fn QGeocodingReply* QGeoEngine::get(const QGeocodingRequest& request)

    Submits a geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn QGeocodingReply* QGeoEngine::get(const QReverseGeocodingRequest& request)

    Submits a reverse geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn QGeoMapTileReply* QGeoEngine::get(const QGeoMapTileRequest& request)

    Submits a map tile request \a request and returns the corresponding QGeoMapTileReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeoMapTileReply*) signal from this object or the
    QGeoMapTileReply::finished() signal from the returned QGeoMapTileReply object before
    working with the reply.
*/

/*!
    \fn quint16 QGeoEngine::maxZoomLevel() const

    Returns the maximum zoom level supported by this geo engine.
*/

/*!
    \fn QList<MapVersion> QGeoEngine::versions() const

    Returns a list of all available \l {MapVersion} {MapVersions}.
*/

/*!
    \fn QList<MapResolution> QGeoEngine::resolutions() const

    Returns a list of all available \l {MapResolution} {MapResolutions}.
*/

/*!
    \fn QList<MapFormat> QGeoEngine::formats() const

    Returns a list of all available \l {MapFormat} {MapFormats}.
*/

/*!
    \fn QList<MapScheme> QGeoEngine::schemes() const

    Returns a list of all available \l {MapScheme} {MapSchemes}.
*/

/*!
    \fn void QGeoEngine::finished(QGeoRouteReply* reply)

    This signal is emitted when a QGeoRouteReply is available in
    response to a previously submitted QGeoRouteRequest.
*/

/*!
    \fn void QGeoEngine::finished(QGeocodingReply* reply)

    This signal is emitted when a QGeocodingReply is available in
    response to a previously submitted QGeocodingRequest
    or QReverseGeocodingRequest.
*/

/*!
    \fn void QGeoEngine::finished(QGeoMapTileReply* reply)

    This signal is emitted when a QGeoMapTileReply is available in
    response to a previously submitted QGeoMapTileRequest.
*/

/*!
    \fn void QGeoEngine::error(QGeoRouteReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QGeoRouteReply encountered an error
    in response to a previously submitted QGeoRouteRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

/*!
    \fn void QGeoEngine::error(QGeocodingReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QGeocodingReply encountered an error
    in response to a previously submitted QGeocodingReques or QReverseGeocodingRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

/*!
    \fn void QGeoEngine::error(QGeoMapTileReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QGeoMapTileReply encountered an error
    in response to a previously submitted QGeoMapTileRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

/*!
    The map tile server uses a Mercator projection to map geo coordinates into a 2D map.
    For each zoom level, that map is then split into 2^zoomLevel x 2^zoomLevel map tiles.

    For the given \a zoomLevel, this function  determines the row and column indices of the
    map tile from the 2D map which contains the geo coordinate \a coordinate and stores
    the indices in \a row and \a col respectively.

    \note This does not mean that the coordinate lies in the center of the calculated tile.
*/
void QGeoEngine::getMercatorTileIndex(const QGeoCoordinate& coordinate, quint16 zoomLevel, quint32* col, quint32* row)
{
    qreal p = pow((double) 2, zoomLevel);

    double x = coordinate.longitude() / 360 + 0.5;
    double y = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * coordinate.latitude() / 180)) / PI) / 2;

    if (y < 0)
        y = 0;

    if (y > 1)
        y = 1;

    *col = (quint32)(x * p);
    *row = (quint32)(y * p);
}

#include "moc_qgeoengine.cpp"

QTM_END_NAMESPACE
