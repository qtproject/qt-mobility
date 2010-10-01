/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

    m_nam = new QNetworkAccessManager(this);
    m_cache = new QNetworkDiskCache(this);

    QDir dir = QDir::temp();
    dir.mkdir("maptiles");
    dir.cd("maptiles");

    m_cache->setCacheDirectory(dir.path());

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

    m_nam->setCache(m_cache);
}

QGeoMappingManagerEngineNokia::~QGeoMappingManagerEngineNokia() {}

QGeoTiledMapReply* QGeoMappingManagerEngineNokia::getTileImage(const QGeoTiledMapRequest &request)
{
    QString rawRequest = getRequestString(request);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    m_cache->metaData(netRequest.url()).setLastModified(QDateTime::currentDateTime());

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
    const char subdomain = 'a' + (request.row()+request.column()) % maxDomains; // a...k
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

