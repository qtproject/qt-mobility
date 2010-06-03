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

#include "qgeomappingmanagerengine_nokia_p.h"
#include "qgeomapreply_nokia_p.h"

#include <qgeotiledmaprequest.h>

#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QDateTime>

#define LARGE_TILE_DIMENSION 256
#define PI 3.14159265
#include <math.h>

QGeoMappingManagerThreadNokia::QGeoMappingManagerThreadNokia(QGeoMappingManagerEngineNokia* engine, const QMap<QString, QString> &parameters)
        : QGeoTiledMappingManagerThread(engine),
        m_engine(engine),
        m_parameters(parameters),
        m_host("loc.desktop.maps.svc.ovi.com") {}

QGeoMappingManagerThreadNokia::~QGeoMappingManagerThreadNokia() {}

void QGeoMappingManagerThreadNokia::initialize()
{
    m_nam = new QNetworkAccessManager(this);
    m_cache = new QNetworkDiskCache(this);

    QDir dir = QDir::temp();
    dir.mkdir("maptiles");
    dir.cd("maptiles");

    m_cache->setCacheDirectory(dir.path());

    QList<QString> keys = m_parameters.keys();

    if (keys.contains("mapping.proxy")) {
        QString proxy = m_parameters.value("mapping.proxy");
        if (!proxy.isEmpty())
            m_nam->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("mapping.host")) {
        QString host = m_parameters.value("mapping.host");
        if (!host.isEmpty())
            m_host = host;
    }

    if (keys.contains("mapping.cache.directory")) {
        QString cacheDir = m_parameters.value("mapping.cache.directory");
        if (!cacheDir.isEmpty())
            m_cache->setCacheDirectory(cacheDir);
    }

    if (keys.contains("mapping.cache.size")) {
        bool ok = false;
        qint64 cacheSize = m_parameters.value("mapping.cache.size").toLongLong(&ok);
        if (ok)
            m_cache->setMaximumCacheSize(cacheSize);
    }

    m_nam->setCache(m_cache);
}

QGeoTiledMapReply* QGeoMappingManagerThreadNokia::getTileImage(const QGeoTiledMapRequest &request)
{
    QString rawRequest = getRequestString(request);

    QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    m_cache->metaData(netRequest.url()).setLastModified(QDateTime::currentDateTime());

    QNetworkReply* netReply = m_nam->get(netRequest);
    QGeoTiledMapReply* mapReply = new QGeoMapReplyNokia(netReply, request, this);

    return mapReply;
}

QString QGeoMappingManagerThreadNokia::getRequestString(const QGeoTiledMapRequest &request) const
{
    QString requestString = "http://";
    requestString += m_host;
    requestString += "/maptiler/maptile/newest/";
    requestString += mapTypeToStr(request.mapType());
    requestString += '/';
    requestString += QString::number(request.zoomLevel());
    requestString += '/';
    requestString += QString::number(request.column());
    requestString += '/';
    requestString += QString::number(request.row());
    requestString += '/';
    requestString += sizeToStr(m_engine->tileSize());
    requestString += '/';
    requestString += "png";

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

QString QGeoMappingManagerThreadNokia::sizeToStr(const QSize &size)
{
    if (size.height() >= LARGE_TILE_DIMENSION ||
            size.width() >= LARGE_TILE_DIMENSION)
        return "256";
    else
        return "128";
}

QString QGeoMappingManagerThreadNokia::mapTypeToStr(QGeoMapWidget::MapType type)
{
    if (type == QGeoMapWidget::StreetMap)
        return "normal.day";
    else if (type == QGeoMapWidget::SatelliteMapDay ||
             type == QGeoMapWidget::SatelliteMapNight) {
        return "satellite.day";
    } else if (type == QGeoMapWidget::TerrainMap)
        return "terrain.day";
    else
        return "normal.day";
}

QGeoMappingManagerEngineNokia::QGeoMappingManagerEngineNokia(const QMap<QString, QString> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : parameters(parameters)
{
    setTileSize(QSize(128, 128));
    setMinimumZoomLevel(0.0);
    setMaximumZoomLevel(18.0);
}

QGeoMappingManagerEngineNokia::~QGeoMappingManagerEngineNokia()
{
}

QGeoTiledMappingManagerThread* QGeoMappingManagerEngineNokia::createTileManagerThread()
{
    return new QGeoMappingManagerThreadNokia(this, parameters);
}

