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

#include <qgeomappingmanager_p.h>

#include <QSize>

#define LARGE_TILE_DIMENSION 256

#define PI 3.14159265
#include <math.h>

QGeoMappingManagerNokia::QGeoMappingManagerNokia()
{
}

QGeoMappingManagerNokia::~QGeoMappingManagerNokia()
{
    //delete m_cache;
}

void QGeoMappingManagerNokia::setProxy(const QNetworkProxy &proxy)
{
    m_nam->setProxy(proxy);
}

void QGeoMappingManagerNokia::setHost(QString host)
{
    m_host = host;
}

QGeoMapReply* QGeoMappingManagerNokia::requestTile(int row, int col, int zoomLevel,
                                                       const QSize &size,
                                                       const QGeoMapRequestOptions &requestOptions)
{
    QuadTileInfo* info = new QuadTileInfo;
    info->row = row;
    info->col = col;
    info->zoomLevel = zoomLevel;
    info->size = size;
    info->options = requestOptions;
    //check cache first
    /*
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
    */
        QString rawRequest = getRequestString(*info);
        QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
        QNetworkReply* netReply = m_nam->get(netRequest);
        QGeoMapReply* mapReply = new QGeoMapReplyNokia(netReply, this);
        m_pendingReplies.insert(mapReply, info);

        connect(mapReply,
                SIGNAL(finished()),
                this,
                SLOT(mapFinished()));

        connect(mapReply,
                SIGNAL(error(QGeoMapReply::Error,QString)),
                this,
                SLOT(mapError(QGeoMapReply::Error,QString)));

    //}

    return mapReply;
}

void QGeoMappingManagerNokia::mapFinished()
{
    QGeoMapReply *reply = qobject_cast<QGeoMapReply*>(sender());

    if (reply) {

        if (m_pendingReplies.contains(reply)) {
            QuadTileInfo* info = m_pendingReplies.take(reply);
            qint64 tileIndex = getTileIndex(info->row, info->col, info->zoomLevel);
            m_mapTiles[tileIndex] = qMakePair(reply->mapImage(), true);
            delete info;
            reply->deleteLater();
        } else {
            //TODO: what happens when no-one is connected to signal -> possible mem leak (reply) ?
            emit finished(reply);
        }
    }
}

void QGeoMappingManagerNokia::mapError(QGeoMapReply::Error error, const QString &errorString)
{
    QGeoMapReply *reply = qobject_cast<QGeoMapReply*>(sender());

    if (reply) {

        if (m_pendingReplies.contains(reply)) {
            QuadTileInfo* info = m_pendingReplies.take(reply);
            delete info;
            reply->deleteLater();
        } else {
            //TODO: what happens when no-one is connected to signal -> possible mem leak (reply) ?
            emit this->error(reply, error, errorString);
        }
    }
}

QString QGeoMappingManagerNokia::getRequestString(const QuadTileInfo &info) const
{
    QString request = "http://";
    request += m_host;
    request += "/maptiler/maptile/newest/";
    request += mapTypeToStr(info.options.mapType());
    request += '/';
    request += QString::number(info.zoomLevel);
    request += '/';
    request += QString::number(info.col);
    request += '/';
    request += QString::number(info.row);
    request += '/';
    request += sizeToStr(info.size);
    request += '/';
    request += info.options.imageFormat();

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
void QGeoMappingManagerNokia::getMercatorTileIndex(const QGeoCoordinate& coordinate, qint32 level, qint32* row, qint32* col)
{
    qreal p = pow((double) 2, static_cast<int>(level));

    double x = coordinate.longitude() / 360 + 0.5;
    double y = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * coordinate.latitude() / 180)) / PI) / 2;

    if (y < 0)
        y = 0;

    if (y > 1)
        y = 1;

    *col = (qint32)(x * p);
    *row = (qint32)(y * p);
}

QString QGeoMappingManagerNokia::sizeToStr(const QSize &size)
{
    if (size.height() >= LARGE_TILE_DIMENSION ||
        size.width() >= LARGE_TILE_DIMENSION)
        return "256";
    else
        return "128";
}

QString QGeoMappingManagerNokia::mapTypeToStr(MapType type)
{
    if (type == QGeoMappingManager::StreetMap)
        return "normal.day";
    else if (type == QGeoMappingManager::SatelliteMapDay ||
             type == QGeoMappingManager::SatelliteMapNight)
    {
        return "satellite.day";
    }
    else if (type == QGeoMappingManager::TerrainMap)
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

/*!
    Translates the input \a coordinate to a pixel representation in the view.
    The pixel position is relative to the viewport (i.e. (0,0) defines the top left visible point in the map).
*/
QPointF QGeoMappingManagerNokia::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoMappingManager);

    qint32 numColRow = 1;
    numColRow <<= d->zoomLevel;
    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    QPointF point(static_cast<qint64>(lng * ((qreal) numColRow) * ((qreal) m_tileSize.width())),
                  static_cast<qint64>(lat * ((qreal) numColRow) * ((qreal) m_tileSize.height())));
    point -= m_viewPort.topLeft();
    return point;
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/*!
    Translates the pixel \a screenPosition in the view to a geometric coordinate.
    The pixel position is relative to the viewport (i.e. (0,0) defines the top left visible point in the map).
*/
QGeoCoordinate QGeoMappingManagerNokia::screenPositionToCoordinate(QPointF screenPosition) const
{
    Q_D(const QGeoMappingManager);

    qint32 numColRow = 1;
    numColRow <<= d->zoomLevel;
    screenPosition += m_viewPort.topLeft();
    QPointF mercCoord(screenPosition.x() / (((qreal) numColRow) * ((qreal) m_tileSize.width())),
                      screenPosition.y() / (((qreal) numColRow) * ((qreal) m_tileSize.height())));

    qreal x = mercCoord.x();
    qreal y = mercCoord.y();

    if (y < 0.0f)
        y = 0.0f;
    else if (y > 1.0f)
        y = 1.0f;

    qreal lat;

    if (y == 0.0f)
        lat = 90.0f;
    else if (y == 1.0f)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * y))) - (PI / 2.0f));

    qreal lng;
    if (x >= 0) {
        lng = rmod(x, 1.0f);
    } else {
        lng = rmod(1.0f - rmod(-1.0f * x, 1.0f), 1.0f);
    }

    lng = lng * 360.0f - 180.0f;

    return QGeoCoordinate(lat, lng);
}

QGeoMapReply* QGeoMappingManagerNokia::requestMap(const QGeoCoordinate &center,
                                                      int zoomLevel,
                                                      const QSize &size,
                                                      const QGeoMapRequestOptions &requestOptions)
{
    return NULL;
}

void QGeoMappingManagerNokia::setZoomLevel(int zoomLevel)
{
}

void QGeoMappingManagerNokia::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
}

void QGeoMappingManagerNokia::setCenter(const QGeoCoordinate &center)
{
}

QGeoCoordinate QGeoMappingManagerNokia::center() const
{
    return QGeoCoordinate();
}

void QGeoMappingManagerNokia::pan(int startX, int startY, int endX, int endY)
{
}

QGeoBoundingBox QGeoMappingManagerNokia::viewBounds() const
{
    return QGeoBoundingBox();
}



