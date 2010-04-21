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

#include "qmaptileservice_nokia_p.h"
#include "qmaptilecache_nokia_p.h"
#include "qmaptilereply_nokia_p.h"
#include "qmaptilecache_nokia_p.h"

#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

QMapTileServiceNokia::QMapTileServiceNokia()
        : m_token(""),
        m_referrer(""),
        m_host("172.24.32.155"),
        m_proxy(QNetworkProxy(QNetworkProxy::NoProxy)),
        m_maxZoomLevel(18),
        m_cache(new QMapTileCacheNokia),
        m_format(Png),
        m_scheme(NormalDay),
        m_size(Tile_256_256),
        m_version(Newest)
{
    setVersion(Newest);
    setTileSize(Tile_256_256);
    setFormat(Png);
    setScheme(NormalDay);
}

QMapTileServiceNokia::~QMapTileServiceNokia()
{
    delete m_cache;
}

QString QMapTileServiceNokia::token() const
{
    return m_token;
}

void QMapTileServiceNokia::setToken(const QString &token)
{
    m_token = token;
}

QString QMapTileServiceNokia::referrer() const
{
    return m_referrer;
}

void QMapTileServiceNokia::setReferrer(const QString &referrer)
{
    m_referrer = referrer;
}

QString QMapTileServiceNokia::host() const
{
    return m_host;
}

void QMapTileServiceNokia::setHost(const QString &host)
{
    m_host = host;
}

QNetworkProxy QMapTileServiceNokia::proxy() const
{
    return m_proxy;
}

void QMapTileServiceNokia::setProxy(const QNetworkProxy &proxy)
{
    m_proxy = proxy;
    m_nam.setProxy(m_proxy);
}

void QMapTileServiceNokia::setVersion(MapVersion version)
{
    m_version = version;
}

void QMapTileServiceNokia::setFormat(MapFormat format)
{
    m_format = format;
}

void QMapTileServiceNokia::setTileSize(TileSize tileSize)
{
    m_size = tileSize;
}

void QMapTileServiceNokia::setScheme(MapScheme scheme)
{
    m_scheme = scheme;
}

QMapTileReply* QMapTileServiceNokia::request(quint32 level, quint32 row, quint32 col)
{
    //check cache first
    QMapTileReplyNokia* tileReply = NULL;

    if ((tileReply = m_cache->get(level, row, col, m_version, m_size, m_format, m_scheme))) {
        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedReply()));
        connect(tileReply,
                SIGNAL(error(QMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorReply(QMapTileReply::ErrorCode, QString)));
        tileReply->done();
    } else {
        QString rawRequest = getRequestString(level, row, col);
        QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
        QNetworkReply* netReply = m_nam.get(netRequest);

        tileReply = new QMapTileReplyNokia(netReply);
        tileReply->setLevel(level);
        tileReply->setRow(row);
        tileReply->setCol(col);
        tileReply->setFormat(m_format);
        tileReply->setScheme(m_scheme);
        tileReply->setVersion(m_version);
        tileReply->setTileSize(m_size);

        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedReply()));
        connect(tileReply,
                SIGNAL(error(QMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorReply(QMapTileReply::ErrorCode, QString)));
    }

    return tileReply;
}

quint32 QMapTileServiceNokia::maxZoomLevel() const
{
    return m_maxZoomLevel;
}

void QMapTileServiceNokia::finishedReply()
{
    QMapTileReplyNokia *reply = static_cast<QMapTileReplyNokia *>(this->sender());

    if (reply->data().length() > 0)
        m_cache->cache(*reply);

    emit finished(reply);
}

void QMapTileServiceNokia::errorReply(QMapTileReply::ErrorCode errorCode, QString errorString)
{
    emit error(static_cast<QMapTileReply *>(this->sender()), errorCode, errorString);
}

QString QMapTileServiceNokia::getRequestString(quint16 level, quint32 row, quint32 col) const
{
    QString request = "http://";
    request += m_host;
    request += "/maptiler/maptile/";
    request += versionToStr(m_version);
    request += '/';
    request += schemeToStr(m_scheme);
    request += '/';
    request += QString::number(level);
    request += '/';
    request += QString::number(col);
    request += '/';
    request += QString::number(row);
    request += '/';
    request += tileSizeToStr(m_size);
    request += '/';
    request += formatToStr(m_format);

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

/*!
    The map tile server uses a Mercator projection to map geo coordinates into a 2D map.
    For each zoom level, that map is then split into 2^zoomLevel x 2^zoomLevel map tiles.

    For the given \a zoomLevel, this function  determines the row and column indices of the
    map tile from the 2D map which contains the geo coordinate \a coordinate and stores
    the indices in \a row and \a col respectively.

    \note This does not mean that the coordinate lies in the center of the calculated tile.
*/
void QMapTileServiceNokia::getMercatorTileIndex(const QGeoCoordinate& coordinate, quint32 level, quint32* row, quint32* col)
{
    qreal p = pow((double) 2, static_cast<int>(level));

    double x = coordinate.longitude() / 360 + 0.5;
    double y = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * coordinate.latitude() / 180)) / PI) / 2;

    if (y < 0)
        y = 0;

    if (y > 1)
        y = 1;

    *col = (quint32)(x * p);
    *row = (quint32)(y * p);
}

QString QMapTileServiceNokia::versionToStr(MapVersion version)
{
    if (version == QMapTileServiceNokia::Newest)
        return "newest";
    else
        return QString();
}

QString QMapTileServiceNokia::tileSizeToStr(TileSize size)
{
    if (size == QMapTileServiceNokia::Tile_128_128)
        return "128";
    else if (size == QMapTileServiceNokia::Tile_256_256)
        return "256";
    else
        return QString();
}

QString QMapTileServiceNokia::formatToStr(MapFormat format)
{
    if (format == QMapTileServiceNokia::Png)
        return "png";
    else if (format == QMapTileServiceNokia::Png8)
        return "png8";
    else
        return QString();
}

QString QMapTileServiceNokia::schemeToStr(MapScheme scheme)
{
    if (scheme == QMapTileServiceNokia::NormalDay)
        return "normal.day";
    else if (scheme == QMapTileServiceNokia::SatelliteDay)
        return "satellite.day";
    else if (scheme == QMapTileServiceNokia::TerrainDay)
        return "terrain.day";
    else
        return QString();
}

#include "moc_qmaptileservice_nokia_p.cpp"

QTM_END_NAMESPACE

