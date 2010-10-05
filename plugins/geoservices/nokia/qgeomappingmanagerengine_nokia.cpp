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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeomappingmanagerengine_nokia.h"
#include "qgeomapreply_nokia.h"

#include <qgeotiledmaprequest.h>

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QDateTime>

#include <QDebug>

#define LARGE_TILE_DIMENSION 256

// TODO: Tweak the max size or create something better
#if defined(Q_OS_SYMBIAN)
#define DISK_CACHE_MAX_SIZE 10*1024*1024  //10MB
#else
#define DISK_CACHE_MAX_SIZE 50*1024*1024  //50MB
#endif

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#undef DISK_CACHE_ENABLED
#else
#define DISK_CACHE_ENABLED 1
#endif

QGeoMappingManagerEngineNokia::QGeoMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoTiledMappingManagerEngine(parameters),
        m_host("maptile.maps.svc.ovi.com")
{
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    setTileSize(QSize(256, 256));
    setMinimumZoomLevel(0.0);
    setMaximumZoomLevel(18.0);

    QList<QGraphicsGeoMap::MapType> types;
    types << QGraphicsGeoMap::StreetMap;
    types << QGraphicsGeoMap::SatelliteMapDay;
    types << QGraphicsGeoMap::TerrainMap;
    setSupportedMapTypes(types);

    QList<QGraphicsGeoMap::ConnectivityMode> modes;
    modes << QGraphicsGeoMap::OnlineMode;
    setSupportedConnectivityModes(modes);

    m_nam = new QNetworkAccessManager(this);

    QList<QString> keys = parameters.keys();

    if (keys.contains("mapping.proxy")) {
        QString proxy = parameters.value("mapping.proxy").toString();
        if (!proxy.isEmpty())
            m_nam->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("mapping.host")) {
        QString host = parameters.value("mapping.host").toString();
        if (!host.isEmpty())
            m_host = host;
    }

#ifdef DISK_CACHE_ENABLED
    m_cache = new QNetworkDiskCache(this);

    QDir dir = QDir::temp();
    dir.mkdir("maptiles");
    dir.cd("maptiles");

    m_cache->setCacheDirectory(dir.path());

    if (keys.contains("mapping.cache.directory")) {
        QString cacheDir = parameters.value("mapping.cache.directory").toString();
        if (!cacheDir.isEmpty())
            m_cache->setCacheDirectory(cacheDir);
    }

    if (keys.contains("mapping.cache.size")) {
        bool ok = false;
        qint64 cacheSize = parameters.value("mapping.cache.size").toString().toLongLong(&ok);
        if (ok)
            m_cache->setMaximumCacheSize(cacheSize);
    }

    if (m_cache->maximumCacheSize() > DISK_CACHE_MAX_SIZE)
        m_cache->setMaximumCacheSize(DISK_CACHE_MAX_SIZE);

    m_nam->setCache(m_cache);
#endif
}

QGeoMappingManagerEngineNokia::~QGeoMappingManagerEngineNokia() {}

QGeoMapData* QGeoMappingManagerEngineNokia::createMapData(QGraphicsGeoMap *geoMap)
{
    QGeoMapData *data = QGeoTiledMappingManagerEngine::createMapData(geoMap);
    if (!data)
        return 0;

    data->setConnectivityMode(QGraphicsGeoMap::OnlineMode);
    return data;
}

QGeoTiledMapReply* QGeoMappingManagerEngineNokia::getTileImage(const QGeoTiledMapRequest &request)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(request);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

#ifdef DISK_CACHE_ENABLED
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    m_cache->metaData(netRequest.url()).setLastModified(QDateTime::currentDateTime());
#endif

    QNetworkReply* netReply = m_nam->get(netRequest);

    QGeoTiledMapReply* mapReply = new QGeoMapReplyNokia(netReply, request);

    // TODO goes badly on linux
    //qDebug() << "request: " << QString::number(reinterpret_cast<int>(mapReply), 16) << " " << request.row() << "," << request.column();
    // this one might work better. It follows defined behaviour, unlike reinterpret_cast
    //qDebug("request: %p %i,%i @ %i", mapReply, request.row(), request.column(), request.zoomLevel());
    return mapReply;
}

QString QGeoMappingManagerEngineNokia::getRequestString(const QGeoTiledMapRequest &request) const
{
    const int maxDomains = 11; // TODO: hmmm....
    const char subdomain = 'a' + (request.row() + request.column()) % maxDomains; // a...k
    static const QString http("http://");
    static const QString path("/maptiler/maptile/newest/");
    static const QChar dot('.');
    static const QChar slash('/');

    QString requestString = http;
    requestString += subdomain;
    requestString += dot;
    requestString += m_host;
    requestString += path;
    requestString += mapTypeToStr(request.mapType());
    requestString += slash;
    requestString += QString::number(request.zoomLevel());
    requestString += slash;
    requestString += QString::number(request.column());
    requestString += slash;
    requestString += QString::number(request.row());
    requestString += slash;
    requestString += sizeToStr(tileSize());
//#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    static const QString slashpng("/png8");
//#else
//    static const QString slashpng("/png");
//#endif
    requestString += slashpng;

    if (!m_token.isEmpty()) {
        requestString += "?token=";
        requestString += m_token;

        if (!m_referrer.isEmpty()) {
            requestString += "&referrer=";
            requestString += m_referrer;
        }
    } else if (!m_referrer.isEmpty()) {
        requestString += "?referrer=";
        requestString += m_referrer;
    }

    return requestString;
}

QString QGeoMappingManagerEngineNokia::sizeToStr(const QSize &size)
{
    static const QString s256("256");
    static const QString s128("128");
    if (size.height() >= LARGE_TILE_DIMENSION ||
            size.width() >= LARGE_TILE_DIMENSION)
        return s256;
    else
        return s128;
}

QString QGeoMappingManagerEngineNokia::mapTypeToStr(QGraphicsGeoMap::MapType type)
{
    if (type == QGraphicsGeoMap::StreetMap)
        return "normal.day";
    else if (type == QGraphicsGeoMap::SatelliteMapDay ||
             type == QGraphicsGeoMap::SatelliteMapNight) {
        return "satellite.day";
    } else if (type == QGraphicsGeoMap::TerrainMap)
        return "terrain.day";
    else
        return "normal.day";
}

