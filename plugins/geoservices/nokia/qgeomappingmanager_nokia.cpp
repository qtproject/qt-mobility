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

#include "qgeomappingmanager_nokia_p.h"
#include "qgeomapreply_nokia_p.h"

#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QDateTime>

#define LARGE_TILE_DIMENSION 256
#define PI 3.14159265
#include <math.h>

QGeoMappingManagerNokia::QGeoMappingManagerNokia(const QMap<QString, QString> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : m_host("loc.desktop.maps.svc.ovi.com")
{
    m_nam = new QNetworkAccessManager(this);
    m_cache = new QNetworkDiskCache(this);

    QDir dir = QDir::temp();
    dir.mkdir("maptiles");
    dir.cd("maptiles");

    m_cache->setCacheDirectory(dir.path());

    QList<QString> keys = parameters.keys();

    if (keys.contains("mapping.proxy")) {
        QString proxy = parameters.value("mapping.proxy");
        if (!proxy.isEmpty())
            m_nam->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("mapping.host")) {
        QString host = parameters.value("mapping.host");
        if (!host.isEmpty())
            m_host = host;
    }

    if (keys.contains("mapping.cache.directory")) {
        QString cacheDir = parameters.value("mapping.cache.directory");
        if (!cacheDir.isEmpty())
            m_cache->setCacheDirectory(cacheDir);
    }

    if (keys.contains("mapping.cache.size")) {
        bool ok = false;
        qint64 cacheSize = parameters.value("mapping.cache.size").toLongLong(&ok);
        if (ok)
            m_cache->setMaximumCacheSize(cacheSize);
    }

    m_nam->setCache(m_cache);

    setMinimumZoomLevel(0.0);
    setMaximumZoomLevel(18.0);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoMappingManagerNokia::~QGeoMappingManagerNokia()
{
    //delete m_cache;
}

QGeoMapReply* QGeoMappingManagerNokia::getTileImage(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex, QGeoMapWidget::MapType mapType, const QString &imageFormat) const
{
    QGeoMapReplyNokia::QuadTileInfo* info = new QGeoMapReplyNokia::QuadTileInfo;
    info->row = rowIndex;
    info->col = columnIndex;
    info->zoomLevel = zoomLevel;
    info->imageFormat = imageFormat;
    info->mapType = mapType;
    info->size = tileSize();

    QString rawRequest = getRequestString(*info);

    QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    m_cache->metaData(netRequest.url()).setLastModified(QDateTime::currentDateTime());

    QNetworkReply* netReply = m_nam->get(netRequest);
    //QGeoMapReply* mapReply = new QGeoMapReplyNokia(netReply, info, this);
    QGeoMapReply* mapReply = new QGeoMapReplyNokia(netReply, info);

    connect(mapReply,
            SIGNAL(finished()),
            this,
            SLOT(mapFinished()));

    connect(mapReply,
            SIGNAL(error(QGeoMapReply::Error, QString)),
            this,
            SLOT(mapError(QGeoMapReply::Error, QString)));

    return mapReply;
}

void QGeoMappingManagerNokia::mapFinished()
{
    QGeoMapReplyNokia *reply = qobject_cast<QGeoMapReplyNokia*>(sender());

    if (!reply)
        return;

    if (reply->error() != QGeoMapReply::NoError)
        m_cache->remove(reply->networkReply()->request().url());

    /*
    QGeoMapReplyNokia::QuadTileInfo* info = reply->tileInfo();
    if (!info) {
        reply->deleteLater();
        return;
    }

    //qint64 tileIndex = getTileIndex(info->row, info->col, info->zoomLevel);
    //m_mapTiles[tileIndex] = qMakePair(reply->mapImage(), true);
*/
    if (receivers(SIGNAL(finished(QGeoMapReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoMappingManagerNokia::mapError(QGeoMapReply::Error error, const QString &errorString)
{
    QGeoMapReply *reply = qobject_cast<QGeoMapReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoMapReply*, QGeoMapReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

QString QGeoMappingManagerNokia::getRequestString(const QGeoMapReplyNokia::QuadTileInfo &info) const
{
    QString request = "http://";
    request += m_host;
    request += "/maptiler/maptile/newest/";
    request += mapTypeToStr(info.mapType);
    request += '/';
    request += QString::number(info.zoomLevel);
    request += '/';
    request += QString::number(info.col);
    request += '/';
    request += QString::number(info.row);
    request += '/';
    request += sizeToStr(tileSize());
    request += '/';
    request += info.imageFormat;

    if (!m_token.isEmpty()) {
        request += "?token=";
        request += m_token;

        if (!m_referrer.isEmpty()) {
            request += "&referrer=";
            request += m_referrer;
        }
    } else if (!m_referrer.isEmpty()) {
        request += "?referrer=";
        request += m_referrer;
    }

    return request;
}

///*!
//    The map tile server uses a Mercator projection to map geo coordinates into a 2D map.
//    For each zoom level, that map is then split into 2^zoomLevel x 2^zoomLevel map tiles.

//    For the given \a zoomLevel, this function  determines the row and column indices of the
//    map tile from the 2D map which contains the geo coordinate \a coordinate and stores
//    the indices in \a row and \a col respectively.

//    \note This does not mean that the coordinate lies in the center of the calculated tile.
//*/
//void QGeoMappingManagerNokia::getTileQuadKey(const QGeoCoordinate& coordinate, qint32 zoomLevel, qint32* row, qint32* col)
//{
//    qreal p = pow((double) 2, static_cast<int>(zoomLevel));

//    double x = coordinate.longitude() / 360 + 0.5;
//    double y = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * coordinate.latitude() / 180)) / PI) / 2;

//    if (y < 0)
//        y = 0;

//    if (y > 1)
//        y = 1;

//    *col = (qint32)(x * p);
//    *row = (qint32)(y * p);
//}

QString QGeoMappingManagerNokia::sizeToStr(const QSize &size)
{
    if (size.height() >= LARGE_TILE_DIMENSION ||
            size.width() >= LARGE_TILE_DIMENSION)
        return "256";
    else
        return "128";
}

QString QGeoMappingManagerNokia::mapTypeToStr(QGeoMapWidget::MapType type)
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

/*!
    Coverts a quad key (\a row, \a col, \a zoomLevel) into a one-dimensional tile index.
*/
qint64 QGeoMappingManagerNokia::getTileIndex(qint32 row, qint32 col, qint32 zoomLevel)
{
    qint32 numColRow = 1;
    numColRow <<= zoomLevel;
    return ((qint64) row) * numColRow + col;
}
