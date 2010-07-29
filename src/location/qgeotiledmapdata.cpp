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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>

#include <QPen>

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

    d->scene = new QGraphicsScene(QRectF(QPointF(0.0, 0.0), d->maxZoomSize));

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

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    QPoint pos(coordinateToWorldPixel(coordinate));

    if (!d->containedInScreen(pos))
        return QPointF();

    int x = pos.x() - d->maxZoomScreenRect.left();
    if (x < 0)
        x += d->maxZoomSize.width();

    int y = pos.y() - d->maxZoomScreenRect.top();
    if (y < 0)
        y = 0;

    QPointF posF(offsetX + qreal(x) / d->zoomFactor, offsetY + qreal(y)/ d->zoomFactor);

    return posF;
}

QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapData);

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    QPointF pos(screenPosition.x() - offsetX, screenPosition.y() - offsetY);

    QRectF bounds = QRectF(QPointF(0.0, 0.0), d->viewportSize);
    if (!bounds.contains(pos))
        return QGeoCoordinate();

    int worldX = int(d->maxZoomScreenRect.left() + pos.x() * d->zoomFactor) % d->maxZoomSize.width();
    int worldY = int(d->maxZoomScreenRect.top() + pos.y() * d->zoomFactor) % d->maxZoomSize.height();

    return worldPixelToCoordinate(QPoint(worldX, worldY));
}

QPoint QGeoTiledMapData::coordinateToWorldPixel(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoTiledMapData);

    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    return QPoint(int(lng * d->maxZoomSize.width()),
                  int(lat * d->maxZoomSize.height()));
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapData::worldPixelToCoordinate(const QPoint &pixel) const
{
    Q_D(const QGeoTiledMapData);

    qreal fx = qreal(pixel.x()) / d->maxZoomSize.width();
    qreal fy = qreal(pixel.y()) / d->maxZoomSize.height();

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

    d->maxZoomCenter = coordinateToWorldPixel(center);
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
    return worldPixelToCoordinate(d->maxZoomCenter);
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

    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    QSize tileSize = tileEngine->tileSize();

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

    QGeoTileIterator it(d);

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        if (d->cache.contains(req))
            continue;

        if (!d->intersectsScreen(tileRect))
            continue;

        QList<QPair<QRect,QRect> > overlaps = d->intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                                   offsetY + int(t.top()) / d->zoomFactor,
                                   int(t.width()) / d->zoomFactor,
                                   int(t.height()) / d->zoomFactor);

            QPixmap *tile = new QPixmap(tileSize);
            tile->fill(Qt::lightGray);

            QRectF target = QRectF(QPointF(0.0, 0.0), tileEngine->tileSize());

            QPainter painter3(tile);
            painter3.drawPixmap(target, newImage, source);
            painter3.end();

            d->zoomCache.insert(req, tile, (tile->depth() * tile->width() * tile->height()) / 8);
        }
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
    if (x < 0)
        x += d->maxZoomSize.width();

    y = (y + dy * d->zoomFactor);
    int height = int(d->maxZoomScreenRect.height() / 2.0);
    if ( y < height)
        y = height;
    if (y > d->maxZoomSize.height() - height)
        y = d->maxZoomSize.height() - height;


    d->maxZoomCenter.setX(x);
    d->maxZoomCenter.setY(y);

    d->updateScreenRect();

    updateMapImage();
}

void QGeoTiledMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);
    paintMapObjects(painter, option);
}

void QGeoTiledMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_D(const QGeoTiledMapData);

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    QGeoTileIterator it(d);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        QList<QPair<QRect,QRect> > overlaps = d->intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect s = overlaps.at(i).first;
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(int(s.left()) / d->zoomFactor,
                                   int(s.top()) / d->zoomFactor,
                                   int(s.width()) / d->zoomFactor,
                                   int(s.height()) / d->zoomFactor);
            QRectF target = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                                   offsetY + int(t.top()) / d->zoomFactor,
                                   int(t.width()) / d->zoomFactor,
                                   int(t.height()) / d->zoomFactor);

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

void QGeoTiledMapData::paintMapObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_D(QGeoTiledMapData);

    d->updateScreenRect();

    qreal targetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (targetX < 0.0)
        targetX = 0.0;
    targetX /= d->zoomFactor;
    qreal targetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (targetY < 0.0)
        targetY = 0.0;
    targetY /= d->zoomFactor;
    qreal targetW = d->viewportSize.width() - 2 * targetX;
    qreal targetH = d->viewportSize.height() - 2 * targetY;

    QRect worldRect = QRect(QPoint(0.0,0.0), d->maxZoomSize);

    if (worldRect.contains(d->maxZoomScreenRect)) {
        d->scene->render(painter,
                         QRectF(targetX, targetY, targetW, targetH),
                         d->maxZoomScreenRect);
        return;
    }

    QRect inside = d->maxZoomScreenRect.intersected(worldRect);

    qreal insideWidth = targetW * inside.width() / d->maxZoomScreenRect.width();

    d->scene->render(painter,
                     QRectF(targetX, targetY, insideWidth, targetH),
                     inside);

    QRect outside = QRect(0,
                          d->maxZoomScreenRect.y(),
                          d->maxZoomScreenRect.width() - inside.width(),
                          d->maxZoomScreenRect.height());

    qreal outsideWidth = targetW * outside.width() / d->maxZoomScreenRect.width();

    d->scene->render(painter,
                     QRectF(targetX + targetW - outsideWidth, targetY, outsideWidth, targetH),
                     outside);
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
        if (!d->intersectsScreen(req.tileRect())) {
            d->requestRects.remove(req.tileRect());
            requestIter.remove();
        }
    }

    QGeoTileIterator it(d);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

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
        QTimer::singleShot(0, this, SLOT(processRequests()));
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
        if (!d->intersectsScreen(reply->request().tileRect())
            || (zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
                reply->abort();
                d->replyRects.remove(reply->request().tileRect());
                replyIter.remove();
                d->zoomCache.remove(reply->request());
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
        if (d->replyRects.contains(req.tileRect()) || !d->intersectsScreen(req.tileRect())) {
            continue;
        }

        QGeoTiledMapReply *reply = tiledEngine->getTileImage(req);

        if (!reply) {
            continue;
        }

        if (reply->error() != QGeoTiledMapReply::NoError) {
            tileError(reply->error(), reply->errorString());
            reply->deleteLater();
            d->zoomCache.remove(reply->request());
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

    QRect tileRect = reply->request().tileRect();

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /=d->zoomFactor;

    QList<QPair<QRect,QRect> > overlaps = d->intersectedScreen(tileRect);
    for (int i = 0; i < overlaps.size(); ++i) {
        QRect t = overlaps.at(i).second;
        QRectF target = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                               offsetY + int(t.top()) / d->zoomFactor,
                               int(t.width()) / d->zoomFactor,
                               int(t.height()) / d->zoomFactor);

        widget()->update(target);
    }

    if (d->requests.size() > 0)
        QTimer::singleShot(0, this, SLOT(processRequests()));

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

    cacheRect1 = d->maxZoomScreenRect.adjusted(-boundaryTiles * tileSize.width(),
                                               -boundaryTiles * tileSize.height(),
                                               boundaryTiles * tileSize.width(),
                                               boundaryTiles * tileSize.height());

    if (cacheRect1.width() > d->maxZoomSize.width()) {
        cacheRect1.setX(0);
        cacheRect1.setWidth(d->maxZoomSize.width());
    } else {
        if (cacheRect1.x() + cacheRect1.width() > d->maxZoomSize.width()) {
            int oldWidth = cacheRect1.width();
            cacheRect1.setWidth(d->maxZoomSize.width() - cacheRect1.x());
            cacheRect2 = QRectF(0,
                                cacheRect1.y(),
                                oldWidth - cacheRect1.width(),
                                cacheRect1.height());
        }
    }

    QList<QGeoTiledMapRequest> keys = d->cache.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QRectF tileRect = keys.at(i).tileRect();
        if (!cacheRect1.intersects(tileRect)) {
            if (cacheRect2.isNull() || !cacheRect2.intersects(tileRect)) {
                d->cache.remove(keys.at(i));
            }
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
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius)
{
    Q_D(QGeoTiledMapData);

    QRectF rect(d->maxZoomScreenRect.left() +(screenPosition.x() - radius) * d->zoomFactor,
                d->maxZoomScreenRect.top() + (screenPosition.y() - radius) * d->zoomFactor,
                2 * radius * d->zoomFactor,
                2 * radius * d->zoomFactor);

    QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(rect);

    QList<QGraphicsItem*> items = d->scene->collidingItems(circle);

    QList<QGeoMapObject*> results;

    for (int i = 0; i < items.size(); ++i) {
        if (d->itemMap.contains(items.at(i)))
            results.append(d->itemMap.value(items.at(i)));
    }

    delete circle;

    return results;

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of map objects managed by this map which are visible and
    which are displayed at least partially within the on screen rectangle
    \a screenRect. The returned map objects are ordered ascendingly on their zIndices.
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsInScreenRect(const QRectF &screenRect)
{
    Q_D(QGeoTiledMapData);

    QRectF rect(d->maxZoomScreenRect.topLeft() + screenRect.topLeft() * d->zoomFactor, screenRect.size() * d->zoomFactor);

    QList<QGraphicsItem*> items = d->scene->items(rect);

    QList<QGeoMapObject*> results;

    for (int i = 0; i < items.size(); ++i) {
        if (d->itemMap.contains(items.at(i)))
            results.append(d->itemMap.value(items.at(i)));
    }

    return results;

    return QList<QGeoMapObject*>();
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
    q_ptr = other.q_ptr;

    return *this;
}

void QGeoTiledMapDataPrivate::updateScreenRect()
{
    qreal viewportWidth = q_ptr->viewportSize().width();
    qreal viewportHeight = q_ptr->viewportSize().height();

    int width = int(viewportWidth * zoomFactor);
    int height = int(viewportHeight * zoomFactor);

    if (width > maxZoomSize.width())
        width = maxZoomSize.width();

    if (height > maxZoomSize.height())
        height = maxZoomSize.height();

    int x = (maxZoomCenter.x() - (width / 2)) % maxZoomSize.width();
    if (x < 0)
        x += maxZoomSize.width();

    int y = maxZoomCenter.y() - (height / 2);

    maxZoomScreenRect = QRect(x, y, width, height);

    if (x + width < maxZoomSize.width()) {
        maxZoomScreenRectClippedLeft = maxZoomScreenRect;
        maxZoomScreenRectClippedRight = QRect();
    } else {
        int widthLeft = maxZoomSize.width() - x;
        int widthRight = width - widthLeft;
        maxZoomScreenRectClippedLeft = QRect(x, y, widthLeft, height);
        maxZoomScreenRectClippedRight = QRect(0, y, widthRight, height);
    }

    containerObject->mapUpdate();
}

bool QGeoTiledMapDataPrivate::containedInScreen(const QPoint &point) const
{
    return (maxZoomScreenRectClippedLeft.contains(point)
            || (maxZoomScreenRectClippedRight.isValid()
                && maxZoomScreenRectClippedRight.contains(point)));
}

bool QGeoTiledMapDataPrivate::intersectsScreen(const QRect &rect) const
{
    return (maxZoomScreenRectClippedLeft.intersects(rect)
            || (maxZoomScreenRectClippedRight.isValid()
                && maxZoomScreenRectClippedRight.intersects(rect)));
}

QList<QPair<QRect, QRect> > QGeoTiledMapDataPrivate::intersectedScreen(const QRect &rect, bool translateToScreen) const
{
    QList<QPair<QRect, QRect> > result;

    QRect rectL = rect.intersected(maxZoomScreenRectClippedLeft);
    if (!rectL.isEmpty()) {
        QRect source = QRect(rectL.topLeft() - rect.topLeft(), rectL.size());
        QRect target = QRect(rectL.topLeft() - maxZoomScreenRectClippedLeft.topLeft(), rectL.size());
        result << QPair<QRect, QRect>(source, target);
    }

    if (maxZoomScreenRectClippedRight.isValid()) {
        QRect rectR = rect.intersected(maxZoomScreenRectClippedRight);
        if (!rectR.isEmpty()) {
            QRect source = QRect(rectR.topLeft() - rect.topLeft(), rectR.size());
            QRect target = QRect(rectR.topLeft() - maxZoomScreenRectClippedRight.topLeft(), rectR.size());
            if (translateToScreen)
                target.translate(maxZoomScreenRectClippedLeft.width(), 0);
            result << QPair<QRect, QRect>(source, target);
        }
    }

    return result;
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

QGeoTiledMapObjectInfo::QGeoTiledMapObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoMapObjectInfo(mapObjectPrivate),
    graphicsItem(0)
{
    mapData = static_cast<QGeoTiledMapDataPrivate*>(mapObjectPrivate->mapData);
}

QGeoTiledMapObjectInfo::~QGeoTiledMapObjectInfo()
{
    if (graphicsItem)
        delete graphicsItem;
}

void QGeoTiledMapObjectInfo::addToParent()
{
    if (graphicsItem) {
        mapData->scene->addItem(graphicsItem);
        mapData->itemMap.insert(graphicsItem, mapObjectPrivate->q_ptr);
    }
}

void QGeoTiledMapObjectInfo::removeFromParent()
{
    if (graphicsItem) {
        mapData->scene->removeItem(graphicsItem);
        mapData->itemMap.remove(graphicsItem);
    }
}

QGeoBoundingBox QGeoTiledMapObjectInfo::boundingBox() const
{
    if (!graphicsItem)
        return QGeoBoundingBox();

    QRectF rect = graphicsItem->boundingRect();
    QGeoCoordinate topLeft = mapData->q_ptr->worldPixelToCoordinate(rect.topLeft().toPoint());
    QGeoCoordinate bottomRight = mapData->q_ptr->worldPixelToCoordinate(rect.bottomRight().toPoint());

    return QGeoBoundingBox(topLeft, bottomRight);
}

bool QGeoTiledMapObjectInfo::contains(const QGeoCoordinate &coord) const
{
    if (!graphicsItem)
        return false;

    return graphicsItem->contains(mapData->q_ptr->coordinateToWorldPixel(coord));
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate),
    rectangleItem1(0),
    rectangleItem2(0)
{
    rectangle = static_cast<const QGeoMapRectangleObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

bool QGeoTiledMapRectangleObjectInfo::contains(const QGeoCoordinate &coord) const
{
    QPoint point = mapData->q_ptr->coordinateToWorldPixel(coord);

    if (rectangleItem1 && rectangleItem1->contains(point))
        return true;

    if (rectangleItem2 && rectangleItem2->contains(point))
        return true;

    return false;
}

void QGeoTiledMapRectangleObjectInfo::objectUpdate()
{
    QPoint topLeft = mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->bounds.topLeft());
    QPoint bottomRight = mapData->q_ptr->coordinateToWorldPixel(mapObjectPrivate->bounds.bottomRight());

    bounds = QRectF(topLeft, bottomRight);

    QRectF bounds1 = bounds;
    QRectF bounds2;

    if (bounds1.right() < bounds1.left()) {
        bounds1.setRight(bounds1.right() + mapData->maxZoomSize.width());
        bounds2 = bounds1.translated(-mapData->maxZoomSize.width(), 0);
    }

    if (!rectangleItem1)
        rectangleItem1 = new QGraphicsRectItem();

    if (bounds2.isValid()) {
        if (!rectangleItem2)
            rectangleItem2 = new QGraphicsRectItem(rectangleItem1);
    } else {
        if (rectangleItem2) {
            delete rectangleItem2;
            rectangleItem2 = 0;
        }
    }

    rectangleItem1->setRect(bounds1);
    if (rectangleItem2)
        rectangleItem2->setRect(bounds2);

    rectangleItem1->setBrush(rectangle->brush);
    if (rectangleItem2)
        rectangleItem2->setBrush(rectangle->brush);

    mapUpdate();

    graphicsItem = rectangleItem1;
}

void QGeoTiledMapRectangleObjectInfo::mapUpdate() {
    if (rectangleItem1) {
        QPen pen = rectangle->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        rectangleItem1->setPen(pen);
        if (rectangleItem2)
            rectangleItem2->setPen(pen);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapCircleObjectInfo::QGeoTiledMapCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate),
    polygonItem(0)
{
    circle = static_cast<const QGeoMapCircleObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapCircleObjectInfo::~QGeoTiledMapCircleObjectInfo() {}

static const double qgeocoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static double qgeocoordinate_degToRad(double deg)
{
    return deg * M_PI / 180;
}
inline static double qgeocoordinate_radToDeg(double rad)
{
    return rad * 180 / M_PI;
}

void QGeoTiledMapCircleObjectInfo::objectUpdate()
{
    QList<QGeoCoordinate> path;
    
    QGeoCoordinate center = circle->center;
    double radius = circle->radius/(qgeocoordinate_EARTH_MEAN_RADIUS * 1000);
    
    // To simplify things, we're using formulae from astronomy, namely those from the nautic triangle for converting from horizontal system to equatorial system.
    // First, we calculate the input coordinates (horizontal system)
    // altitude - derivation:
    /*
                   A________B
                   /\ h) (.|
                  /  \/   \|
                 / 90°\    |
                /\____/\   |
               /        \  |
              /          \ |
             /            \|
            /              |C
           /               |\
          /                | \
         /                 |  \
        /                  |   \
       /                   |    \
      /\                   |\   /\
    F/ r)                 E|.) (x \D
     ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
    explanations:
        A is the central point
        E is the peripheral point
        
        AD is the tangent at the central point
        BE is the tangent at the peripheral point
        
        h = <)ACB is the altitude
        r = <)FDA is the radius, in radians, of the small circle we want to achieve
        x = <)DCE is an auxilary angle
        
        <)EDC, <)DCF, <)BAC and <)AFD are rectangular (as denoted by the dot or 90°)
        AB and FD are parallel
    
    knowns:
        r
    
    unknowns:
        h
    
    derivation:
        h = x | the triangles ABC and CDE have the same angles (h/x, 90° and the angle between the two tangents)
                
        r + x + 90° = 180° | sum of interior angles
        <=> r + h + 90° = 180° | h = x
        <=> h = 90° - r
    */
    //double h = M_PI/2 - radius;
    double cosh = sin(radius); // cos(M_PI/2 - radius); // cos(h);
    double sinh = cos(radius); // sin(M_PI/2 - radius); // sin(h);
    // Location latitude
    double phi = qgeocoordinate_degToRad(center.latitude());
    double cosphi = cos(phi), sinphi = sin(phi);
    // Star time (since we're not doing actual astronomy here, we use a greenwich star time of 0)
    double theta = qgeocoordinate_degToRad(center.longitude());
    
    double sinphi_sinh = sinphi*sinh;
    double cosphi_cosh = cosphi*cosh;
    double sinphi_cosh = sinphi*cosh;
    double cosphi_sinh = cosphi*sinh;
    
    //qDebug("circle: h=%f lat=%f lng=%f radius=%fm",h,phi,theta, radius*(qgeocoordinate_EARTH_MEAN_RADIUS * 1000));
    // Azimut - we iterate over a full circle, in 128 steps for now. TODO: find a better step count, adjust by display size etc
    int steps = 128;
    for (int i = 0; i < steps; ++i) {
        double a = M_PI/2+2*M_PI*i/steps;
        // next, we convert from horizontal system -> equatorial system
        // First, sin(delta)
        double sindelta = sinphi_sinh - cosphi_cosh * cos(a);

        // calculate the value of cos(delta)*cos(tau)...
        double cosdelta_costau = cosphi_sinh + sinphi_cosh * cos(a);
        // ...and cos(delta)*sin(tau)
        double cosdelta_sintau = -sin(a) * cosh;
        
        // now we obtain the actual value of the hour angle...
        // convert from cartesian to polar ( cos(delta)*cos(tau) | cos(delta)*sin(tau) ) -> ( tau | cos(delta) )
        double tau = atan2(cosdelta_sintau, cosdelta_costau);
        double cosdelta = sqrt(cosdelta_sintau*cosdelta_sintau + cosdelta_costau*cosdelta_costau);
        
        // ...and the declination
        // convert from cartesian to polar ( cos(delta) | sin(delta) ) -> ( delta | 1 )
        double delta = atan2(sindelta, cosdelta);
        //qAssert(qFuzzyCompare(sqrt(sindelta*sindelta + cosdelta*cosdelta), 1)); // taken out because it takes too much cpu
        
        // we calculate right ascension from tau
        double alpha = theta-tau;

        // we interpret right ascension as latitude and declination as longtitude
        double lat = qgeocoordinate_radToDeg(delta);
        double lng = qgeocoordinate_radToDeg(alpha);
        
        //qDebug("lat=%f lng=%f",lat,lng);
        path.append(QGeoCoordinate(lat,lng));
    }
    
    // --- copy-pasted from polygon (replaced polygon with circle) ---
    points.clear();

    //TODO - handle when polygons are drawn across the dateline...
    // regular graphics item with polygon item children?

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        points.append(mapData->q_ptr->coordinateToWorldPixel(coord));
    }

    if (!polygonItem)
        polygonItem = new QGraphicsPolygonItem();

    polygonItem->setPolygon(points);
    polygonItem->setPen(circle->pen);
    polygonItem->setBrush(circle->brush);

    graphicsItem = polygonItem;
    // --- snip ---
}

void QGeoTiledMapCircleObjectInfo::mapUpdate()
{
    // --- copy-pasted from polygon (replaced polygon with circle) ---
    if (polygonItem) {
        QPen pen = circle->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        polygonItem->setPen(pen);
    }
    // --- snip ---
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapPolylineObjectInfo::QGeoTiledMapPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate)
    , pathItem(0)
{
    polyline = static_cast<const QGeoMapPolylineObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapPolylineObjectInfo::~QGeoTiledMapPolylineObjectInfo() {}

void QGeoTiledMapPolylineObjectInfo::objectUpdate()
{
    QList<QGeoCoordinate> path = polyline->path;

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        points.append(mapData->q_ptr->coordinateToWorldPixel(coord));
    }

    if (points.size() < 2)
        return;

    QPainterPath painterPath(points.at(0));
    for (int i = 1; i < points.size(); ++i)
        painterPath.lineTo(points.at(i));

    if (!pathItem)
        pathItem = new QGraphicsPathItem();

    pathItem->setPath(painterPath);
    mapUpdate();

    graphicsItem = pathItem;
}

void QGeoTiledMapPolylineObjectInfo::mapUpdate()
{
    if (pathItem) {
        QPen pen = polyline->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        pathItem->setPen(pen);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapPolygonObjectInfo::QGeoTiledMapPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate),
    polygonItem(0)
{
    polygon = static_cast<const QGeoMapPolygonObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapPolygonObjectInfo::~QGeoTiledMapPolygonObjectInfo() {}

void QGeoTiledMapPolygonObjectInfo::objectUpdate()
{
    QList<QGeoCoordinate> path = polygon->path;

    points.clear();

    //TODO - handle when polygons are drawn across the dateline...
    // regular graphics item with polygon item children?

    for (int i = 0; i < path.size(); ++i) {
        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        points.append(mapData->q_ptr->coordinateToWorldPixel(coord));
    }

    if (!polygonItem)
        polygonItem = new QGraphicsPolygonItem();

    polygonItem->setPolygon(points);
    polygonItem->setPen(polygon->pen);
    polygonItem->setBrush(polygon->brush);

    graphicsItem = polygonItem;
}

void QGeoTiledMapPolygonObjectInfo::mapUpdate()
{
    if (polygonItem) {
        QPen pen = polygon->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        polygonItem->setPen(pen);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapMarkerObjectInfo::QGeoTiledMapMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate),
    pixmapItem(0)

{
    marker = static_cast<const QGeoMapMarkerObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapMarkerObjectInfo::~QGeoTiledMapMarkerObjectInfo() {}

void QGeoTiledMapMarkerObjectInfo::objectUpdate()
{
    QPointF position = mapData->q_ptr->coordinateToWorldPixel(marker->coordinate);

    if (!pixmapItem)
        pixmapItem = new QGraphicsPixmapItem();

    pixmapItem->setPixmap(marker->icon);
    pixmapItem->setOffset(position);
    pixmapItem->setTransformOriginPoint(position);

    mapUpdate();

    graphicsItem = pixmapItem;
}

void QGeoTiledMapMarkerObjectInfo::mapUpdate()
{
    if (pixmapItem) {
        pixmapItem->resetTransform();
        pixmapItem->setScale(mapData->zoomFactor);
        pixmapItem->translate(marker->anchor.x() * mapData->zoomFactor, marker->anchor.y() * mapData->zoomFactor);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRouteObjectInfo::QGeoTiledMapRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate)
    : QGeoTiledMapObjectInfo(mapObjectPrivate),
    pathItem(0),
    //groupItem(0),
    oldZoom(-1.0)
{
    route = static_cast<const QGeoMapRouteObjectPrivate*>(mapObjectPrivate);
}

QGeoTiledMapRouteObjectInfo::~QGeoTiledMapRouteObjectInfo() {}

void QGeoTiledMapRouteObjectInfo::objectUpdate()
{
    QListIterator<QGeoRouteSegment> segIt(route->route.routeSegments());

    while (segIt.hasNext()) {
        QListIterator<QGeoCoordinate> coordIt(segIt.next().path());
        while (coordIt.hasNext()) {
            QGeoCoordinate coord = coordIt.next();

            if (!coord.isValid())
                continue;

            points.append(mapData->q_ptr->coordinateToWorldPixel(coord));
        }
    }

    if (!pathItem)
        pathItem = new QGraphicsPathItem();

    mapUpdate();

    graphicsItem = pathItem;
}

void QGeoTiledMapRouteObjectInfo::mapUpdate()
{
    if (!pathItem)
        return;

    if (mapData->zoomLevel != oldZoom) {
        oldZoom = mapData->zoomLevel;

        distanceFilteredPoints.clear();

        QPointF lastPoint = points.at(0);
        distanceFilteredPoints.append(points.at(0));
        for (int i = 1; i < points.size() - 1; ++i) {
            if ((lastPoint - points.at(i)).manhattanLength() >= route->detailLevel * mapData->zoomFactor) {
                distanceFilteredPoints.append(points.at(i));
                lastPoint = points.at(i);
            }
        }

        distanceFilteredPoints.append(points.at(points.size() - 1));

        QPen pen = route->pen;
        pen.setWidthF(pen.widthF() * mapData->zoomFactor);
        pathItem->setPen(pen);
    }

    QPainterPath painterPath;

    if (distanceFilteredPoints.size() < 2) {
        pathItem->setPath(painterPath);
        return;
    }

    bool offScreen = true;

    for (int i = 0; i < distanceFilteredPoints.size() - 1; ++i) {
        if (!offScreen)
            painterPath.lineTo(distanceFilteredPoints.at(i));

        bool wasOffScreen = offScreen;

        QPointF point1 = distanceFilteredPoints.at(i);
        QPointF point2 = distanceFilteredPoints.at(i + 1);
        QPointF midpoint = (point1 + point2) / 2.0;

        offScreen = !(mapData->maxZoomScreenRect.contains(point1.toPoint())
                     || mapData->maxZoomScreenRect.contains(point2.toPoint())
                     || mapData->maxZoomScreenRect.contains(midpoint.toPoint()));

        if (wasOffScreen && !offScreen)
            painterPath.moveTo(distanceFilteredPoints.at(i));
    }

    pathItem->setPath(painterPath);
}

//QLineF QGeoTiledMapRouteObjectInfo::connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const
//{
//    //order from west to east
//    QGeoCoordinate pt1;
//    QGeoCoordinate pt2;

//    if (point1.longitude() < point2.longitude()) {
//        pt1 = point1;
//        pt2 = point2;
//    } else {
//        pt1 = point2;
//        pt2 = point1;
//    }

//    qulonglong x;
//    qulonglong y;
//    mapData->q_ptr->coordinateToWorldPixel(pt1, &x, &y);
//    QPointF mpt1(x, y);
//    mapData->q_ptr->coordinateToWorldPixel(pt2, &x, &y);
//    QPointF mpt2(x, y);

//    if (pt2.longitude() - pt1.longitude() > 180.0) {
//        mpt1.rx() += mapData->maxZoomSize.width();
//        return QLineF(mpt2, mpt1);
//    }

//    return QLineF(mpt1, mpt2);
//}

///*******************************************************************************
//*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QGeoTiledMapDataPrivate *mapDataPrivate)
    : mapData(mapDataPrivate->q_ptr),
    atEnd(false),
    row(-1),
    col(-1),
    screenRect(mapDataPrivate->maxZoomScreenRect),
    zoomLevel(mapDataPrivate->zoomLevel)
{
    QGeoTiledMappingManagerEngine *tiledEngine
            = static_cast<QGeoTiledMappingManagerEngine*>(mapDataPrivate->engine);
    tileSize = tiledEngine->tileSize() * mapDataPrivate->zoomFactor;
    tileRect = QRect(QPoint(0, 0), tileSize);

    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    width = tileSize.width() * (1 << zoomLevel);

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

QGeoTileIterator::QGeoTileIterator(QGeoTiledMapData *mapData, const QRect &screenRect, const QSize &tileSize, int zoomLevel)
    : mapData(mapData),
    atEnd(false),
    row(-1),
    col(-1),
    screenRect(screenRect),
    tileSize(tileSize),
    zoomLevel(zoomLevel),
    tileRect(QPoint(0,0), tileSize)
{
    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    width = tileSize.width() * (1 << zoomLevel);

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

bool QGeoTileIterator::hasNext()
{
    return !atEnd;
}

QGeoTiledMapRequest QGeoTileIterator::next()
{
    int numCols = 1 << zoomLevel;
    col = static_cast<int>(currTopLeft.x() / tileSize.width()) % numCols;
    row = static_cast<int>(currTopLeft.y() / tileSize.height()) % numCols;
    tileRect.moveTopLeft(currTopLeft);
    if (tileRect.left() >= width)
        tileRect.translate(-width, 0);

    currTopLeft.rx() += tileSize.width();

    if (currTopLeft.x() > screenRect.right()) { //next row
        qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
        currTopLeft.setX(x * tileSize.width());
        currTopLeft.ry() += tileSize.height();
    }

    if (currTopLeft.y() > screenRect.bottom()) //done
        atEnd = true;

    return QGeoTiledMapRequest(mapData, row, col, tileRect);
}

#include "moc_qgeotiledmapdata.cpp"

QTM_END_NAMESPACE
