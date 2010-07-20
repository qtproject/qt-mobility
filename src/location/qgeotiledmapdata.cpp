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

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeotiledmappingmanagerengine.h"
#include "qgeotiledmappingmanagerengine_p.h"
#include "qgeotiledmaprequest.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomaprectangleobject.h"
#include "qgeomapmarkerobject.h"
#include "qgeomappolylineobject.h"
#include "qgeomappolygonobject.h"
#include "qgeomaprouteobject.h"
#include "qgeoroutesegment.h"

#include "qgeomaprectangleobject_p.h"
#include "qgeomapcircleobject_p.h"
#include "qgeomappolylineobject_p.h"
#include "qgeomappolygonobject_p.h"
#include "qgeomapmarkerobject_p.h"
#include "qgeomaprouteobject_p.h"

#include <QTimer>
#include <QImage>

#include <QDebug>

#define DEFAULT_ZOOMLEVEL 8
#define PI 3.14159265
#define HIT_DETECTION_COLOR qRgba(0, 0, 255, 127) //semi-transparent blue

#include <math.h>

uint qHash(const QRectF& key)
{
    uint result = qHash(qRound(key.x()));
    result += qHash(qRound(key.y()));
    result += qHash(qRound(key.width()));
    result += qHash(qRound(key.height()));
    return result;
}

QTM_BEGIN_NAMESPACE

QGeoTiledMapData::QGeoTiledMapData(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
        : QGeoMapData(new QGeoTiledMapDataPrivate(engine, widget, this))
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    setZoomLevel(8.0);

    d->maxZoomSize = (1 << qRound(tileEngine->maximumZoomLevel())) * tileEngine->tileSize();

    // TODO get this from the engine, which should give different values depending on if this is running on a device or not
    d->cache.setMaxCost(10 * 1024 * 1024);
    d->zoomCache.setMaxCost(10 * 1024 * 1024);
    d->emptyTile = QPixmap(tileEngine->tileSize());
    d->emptyTile.fill(Qt::lightGray);
}

QGeoTiledMapData::~QGeoTiledMapData()
{
}

QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoTiledMapData);

    qulonglong worldX;
    qulonglong worldY;

    coordinateToWorldPixel(coordinate, &worldX, &worldY);

    if (!d->maxZoomScreenRect.contains(QPoint(worldX, worldY)))
        return QPointF();

    QPointF pos(worldX, worldY);

    pos -= d->maxZoomScreenRect.topLeft();
    pos /= d->zoomFactor * 1.0;

    return pos;
}

QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapData);

    QRectF bounds = QRectF(QPointF(0.0, 0.0), d->viewportSize);
    if (!bounds.contains(screenPosition))
        return QGeoCoordinate();

    qulonglong worldX = qulonglong(d->maxZoomScreenRect.left() + screenPosition.x() * d->zoomFactor) % d->maxZoomSize.width();
    qulonglong worldY = qulonglong(d->maxZoomScreenRect.top() + screenPosition.y() * d->zoomFactor) % d->maxZoomSize.height();

    return worldPixelToCoordinate(worldX, worldY);
}

// TODO check whether we need it at all
/*!
    Returns the (row,col) index of the tile that \a screenPosition lies on.
    The screen position (0,0) represents the top-left corner of the current map view.
*/
QPoint QGeoTiledMapData::screenPositionToTileIndices(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapData);

    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    QPointF pos = d->maxZoomScreenRect.topLeft() + screenPosition * d->zoomFactor;

    int numCols = 1 << static_cast<int>(zoomLevel());
    int tileX = static_cast<int>(pos.x() / tileEngine->tileSize().width()) % numCols;
    int tileY = static_cast<int>(pos.y() / tileEngine->tileSize().height()) % numCols;

    return QPoint(tileX, tileY);
}

void QGeoTiledMapData::coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const
{
    Q_D(const QGeoTiledMapData);

    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    *x = qulonglong(lng * d->maxZoomSize.width());
    *y = qulonglong(lat * d->maxZoomSize.height());
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapData::worldPixelToCoordinate(qulonglong x, qulonglong y) const
{
    Q_D(const QGeoTiledMapData);

    qreal fx = qreal(x) / d->maxZoomSize.width();
    qreal fy = qreal(y) / d->maxZoomSize.height();

    if (fy < 0.0f)
        fy = 0.0f;
    else if (fy > 1.0f)
        fy = 1.0f;

    qreal lat;

    if (fy == 0.0f)
        lat = 90.0f;
    else if (fy == 1.0f)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * fy))) - (PI / 2.0f));

    qreal lng;
    if (fx >= 0) {
        lng = rmod(fx, 1.0f);
    } else {
        lng = rmod(1.0f - rmod(-1.0f * fx, 1.0f), 1.0f);
    }

    lng = lng * 360.0f - 180.0f;

    return QGeoCoordinate(lat, lng);
}

void QGeoTiledMapData::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoTiledMapData);

    qulonglong x;
    qulonglong y;
    coordinateToWorldPixel(center, &x, &y);

    d->maxZoomCenter.setX(x);
    d->maxZoomCenter.setY(y);
    d->updateScreenRect();


    updateMapImage();
}

void QGeoTiledMapData::setMapType(QGeoMapWidget::MapType mapType)
{
    QGeoMapData::setMapType(mapType);

    clearRequests();

    updateMapImage();
}

QGeoCoordinate QGeoTiledMapData::center() const
{
    Q_D(const QGeoTiledMapData);

    return worldPixelToCoordinate(d->maxZoomCenter.x(), d->maxZoomCenter.y());
}

void QGeoTiledMapData::setZoomLevel(qreal zoomLevel)
{
    Q_D(QGeoTiledMapData);

    QPixmap oldImage(viewportSize().toSize());
    if (d->zoomLevel != -1.0) {
        // grab the old image
        QPainter painter1(&oldImage);
        paintMap(&painter1, 0);
        painter1.end();
    }

    qreal oldZoomLevel = d->zoomLevel;

    QGeoMapData::setZoomLevel(zoomLevel);

    // QGeoMapData::setZoomLevel clips the zoom level to be
    // in between the minimum and maximum zoom levels
    zoomLevel = QGeoMapData::zoomLevel();

    int zoomDiff = qRound(zoomLevel - oldZoomLevel);

    if (zoomDiff == 0)
        return;

    d->zoomFactor = 1 << qRound(d->engine->maximumZoomLevel() - d->zoomLevel);

    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());

    d->updateScreenRect();

    if (oldZoomLevel == -1.0) {
        updateMapImage();
        return;
    }

    //scale old image
    QRectF target = oldImage.rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.height() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap newImage(oldImage.size());
    newImage.fill(Qt::lightGray);
    QPainter painter2(&newImage);
    if (zoomDiff < 0) {
        painter2.drawPixmap(source, oldImage, target);
    } else {
        painter2.drawPixmap(target, oldImage, source);
    }
    painter2.end();

    d->zoomCache.clear();

    QGeoTileIterator it(d->maxZoomScreenRect, tileEngine->tileSize() * d->zoomFactor, qRound(QGeoMapData::zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRect tileRect = it.tileRect();

        // populate zoom cache

        if (!d->maxZoomScreenRect.intersects(tileRect))
        //if (!d->maxZoomScreenRect.contains(tileRect))
            continue;

        QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);

        if (d->cache.contains(req))
            continue;

        QPixmap *tile = new QPixmap(tileEngine->tileSize());
        tile->fill(Qt::lightGray);

        QRectF target = QRectF(QPointF(0.0, 0.0), tileEngine->tileSize());
        QRectF source = QRectF(int((tileRect.left() - d->maxZoomScreenRect.left()) / d->zoomFactor),
                               int((tileRect.top() - d->maxZoomScreenRect.top()) / d->zoomFactor),
                               int(tileRect.width() / d->zoomFactor),
                               int(tileRect.height() / d->zoomFactor));

        QPainter painter3(tile);
        painter3.drawPixmap(target, newImage, source);
        painter3.end();

        d->zoomCache.insert(req, tile, (tile->depth() * tile->width() * tile->height()) / 8);
    }

    widget()->update();

    clearRequests();
    updateMapImage();
}

void QGeoTiledMapData::setViewportSize(const QSizeF &size)
{
    Q_D(QGeoTiledMapData);

    QGeoMapData::setViewportSize(size);

    d->updateScreenRect();

    updateMapImage();
}

void QGeoTiledMapData::startPanning()
{
    //setImageChangesTriggerUpdates(false);
}

void QGeoTiledMapData::stopPanning()
{
    //setImageChangesTriggerUpdates(true);
}

void QGeoTiledMapData::pan(int dx, int dy)
{
    Q_D(QGeoTiledMapData);

    int x = d->maxZoomCenter.x();
    int y = d->maxZoomCenter.y();

    x = (x + dx * d->zoomFactor) % d->maxZoomSize.width();
    // TODO limit viewport from going past +/- 90 degrees latitude?
    y = (y + dy * d->zoomFactor);

    d->maxZoomCenter.setX(x);
    d->maxZoomCenter.setY(y);

    d->updateScreenRect();

    updateMapImage();
}

void QGeoTiledMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);
    paintMapOverlay(painter, option);
}

void QGeoTiledMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_D(const QGeoTiledMapData);

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QGeoTileIterator it(d->maxZoomScreenRect, tiledEngine->tileSize() * d->zoomFactor, qRound(zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRect tileRect = it.tileRect();

        QRect overlap = tileRect.intersected(d->maxZoomScreenRect);

        if (!overlap.isEmpty()) {
            QRectF source = QRectF(int((overlap.left() - tileRect.left()) / d->zoomFactor),
                                   int((overlap.top() - tileRect.top()) / d->zoomFactor),
                                   int(overlap.width() / d->zoomFactor),
                                   int(overlap.height() / d->zoomFactor));

            QRectF target = QRectF(int((overlap.left() - d->maxZoomScreenRect.left()) / d->zoomFactor),
                                   int((overlap.top() - d->maxZoomScreenRect.top()) / d->zoomFactor),
                                   int(overlap.width() / d->zoomFactor),
                                   int(overlap.height() / d->zoomFactor));

            QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);
            if (d->cache.contains(req)) {
                painter->drawPixmap(target, *d->cache.object(req), source);
            } else {
                if (d->zoomCache.contains(req)) {
                    painter->drawPixmap(target, *d->zoomCache.object(req), source);
                } else {
                    painter->drawPixmap(target, d->emptyTile, source);
                }
            }
        }
    }
}

void QGeoTiledMapData::paintMapOverlay(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    QPixmap objOverlay = mapObjectsOverlay();

    if (!objOverlay.isNull())
        painter->drawPixmap(objOverlay.rect(), objOverlay, objOverlay.rect());
}

void QGeoTiledMapData::updateMapImage()
{
    Q_D(QGeoTiledMapData);

    if (zoomLevel() == -1.0)
        return;

    bool wasEmpty = (d->requests.size() == 0);

    QMutableListIterator<QGeoTiledMapRequest> requestIter(d->requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        if (!d->maxZoomScreenRect.intersects(req.tileRect())) {
            d->requestRects.remove(req.tileRect());
            requestIter.remove();
        }
    }

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QGeoTileIterator it(d->maxZoomScreenRect, tiledEngine->tileSize() * d->zoomFactor, qRound(zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRect tileRect = it.tileRect();

        QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);
        if (!d->cache.contains(req)) {
            if (!d->requestRects.contains(tileRect) && !d->replyRects.contains(tileRect)) {
                d->requests.append(req);
                d->requestRects.insert(tileRect);
            }
        }
    }

//    qWarning()
//            << d->requests.size()
//            << d->cache.size()
//            << d->cache.totalCost()
//            << d->zoomCache.size()
//            << d->zoomCache.totalCost();

    if (wasEmpty && d->requests.size() > 0) {
        QTimer::singleShot(50, this, SLOT(processRequests()));
    }
}

void QGeoTiledMapData::clearRequests()
{
    Q_D(QGeoTiledMapData);

    d->requests.clear();
    d->requestRects.clear();
}

void QGeoTiledMapData::processRequests()
{
    Q_D(QGeoTiledMapData);

    QMutableSetIterator<QGeoTiledMapReply*> replyIter(d->replies);
    //Kill off screen replies
    while (replyIter.hasNext()) {
        QGeoTiledMapReply *reply = replyIter.next();
        if (!d->maxZoomScreenRect.intersects(reply->request().tileRect())
            || (zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
                reply->abort();
                d->replyRects.remove(reply->request().tileRect());
                replyIter.remove();
        }
    }

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QMutableListIterator<QGeoTiledMapRequest> requestIter(d->requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        d->requestRects.remove(req.tileRect());
        requestIter.remove();

        // Do not use the requests which have pending replies or are off screen
        if (d->replyRects.contains(req.tileRect()) || !d->maxZoomScreenRect.intersects(req.tileRect())) {
            continue;
        }

        QGeoTiledMapReply *reply = tiledEngine->getTileImage(req);

        if (!reply) {
            continue;
        }

        if (reply->error() != QGeoTiledMapReply::NoError) {
            tileError(reply->error(), reply->errorString());
            reply->deleteLater();
            continue;
        }

        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(tileFinished()));

        connect(reply,
                SIGNAL(error(QGeoTiledMapReply::Error, QString)),
                this,
                SLOT(tileError(QGeoTiledMapReply::Error, QString)));

        d->replies.insert(reply);
        d->replyRects.insert(reply->request().tileRect());

        break;
    }
}

void QGeoTiledMapData::tileFinished()
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

    d->replyRects.remove(reply->request().tileRect());
    d->replies.remove(reply);

    d->zoomCache.remove(reply->request());

    if (reply->error() != QGeoTiledMapReply::NoError) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    if ((zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QPixmap *tile = new QPixmap();

    if (!tile->loadFromData(reply->mapImageData(), reply->mapImageFormat().toAscii())) {
        delete tile;
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (tile->isNull() || tile->size().isEmpty()) {
        delete tile;
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

    d->cache.insert(reply->request(), tile, (tile->depth() * tile->width() * tile->height()) / 8);

    cleanupCaches();

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QSize tileSize = tiledEngine->tileSize();

    int tileHeight = tileSize.height() * d->zoomFactor;
    int tileWidth = tileSize.width() * d->zoomFactor;

    int numCols = 1 << static_cast<int>(zoomLevel());
    qreal totalPixelWidth = static_cast<qreal>(numCols) * tileWidth;
    QRectF tileRect(tileWidth * reply->request().column(),
                    tileHeight * reply->request().row(),
                    tileWidth, tileHeight);

    while (tileRect.left() <= d->maxZoomScreenRect.right()) {
        QRectF overlap = tileRect.intersected(d->maxZoomScreenRect);
        if (!overlap.isEmpty()) {
            QRectF dest = QRectF(int((overlap.left() - d->maxZoomScreenRect.left()) / d->zoomFactor),
                                 int((overlap.top() - d->maxZoomScreenRect.top()) / d->zoomFactor),
                                 int(overlap.width() / d->zoomFactor),
                                 int(overlap.height() / d->zoomFactor));
            widget()->update(dest);
        }

        tileRect.translate(totalPixelWidth, 0);
    }

    if (d->requests.size() > 0) {
        if (reply->isCached())
            QTimer::singleShot(50, this, SLOT(processRequests()));
        else
            QTimer::singleShot(0, this, SLOT(processRequests()));
    }

    QTimer::singleShot(0, reply, SLOT(deleteLater()));
}

void QGeoTiledMapData::tileError(QGeoTiledMapReply::Error error, QString errorString)
{
    qWarning() << errorString;
}

void QGeoTiledMapData::cleanupCaches()
{
    Q_D(QGeoTiledMapData);

    int boundaryTiles = 3;

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QSize tileSize = tiledEngine->tileSize();

    QRectF cacheRect1;
    QRectF cacheRect2;

    int tileHeight = tileSize.height() * d->zoomFactor;
    int tileWidth = tileSize.width() * d->zoomFactor;
    int zoomHeight = d->maxZoomSize.height();
    int zoomWidth = d->maxZoomSize.width();

    int minY = qMax(int(0), d->maxZoomScreenRect.y() - boundaryTiles * tileHeight);
    int maxY = qMin(zoomHeight, d->maxZoomScreenRect.y() + d->maxZoomScreenRect.height() + boundaryTiles * tileHeight);
    int height = maxY - minY;
    int minX = d->maxZoomScreenRect.x() - boundaryTiles * tileWidth;
    int maxX = d->maxZoomScreenRect.x() + d->maxZoomScreenRect.width() + boundaryTiles * tileWidth;
    int width = 0;

    if ((minX < 0) && (zoomWidth < maxX)) {
        cacheRect1 = QRectF(0, minY, zoomWidth, height);
    } else {
        minX = minX % zoomWidth;
        maxX = maxX % zoomWidth;
        if (minX < maxX) {
            cacheRect1 = QRectF(minX, minY, maxX - minX, height);
        } else {
            cacheRect1 = QRectF(minX, minY, zoomWidth - minX, height);
            cacheRect2 = QRectF(0, minY, maxX, height);
        }
    }

    QList<QGeoTiledMapRequest> keys = d->cache.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QRectF tileRect = keys.at(i).tileRect();
        if (!cacheRect1.intersects(tileRect)) {
            if (cacheRect2.isNull() || !cacheRect2.intersects(tileRect))
                d->cache.remove(keys.at(i));
        }
    }
}

QList<QGeoMapObject*> QGeoTiledMapData::visibleMapObjects()
{
    QList<QGeoMapObject*> visibleObjects;
    QList<QGeoMapObject*> queue(this->mapObjects());

    //iterate through all map objects as defined by their (composite) zValues
    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();

        if (obj->isVisible()) {
            visibleObjects.append(obj);
            //prepend children to queue
            QList<QGeoMapObject*> children = obj->childObjects();
            int sz = children.size();

            for (int i = 0; i < sz; ++i)
                queue.prepend(children.at(i));
        }
    }

    return visibleObjects;
}

/*!
    Returns the list of map objects managed by this map which are visible and
    which are within a pixel \a radius from the \a screenPosition.
    The returned map objects are ordered ascendingly on their zIndices.

    Note that this method takes into consideration potentially set QPen widths of map objects.
    It therefore needs to operate on a pixel comparison and can potentially be slow.
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius)
{
    Q_D(QGeoTiledMapData);

    QPointF worldPosition = screenPosition - d->maxZoomScreenRect.topLeft() / d->zoomFactor;
    QRectF worldRect(0., 0., 2*radius, 2*radius);
    worldRect.moveCenter(worldPosition);
    QSize imgSize = worldRect.size().toSize();
    QRgb paintColor = HIT_DETECTION_COLOR;
    QRgb emptyColor = qRgba(0, 0, 0, 0);
    QList<QGeoMapObject*> mapObjs;
    QList<QGeoMapObject*> queue(this->mapObjects());

    //iterate through all map objects as defined by their (composite) zValues
    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();

        if (obj->isVisible()) {
            //prepend children to queue
            QList<QGeoMapObject*> children = obj->childObjects();
            int sz = children.size();

            for (int i = 0; i < sz; ++i)
                queue.prepend(children.at(i));

            //Now test whether map object intersects circle with center at 
            //screenPosition and given radius.
            //
            //The idea is that we construct a rectangle that exactly contains the
            //circle. The circle is then drawn (and filled out) with a semi-transparent
            //color. We then paint the current map object with the same semi-transparent
            //color into the rectangle. If then any pixel is different from the background
            //color and this semi-transparent paint color, the map object must have intersected
            //(i.e. over-painted parts of) the circle.
            QImage buffer(imgSize, QImage::Format_ARGB32);
            QPainter painter(&buffer);
            painter.fillRect(0, 0, imgSize.width(), imgSize.height(), QColor(emptyColor));
            painter.setPen(QColor(paintColor));
            painter.setBrush(QBrush(QColor(paintColor)));
            painter.drawEllipse(0, 0, imgSize.width(), imgSize.height());
            obj->paint(&painter, worldRect, true);

            //now test whether any pixels in buffer have been over-painted
            for (int x = 0; x < imgSize.width(); x++) {
                bool pixelSet = false;

                for (int y = 0; y < imgSize.height(); y++) {
                    QRgb pixel = buffer.pixel(x, y);

                    if (pixel != emptyColor && pixel != paintColor) {
                        mapObjs.append(obj);
                        pixelSet = true;
                        break;
                    }
                }
                if (pixelSet)
                    break;
            }
        }
    }

    return mapObjs;
}

/*!
    Returns the list of map objects managed by this map which are visible and
    which are displayed at least partially within the on screen rectangle
    \a screenRect. The returned map objects are ordered ascendingly on their zIndices.

    Note that this method takes into consideration potentially set QPen widths of map objects.
    It therefore needs to operate on a pixel comparison and can potentially be slow.
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsInScreenRect(const QRectF &screenRect)
{
    Q_D(QGeoTiledMapData);

    QRectF worldRect = screenRect.translated(d->maxZoomScreenRect.topLeft() / d->zoomFactor);
    QSize imgSize = worldRect.size().toSize();
    QList<QGeoMapObject*> mapObjs;
    QList<QGeoMapObject*> queue(this->mapObjects());

    //iterate through all map objects as defined by their (composite) zValues
    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();

        if (obj->isVisible()) {
            //prepend children to queue
            QList<QGeoMapObject*> children = obj->childObjects();
            int sz = children.size();

            for (int i = 0; i < sz; ++i)
                queue.prepend(children.at(i));

            //now test whether map object intersects screenRect pixel-wise
            QImage buffer(imgSize, QImage::Format_Mono);
            buffer.setColor(0, qRgba(0, 0, 0, 0));
            buffer.setColor(1, HIT_DETECTION_COLOR);
            buffer.fill(0);
            QPainter painter(&buffer);
            obj->paint(&painter, worldRect, true);

            //now test whether any pixels in buffer have been painted
            for (int x = 0; x < imgSize.width(); x++) {
                bool pixelSet = false;

                for (int y = 0; y < imgSize.height(); y++) {
                    if (buffer.pixelIndex(x, y) == 1) {
                        mapObjs.append(obj);
                        pixelSet = true;
                        break;
                    }
                }
                if (pixelSet)
                    break;
            }
        }
    }

    return mapObjs;
}

QPixmap QGeoTiledMapData::mapObjectsOverlay()
{
    Q_D(QGeoTiledMapData);

    QList<QGeoMapObject*> queue;
    queue.append(this->mapObjects());
    bool needsPainting = false;
    QPixmap overlay(d->viewportSize.width(), d->viewportSize.height());
    overlay.fill(Qt::transparent);
    QPainter painter(&overlay);

    //iterate through all map objects as defined by their (composite) zValues
    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();

        // TODO can remove this once route updates itself properly
        obj->update();
        if (obj->intersects(d->maxZoomScreenRect)) {
            needsPainting = true;
            obj->paint(&painter, d->maxZoomScreenRect, false);
        }

        //prepend children to queue
        QList<QGeoMapObject*> children = obj->childObjects();
        int sz = children.size();

        for (int i = 0; i < sz; ++i)
            queue.prepend(children.at(i));
    }

    if (needsPainting)
        return overlay;

    return QPixmap();
}

void QGeoTiledMapData::addMapObject(QGeoMapObject *mapObject)
{
    mapObject->update();
    QGeoMapData::addMapObject(mapObject);
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget, QGeoTiledMapData *q)
    : QGeoMapDataPrivate(engine, widget),
    q_ptr(q) {}

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(const QGeoTiledMapDataPrivate &other)
        : QGeoMapDataPrivate(other),
        zoomFactor(other.zoomFactor),
        maxZoomCenter(other.maxZoomCenter),
        maxZoomSize(other.maxZoomSize),
        maxZoomScreenRect(other.maxZoomScreenRect),
        screenRect(other.screenRect),
        q_ptr(other.q_ptr)
{}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate()
{
    QList<QGeoTiledMapReply*> replyList = replies.toList();
    for (int i = 0; i < replyList.size(); ++i) {
        replyList.at(i)->abort();
        replyList.at(i)->deleteLater();
    }
}

QGeoTiledMapDataPrivate& QGeoTiledMapDataPrivate::operator= (const QGeoTiledMapDataPrivate & other)
{
    QGeoMapDataPrivate::operator =(other);

    zoomFactor = other.zoomFactor;
    maxZoomCenter = other.maxZoomCenter;
    maxZoomSize = other.maxZoomSize;
    maxZoomScreenRect = other.maxZoomScreenRect;
    screenRect = other.screenRect;
    q_ptr = other.q_ptr;

    return *this;
}

void QGeoTiledMapDataPrivate::updateScreenRect()
{
    qreal width = q_ptr->viewportSize().width();
    qreal height = q_ptr->viewportSize().height();

    int x = (maxZoomCenter.x() - (static_cast<int>(width * zoomFactor) / 2)) % maxZoomSize.width();
    //TODO squash screen rectangle if we decide that's what we're doing near the poles
    int y = maxZoomCenter.y() - (static_cast<int>(height * zoomFactor) / 2);

    maxZoomScreenRect = QRect(x, y, static_cast<int>(width * zoomFactor), static_cast<int>(height * zoomFactor));
    screenRect = QRectF(qreal(x) / zoomFactor, qreal(y) / zoomFactor, width, height);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapRectangleObjectInfo(mapObjectPrivate);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapCircleObjectInfo(mapObjectPrivate);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapPolylineObjectInfo(mapObjectPrivate);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapPolygonObjectInfo(mapObjectPrivate);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapMarkerObjectInfo(mapObjectPrivate);
}

QGeoMapObjectInfo* QGeoTiledMapDataPrivate::createRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const
{
    return new QGeoTiledMapRouteObjectInfo(mapObjectPrivate);
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate)
{
    rectangle = static_cast<const QGeoMapRectangleObjectPrivate*>(mapObjectPrivate);
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

void QGeoTiledMapRectangleObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();

    if (!hitDetection) {
        painter->setPen(rectangle->pen);
        painter->setBrush(rectangle->brush);
    }
    else {
        QPen pen(rectangle->pen);
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter->setPen(pen);
        painter->setBrush(QColor(HIT_DETECTION_COLOR));
    }

    QRectF rect = QRectF(int((boundingBox.left() - viewPort.left()) / mapData->zoomFactor),
                         int((boundingBox.top() - viewPort.top()) / mapData->zoomFactor),
                         int(boundingBox.width() / mapData->zoomFactor),
                         int(boundingBox.height() / mapData->zoomFactor));

    painter->drawRect(rect);
    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

void QGeoTiledMapRectangleObjectInfo::update()
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;

    mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->boundingBox.topLeft(), &topLeftX, &topLeftY);
    mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->boundingBox.bottomRight(), &bottomRightX, &bottomRightY);

    boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapCircleObjectInfo::QGeoTiledMapCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate) {}

QGeoTiledMapCircleObjectInfo::~QGeoTiledMapCircleObjectInfo() {}

void QGeoTiledMapCircleObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
}

void QGeoTiledMapCircleObjectInfo::update()
{
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapPolylineObjectInfo::QGeoTiledMapPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate)
{
    polyline = static_cast<const QGeoMapPolylineObjectPrivate*>(mapObjectPrivate);
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapPolylineObjectInfo::~QGeoTiledMapPolylineObjectInfo() {}

void QGeoTiledMapPolylineObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
    if (points.size() == 0)
        return;

    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();

    if (!hitDetection)
        painter->setPen(polyline->pen);
    else {
        QPen pen(polyline->pen);
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter->setPen(pen);
    }

    QPainterPath paintPath = QPainterPath(QPointF(int((points.at(0).x() - viewPort.left()) / mapData->zoomFactor),
                                                  int((points.at(0).y() - viewPort.top()) / mapData->zoomFactor)));
    for (int i = 1; i < points.size(); ++i) {
        paintPath.lineTo(QPointF(int((points.at(i).x() - viewPort.left()) / mapData->zoomFactor),
                                 int((points.at(i).y() - viewPort.top()) / mapData->zoomFactor)));
    }

    painter->drawPath(paintPath);

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

void QGeoTiledMapPolylineObjectInfo::update()
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;

    mapData->q_ptr->coordinateToWorldPixel(polyline->boundingBox.topLeft(), &topLeftX, &topLeftY);
    mapData->q_ptr->coordinateToWorldPixel(polyline->boundingBox.bottomRight(), &bottomRightX, &bottomRightY);

    boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));

    QList<QGeoCoordinate> path = polyline->path;

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        qulonglong x;
        qulonglong y;
        mapData->q_ptr->coordinateToWorldPixel(coord, &x, &y);

        points.append(QPoint(x, y));
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapPolygonObjectInfo::QGeoTiledMapPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate)
{
    polygon = static_cast<const QGeoMapPolygonObjectPrivate*>(mapObjectPrivate);
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapPolygonObjectInfo::~QGeoTiledMapPolygonObjectInfo() {}

void QGeoTiledMapPolygonObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();

    if (!hitDetection)
        painter->setPen(polygon->pen);
    else {
        QPen pen(polygon->pen);
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter->setPen(pen);
    }

    if (!hitDetection)
        painter->setBrush(polygon->brush);
    else
        painter->setBrush(QColor(HIT_DETECTION_COLOR));

    QPainterPath paintPath = QPainterPath(QPointF(int((points.at(0).x() - viewPort.left()) / mapData->zoomFactor),
                                                  int((points.at(0).y() - viewPort.top()) / mapData->zoomFactor)));
    for (int i = 1; i < points.size(); ++i) {
        paintPath.lineTo(QPointF(int((points.at(i).x() - viewPort.left()) / mapData->zoomFactor),
                                 int((points.at(i).y() - viewPort.top()) / mapData->zoomFactor)));
    }

    painter->drawPath(paintPath);

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

void QGeoTiledMapPolygonObjectInfo::update()
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;

    mapData->q_ptr->coordinateToWorldPixel(polygon->boundingBox.topLeft(), &topLeftX, &topLeftY);
    mapData->q_ptr->coordinateToWorldPixel(polygon->boundingBox.bottomRight(), &bottomRightX, &bottomRightY);

    boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));

    QList<QGeoCoordinate> path = polygon->path;

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        qulonglong x;
        qulonglong y;
        mapData->q_ptr->coordinateToWorldPixel(coord, &x, &y);

        points.append(QPoint(x, y));
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapMarkerObjectInfo::QGeoTiledMapMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate)
{
    marker = static_cast<const QGeoMapMarkerObjectPrivate*>(mapObjectPrivate);
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapMarkerObjectInfo::~QGeoTiledMapMarkerObjectInfo() {}

void QGeoTiledMapMarkerObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
    QPixmap icon = marker->icon;

    if (icon.isNull())
        return;

    QRectF rect = QRectF(QPointF(int(marker->anchor.x() + (boundingBox.left() - viewPort.left()) / mapData->zoomFactor),
                                 int(marker->anchor.y() + (boundingBox.top() - viewPort.top()) / mapData->zoomFactor)),
                        boundingBox.size());

    if (!hitDetection)
        painter->drawPixmap(rect, icon, QRectF(QPointF(0, 0), icon.size()));
    else {
        painter->setPen(QPen(QColor(HIT_DETECTION_COLOR)));
        painter->drawRect(rect);
    }
}

void QGeoTiledMapMarkerObjectInfo::update()
{
    qulonglong topLeftX;
    qulonglong topLeftY;

    mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->boundingBox.topLeft(), &topLeftX, &topLeftY);

    QPointF topLeft(topLeftX, topLeftY);
    boundingBox = QRectF(topLeft, marker->icon.size());
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRouteObjectInfo::QGeoTiledMapRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate)
{
    route = static_cast<const QGeoMapRouteObjectPrivate*>(mapObjectPrivate);
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapRouteObjectInfo::~QGeoTiledMapRouteObjectInfo() {}

void QGeoTiledMapRouteObjectInfo::paint(QPainter *painter, const QRectF &viewPort, bool hitDetection)
{
    QSet<int> alreadyPainted;
    QPainterPath path;

    //TODO: add some type check
    QGeoTiledMappingManagerEngine *tiledEngine = static_cast<QGeoTiledMappingManagerEngine*>(mapData->engine);

    QGeoTileIterator it(viewPort.toRect(), tiledEngine->tileSize() * mapData->zoomFactor, qRound(mapData->zoomLevel));

    while (it.hasNext()) {
        it.next();
        qulonglong key = tileKey(it.row(), it.col(), it.zoomLevel());

        if (intersectedTiles.contains(key)) {
            QListIterator<QPair<int, QLineF> > segments(intersectedTiles[key]);

            while (segments.hasNext()) {
                const QPair<int, QLineF> &currSegment = segments.next();

                if (!alreadyPainted.contains(currSegment.first)) {
                    QPointF p1 = currSegment.second.p1();
                    QPointF p2 = currSegment.second.p2();

                    path.moveTo(QPointF(int((p1.x() - viewPort.left()) / mapData->zoomFactor),
                                        int((p1.y() - viewPort.top()) / mapData->zoomFactor)));
                    path.lineTo(QPointF(int((p2.x() - viewPort.left()) / mapData->zoomFactor),
                                        int((p2.y() - viewPort.top()) / mapData->zoomFactor)));

                    alreadyPainted.insert(currSegment.first);
                }
            }
        }
    }

    QPen oldPen = painter->pen();

    if (!hitDetection)
        painter->setPen(route->pen);
    else {
        QPen pen(route->pen);
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter->setPen(pen);
    }

    painter->drawPath(path);

    painter->setPen(oldPen);
}

void QGeoTiledMapRouteObjectInfo::update()
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;

    mapData->q_ptr->coordinateToWorldPixel(route->boundingBox.topLeft(), &topLeftX, &topLeftY);
    mapData->q_ptr->coordinateToWorldPixel(route->boundingBox.bottomRight(), &bottomRightX, &bottomRightY);

    boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));

    quint32 minDist = route->detailLevel;
    QGeoCoordinate last;
    QGeoCoordinate here;
    int lineIndex = 0;
    QListIterator<QGeoRouteSegment> segIt(route->route.routeSegments());

    while (segIt.hasNext()) {
        QListIterator<QGeoCoordinate> coordIt(segIt.next().path());

        while (coordIt.hasNext()) {
            QGeoCoordinate curr = coordIt.next();

            //make sure first waypoint is always shown
            if (!last.isValid()) {
                last = curr;
                continue;
            }

            here = curr;
            QLineF line = connectShortest(here, last);
            qreal dist = (line.p1() - line.p2()).manhattanLength();

            if (dist >= minDist || mapData->zoomLevel == mapData->engine->maximumZoomLevel()) {
                addRouteSegmentInfo(line, lineIndex++);
                last = here;
                here = QGeoCoordinate();
            }
        }
    }

    //make sure last waypoint is always shown
    if (here.isValid()) {
        QLineF line = connectShortest(here, last);
        addRouteSegmentInfo(line, lineIndex++);
    }
}

qulonglong QGeoTiledMapRouteObjectInfo::tileKey(int row, int col, int zoomLevel) const
{
    qulonglong result = 1 << (zoomLevel);
    result *= row;
    result += col;
    return result;
}

QLineF QGeoTiledMapRouteObjectInfo::connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const
{
    //order from west to east
    QGeoCoordinate pt1;
    QGeoCoordinate pt2;

    if (point1.longitude() < point2.longitude()) {
        pt1 = point1;
        pt2 = point2;
    } else {
        pt1 = point2;
        pt2 = point1;
    }

    qulonglong x;
    qulonglong y;
    mapData->q_ptr->coordinateToWorldPixel(pt1, &x, &y);
    QPointF mpt1(x, y);
    mapData->q_ptr->coordinateToWorldPixel(pt2, &x, &y);
    QPointF mpt2(x, y);

    if (pt2.longitude() - pt1.longitude() > 180.0) {
        mpt1.rx() += mapData->maxZoomSize.width();
        return QLineF(mpt2, mpt1);
    }

    return QLineF(mpt1, mpt2);
}

void QGeoTiledMapRouteObjectInfo::addRouteSegmentInfo(const QLineF &line, int index)
{
    //TODO: add some type check
    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(mapData->engine);
    QGeoTileIterator it(mapData->maxZoomScreenRect, tiledEngine->tileSize() * mapData->zoomFactor, qRound(mapData->zoomLevel));

    while (it.hasNext()) {
        it.next();
        qulonglong key = tileKey(it.row(), it.col(), it.zoomLevel());

        if (!intersectedTiles.contains(key))
            intersectedTiles.insert(key, QList< QPair<int, QLineF> >());

        intersectedTiles[key].append(QPair<int, QLineF>(index, line));
    }
}

///*******************************************************************************
//*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QRect &screenRect, const QSize &tileSize, int zoomLevel)
    : aHasNext(true), aRow(-1), aCol(-1), aScreenRect(screenRect),
    aTileSize(tileSize), aZoomLevel(zoomLevel), aTileRect(QPoint(0,0), tileSize)
{
    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

bool QGeoTileIterator::hasNext()
{
    return aHasNext;
}

QRect QGeoTileIterator::tileRect() const
{
    return aTileRect;
}

void QGeoTileIterator::next()
{
    int numCols = 1 << aZoomLevel;
    aCol = static_cast<int>(currTopLeft.x() / aTileSize.width()) % numCols;
    aRow = static_cast<int>(currTopLeft.y() / aTileSize.height()) % numCols;
    aTileRect.moveTopLeft(currTopLeft);

    currTopLeft.rx() += aTileSize.width();

    if (currTopLeft.x() > aScreenRect.right()) { //next row
        qulonglong x = static_cast<qulonglong>(aScreenRect.topLeft().x() / aTileSize.width());
        currTopLeft.setX(x * aTileSize.width());
        currTopLeft.ry() += aTileSize.height();
    }

    if (currTopLeft.y() > aScreenRect.bottom()) //done
        aHasNext = false;
}

int QGeoTileIterator::row() const
{
    return aRow;
}

int QGeoTileIterator::col() const
{
    return aCol;
}

int QGeoTileIterator::zoomLevel() const
{
    return aZoomLevel;
}

#include "moc_qgeotiledmapdata.cpp"

QTM_END_NAMESPACE
