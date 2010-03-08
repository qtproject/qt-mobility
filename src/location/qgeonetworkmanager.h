/*
* ==============================================================================
*  Name        : QGeoNetworkManager.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEONETWORKMANAGER_H
#define QLOCATION_GEONETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QPair>
#include <QPointF>
#include <QString>

#include "qgeoengine.h"
#include "qmaptilecache.h"

#define MAPTILE_MAX_ZOOMLEVEL 18

QTM_BEGIN_NAMESPACE

/*!
* This class extends \ref QGeoEngine.
* It handles all communication with the REST servers.
* Internally it uses Qt::QNetworkAccessManager for network
* communication, and it handles all mapping between
* raw network requests/replies and geo requests/replies.
*/
class Q_LOCATION_EXPORT QGeoNetworkManager : public QGeoEngine
{
    Q_OBJECT

public:
    /*!
    * Constructor.
    * @param token The authentification token used for server communication.
    * @param referrer The referrer (usually a URL) for the token.
    */
    QGeoNetworkManager(QString token, QString referrer);
    /*!
    * Destructor.
    */
    virtual ~QGeoNetworkManager();

    /*!
    * Sets the server to be used for (reverse) geocoding requests.
    * @param server The server.
    */
    void setGeocodingServer(const QString& server);
    /*!
    * @return The server to be used for (reverse) geocoding requests.
    */
    QString geocodingServer() const {
        return geocdSrv;
    }
    /*!
    * @return The proxy to be used for (reverse) geocoding requests.
    */
    QNetworkProxy geocodingProxy() const {
        return geocdProx;
    }
    /*!
    * Sets the proxy to be used for (reverse) geocoding requests.
    * @param proxy The proxy.
    */
    void setGeocodingProxy(const QNetworkProxy& proxy);

    /*!
    * Sets the server to be used for route requests.
    * @param server The server.
    */
    void setRouteServer(const QString& server);
    /*!
    * @return The server to be used for route requests.
    */
    QString routeServer() const {
        return rtSrv;
    }
    /*!
    * @return The proxy to be used for route requests.
    */
    QNetworkProxy routeProxy() const {
        return rtProx;
    }
    /*!
    * Sets the proxy to be used for route requests.
    * @param proxy The proxy.
    */
    void setRouteProxy(const QNetworkProxy& proxy);

    /*!
    * Sets the server to be used for map tile requests.
    * @param server The server.
    */
    void setMapServer(const QString& server);
    /*!
    * @return The server to be used for map tile requests.
    */
    QString mapServer() const {
        return mapSrv;
    }
    /*!
    * @return The proxy to be used for map tile requests.
    */
    QNetworkProxy mapProxy() const {
        return mapProx;
    }
    /*!
    * Sets the proxy to be used for map tile requests.
    * @param proxy The proxy.
    */
    void setMapProxy(const QNetworkProxy& proxy);

    /*!
    * Submits a route request.
    * @param request The route request.
    * @return The corresponding route reply.
    * @note Due to the asynchronous nature of requests,
    *       you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QRouteReply*) signal.
    */
    virtual QRouteReply* get(const QRouteRequest& request);
    /*!
    * Submits a geocoding request.
    * @param request The geocoding request.
    * @return The corresponding geocoding repy.
    * @note Due to the asynchronous nature of requests,
    *       you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QGeocodingReply*) signal.
    */
    virtual QGeocodingReply* get(const QGeocodingRequest& request);
    /*!
    * Submits a reverse geocoding request.
    * @param request The route request.
    * @return The corresponding geocoding reply.
    * @note Due to the asynchronous nature of requests,
    *       you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QGeocodingReply*) signal.
    */
    virtual QGeocodingReply* get(const QReverseGeocodingRequest& request);
    /*!
    * Submits a map tile request.
    * @param request The map tile request.
    * @return The corresponding map tile reply.<BR>
    * @note Due to the potentially asynchronous nature of requests,
    *       you should never directly start working with the returned
    *       reply, but instead wait for the \ref finished(QMapTileReply*) signal.
    * @note If the request can be served from the tile cache,
    *       this method will directly emit \ref finished(QRouteReply*).
    *       If you need the returned QMapTileReply object first,
    *       you should connect to \ref finished(QRouteReply*) with <b>Qt::QueuedConnection</b>.
    */
    virtual QMapTileReply* get(const QMapTileRequest& request);

    /*!
    * @param reply A QGeoReply.
    * @return The raw network reply associated with the QGeoReply.
    */
    QNetworkReply* getRawReply(const QGeoReply* reply) const;

    /*!
    * When a \ref QGeoReply is no longer needed, it should never be manually
    * deleted. Instead this method should be used to properly remove the reply
    * from corresponding internal maps.
    * @param reply The reply to be released.
    */
    virtual void release(QGeoReply* reply);
    /*!
    * This methods aborts a (pending) reply. It essentially calls abort() on the
    * associated Qt::QNetworkReply object.
    * @param reply The reply to abort.
    */
    virtual void cancel(QGeoReply* reply);

    /*!
    * @return The maximum zoom level supported
    */
    virtual quint16 maxZoomLevel() const {
        return MAPTILE_MAX_ZOOMLEVEL;
    }
    /*!
    * @return A list of all available \ref MapVersion<i>s</i>.
    */
    virtual QList<MapVersion> versions() const {
        return mapVersions.values();
    }
    /*!
    * @return A list of all available \ref MapResolution<i>s</i>.
    */
    virtual QList<MapResolution> resolutions() const {
        return mapResolutions.values();
    }
    /*!
    * @return A list of all available \ref MapFormat<i>s</i>.
    */
    virtual QList<MapFormat> formats() const {
        return mapFormats.values();
    }
    /*!
    * @return A list of all available \ref MapScheme<i>s</i>.
    */
    virtual QList<MapScheme> schemes() const {
        return mapSchemes.values();
    }

private:
    Q_DISABLE_COPY(QGeoNetworkManager)

private slots:
    /*!
    * This slot is invoked when a network reply has finished.
    * @param reply The QNetworkReply that has finished.
    */
    void netReplyFinished(QNetworkReply* reply);
    /*!
    * This slot is invoked when a network reply has detected an error.
    * @param code The network error code that occured.
    */
    void netReplyError(QNetworkReply::NetworkError code);

private:
    /*!
    * Parses the netReply to a \ref QRouteXmlHandler to populate routeReply.
    * @param netReply The raw QNetworkReply.
    * @param routeReply The QRouteReply to be populated from netReply.
    */
    bool parseRouteReply(QNetworkReply* netReply, QRouteReply* routeReply);
    /*!
    * Parses the netReply to a \ref QGeocodingXmlHandler to populate codingReply.
    * @param netReply The raw QNetworkReply.
    * @param routeReply The QGeocodingReply to be populated from netReply.
    */
    bool parseCodingReply(QNetworkReply* netReply, QGeocodingReply* codingReply);

    /*!
    * @return A string representation of degree value with at most DEGREE_DECIMALS
    *         decimals.
    */
    static QString trimGeoCoordinate(qreal degree);

private:
    QNetworkAccessManager netManager; //!< The internal network manager
    QMapTileCache cache; //!< The map tile cache

    QString geocdSrv;
    QString rtSrv;
    QString mapSrv;
    QNetworkProxy geocdProx;
    QNetworkProxy rtProx;
    QNetworkProxy mapProx;
    QString token;
    QString referrer;

    //! maps QNetworkReplies to their corresponding QGeoReplies
    QHash<QNetworkReply*, QGeoReply*> replyMap;
    //! maps a QGeoReply to its corresponding QNetworkReply
    QHash<QGeoReply*, QNetworkReply*> revReplyMap;

    QHash<QString, MapVersion> mapVersions;
    QHash<QString, MapResolution> mapResolutions;
    QHash<QString, MapFormat> mapFormats;
    QHash<QString, MapScheme> mapSchemes;
};

QTM_END_NAMESPACE

#endif
