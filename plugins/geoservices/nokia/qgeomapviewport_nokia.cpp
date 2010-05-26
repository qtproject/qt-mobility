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

#include "qgeomapviewport_nokia_p.h"
#include "qgeomappingmanager_nokia_p.h"

#include <QNetworkProxy>

#define PI 3.14159265
#include <math.h>

QGeoMapViewportNokia::QGeoMapViewportNokia(QGeoMappingManager *manager,
        const QMap<QString, QString> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString)
        : QGeoMapViewport(manager)
{
    connect(manager,
            SIGNAL(QGeoMapReply*),
            this,
            SLOT(tileFinished(QGeoMapReply*)));
}

QGeoMapViewportNokia::~QGeoMapViewportNokia()
{
    //delete m_cache;
    //TODO: clear m_pendingReplies
}

void QGeoMapViewportNokia::setZoomLevel(int zoomLevel)
{
}

void QGeoMapViewportNokia::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    QSize vpSize = viewportSize();
    int zLevel = zoomLevel();
    painter->setClipRect(0, 0, vpSize.width() + 1, vpSize.height() + 1);
    QRectF rect = option->exposedRect;
    rect.translate(m_boundingBox.left(), m_boundingBox.top());
    TileIterator it(rect, zLevel, m_tileSize);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        qint64 index = getTileIndex(it.row(), it.col(), zLevel);

        if (m_mapTiles.contains(index)) {
            QPair<QPixmap, bool>& tileData = m_mapTiles[index];
            QPixmap& pixmap = tileData.first;
            QPointF tileTopLeft = it.tileRect().topLeft();
            tileTopLeft -= m_boundingBox.topLeft();
            painter->drawPixmap(tileTopLeft, pixmap);
            //painter->drawRect(tileTopLeft.x(), tileTopLeft.y(), TILE_WIDTH, TILE_HEIGHT);

            if (!tileData.second)
                requestTile(it.row(), it.col());
        } else
            requestTile(it.row(), it.col());
    }
    //TODO: paint map layers
}

void QGeoMapViewportNokia::requestTile(qint32 row, qint32 col)
{
    QuadTileInfo* info = new QuadTileInfo;
    info->row = row;
    info->col = col;
    info->zoomLevel = zoomLevel();
    info->size = m_tileSize;

    QGeoMapReply* reply = mappingManager()->getTileImage(row, col, info->zoomLevel,
                                                         m_tileSize, info->options);
    m_pendingReplies.insert(reply, info);
}

void QGeoMapViewportNokia::tileFinished(QGeoMapReply* reply)
{
    if (!reply)
        return;

    //Unknown reply?
    if (!m_pendingReplies.contains(reply)) {
        reply->deleteLater();
        return; //discard
    }

    QuadTileInfo* info = m_pendingReplies.take(reply);

    if (reply->error() != QGeoMapReply::NoError) {
        delete info;
        reply->deleteLater();
        return;
    }

    qint64 tileIndex = getTileIndex(info->row, info->col, info->zoomLevel);

    ////has map configuration changed in the meantime?
    //if (request.zoomLevel() != d->currZoomLevel ||
    //        request.format().id != d->mapFormat.id ||
    //        request.resolution().id != d->mapResolution.id ||
    //        request.scheme().id != d->mapSchmeme.id ||
    //        request.version().id != d->mapVersion.id) {
    //    delete reply;
    //    return; //discard
    //}

    QPixmap tile = reply->mapImage();

    if(!tile.isNull() && !tile.size().isEmpty()) {
        m_mapTiles[tileIndex] = qMakePair(tile, true);
        //TODO: call update on associated map widget
    }

    delete info;
    reply->deleteLater();
}

void QGeoMapViewportNokia::setCenter(const QGeoCoordinate &center)
{
}

QGeoCoordinate QGeoMapViewportNokia::center() const
{
    return QGeoCoordinate();
}

void QGeoMapViewportNokia::pan(int startX, int startY, int endX, int endY)
{
}

QGeoBoundingBox QGeoMapViewportNokia::viewBounds() const
{
    return QGeoBoundingBox();
}

/*!
    Translates the input \a coordinate to a pixel representation in the view.
    The pixel position is relative to the viewport (i.e. (0,0) defines the top left visible point in the map).
*/
QPointF QGeoMapViewportNokia::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    qint32 numColRow = 1;
    numColRow <<= zoomLevel();
    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    QPointF point(static_cast<qint64>(lng * ((qreal) numColRow) * ((qreal) m_tileSize.width())),
                  static_cast<qint64>(lat * ((qreal) numColRow) * ((qreal) m_tileSize.height())));
    point -= m_boundingBox.topLeft();
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
QGeoCoordinate QGeoMapViewportNokia::screenPositionToCoordinate(QPointF screenPosition) const
{
    qint32 numColRow = 1;
    numColRow <<= zoomLevel();
    screenPosition += m_boundingBox.topLeft();
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

/*!
    Maps a map tile quad key (\a row, \a col, \a zoomLevel) onto a one-dimensional one.
*/
qint64 QGeoMapViewportNokia::getTileIndex(qint32 row, qint32 col, qint32 zoomLevel)
{
    qint64 numColRow = 1;
    numColRow <<= zoomLevel;
    return ((qint64) row) * numColRow + col;
}

/******************************************************************************************************
 ******************************************************************************************************/

/*!
    \class QGeoMapViewportNokia::TileIterator
    \brief The QGeoMapViewportNokia::TileIterator can be used to iterate through all map tiles that are
    covered by a specified view port
    \ingroup location

    The iteration goes row by row
    (top-down), with each row being walked from left to right.
*/

/*!
    Constructs a TileIterator with its associated \a mapView and \a viewPort.
*/
QGeoMapViewportNokia::TileIterator::TileIterator(const QRectF &boundingBox, qint32 zoomLevel, const QSize &tileSize)
    : m_hasNext(true),
    m_boundingBox(boundingBox),
    m_tileSize(tileSize),
    m_rect(QPointF(), tileSize),
    m_currX(static_cast<qint64>(boundingBox.left())),
    m_currY(static_cast<qint64>(boundingBox.top())),
    m_valid(false)
{
    m_numColRow = 1;
    m_numColRow <<= zoomLevel;
}

/*!
    Moves iterator to next tile.
*/
void QGeoMapViewportNokia::TileIterator::next()
{
    m_col = (m_currX / m_tileSize.width()) % m_numColRow;
    qint64 left = (m_currX / m_tileSize.width()) * m_tileSize.width();
    m_row = m_currY / m_tileSize.height();
    m_tileIndex = ((qint64) m_row) * m_numColRow + m_col;

    if (m_currY > 0) {
        qint64 top = (m_currY / m_tileSize.height()) * m_tileSize.height();
        m_rect.moveTopLeft(QPointF(left, top));
        m_valid = true;
    } else
        m_valid = false;

    m_currX += m_tileSize.width();
    qint64 nextLeft = (m_currX / m_tileSize.width()) * m_tileSize.width();

    if (nextLeft > m_boundingBox.right()) {
        m_currX = m_boundingBox.left();
        m_currY += m_tileSize.height();
    }

    qint64 nextTop = (m_currY / m_tileSize.height()) * m_tileSize.height();

    if (nextTop > m_boundingBox.bottom())
        m_hasNext = false;
}

/*!
    Returns True (at least one more tile is available), False (last tile has been reached)
*/
bool QGeoMapViewportNokia::TileIterator::hasNext() const
{
    return m_hasNext;
}

/*!
    Returns whether the current tile is valid,
    invalid tiles occur beyond the north and south pole.
*/
bool QGeoMapViewportNokia::TileIterator::isValid() const
{
    return m_valid;
}

/*!
    Returns the column index of the current map tile.
*/
qint32 QGeoMapViewportNokia::TileIterator::col() const
{
    return m_col;
}

/*!
    Returns the row index of the current map tile.
*/
qint32 QGeoMapViewportNokia::TileIterator::row() const
{
    return m_row;
}

qint64 QGeoMapViewportNokia::TileIterator::index() const
{
    return m_tileIndex;
}

/*!
    Returns the bounding box of the map tile (in map pixel coordinates).
*/
QRectF QGeoMapViewportNokia::TileIterator::tileRect() const
{
    return m_rect;
}
