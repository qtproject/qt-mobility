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
        : QGeoMapData(engine, widget),
        d_ptr(new QGeoTiledMapDataPrivate(this))
{
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    QGeoMapData::setZoomLevel(8.0);
    d_ptr->width = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().width();
    d_ptr->height = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().height();

    d_ptr->maxZoomSize = (1 << qRound(tileEngine->maximumZoomLevel())) * tileEngine->tileSize();

    // TODO get this from the engine, which should give different values depending on if this is running on a device or not
    d_ptr->cache.setMaxCost(10 * 1024 * 1024);
    d_ptr->zoomCache.setMaxCost(10 * 1024 * 1024);
    d_ptr->emptyTile = QPixmap(tileEngine->tileSize());
    d_ptr->emptyTile.fill(Qt::lightGray);
}

QGeoTiledMapData::~QGeoTiledMapData()
{
}

// TODO check accuracy
QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    qulonglong worldX;
    qulonglong worldY;

    coordinateToWorldPixel(coordinate, &worldX, &worldY);

    QPointF pos(worldX, worldY);
    pos -= d_ptr->screenRect.topLeft();
    return pos;
}

// TODO check accuracy
QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    qulonglong worldX = static_cast<qulonglong>(d_ptr->screenRect.left() + screenPosition.x()) % d_ptr->width;
    qulonglong worldY = static_cast<qulonglong>(d_ptr->screenRect.top() + screenPosition.y()) % d_ptr->height;

    return worldPixelToCoordinate(worldX, worldY);
}

// TODO check accuracy
/*!
    Returns the (row,col) index of the tile that \a screenPosition lies on.
    The screen position (0,0) represents the top-left corner of the current map view.
*/
QPoint QGeoTiledMapData::screenPositionToTileIndices(const QPointF &screenPosition) const
{
    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    QPointF pos = d_ptr->screenRect.topLeft() + screenPosition;

    int numCols = 1 << static_cast<int>(zoomLevel());
    int tileX = static_cast<int>(pos.x() / tileEngine->tileSize().width()) % numCols;
    int tileY = static_cast<int>(pos.y() / tileEngine->tileSize().height()) % numCols;

    return QPoint(tileX, tileY);
}

void QGeoTiledMapData::coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const
{
    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    *x = qRound64(lng * qreal(d_ptr->maxZoomSize.width()));
    *y = qRound64(lat * qreal(d_ptr->maxZoomSize.height()));
}


qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapData::worldPixelToCoordinate(qulonglong x, qulonglong y) const
{
    qreal fx = qreal(x) / qreal(d_ptr->maxZoomSize.width());
    qreal fy = qreal(y) / qreal(d_ptr->maxZoomSize.height());

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
    qulonglong x;
    qulonglong y;
    coordinateToWorldPixel(center, &x, &y);

    d_ptr->maxZoomCenter.setX(x);
    d_ptr->maxZoomCenter.setY(y);
    d_ptr->updateScreenRect();

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
    return worldPixelToCoordinate(d_ptr->maxZoomCenter.x(), d_ptr->maxZoomCenter.y());
}

void QGeoTiledMapData::setZoomLevel(qreal zoomLevel)
{
    // grab the old image
    QPixmap oldImage(viewportSize().toSize());
    QPainter painter1(&oldImage);
    paintMap(&painter1, 0);
    painter1.end();

    qreal oldZoomLevel = QGeoMapData::zoomLevel();

    QGeoMapData::setZoomLevel(zoomLevel);

    // QGeoMapData::setZoomLevel clips the zoom level to be
    // in between the minimum and maximum zoom levels
    zoomLevel = QGeoMapData::zoomLevel();

    int zoomDiff = qRound(zoomLevel - oldZoomLevel);

    if (zoomDiff == 0)
        return;

    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    d_ptr->width = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().width();
    d_ptr->height = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().height();

    d_ptr->updateScreenRect();

    //scale old image
    QRectF target = oldImage.rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.width() / (1 << qAbs(zoomDiff));
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

    d_ptr->zoomCache.clear();

    QGeoTileIterator it(d_ptr->screenRect, tileEngine->tileSize(), qRound(QGeoMapData::zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRectF tileRect = it.tileRect();

        // populate zoom cache

        //if (!d_ptr->screenRect.intersects(tileRect))
        if (!d_ptr->screenRect.contains(tileRect))
            continue;

        QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);

        if (d_ptr->cache.contains(req))
            continue;

        QPixmap *tile = new QPixmap(tileEngine->tileSize());
        tile->fill(Qt::lightGray);

        QRectF target = QRectF(QPointF(0.0, 0.0), tileEngine->tileSize());
        QRectF source = tileRect.translated(-d_ptr->screenRect.x(), -d_ptr->screenRect.y());

        QPainter painter3(tile);
        painter3.drawPixmap(target, newImage, source);
        painter3.end();

        d_ptr->zoomCache.insert(req, tile, (tile->depth() * tile->width() * tile->height()) / 8);
    }

    widget()->update();

    //clear obj info
    d_ptr->clearObjInfo();

    clearRequests();
    updateMapImage();
}

void QGeoTiledMapData::setViewportSize(const QSizeF &size)
{
    QGeoMapData::setViewportSize(size);

    d_ptr->updateScreenRect();

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
    int x = d_ptr->maxZoomCenter.x();
    int y = d_ptr->maxZoomCenter.y();

    int zoomFactor = 1 << qRound(engine()->maximumZoomLevel() - zoomLevel());

    x = (x + (dx * zoomFactor)) % d_ptr->maxZoomSize.width();
    // TODO - limit viewport from going past +/- 90 latitude?
    y = y + (dy * zoomFactor);

    d_ptr->maxZoomCenter.setX(x);
    d_ptr->maxZoomCenter.setY(y);

    d_ptr->updateScreenRect();

    updateMapImage();
}

void QGeoTiledMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);
    paintMapOverlay(painter, option);
}

void QGeoTiledMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());
    QGeoTileIterator it(d_ptr->screenRect, tiledEngine->tileSize(), qRound(zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRectF tileRect = it.tileRect();

        QRectF overlap = tileRect.intersected(d_ptr->screenRect);

        //if (!overlap.isEmpty()) {
        QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
        QRectF target = overlap.translated(-1.0 * d_ptr->screenRect.x(), -1.0 * d_ptr->screenRect.y());

        QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);
        if (d_ptr->cache.contains(req)) {
            painter->drawPixmap(target, *d_ptr->cache.object(req), source);
        } else {
            if (d_ptr->zoomCache.contains(req)) {
                painter->drawPixmap(target, *d_ptr->zoomCache.object(req), source);
            } else {
                painter->drawPixmap(target, d_ptr->emptyTile, source);
            }
        }
        //}
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
    if (zoomLevel() == -1.0)
        return;

    bool wasEmpty = (d_ptr->requests.size() == 0);

    QMutableListIterator<QGeoTiledMapRequest> requestIter(d_ptr->requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        if (!d_ptr->screenRect.intersects(req.tileRect())) {
            d_ptr->requestRects.remove(req.tileRect());
            requestIter.remove();
        }
    }

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QGeoTileIterator it(d_ptr->screenRect, tiledEngine->tileSize(), qRound(zoomLevel()));

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRectF tileRect = it.tileRect();

        QGeoTiledMapRequest req = QGeoTiledMapRequest(this, row, col, tileRect);
        if (!d_ptr->cache.contains(req)) {
            if (!d_ptr->requestRects.contains(tileRect) && !d_ptr->replyRects.contains(tileRect)) {
                d_ptr->requests.append(req);
                d_ptr->requestRects.insert(tileRect);
            }
        }
    }

    qWarning()
            << d_ptr->requests.size()
            << d_ptr->cache.size()
            << d_ptr->cache.totalCost()
            << d_ptr->zoomCache.size()
            << d_ptr->zoomCache.totalCost();

    if (wasEmpty && d_ptr->requests.size() > 0) {
        QTimer::singleShot(50, this, SLOT(processRequests()));
    }
}

void QGeoTiledMapData::clearRequests()
{
    d_ptr->requests.clear();
    d_ptr->requestRects.clear();
}

void QGeoTiledMapData::processRequests()
{
    QMutableSetIterator<QGeoTiledMapReply*> replyIter(d_ptr->replies);
    //Kill off screen replies
    while (replyIter.hasNext()) {
        QGeoTiledMapReply *reply = replyIter.next();
        if (!d_ptr->screenRect.intersects(reply->request().tileRect())
            || (zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
                reply->abort();
                d_ptr->replyRects.remove(reply->request().tileRect());
                replyIter.remove();
        }
    }

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QMutableListIterator<QGeoTiledMapRequest> requestIter(d_ptr->requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        d_ptr->requestRects.remove(req.tileRect());
        requestIter.remove();

        // Do not use the requests which have pending replies or are off screen
        if (d_ptr->replyRects.contains(req.tileRect()) || !d_ptr->screenRect.intersects(req.tileRect())) {
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

        d_ptr->replies.insert(reply);
        d_ptr->replyRects.insert(reply->request().tileRect());

        break;
    }
}

void QGeoTiledMapData::tileFinished()
{
    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

    d_ptr->replyRects.remove(reply->request().tileRect());
    d_ptr->replies.remove(reply);

    d_ptr->zoomCache.remove(reply->request());

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

    d_ptr->cache.insert(reply->request(), tile, (tile->depth() * tile->width() * tile->height()) / 8);

    cleanupCaches();

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QSize tileSize = tiledEngine->tileSize();

    int tileHeight = tileSize.height();
    int tileWidth = tileSize.width();

    int numCols = 1 << static_cast<int>(zoomLevel());
    qreal totalPixelWidth = static_cast<qreal>(numCols) * tileWidth;
    QRectF tileRect(tileWidth * reply->request().column(),
                    tileHeight * reply->request().row(),
                    tileWidth, tileHeight);

    while (tileRect.left() <= d_ptr->screenRect.right()) {
        QRectF overlap = tileRect.intersected(d_ptr->screenRect);

        if (!overlap.isEmpty()) {
            QRectF dest = overlap.translated(-1.0 * d_ptr->screenRect.x(), -1.0 * d_ptr->screenRect.y());
            widget()->update(dest);
        }

        tileRect.translate(totalPixelWidth, 0);
    }

    if (d_ptr->requests.size() > 0) {
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
    int boundaryTiles = 3;

    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(engine());

    QSize tileSize = tiledEngine->tileSize();

    int tileHeight = tileSize.height();
    int tileWidth = tileSize.width();

    QRectF cacheRect1;
    QRectF cacheRect2;

    int minY = qMax(0.0, d_ptr->screenRect.y() - boundaryTiles * tileHeight);
    int maxY = qMin(qreal(d_ptr->height), d_ptr->screenRect.y() + d_ptr->screenRect.height() + boundaryTiles * tileHeight);
    int height = maxY - minY;
    int minX = d_ptr->screenRect.x() - boundaryTiles * tileWidth;
    int maxX = d_ptr->screenRect.x() + d_ptr->screenRect.width() + boundaryTiles * tileWidth;
    int width = 0;

    if ((minX < 0) && (d_ptr->width < maxX)) {
        cacheRect1 = QRectF(0, minY, d_ptr->width, height);
    } else {
        minX = minX % d_ptr->width;
        maxX = maxX % d_ptr->width;
        if (minX < maxX) {
            cacheRect1 = QRectF(minX, minY, maxX - minX, height);
        } else {
            cacheRect1 = QRectF(minX, minY, d_ptr->width - minX, height);
            cacheRect2 = QRectF(0, minY, maxX, height);
        }
    }

    QList<QGeoTiledMapRequest> keys = d_ptr->cache.keys();
    for (int i = 0; i < keys.size(); ++i) {
        if (!cacheRect1.intersects(keys.at(i).tileRect())) {
            if (cacheRect2.isNull() || !cacheRect2.intersects(keys.at(i).tileRect()))
                d_ptr->cache.remove(keys.at(i));
        }
    }
}

QRectF QGeoTiledMapData::screenRect() const
{
    return d_ptr->screenRect;
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

QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius)
{
    return QList<QGeoMapObject*>();
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
    QRectF worldRect = screenRect.translated(d_ptr->screenRect.topLeft());
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
            d_ptr->paintMapObject(painter, worldRect, obj, true);

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
    QList<QGeoMapObject*> queue;
    queue.append(this->mapObjects());
    bool needsPainting = false;
    QPixmap overlay(d_ptr->screenRect.width(), d_ptr->screenRect.height());
    overlay.fill(Qt::transparent);
    QPainter painter(&overlay);

    //iterate through all map objects as defined by their (composite) zValues
    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();

        if (d_ptr->intersects(obj, d_ptr->screenRect)) {
            needsPainting = true;
            d_ptr->paintMapObject(painter, d_ptr->screenRect, obj);
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
    //we need to connect this object's and its children's
    //childObjectRemoved signal to keep objInfo up-to-date
    QList<QGeoMapObject*> queue;
    queue.append(mapObject);

    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();
        //add all children to queue
        queue.append(obj->childObjects());
        
        //connect to childObjectRemoved signal
        connect(obj, SIGNAL(childObjectRemoved(QGeoMapObject*)),
                this, SLOT(mapObjectRemoved(QGeoMapObject*)));
    }

    d_ptr->calculateInfo(mapObject);
    QGeoMapData::addMapObject(mapObject);
}

void QGeoTiledMapData::removeMapObject(QGeoMapObject *mapObject)
{
    mapObjectRemoved(mapObject);
    QGeoMapData::removeMapObject(mapObject);
}

void QGeoTiledMapData::mapObjectRemoved(QGeoMapObject *mapObject)
{
    //remove obj info for this mapObject and all its descendants
    QList<QGeoMapObject*> queue;
    queue.append(mapObject);

    while (queue.size() > 0) {
        QGeoMapObject *obj = queue.takeFirst();
        //add children to queue
        queue.append(obj->childObjects());
        obj->disconnect(this);
        d_ptr->objInfo.remove(obj);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoTiledMapData *q)
    : q_ptr(q)
{}

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(const QGeoTiledMapDataPrivate &other)
        : width(other.width),
        height(other.height),
        screenRect(other.screenRect),
        q_ptr(other.q_ptr)
{}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate()
{
    clearObjInfo();

    QList<QGeoTiledMapReply*> replyList = replies.toList();
    for (int i = 0; i < replyList.size(); ++i) {
        replyList.at(i)->abort();
        replyList.at(i)->deleteLater();
    }
}

void QGeoTiledMapDataPrivate::updateScreenRect()
{
    int zoomFactor = 1 << qRound(q_ptr->engine()->maximumZoomLevel() - q_ptr->zoomLevel());

    qreal width = q_ptr->viewportSize().width();
    qreal height = q_ptr->viewportSize().height();

    int x = (maxZoomCenter.x() - (qRound(width * zoomFactor) / 2)) % maxZoomSize.width();
    int y = maxZoomCenter.y() - (qRound(height * zoomFactor) / 2);

    screenRect = QRectF(qreal(x) / zoomFactor, qreal(y) / zoomFactor, width, height);
}

void QGeoTiledMapDataPrivate::clearObjInfo()
{
    QMutableHashIterator<QGeoMapObject*, QGeoTiledMapObjectInfo*> it(objInfo);

    while (it.hasNext()) {
        it.next();
        QGeoTiledMapObjectInfo* info = it.value();
        it.remove();
        delete info;
    }
}
QGeoTiledMapDataPrivate& QGeoTiledMapDataPrivate::operator= (const QGeoTiledMapDataPrivate & other)
{
    width = other.width;
    height = other.height;
    screenRect = other.screenRect;
    q_ptr = other.q_ptr;

    return *this;
}

qulonglong QGeoTiledMapDataPrivate::tileKey(int row, int col, int zoomLevel)
{
    qulonglong result = 1 << (zoomLevel);
    result *= row;
    result += col;
    return result;
}

bool QGeoTiledMapDataPrivate::intersects(QGeoMapObject *mapObject, const QRectF &rect)
{
    if (!mapObject)
        return false;

    if (!objInfo.contains(mapObject))
        calculateInfo(mapObject);

    //TODO: consider dateline wrapping
    return rect.intersects(objInfo[mapObject]->boundingBox);
}

void QGeoTiledMapDataPrivate::paintMapObject(QPainter &painter, const QRectF &viewPort, QGeoMapObject *mapObject, bool hitDetection)
{
    if (!mapObject)
        return;

    if (!objInfo.contains(mapObject))
        calculateInfo(mapObject);

    if (mapObject->type() == QGeoMapObject::RectangleType)
        paintMapRectangle(painter, viewPort, static_cast<QGeoMapRectangleObject*>(mapObject), hitDetection);
    else if (mapObject->type() == QGeoMapObject::MarkerType)
        paintMapMarker(painter, viewPort, static_cast<QGeoMapMarkerObject*>(mapObject), hitDetection);
    else if (mapObject->type() == QGeoMapObject::PolylineType ||
             mapObject->type() == QGeoMapObject::PolygonType)
        paintMapPolyline(painter, viewPort, static_cast<QGeoMapPolylineObject*>(mapObject), hitDetection);
    else if (mapObject->type() == QGeoMapObject::GeoRouteType)
        paintMapRoute(painter, viewPort, static_cast<QGeoMapRouteObject*>(mapObject), hitDetection);
}

void QGeoTiledMapDataPrivate::paintMapRectangle(QPainter &painter, const QRectF &viewPort, QGeoMapRectangleObject *rectangle, bool hitDetection)
{
    QPen oldPen = painter.pen();
    QBrush oldBrush = painter.brush();

    if (!hitDetection) {
        painter.setPen(rectangle->pen());
        painter.setBrush(rectangle->brush());
    }
    else {
        QPen pen(rectangle->pen());
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter.setPen(pen);
        painter.setBrush(QColor(HIT_DETECTION_COLOR));
    }

    QGeoTiledMapObjectInfo* info = objInfo.value(rectangle);
    QRectF rect = info->boundingBox.translated(-(viewPort.topLeft()));
    painter.drawRect(rect);
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void QGeoTiledMapDataPrivate::paintMapMarker(QPainter &painter, const QRectF &viewPort, QGeoMapMarkerObject *marker, bool hitDetection)
{
    QPixmap icon = marker->icon();

    if (icon.isNull())
        return;

    QGeoTiledMapObjectInfo* info = objInfo.value(marker);
    QRectF rect = info->boundingBox.translated(-(viewPort.topLeft()));

    if (!hitDetection)
        painter.drawPixmap(rect, icon, QRectF(QPointF(0, 0), icon.size()));
    else {
        painter.setPen(QPen(QColor(HIT_DETECTION_COLOR)));
        painter.drawRect(rect);
    }
}

void QGeoTiledMapDataPrivate::paintMapPolyline(QPainter &painter, const QRectF &viewPort, QGeoMapPolylineObject *polyline, bool hitDetection)
{
    QPen oldPen = painter.pen();
    QBrush oldBrush = painter.brush();
    
    if (!hitDetection)
        painter.setPen(polyline->pen());
    else {
        QPen pen(polyline->pen());
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter.setPen(pen);
    }

    if (polyline->type() == QGeoMapObject::PolygonType) {
        if (!hitDetection)
            painter.setBrush(static_cast<QGeoMapPolygonObject*>(polyline)->brush());
        else
            painter.setBrush(QColor(HIT_DETECTION_COLOR));
    }

    QGeoTiledMapPolylineInfo* info = static_cast<QGeoTiledMapPolylineInfo*>(objInfo.value(polyline));
    QPainterPath path = info->path.translated(-(viewPort.topLeft()));
    painter.drawPath(path);
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void QGeoTiledMapDataPrivate::paintMapRoute(QPainter &painter, const QRectF &viewPort, QGeoMapRouteObject *route, bool hitDetection)
{
    QSet<int> alreadyPainted;
    QPainterPath path;
    QGeoTiledMapRouteInfo* info = static_cast<QGeoTiledMapRouteInfo*>(objInfo.value(route));
    //TODO: add some type check
    QGeoTiledMappingManagerEngine *tiledEngine = static_cast<QGeoTiledMappingManagerEngine*>(q_ptr->engine());
    QGeoTileIterator it(viewPort, tiledEngine->tileSize(), qRound(q_ptr->zoomLevel()));

    while (it.hasNext()) {
        it.next();
        qulonglong key = tileKey(it.row(), it.col(), it.zoomLevel());

        if (info->intersectedTiles.contains(key)) {
            QListIterator<QPair<int, QLineF> > segments(info->intersectedTiles[key]);

            while (segments.hasNext()) {
                const QPair<int, QLineF> &currSegment = segments.next();

                if (!alreadyPainted.contains(currSegment.first)) {
                    path.moveTo(currSegment.second.p1());
                    path.lineTo(currSegment.second.p2());
                    alreadyPainted.insert(currSegment.first);
                }
            }
        }
    }

    path.translate(-(viewPort.topLeft()));
    QPen oldPen = painter.pen();

    if (!hitDetection)
        painter.setPen(route->pen());
    else {
        QPen pen(route->pen());
        pen.setColor(QColor(HIT_DETECTION_COLOR));
        painter.setPen(pen);
    }

    painter.drawPath(path);
    painter.setPen(oldPen);
}

void QGeoTiledMapDataPrivate::calculateInfo(QGeoMapObject *mapObject)
{
    if (!mapObject)
        return;

    if (objInfo.contains(mapObject))
        delete objInfo.take(mapObject);

    if (mapObject->type() == QGeoMapObject::RectangleType)
        calculateMapRectangleInfo(static_cast<QGeoMapRectangleObject*>(mapObject));
    else if (mapObject->type() == QGeoMapObject::MarkerType)
        calculateMapMarkerInfo(static_cast<QGeoMapMarkerObject*>(mapObject));
    else if (mapObject->type() == QGeoMapObject::PolylineType || 
             mapObject->type() == QGeoMapObject::PolygonType)
        calculateMapPolylineInfo(static_cast<QGeoMapPolylineObject*>(mapObject));
    else if (mapObject->type() == QGeoMapObject::GeoRouteType)
        calculateMapRouteInfo(static_cast<QGeoMapRouteObject*>(mapObject));
}

void QGeoTiledMapDataPrivate::calculateMapRectangleInfo(QGeoMapRectangleObject *rectangle)
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;
    
    q_ptr->coordinateToWorldPixel(rectangle->boundingBox().topLeft(), &topLeftX, &topLeftY);
    q_ptr->coordinateToWorldPixel(rectangle->boundingBox().bottomRight(), &bottomRightX, &bottomRightY);

    QGeoTiledMapObjectInfo* info = new QGeoTiledMapObjectInfo;
    info->boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
    objInfo[rectangle] = info;
}

void QGeoTiledMapDataPrivate::calculateMapMarkerInfo(QGeoMapMarkerObject *marker)
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    
    q_ptr->coordinateToWorldPixel(marker->boundingBox().topLeft(), &topLeftX, &topLeftY);

    QPointF topLeft(topLeftX, topLeftY);
    topLeft += marker->anchor();
    QGeoTiledMapObjectInfo* info = new QGeoTiledMapObjectInfo;
    info->boundingBox = QRectF(topLeft, marker->icon().size());
    objInfo[marker] = info;
}

void QGeoTiledMapDataPrivate::calculateMapPolylineInfo(QGeoMapPolylineObject *polyline)
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;
    
    q_ptr->coordinateToWorldPixel(polyline->boundingBox().topLeft(), &topLeftX, &topLeftY);
    q_ptr->coordinateToWorldPixel(polyline->boundingBox().bottomRight(), &bottomRightX, &bottomRightY);

    QGeoTiledMapPolylineInfo* info = new QGeoTiledMapPolylineInfo;
    info->boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
    objInfo[polyline] = info;

    QPointF startPoint;
    QList<QGeoCoordinate> points = polyline->path();
    int sz = points.size();

    for (int i = 0; i < sz; i++) {
        const QGeoCoordinate &coord = points.at(i);

        if (!coord.isValid())
            continue;

        qulonglong x;
        qulonglong y;
        q_ptr->coordinateToWorldPixel(coord, &x, &y);

        if (i > 0)
            info->path.lineTo(x, y);
        else {
            startPoint = QPointF(x, y);
            info->path = QPainterPath(startPoint);
        }
    }

    if (!startPoint.isNull())
        info->path.lineTo(startPoint);
}

void QGeoTiledMapDataPrivate::calculateMapRouteInfo(QGeoMapRouteObject *route)
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;
    
    q_ptr->coordinateToWorldPixel(route->boundingBox().topLeft(), &topLeftX, &topLeftY);
    q_ptr->coordinateToWorldPixel(route->boundingBox().bottomRight(), &bottomRightX, &bottomRightY);

    QGeoTiledMapRouteInfo* info = new QGeoTiledMapRouteInfo;
    info->boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
    objInfo[route] = info;

    quint32 minDist = route->detailLevel();
    QGeoCoordinate last;
    QGeoCoordinate here;
    int lineIndex = 0;
    QListIterator<QGeoRouteSegment> segIt(route->route().routeSegments());

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

            if (dist >= minDist || q_ptr->zoomLevel() == q_ptr->engine()->maximumZoomLevel()) {
                addRouteSegmentInfo(info, line, lineIndex++);
                last = here;
                here = QGeoCoordinate();
            }
        }
    }

    //make sure last waypoint is always shown
    if (here.isValid()) {
        QLineF line = connectShortest(here, last);
        addRouteSegmentInfo(info, line, lineIndex++);
    }
}

QLineF QGeoTiledMapDataPrivate::connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const
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
    q_ptr->coordinateToWorldPixel(pt1, &x, &y);
    QPointF mpt1(x, y);
    q_ptr->coordinateToWorldPixel(pt2, &x, &y);
    QPointF mpt2(x, y);

    if (pt2.longitude() - pt1.longitude() > 180.0) {
        mpt1.rx() += this->width;
        return QLineF(mpt2, mpt1);
    }

    return QLineF(mpt1, mpt2);
}

void QGeoTiledMapDataPrivate::addRouteSegmentInfo(QGeoTiledMapRouteInfo *info, const QLineF &line, int index) const
{
    //TODO: add some type check
    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(q_ptr->engine());

    QGeoTileIterator it(screenRect, tiledEngine->tileSize(), qRound(q_ptr->zoomLevel()));

    while (it.hasNext()) {
        it.next();
        qulonglong key = tileKey(it.row(), it.col(), it.zoomLevel());

        if (!info->intersectedTiles.contains(key))
            info->intersectedTiles.insert(key, QList< QPair<int, QLineF> >());

        info->intersectedTiles[key].append(QPair<int, QLineF>(index, line));
    }
}

///*******************************************************************************
//*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QRectF &screenRect, const QSize &tileSize, int zoomLevel)
    : aHasNext(true), aRow(-1), aCol(-1), aScreenRect(screenRect),
    aTileSize(tileSize), aZoomLevel(zoomLevel), aTileRect(QPointF(0,0), tileSize)
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

QRectF QGeoTileIterator::tileRect() const
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
