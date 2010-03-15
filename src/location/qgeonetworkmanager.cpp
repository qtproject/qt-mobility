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


#include <math.h>

#include <QStringBuilder>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "qgeonetworkmanager.h"
#include "qroutereply.h"
#include "qroutexmlparser.h"
#include "qgeocodingreply.h"
#include "qgeocodingxmlparser.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoNetworkManager
    \brief The class QGeoNetworkManager is a subclass of QGeoEngine for use with Nokia maps.
    \ingroup location

    This class extends QGeoEngine.
    It handles all communication with the REST servers.
    Internally it uses QNetworkAccessManager for network
    communication, and it handles all mapping between
    raw network requests/replies and geo requests/replies.
*/

static const int DEGREE_DECIMALS = 10;

/*!
    Constructs a QGeoNetworkManager given an authentification \a token used for server communication 
    and a \a referrer (usually a URL) for the token.
*/
QGeoNetworkManager::QGeoNetworkManager(QString token, QString referrer)
        : netManager(this),
        geocdProx(QNetworkProxy::NoProxy),
        rtProx(QNetworkProxy::NoProxy),
        mapProx(QNetworkProxy::NoProxy),
        token(token),
        referrer(referrer)
{
    connect(&netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(netReplyFinished(QNetworkReply*)));

    geocdSrv = "dev-a7.bln.gate5.de";
    mapSrv = "dev-a7.bln.gate5.de";
    //mapSrv = "maptile.mapplayer.maps.svc.ovi.com";
    rtSrv = "172.24.32.155";
}

/*!
    Destructor.
*/
QGeoNetworkManager::~QGeoNetworkManager()
{
    QList<QNetworkReply*> keys = replyMap.keys();

    while (keys.length() > 0) {
        QNetworkReply* key = keys[0];
        QGeoReply* locReply = replyMap[key];
        replyMap.remove(key);
        delete locReply;
        keys.pop_front();
        delete key;
    }
}

/*!
    Sets the server to be used for (reverse) geocoding requests \a server.
*/
void QGeoNetworkManager::setGeocodingServer(const QString& server)
{
    geocdSrv = server;
}
/*!
    Return the server to be used for (reverse) geocoding requests.
*/
QString QGeoNetworkManager::geocodingServer() const {
    return geocdSrv;
}
/*!
    Returns the proxy to be used for (reverse) geocoding requests.
*/
QNetworkProxy QGeoNetworkManager::geocodingProxy() const {
    return geocdProx;
}
/*!
    Sets the proxy to be used for (reverse) geocoding requests to \a proxy.
*/
void QGeoNetworkManager::setGeocodingProxy(const QNetworkProxy& proxy)
{
    geocdProx = proxy;
}

/*!
    Sets the server to be used for route requests to \a server.
*/
void QGeoNetworkManager::setRouteServer(const QString& server)
{
    rtSrv = server;
}
/*!
    Returns the server to be used for route requests.
*/
QString QGeoNetworkManager::routeServer() const {
    return rtSrv;
}
/*!
    Returns the proxy to be used for route requests.
*/
QNetworkProxy QGeoNetworkManager::routeProxy() const {
    return rtProx;
}
/*!
    Sets the proxy to be used for route requests to \a proxy.
*/
void QGeoNetworkManager::setRouteProxy(const QNetworkProxy& proxy)
{
    rtProx = proxy;
}

/*!
    Sets the server to be used for map tile requests to \a server.
*/
void QGeoNetworkManager::setMapServer(const QString& server)
{
    mapSrv = server;
}
/*!
    Returns the server to be used for map tile requests.
*/
QString QGeoNetworkManager::mapServer() const 
{
    return mapSrv;
}
/*!
    Returns the proxy to be used for map tile requests.
*/
QNetworkProxy QGeoNetworkManager::mapProxy() const
{
    return mapProx;
}
/*!
    Sets the proxy to be used for map tile requests to \a proxy.
*/
void QGeoNetworkManager::setMapProxy(const QNetworkProxy& proxy)
{
    mapProx = proxy;
}

/*!
    Submits a route request \a request and returns the corresponding QRouteReply.
    \note Due to the asynchronous nature of requests,
          you should never directly start working with the returned
          reply, but instead wait for the finished(QRouteReply*) signal.
*/
QRouteReply* QGeoNetworkManager::get(const QRouteRequest& request)
{
    QString rawRequest = "http://" % rtSrv % "/routing/rt/" % request.version() %
                         "?referer=localhost" %
                         "&slong=" % trimGeoCoordinate(request.src.longitude()) %
                         "&slat=" % trimGeoCoordinate(request.src.latitude()) %
                         "&dlong=" % trimGeoCoordinate(request.dst.longitude()) %
                         "&dlat=" % trimGeoCoordinate(request.dst.latitude());

    if (request.nTotal > 0)
        rawRequest += "&total=" % QString::number(request.nTotal);
    if (request.nAlternatives > 0)
        rawRequest += "&alternatives=" % QString::number(request.nAlternatives);
    if (request.languageCode != "")
        rawRequest += "&lg=" % request.languageCode;
    if (request.tod.isValid())
        rawRequest += "&tod=" % request.tod.toUTC().toString();
    if (request.toa.isValid())
        rawRequest += "&toa=" % request.toa.toUTC().toString();

    rawRequest += "&type=" % request.typeToString() %
                  "&mode=" % request.modeToString();

    if (request.rAvoid.count() > 0)
        rawRequest += "&avoid=" % request.avoidToString();

    const QList<QGeoCoordinate>& stopOvers = request.stopOvers();

    for (int i = 0; i < stopOvers.length(); i++) {
        rawRequest += QString::number(stopOvers[i].latitude(), 'f') % "," %
                      QString::number(stopOvers[i].longitude(), 'f') % " ";
    }

    netManager.setProxy(rtProx);
    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* netReply = netManager.get(*netRequest);
    QRouteReply* routeReply = new QRouteReply();
    replyMap.insert(netReply, routeReply);
    revReplyMap.insert(routeReply, netReply);

    return routeReply;
}

/*!
    Submits a geocoding request \a request and returns the corresponding QGeocodingReply.
    \note Due to the asynchronous nature of requests,
          you should never directly start working with the returned
          reply, but instead wait for the finished(QGeocodingReply*) signal.
*/
QGeocodingReply* QGeoNetworkManager::get(const QGeocodingRequest& request)
{
    QString rawRequest = "http://" % geocdSrv % "/geocoder/gc/" % request.version() %
                         "?referer=localhost";

    if (request.languageMARC != "")
        rawRequest += "&lg=" % request.languageMARC;

    if (request.obloc.isEmpty()) {
        rawRequest += "&country=" % request.cntry;

        if (!request.st.isEmpty())
            rawRequest += "&state=" % request.st;
        if (!request.cty.isEmpty())
            rawRequest += "&city=" % request.cty;
        if (!request.pCode.isEmpty())
            rawRequest += "&zip=" % request.pCode;
        if (!request.strt.isEmpty())
            rawRequest += "&street=" % request.strt;
        if (!request.num.isEmpty())
            rawRequest += "&number=" % request.num;
    } else {
        rawRequest += "&obloc=" % request.obloc;
    }

    netManager.setProxy(geocdProx);
    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = netManager.get(*netRequest);
    QGeocodingReply* codingReply = new QGeocodingReply();
    replyMap.insert(reply, codingReply);
    revReplyMap.insert(codingReply, reply);
    return codingReply;
}

/*!
    Submits a reverse geocoding request \a request and return the corresponding QGeocodingReply.
    \note Due to the asynchronous nature of requests,
          you should never directly start working with the returned
          reply, but instead wait for the finished(QGeocodingReply*) signal.
*/
QGeocodingReply* QGeoNetworkManager::get(const QReverseGeocodingRequest& request)
{
    QString rawRequest = "http://" % geocdSrv % "/geocoder/rgc/" % request.version() %
                         "?referer=localhost" %
                         "&long=" % trimGeoCoordinate(request.coord.longitude()) %
                         "&lat=" % trimGeoCoordinate(request.coord.latitude());

    if (request.languageMARC != "")
        rawRequest += "&lg=" % request.languageMARC;

    netManager.setProxy(geocdProx);
    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = netManager.get(*netRequest);
    QGeocodingReply* codingReply = new QGeocodingReply();
    replyMap.insert(reply, codingReply);
    revReplyMap.insert(codingReply, reply);
    return codingReply;
}

/*!
    Submits a map tile request \a request and returns the corresponding QMapTileReply.

    \note Due to the potentially asynchronous nature of requests,
          you should never directly start working with the returned
          reply, but instead wait for the finished(QMapTileReply*) signal.
    \note If the request can be served from the tile cache,
          this method will directly emit finished(QRouteReply*).
          If you need the returned QMapTileReply object first,
          you should connect to finished(QRouteReply*) with Qt::QueuedConnection.
*/
QMapTileReply* QGeoNetworkManager::get(const QMapTileRequest& request)
{
    //check cache first
    QMapTileReply* tileReply = NULL;

    if ((tileReply = cache.get(request))) {
        tileReply->fin = true;
        emit finished(tileReply);
        emit tileReply->finished();
    } else {
        QString rawRequest = "http://" % mapSrv % "/maptiler/maptile/" %
                             request.version().id % '/' %
                             request.scheme().id % '/' %
                             QString::number(request.zoomLevel()) % '/' %
                             QString::number(request.col()) % '/' %
                             QString::number(request.row()) % '/' %
                             request.resolution().id % '/' %
                             request.format().id;

        if (!token.isEmpty()) {
            rawRequest = rawRequest % "?token=" % token;

            if (!referrer.isEmpty())
                rawRequest = rawRequest % "&referrer=" % referrer;
        } else if (!referrer.isEmpty())
            rawRequest = rawRequest % "?referrer=" % referrer;

        netManager.setProxy(mapProx);
        QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
        QNetworkReply* reply = netManager.get(*netRequest);
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                         this, SLOT(netReplyError(QNetworkReply::NetworkError)));
        tileReply = new QMapTileReply(request);
        replyMap.insert(reply, tileReply);
        revReplyMap.insert(tileReply, reply);
    }

    return tileReply;
}

/*!
    This slot is invoked when a network reply \a reply has finished.
*/
void QGeoNetworkManager::netReplyFinished(QNetworkReply* reply)
{
    //get associated geo reply
    if (!replyMap.contains(reply)) {
        //no associated geo reply found, bail out
        reply->deleteLater();
        return;
    }

    QGeoReply* rawReply = replyMap[reply];
    QString className = rawReply->metaObject()->className();

    if (className == "QRouteReply") {
        QRouteReply* routeReply = static_cast<QRouteReply*>(rawReply);
        routeReply->fin = true;

        if (parseRouteReply(reply, routeReply)) {
            if (routeReply->rCode == QRouteReply::Failed) {
                emit error(routeReply, (QGeoReply::ErrorCode) QNetworkReply::ContentNotFoundError);
                emit routeReply->error((QGeoReply::ErrorCode) QNetworkReply::ContentNotFoundError);
            } else {
                emit finished(routeReply);
                emit routeReply->finished();
            }
        } else {
            routeReply->rCode = QRouteReply::Failed;
            emit error(routeReply, (QGeoReply::ErrorCode) QNetworkReply::UnknownContentError);
            emit routeReply->error((QGeoReply::ErrorCode) QNetworkReply::UnknownContentError);
        }

    } else if (className == "QGeocodingReply") {
        QGeocodingReply* geoReply = static_cast<QGeocodingReply*>(rawReply);
        geoReply->fin = true;

        if (parseCodingReply(reply, geoReply)) {
            if (geoReply->code == QGeocodingReply::Failed) {
                emit error(geoReply, (QGeoReply::ErrorCode) QNetworkReply::ContentNotFoundError);
                emit geoReply->error((QGeoReply::ErrorCode) QNetworkReply::ContentNotFoundError);
            } else {
                emit finished(geoReply);
                emit geoReply->finished();
            }
        } else {
            geoReply->code = QGeocodingReply::Failed;
            emit error(geoReply, (QGeoReply::ErrorCode) QNetworkReply::UnknownContentError);
            emit geoReply->error((QGeoReply::ErrorCode) QNetworkReply::UnknownContentError);
        }
    } else if (className == "QMapTileReply") {
        QMapTileReply* tileReply = static_cast<QMapTileReply*>(rawReply);
        tileReply->fin = true;
        tileReply->rawData() = reply->readAll();

        if (tileReply->data.length() > 0)
            cache.cache(*tileReply);

        emit finished(tileReply);
        emit tileReply->finished();
    }
}

/*!
    This slot is invoked when a network reply has detected an error with error code \a code.
*/
void QGeoNetworkManager::netReplyError(QNetworkReply::NetworkError code)
{
    QObject* sgnSender = sender();
    QString senderType = sgnSender->metaObject()->className();

    if (senderType == "QtMobility::QMapTileReply") {
        QMapTileReply* tileReply = static_cast<QMapTileReply*>(sgnSender);
        tileReply->fin = true;

        emit error(tileReply, (QGeoReply::ErrorCode) code);
        emit tileReply->error((QGeoReply::ErrorCode) code);
    } else if (senderType == "QtMobility::QRouteReply") {
        QRouteReply* routeReply = static_cast<QRouteReply*>(sgnSender);
        routeReply->fin = true;
        routeReply->rCode = QRouteReply::Failed;

        emit error(routeReply, (QGeoReply::ErrorCode) code);
        emit routeReply->error((QGeoReply::ErrorCode) code);
    } else if (senderType == "QtMobility::QGeocodingReply") {
        QGeocodingReply* geoReply = static_cast<QGeocodingReply*>(sgnSender);
        geoReply->fin = true;
        geoReply->code = QGeocodingReply::Failed;

        emit error(geoReply, (QGeoReply::ErrorCode) code);
        emit geoReply->error((QGeoReply::ErrorCode) code);
    }

}

/*!
    When a \a reply is no longer needed, it should never be manually
    deleted. Instead this method should be used to properly remove the reply
    from corresponding internal maps.
*/
void QGeoNetworkManager::release(QGeoReply* reply)
{
    if (revReplyMap.contains(reply)) {
        QNetworkReply* netReply = revReplyMap[reply];
        revReplyMap.remove(reply);
        replyMap.remove(netReply);
        netReply->deleteLater();
    }

    reply->deleteLater();
}

/*!
    This methods aborts the (pending) reply \a reply. 

    It essentially calls abort() on the
    * associated QNetworkReply object.
*/
void QGeoNetworkManager::cancel(QGeoReply* reply)
{
    if (revReplyMap.contains(reply)) {
        revReplyMap[reply]->abort();
    }
}

/*!
    Returns the maximum zoom level supported.
*/
quint16 QGeoNetworkManager::maxZoomLevel() const {
    return MAPTILE_MAX_ZOOMLEVEL;
}
/*!
    Returns a list of all available \l {MapVersion}{MapVersions}.
*/
QList<MapVersion> QGeoNetworkManager::versions() const {
    return mapVersions.values();
}
/*!
    Returns a list of all available \l {MapResolution}{MapResolutions}.
*/
QList<MapResolution> QGeoNetworkManager::resolutions() const {
    return mapResolutions.values();
}
/*!
    Returns a list of all available \l {MapFormat}{MapFormats}.
*/
QList<MapFormat> QGeoNetworkManager::formats() const {
    return mapFormats.values();
}
/*!
    Returns a list of all available \l {MapScheme}{MapSchemes}.
*/
QList<MapScheme> QGeoNetworkManager::schemes() const {
    return mapSchemes.values();
}

/*!
    Parses \a netReply with a QRouteXmlHandler to populate \a routeReply.
*/
bool QGeoNetworkManager::parseRouteReply(QNetworkReply* netReply, QRouteReply* routeReply)
{
    QRouteXmlParser parser;
    return parser.parse(netReply, routeReply);
}

/*!
    * Parses \a netReply with a QGeocodingXmlHandler to populate \a codingReply.
*/
bool QGeoNetworkManager::parseCodingReply(QNetworkReply* netReply, QGeocodingReply* codingReply)
{
    QGeocodingXmlParser parser;
    return parser.parse(netReply, codingReply);
}

/*!
    Returns a string representation of degree value with at most DEGREE_DECIMALS decimals.
*/
QString QGeoNetworkManager::trimGeoCoordinate(qreal degree)
{
    QString sDegree = QString::number(degree, 'f', DEGREE_DECIMALS);
    int index;
    int decSepIndex = sDegree.indexOf('.');

    if (decSepIndex < 0)
        return sDegree;

    for (index = sDegree.length() - 1; index >= decSepIndex; index--) {
        if (sDegree[index] != '0')
            break;
    }

    if (sDegree[index] == '.')
        index--;

    return sDegree.left(index + 1);
}

#include "moc_qgeonetworkmanager.cpp"

QTM_END_NAMESPACE

