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

#include "qgeocodingservice.h"

QTM_BEGIN_NAMESPACE


/* TODO
 - this doc section came from the old qgeoengine.cpp file and needs a new home
 - should update this and merge most of it with location overview
*/

/* \mainpage
* \section sec_brief_overview Brief Overview
* The qlocation package provides wrapper classes for (reverse) geocoding requests/replies
* (QGeocodingRequest, QReverseGeocodingRequest, QGeocodingReply),
* geo routing requests/replies (QGeoRouteRequest, QGeoRouteReply), and map tile requests/replies
* (QGeoMapTileRequest, QMapTileReply). QMapView is a Qt::QGraphicsView based widget that displays
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
* The returned QMapTileReply contains the raw byte array of the Qt::QPixmap of the tile.
*
* \section sec_map_view The QMapView Widget
* The QMapView displays a tile based map. It supports panning and zooming, different map formats,
* and provides overlay layers for QMapObject<i></i>s and QGeoRoute<i></i>s.
* A QMapView is always associated with a QGeoEngine, through which it handles all the required
* map tile management.
*/

/*!
    \class QGeocodingService
    \brief The QGeocodingService class handles all communications to and from a geocoding service.
    \ingroup location

    This is the abstract base class for all classes that handle
    geocoding and reverse geocoding requests/replies to and from a geocoding service.
*/

/*!
    Default constructor.
*/
QGeocodingService::QGeocodingService() {}
/*!
    Virtual destructor.
*/
QGeocodingService::~QGeocodingService() {}

/*!
    \fn QGeocodingReply* QGeocodingService::geocode(const QGeocodingRequest& request)

    Submits a geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeocodingService::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn QGeocodingReply* QGeocodingService::reverseGeocode(const QReverseGeocodingRequest& request)

    Submits a reverse geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeocodingService::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn void QGeocodingService::finished(QGeocodingReply* reply)

    This signal is emitted when a QGeocodingReply is available in
    response to a previously submitted QGeocodingRequest
    or QReverseGeocodingRequest.
*/

/*!
    \fn void QGeocodingService::error(QGeocodingReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QGeocodingReply encountered an error
    in response to a previously submitted QGeocodingReques or QReverseGeocodingRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

#include "moc_qgeocodingservice.cpp"

QTM_END_NAMESPACE
