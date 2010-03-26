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
#include "qgeonetworkmanager_p.h"
#include "qgeorouterequest.h"
#include "qgeorouterequest_p.h"
#include "qgeoroutereply.h"
#include "qgeocodingreply.h"

#include "qgeoroutereply_nokia_p.h"
#include "qgeocodingreply_nokia_p.h"
#include "qgeomaptilereply_nokia_p.h"

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

/*!
    Constructs a QGeoNetworkManager given an authentification \a token used for server communication 
    and a \a referrer (usually a URL) for the token.
*/
QGeoNetworkManager::QGeoNetworkManager(QString token, QString referrer)
        : d_ptr(new QGeoNetworkManagerPrivate(this))
{
    Q_D(QGeoNetworkManager);
    d->token = token;
    d->referrer = referrer;
}

/*!
    Destructor.
*/
QGeoNetworkManager::~QGeoNetworkManager()
{
}

/*!
    Sets the server to be used for (reverse) geocoding requests \a server.
*/
void QGeoNetworkManager::setGeocodingServer(const QString& server)
{
    Q_D(QGeoNetworkManager);
    d->geocdSrv = server;
}
/*!
    Return the server to be used for (reverse) geocoding requests.
*/
QString QGeoNetworkManager::geocodingServer() const
{
    Q_D(const QGeoNetworkManager);
    return d->geocdSrv;
}
/*!
    Returns the proxy to be used for (reverse) geocoding requests.
*/
QNetworkProxy QGeoNetworkManager::geocodingProxy() const
{
    Q_D(const QGeoNetworkManager);
    return d->geocdProx;
}
/*!
    Sets the proxy to be used for (reverse) geocoding requests to \a proxy.
*/
void QGeoNetworkManager::setGeocodingProxy(const QNetworkProxy& proxy)
{
    Q_D(QGeoNetworkManager);
    d->geocdProx = proxy;
}

/*!
    Sets the server to be used for route requests to \a server.
*/
void QGeoNetworkManager::setRouteServer(const QString& server)
{
    Q_D(QGeoNetworkManager);
    d->rtSrv = server;
}
/*!
    Returns the server to be used for route requests.
*/
QString QGeoNetworkManager::routeServer() const
{
    Q_D(const QGeoNetworkManager);
    return d->rtSrv;
}
/*!
    Returns the proxy to be used for route requests.
*/
QNetworkProxy QGeoNetworkManager::routeProxy() const
{
    Q_D(const QGeoNetworkManager);
    return d->rtProx;
}
/*!
    Sets the proxy to be used for route requests to \a proxy.
*/
void QGeoNetworkManager::setRouteProxy(const QNetworkProxy& proxy)
{
    Q_D(QGeoNetworkManager);
    d->rtProx = proxy;
}

/*!
    Sets the server to be used for map tile requests to \a server.
*/
void QGeoNetworkManager::setMapServer(const QString& server)
{
    Q_D(QGeoNetworkManager);
    d->mapSrv = server;
}
/*!
    Returns the server to be used for map tile requests.
*/
QString QGeoNetworkManager::mapServer() const 
{
    Q_D(const QGeoNetworkManager);
    return d->mapSrv;
}
/*!
    Returns the proxy to be used for map tile requests.
*/
QNetworkProxy QGeoNetworkManager::mapProxy() const
{
    Q_D(const QGeoNetworkManager);
    return d->mapProx;
}
/*!
    Sets the proxy to be used for map tile requests to \a proxy.
*/
void QGeoNetworkManager::setMapProxy(const QNetworkProxy& proxy)
{
    Q_D(QGeoNetworkManager);
    d->mapProx = proxy;
}

/*!
    Submits a route request \a request and returns the corresponding QGeoRouteReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeoRouteReply*) signal from this object or the
    QGeoRouteReply::finished() signal from the returned QGeoRouteReply object before
    working with the reply.
*/
QGeoRouteReply* QGeoNetworkManager::get(const QGeoRouteRequest& request)
{
    Q_D(QGeoNetworkManager);
    return d->get(request);
}

/*!
    Submits a geocoding request \a request and returns the corresponding QGeocodingReply.

     \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/
QGeocodingReply* QGeoNetworkManager::get(const QGeocodingRequest& request)
{
    Q_D(QGeoNetworkManager);
    return d->get(request);
}

/*!
    Submits a reverse geocoding request \a request and return the corresponding QGeocodingReply.

     \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/
QGeocodingReply* QGeoNetworkManager::get(const QReverseGeocodingRequest& request)
{
    Q_D(QGeoNetworkManager);
    return d->get(request);
}

/*!
    Submits a map tile request \a request and returns the corresponding QGeoMapTileReply.

     \note Due to the asynchronous nature of requests, you should wait for the
    QGeoEngine::finished(QGeoMapTileReply*) signal from this object or the
    QGeoMapTileReply::finished() signal from the returned QGeoMapTileReply object before
    working with the reply.

    \note If the request can be served from the tile cache,
          this method will directly emit finished(QGeoRouteReply*).
          If you need the returned QGeoMapTileReply object first,
          you should connect to finished(QGeoRouteReply*) with Qt::QueuedConnection.
*/
QGeoMapTileReply* QGeoNetworkManager::get(const QGeoMapTileRequest& request)
{
    Q_D(QGeoNetworkManager);
    return d->get(request);
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
    Q_D(const QGeoNetworkManager);
    return d->mapVersions.values();
}
/*!
    Returns a list of all available \l {MapResolution}{MapResolutions}.
*/
QList<MapResolution> QGeoNetworkManager::resolutions() const {
    Q_D(const QGeoNetworkManager);
    return d->mapResolutions.values();
}
/*!
    Returns a list of all available \l {MapFormat}{MapFormats}.
*/
QList<MapFormat> QGeoNetworkManager::formats() const {
    Q_D(const QGeoNetworkManager);
    return d->mapFormats.values();
}
/*!
    Returns a list of all available \l {MapScheme}{MapSchemes}.
*/
QList<MapScheme> QGeoNetworkManager::schemes() const {
    Q_D(const QGeoNetworkManager);
    return d->mapSchemes.values();
}

/*****************************************************************************
 *****************************************************************************/

QGeoNetworkManagerPrivate::QGeoNetworkManagerPrivate(QGeoNetworkManager *parent) :
    QObject(parent),
    q_ptr(parent),
    netManager(),
    geocdProx(QNetworkProxy::NoProxy),
    rtProx(QNetworkProxy::NoProxy),
    mapProx(QNetworkProxy::NoProxy)
{
    geocdSrv = "dev-a7.bln.gate5.de";
    mapSrv = "dev-a7.bln.gate5.de";
    //mapSrv = "maptile.mapplayer.maps.svc.ovi.com";
    rtSrv = "172.24.32.155";
}

QGeoNetworkManagerPrivate::~QGeoNetworkManagerPrivate()
{
}

QGeoRouteReply* QGeoNetworkManagerPrivate::get(const QGeoRouteRequest& request)
{
    QString rawRequest = request.requestString(rtSrv);
    netManager.setProxy(rtProx);

    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* netReply = netManager.get(*netRequest);

    QGeoRouteReply* routeReply = new QGeoRouteReplyNokia(netReply);

    connect(routeReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedRouteRequest()));
    connect(routeReply,
            SIGNAL(error(QGeoRouteReply::ErrorCode, QString)),
            this,
            SLOT(errorRouteRequest(QGeoRouteReply::ErrorCode, QString)));

    return routeReply;
}

QGeocodingReply* QGeoNetworkManagerPrivate::get(const QGeocodingRequest& request)
{
    QString rawRequest = request.requestString(geocdSrv);
    netManager.setProxy(geocdProx);

    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = netManager.get(*netRequest);

    QGeocodingReply* codingReply = new QGeocodingReplyNokia(reply);

    connect(codingReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedGeocodingRequest()));
    connect(codingReply,
            SIGNAL(error(QGeocodingReply::ErrorCode, QString)),
            this,
            SLOT(errorGeocodingRequest(QGeocodingReply::ErrorCode, QString)));

    return codingReply;
}

QGeocodingReply* QGeoNetworkManagerPrivate::get(const QReverseGeocodingRequest& request)
{
    QString rawRequest = request.requestString(geocdSrv);
    netManager.setProxy(geocdProx);

    QNetworkRequest* netRequest = new QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = netManager.get(*netRequest);

    QGeocodingReply* codingReply = new QGeocodingReplyNokia(reply);

    connect(codingReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedGeocodingRequest()));
    connect(codingReply,
            SIGNAL(error(QGeocodingReply::ErrorCode, QString)),
            this,
            SLOT(errorGeocodingRequest(QGeocodingReply::ErrorCode, QString)));

    return codingReply;
}

QGeoMapTileReply* QGeoNetworkManagerPrivate::get(const QGeoMapTileRequest& request)
{
    //check cache first
    QGeoMapTileReply* tileReply = NULL;

    if ((tileReply = cache.get(request))) {
        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedMapTileRequest()));
        connect(tileReply,
                SIGNAL(error(QGeoMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorMapTileRequest(QGeoMapTileReply::ErrorCode, QString)));
        tileReply->done();
    } else {
        QString rawRequest = request.requestString(mapSrv, token, referrer);

        netManager.setProxy(mapProx);
        QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
        QNetworkReply* reply = netManager.get(netRequest);

        tileReply = new QGeoMapTileReplyNokia(request, reply);

        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedMapTileRequest()));
        connect(tileReply,
                SIGNAL(error(QGeoMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorMapTileRequest(QGeoMapTileReply::ErrorCode, QString)));
    }

    return tileReply;
}


void QGeoNetworkManagerPrivate::finishedRouteRequest()
{
    Q_Q(QGeoNetworkManager);
    emit q->finished(static_cast<QGeoRouteReply *>(this->sender()));
}

void QGeoNetworkManagerPrivate::finishedGeocodingRequest()
{
    Q_Q(QGeoNetworkManager);
    emit q->finished(static_cast<QGeocodingReply *>(this->sender()));
}

void QGeoNetworkManagerPrivate::finishedMapTileRequest()
{
    Q_Q(QGeoNetworkManager);

    QGeoMapTileReply *reply = static_cast<QGeoMapTileReply *>(this->sender());

    if (reply->data().length() > 0)
        cache.cache(reply->request(), *reply);

    emit q->finished(reply);
}

void QGeoNetworkManagerPrivate::errorRouteRequest(QGeoRouteReply::ErrorCode errorCode, const QString &errorString)
{
    Q_Q(QGeoNetworkManager);
    // TODO abort request - or has this already happened?
    emit q->error(static_cast<QGeoRouteReply *>(this->sender()), errorCode, errorString);
}

void QGeoNetworkManagerPrivate::errorGeocodingRequest(QGeocodingReply::ErrorCode errorCode, const QString &errorString)
{
    Q_Q(QGeoNetworkManager);
    // TODO abort request - or has this already happened?
    emit q->error(static_cast<QGeocodingReply *>(this->sender()), errorCode, errorString);
}

void QGeoNetworkManagerPrivate::errorMapTileRequest(QGeoMapTileReply::ErrorCode errorCode, const QString &errorString)
{
    Q_Q(QGeoNetworkManager);
    // TODO abort request - or has this already happened?
    // evict from cache?
    emit q->error(static_cast<QGeoMapTileReply *>(this->sender()), errorCode, errorString);
}


#include "moc_qgeonetworkmanager.cpp"
#include "moc_qgeonetworkmanager_p.cpp"

QTM_END_NAMESPACE

