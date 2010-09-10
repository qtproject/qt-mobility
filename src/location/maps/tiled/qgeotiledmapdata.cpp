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
#include "qgeomapoverlay.h"

#include "qgeotiledmapcontainerobjectinfo_p.h"
#include "qgeotiledmapcircleobjectinfo_p.h"
#include "qgeotiledmaprectangleobjectinfo_p.h"
#include "qgeotiledmappolylineobjectinfo_p.h"
#include "qgeotiledmappolygonobjectinfo_p.h"
#include "qgeotiledmappixmapobjectinfo_p.h"
#include "qgeotiledmaptextobjectinfo_p.h"
#include "qgeotiledmaprouteobjectinfo_p.h"

#include <QTimer>
#include <QImage>
#include <QGraphicsScene>

#include <QDebug>

#define DEFAULT_ZOOMLEVEL 8
#define PI 3.14159265
#define HIT_DETECTION_COLOR qRgba(0, 0, 255, 127) //semi-transparent blue

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

/*!
    \class QGeoTiledMapData
    \brief The QGeoTiledMapData class is a subclass of QGeoMapData provided
    to make working with tile based mapping services more convenient.

    \inmodule QtLocation

    \ingroup maps-impl-tiled

    This class assumes that at a zoom level of z the world is represented as a
    2^z by 2^z grid of tiles, and that the Mercator projection is used to map
    back and forth between coordinate and positions on the map.

    Different projections can be provided by reimplementing
    coordinateToWorldPixel() and worldPixelToCoordinate().

    Many of the internal calculations deal with positions as though they are
    pixel positions on the map at the maximum zoom level. Several functions are
    provided which expose information about the map and the viewport onto the
    map in these terms for use with custom QGeoMapObjectInfo subclasses.

    These functions include maxZoomCenter(), maxZoomSize() and
    maxZoomScreenRect().
*/

/*!
    Constructs a new tiled map data object, which stores the map data required by
    \a geoMap and makes use of the functionality provided by \a engine.
*/
QGeoTiledMapData::QGeoTiledMapData(QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : QGeoMapData(new QGeoTiledMapDataPrivate(this, engine, geoMap))
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);

    setZoomLevel(8.0);

    d->maxZoomSize = (1 << qRound(tileEngine->maximumZoomLevel())) * tileEngine->tileSize();

    d->scene = new QGraphicsScene(QRectF(QPointF(0.0, 0.0), d->maxZoomSize));
    d->scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // TODO get this from the engine, which should give different values depending on if this is running on a device or not
    d->cache.setMaxCost(10 * 1024 * 1024);
    d->zoomCache.setMaxCost(10 * 1024 * 1024);
}

/*!
    Destroys this tiled map data object.
*/
QGeoTiledMapData::~QGeoTiledMapData()
{
}

/*!
    \reimp
*/
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

    QPointF posF(offsetX + qreal(x) / d->zoomFactor, offsetY + qreal(y) / d->zoomFactor);

    return posF;
}

/*!
    \reimp
*/
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

/*!
    Converts the coordinate \a coordinate to a pixel position on the entire
    map at the maximum zoom level.

    The default implementation is based on the Mercator projection.
*/
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

/*!
    Converts the pixel position \a pixel on the map to a coordinate.

    The pixel position is relative the entire map at the maximum zoom level.

    The default implementation is based on the Mercator projection.
*/
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

/*!
    \reimp
*/
void QGeoTiledMapData::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoTiledMapData);

    d->maxZoomCenter = coordinateToWorldPixel(center);
    d->updateScreenRect();
    geoMap()->update();
    d->updateMapImage();
}

/*!
    \reimp
*/
void QGeoTiledMapData::setMapType(QGraphicsGeoMap::MapType mapType)
{
    Q_D(QGeoTiledMapData);

    QGeoMapData::setMapType(mapType);

    d->clearRequests();
    d->cache.clear();
    d->zoomCache.clear();
    geoMap()->update();
    d->updateMapImage();
}

/*!
    \reimp
*/
QGeoCoordinate QGeoTiledMapData::center() const
{
    Q_D(const QGeoTiledMapData);
    return worldPixelToCoordinate(d->maxZoomCenter);
}

/*!
    \reimp
*/
void QGeoTiledMapData::setZoomLevel(qreal zoomLevel)
{
    Q_D(QGeoTiledMapData);

    QPixmap oldImage(viewportSize().toSize());
    if (d->zoomLevel != -1.0) {
        // grab the old image
        QPainter painter1(&oldImage);
        d->paintMap(&painter1, 0);
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
        d->updateMapImage();
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
#if !(defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
    painter2.setRenderHint(QPainter::SmoothPixmapTransform, true);
#endif
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

        QList<QPair<QRect, QRect> > overlaps = d->intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect s = overlaps.at(i).first;
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                                   offsetY + int(t.top()) / d->zoomFactor,
                                   int(t.width()) / d->zoomFactor,
                                   int(t.height()) / d->zoomFactor);

            QPixmap *tile = new QPixmap(tileSize);
            tile->fill(Qt::lightGray); // TODO: this looks useless

            QRectF target = QRectF(int(s.left()) / d->zoomFactor,
                                   int(s.top()) / d->zoomFactor,
                                   int(s.width()) / d->zoomFactor,
                                   int(s.height()) / d->zoomFactor);

            QPainter painter3(tile);
            painter3.drawPixmap(target, newImage, source);
            painter3.end();

            d->zoomCache.insert(req, tile, (tile->depth() * tile->width() * tile->height()) / 8);
        }
    }

    geoMap()->update();

    d->clearRequests();
    d->updateMapImage();
}

/*!
    \reimp
*/
void QGeoTiledMapData::setViewportSize(const QSizeF &size)
{
    Q_D(QGeoTiledMapData);

    QGeoMapData::setViewportSize(size);
    d->updateScreenRect();
    d->updateMapImage();
}

/*!
    \reimp
*/
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
    if (y < height)
        y = height;
    if (y > d->maxZoomSize.height() - height)
        y = d->maxZoomSize.height() - height;

    d->maxZoomCenter.setX(x);
    d->maxZoomCenter.setY(y);

    d->updateScreenRect();
    d->updateMapImage();
}

/*!
    \reimp
*/
void QGeoTiledMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_D(QGeoTiledMapData);

    d->paintMap(painter, option);

    for (int i = 0; i < d->overlays.size(); ++i)
        d->overlays[i]->paint(painter, option);

    d->paintMapObjects(painter, option);
}

void QGeoTiledMapData::processRequests()
{
    Q_D(QGeoTiledMapData);

    QMutableSetIterator<QGeoTiledMapReply*> replyIter(d->replies);
    // Abort off-screen replies
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

        if (reply->isCached())
            break;
    }
}

void QGeoTiledMapData::tileFinished()
{
    Q_D(QGeoTiledMapData);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        return;
    }

    d->replyRects.remove(reply->request().tileRect());
    d->replies.remove(reply);
    d->zoomCache.remove(reply->request());

    if (reply->error() != QGeoTiledMapReply::NoError) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    if ((zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QPixmap *tile = new QPixmap();

    if (!tile->loadFromData(reply->mapImageData(), reply->mapImageFormat().toAscii())) {
        delete tile;
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (tile->isNull() || tile->size().isEmpty()) {
        delete tile;
        if (d->requests.size() > 0)
            QTimer::singleShot(0, this, SLOT(processRequests()));
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

    d->cache.insert(reply->request(), tile, (tile->depth() * tile->width() * tile->height()) / 8);

    d->cleanupCaches();

    QRect tileRect = reply->request().tileRect();

    qreal offsetX = ((d->viewportSize.width() * d->zoomFactor) - d->maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= d->zoomFactor;
    qreal offsetY = ((d->viewportSize.height() * d->zoomFactor) - d->maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= d->zoomFactor;

    QList<QPair<QRect, QRect> > overlaps = d->intersectedScreen(tileRect);
    for (int i = 0; i < overlaps.size(); ++i) {
        QRect t = overlaps.at(i).second;
        QRectF target = QRectF(offsetX + int(t.left()) / d->zoomFactor,
                               offsetY + int(t.top()) / d->zoomFactor,
                               int(t.width()) / d->zoomFactor,
                               int(t.height()) / d->zoomFactor);

        geoMap()->update(target);
    }

    if (d->requests.size() > 0)
        QTimer::singleShot(0, this, SLOT(processRequests()));

    QTimer::singleShot(0, reply, SLOT(deleteLater()));
}

void QGeoTiledMapData::tileError(QGeoTiledMapReply::Error error, QString errorString)
{
    qWarning() << errorString;
}

/*!
    \reimp
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition)
{
    Q_D(QGeoTiledMapData);

    QGeoCoordinate coord = screenPositionToCoordinate(screenPosition);

    QRectF rect(d->maxZoomScreenRect.x() + (screenPosition.x() - 1) * d->zoomFactor,
                d->maxZoomScreenRect.y() + (screenPosition.y() - 1) * d->zoomFactor,
                2 * d->zoomFactor,
                2 * d->zoomFactor);

    QList<QGraphicsItem*> items = d->scene->items(rect, Qt::IntersectsItemShape, Qt::AscendingOrder);

    QList<QGeoMapObject*> results;

    for (int i = 0; i < items.size(); ++i) {
        if (!d->itemMap.contains(items.at(i)))
            continue;
        QGeoMapObject *result = d->itemMap.value(items.at(i));
        if (result->contains(coord))
            results.append(result);
    }

    return results;
}

/*!
    \reimp
*/
QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsInScreenRect(const QRectF &screenRect)
{
    Q_D(QGeoTiledMapData);

    QRectF rect(d->maxZoomScreenRect.topLeft() + screenRect.topLeft() * d->zoomFactor, screenRect.size() * d->zoomFactor);

    QList<QGraphicsItem*> items = d->scene->items(rect, Qt::IntersectsItemShape, Qt::AscendingOrder);
    QList<QGeoMapObject*> results;

    for (int i = 0; i < items.size(); ++i) {
        if (d->itemMap.contains(items.at(i)))
            results.append(d->itemMap.value(items.at(i)));
    }

    return results;
}

/*!
    \reimp
*/
QGeoMapObjectInfo* QGeoTiledMapData::createMapObjectInfo(QGeoMapObject *mapObject)
{
    switch (mapObject->type()) {
        case QGeoMapObject::ContainerType:
            return new QGeoTiledMapContainerObjectInfo(this, mapObject);
        case QGeoMapObject::RectangleType:
            return new QGeoTiledMapRectangleObjectInfo(this, mapObject);
        case QGeoMapObject::CircleType:
            return  new QGeoTiledMapCircleObjectInfo(this, mapObject);
        case QGeoMapObject::PolylineType:
            return  new QGeoTiledMapPolylineObjectInfo(this, mapObject);
        case QGeoMapObject::PolygonType:
            return  new QGeoTiledMapPolygonObjectInfo(this, mapObject);
        case QGeoMapObject::PixmapType:
            return  new QGeoTiledMapPixmapObjectInfo(this, mapObject);
        case QGeoMapObject::TextType:
            return  new QGeoTiledMapTextObjectInfo(this, mapObject);
        case QGeoMapObject::RouteType:
            return  new QGeoTiledMapRouteObjectInfo(this, mapObject);
        default:
            return 0;
    }
}

/*!
    Returns the center of the viewport, in pixels on the entire
    map as a pixmap at the maximum zoom level.
*/
QPoint QGeoTiledMapData::maxZoomCenter() const
{
    Q_D(const QGeoTiledMapData);
    return d->maxZoomCenter;
}

/*!
    Returns the size, in pixels, of the entire map as a pixmap at the maximum
    zoom level.
*/
QSize QGeoTiledMapData::maxZoomSize() const
{
    Q_D(const QGeoTiledMapData);
    return d->maxZoomSize;
}

/*!
    Returns the visible screen rectangle, in pixels on the entire map
    as a pixmap at the maximum zoom level.
*/
QRect QGeoTiledMapData::maxZoomScreenRect() const
{
    Q_D(const QGeoTiledMapData);
    return d->maxZoomScreenRect;
}

/*!
    Returns the ratio between a single pixel on the screen and a pixel on
    the entire map as a pixmap at the maximum zoom level.
*/
int QGeoTiledMapData::zoomFactor() const
{
    Q_D(const QGeoTiledMapData);
    return d->zoomFactor;
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : QGeoMapDataPrivate(parent, engine, geoMap) {}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate()
{
    foreach(QGeoTiledMapReply *reply, replies) {
        reply->abort();
        reply->deleteLater();
    }
}

void QGeoTiledMapDataPrivate::updateMapImage()
{
    Q_Q(QGeoTiledMapData);

    if (zoomLevel == -1.0)
        return;

    bool wasEmpty = (requests.size() == 0);

    QMutableListIterator<QGeoTiledMapRequest> requestIter(requests);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();
        if (!intersectsScreen(req.tileRect())) {
            requestRects.remove(req.tileRect());
            requestIter.remove();
        }
    }

    QGeoTileIterator it(this);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        if (!cache.contains(req)) {
            if (!requestRects.contains(tileRect) && !replyRects.contains(tileRect)) {
                requests.append(req);
                requestRects.insert(tileRect);
            }
        }
    }

//    qWarning()
//            << d->requests.size()
//            << d->cache.size()
//            << d->cache.totalCost()
//            << d->zoomCache.size()
//            << d->zoomCache.totalCost();

    if (wasEmpty && requests.size() > 0) {
        QTimer::singleShot(0, q, SLOT(processRequests()));
    }
}

void QGeoTiledMapDataPrivate::clearRequests()
{
    requests.clear();
    requestRects.clear();
}

void QGeoTiledMapDataPrivate::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    qreal offsetX = ((viewportSize.width() * zoomFactor) - maxZoomScreenRect.width()) / 2.0;
    if (offsetX < 0.0)
        offsetX = 0.0;
    offsetX /= zoomFactor;
    qreal offsetY = ((viewportSize.height() * zoomFactor) - maxZoomScreenRect.height()) / 2.0;
    if (offsetY < 0.0)
        offsetY = 0.0;
    offsetY /= zoomFactor;

    QGeoTileIterator it(this);

    while (it.hasNext()) {
        QGeoTiledMapRequest req = it.next();
        QRect tileRect = req.tileRect();

        QList<QPair<QRect, QRect> > overlaps = intersectedScreen(tileRect);
        for (int i = 0; i < overlaps.size(); ++i) {
            QRect s = overlaps.at(i).first;
            QRect t = overlaps.at(i).second;

            QRectF source = QRectF(int(s.left()) / zoomFactor,
                                   int(s.top()) / zoomFactor,
                                   int(s.width()) / zoomFactor,
                                   int(s.height()) / zoomFactor);
            QRectF target = QRectF(offsetX + int(t.left()) / zoomFactor,
                                   offsetY + int(t.top()) / zoomFactor,
                                   int(t.width()) / zoomFactor,
                                   int(t.height()) / zoomFactor);

            if (cache.contains(req)) {
                painter->drawPixmap(target, *cache.object(req), source);
            } else {
                if (zoomCache.contains(req)) {
                    painter->drawPixmap(target, *zoomCache.object(req), source);
                } else {
                    painter->fillRect(target, Qt::lightGray);
                }
            }
        }
    }
}

void QGeoTiledMapDataPrivate::paintMapObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    updateScreenRect();

    qreal targetX = ((viewportSize.width() * zoomFactor) - maxZoomScreenRect.width()) / 2.0;
    Q_ASSERT(targetX >= 0.0); // This should not be possible
    targetX /= zoomFactor;

    qreal targetY = ((viewportSize.height() * zoomFactor) - maxZoomScreenRect.height()) / 2.0;
    Q_ASSERT(targetY >= 0.0); // This should not be possible
    targetY /= zoomFactor;

    qreal targetW = qreal(maxZoomScreenRect.width())/zoomFactor;
    qreal targetH = qreal(maxZoomScreenRect.height())/zoomFactor;

    QRect worldRect = QRect(QPoint(0.0, 0.0), maxZoomSize);

    if (worldRect.contains(maxZoomScreenRect)) {
        // the screen is completely contained inside the map, which means we can just draw once and be done.
        scene->render(painter,
                      QRectF(targetX, targetY, targetW, targetH),
                      maxZoomScreenRect,
                      Qt::IgnoreAspectRatio);
        return;
    }

    // cut off the part east of the dateline
    QRect westside = maxZoomScreenRect.intersected(worldRect);

    qreal westsideWidth = floor(qreal(westside.width()) / zoomFactor);

    westside.setWidth(westsideWidth*zoomFactor);
    westside.setHeight(maxZoomScreenRect.height());

    scene->render(painter,
                  QRectF(targetX, targetY, westsideWidth, targetH),
                  westside,
                  Qt::IgnoreAspectRatio);

    qreal eastsideWidth = targetW-westsideWidth;

    QRect eastside = QRect(0,
                          maxZoomScreenRect.y(),
                          eastsideWidth*zoomFactor,
                          maxZoomScreenRect.height());

    scene->render(painter,
                  QRectF(targetX + targetW - eastsideWidth, targetY, eastsideWidth, targetH),
                  eastside,
                  Qt::IgnoreAspectRatio);
    // TODO: call this recursively to draw more than 2 sets of objects
}


void QGeoTiledMapDataPrivate::cleanupCaches()
{
    int boundaryTiles = 3;

    QGeoTiledMappingManagerEngine *tiledEngine = static_cast<QGeoTiledMappingManagerEngine*>(engine);

    QSize tileSize = tiledEngine->tileSize();

    QRectF cacheRect1;
    QRectF cacheRect2;

    cacheRect1 = maxZoomScreenRect.adjusted(-boundaryTiles * tileSize.width(),
                                            -boundaryTiles * tileSize.height(),
                                            boundaryTiles * tileSize.width(),
                                            boundaryTiles * tileSize.height());

    if (cacheRect1.width() > maxZoomSize.width()) {
        cacheRect1.setX(0);
        cacheRect1.setWidth(maxZoomSize.width());
    } else {
        if (cacheRect1.x() + cacheRect1.width() > maxZoomSize.width()) {
            int oldWidth = cacheRect1.width();
            cacheRect1.setWidth(maxZoomSize.width() - cacheRect1.x());
            cacheRect2 = QRectF(0,
                                cacheRect1.y(),
                                oldWidth - cacheRect1.width(),
                                cacheRect1.height());
        }
    }

    QList<QGeoTiledMapRequest> keys = cache.keys();
    for (int i = 0; i < keys.size(); ++i) {
        QRectF tileRect = keys.at(i).tileRect();
        if (!cacheRect1.intersects(tileRect)) {
            if (cacheRect2.isNull() || !cacheRect2.intersects(tileRect)) {
                cache.remove(keys.at(i));
            }
        }
    }
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

    if (height == maxZoomSize.height())
        y = 0;

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

    containerObject->mapUpdated();
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

/*******************************************************************************
*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QGeoTiledMapDataPrivate *mapDataPrivate)
        : mapData(static_cast<QGeoTiledMapData*>(mapDataPrivate->q_ptr)),
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
        tileRect(QPoint(0, 0), tileSize)
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
