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
}

QGeoTiledMapData::~QGeoTiledMapData()
{
}

QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    qulonglong worldX;
    qulonglong worldY;

    coordinateToWorldPixel(coordinate, &worldX, &worldY);
    QPointF pos(worldX, worldY);
    pos -= d_ptr->screenRect.topLeft();
    return pos;
}

QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    qulonglong worldX = static_cast<qulonglong>(d_ptr->screenRect.left() + screenPosition.x()) % d_ptr->width;
    qulonglong worldY = static_cast<qulonglong>(d_ptr->screenRect.top() + screenPosition.y()) % d_ptr->height;

    return worldPixelToCoordinate(worldX, worldY);
}

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

    *x = qRound64(lng * qreal(d_ptr->width));
    *y = qRound64(lat * qreal(d_ptr->height));
}


qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapData::worldPixelToCoordinate(qulonglong x, qulonglong y) const
{
    qreal fx = qreal(x) / qreal(d_ptr->width);
    qreal fy = qreal(y) / qreal(d_ptr->height);

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
    d_ptr->protectRegion = QRectF();
    qulonglong x;
    qulonglong y;
    coordinateToWorldPixel(center, &x, &y);
    d_ptr->screenRect.moveCenter(QPointF(x, y));
    d_ptr->screenRect.moveLeft(qRound64(d_ptr->screenRect.left()));
    d_ptr->screenRect.moveTop(qRound64(d_ptr->screenRect.top()));

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
    QPointF center = d_ptr->screenRect.center();
    return worldPixelToCoordinate(center.x(), center.y());
}

void QGeoTiledMapData::setZoomLevel(qreal zoomLevel)
{
    qreal oldZoomLevel = QGeoMapData::zoomLevel();

    QGeoMapData::setZoomLevel(zoomLevel);

    // QGeoMapData::setZoomLevel clips the zoom level to be
    // in between the minimum and maximum zoom levels
    zoomLevel = QGeoMapData::zoomLevel();

    int zoomDiff = qRound(zoomLevel - oldZoomLevel);

    if (zoomDiff == 0)
        return;

    d_ptr->protectRegion = QRectF();

    QGeoCoordinate currCenter = center();
    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    d_ptr->width = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().width();
    d_ptr->height = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().height();

    setCenter(currCenter);

    //scale old image
    QRectF target = QGeoMapData::mapImage().rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.width() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap pm(QGeoMapData::mapImage().size());
    QPainter painter(&pm);

    if (zoomDiff < 0) {
        painter.drawPixmap(source, QGeoMapData::mapImage(), target);
    } else {
        painter.drawPixmap(target, QGeoMapData::mapImage(), source);
    }

    //clear obj info
    d_ptr->clearObjInfo();

    setMapImage(pm);

    clearRequests();
    updateMapImage();
}

void QGeoTiledMapData::setViewportSize(const QSizeF &size)
{
    d_ptr->protectRegion = d_ptr->screenRect;
    d_ptr->screenRect.setSize(size);
    QGeoMapData::setViewportSize(size);

    QPixmap pm(size.toSize());
    QPainter p(&pm);
    if (!QGeoMapData::mapImage().isNull())
        p.drawPixmap(QGeoMapData::mapImage().rect(), QGeoMapData::mapImage(), QGeoMapData::mapImage().rect());
    setMapImage(pm);

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
    QRectF oldScreenRect(d_ptr->screenRect);
    d_ptr->screenRect.translate(dx, dy);

    //Have we crossed the dateline from east-->west (i.e. "left edge")
    while (d_ptr->screenRect.left() < 0) {
        d_ptr->screenRect.translate(d_ptr->width, 0);
    }

    //Have we crossed the dateline from west-->east (i.e. "right edge")
    if (d_ptr->screenRect.left()>= d_ptr->width) {
        d_ptr->screenRect.moveLeft(static_cast<qulonglong>(d_ptr->screenRect.left()) % d_ptr->width);
    }

    qreal deltaX = oldScreenRect.left() - d_ptr->screenRect.left();
    qreal deltaY = oldScreenRect.top() - d_ptr->screenRect.top();

    int width = QGeoMapData::mapImage().width() - qAbs(deltaX);
    int height = QGeoMapData::mapImage().height() - qAbs(deltaY);

    if ((width <= 0) || (height <= 0)) {
        d_ptr->protectRegion = QRectF();
        updateMapImage();
        return;
    }

    qreal sx;
    qreal sy;
    qreal tx;
    qreal ty;

    //TODO: make this work in case we wrapped around the date line
    if (deltaX >= 0) {
        sx = 0.0;
        tx = deltaX;
    } else {
        sx = qAbs(deltaX);
        tx = 0.0;
    }

    if (deltaY >= 0) {
        sy = 0.0;
        ty = deltaY;
    } else {
        sy = qAbs(deltaY);
        ty = 0.0;
    }

    QRectF source = QRectF(sx, sy, width, height);
    QRectF target = QRectF(tx, ty, width, height);

    if (!QGeoMapData::mapImage().isNull()) {
        qreal prX = d_ptr->screenRect.x() + target.x();
        // TODO dateline fun, will probably also involve updateMapImage
//        if (prX < 0)
//            prX += d_ptr->width;
//        if (prX >= d_ptr->width)
//            prX -= d_ptr->width;
        qreal prY = d_ptr->screenRect.y() + target.y();

        d_ptr->protectRegion = QRectF(prX, prY, width, height);
        //d_ptr->protectRegion = d_ptr->screenRect.intersected(d_ptr->protectRegion.translated(-dx, -dy));

        QRegion region;
        mapImage().scroll(-dx, -dy, source.toRect(), &region);

        QPainter painter(&(mapImage()));
        painter.setClipRegion(region);
        painter.fillRect(mapImage().rect(), Qt::lightGray);
        painter.end();

        imageChanged(target);
    }

    updateMapImage();
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

        // Protected region is the area that was on the screen before the
        // start of a resize or pan.
        // We shouldn't request tiles that are entirely contained in this
        // region.
        if (d_ptr->protectRegion.isNull() || !d_ptr->protectRegion.contains(tileRect.intersected(d_ptr->screenRect))) {
            if (!d_ptr->requestRects.contains(tileRect) && !d_ptr->replyRects.contains(tileRect)) {
                d_ptr->requests.append(QGeoTiledMapRequest(this, row, col, tileRect));
                d_ptr->requestRects.insert(tileRect);
            }
        }
    }

    if (wasEmpty && d_ptr->requests.size() > 0) {
        QTimer::singleShot(50, this, SLOT(processRequests()));
    }

    d_ptr->protectRegion = QRectF();
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

    if (reply->error() != QGeoTiledMapReply::NoError) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    if ((zoomLevel() != reply->request().zoomLevel())
            || (mapType() != reply->request().mapType())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QPixmap tile;

    if (!tile.loadFromData(reply->mapImageData(), reply->mapImageFormat().toAscii())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (tile.isNull() || tile.size().isEmpty()) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

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

    while (tileRect.left() <= d_ptr->screenRect.right())
    {
        QRectF overlap = tileRect.intersected(d_ptr->screenRect);

        if (!overlap.isEmpty())
        {
            QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
            QRectF dest = overlap.translated(-1.0 * d_ptr->screenRect.x(), -1.0 * d_ptr->screenRect.y());
            QPainter painter(&(mapImage()));
            painter.drawPixmap(dest, tile, source);
            painter.end();
            imageChanged(dest);
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

QRectF QGeoTiledMapData::screenRect() const
{
    return d_ptr->screenRect;
}

QRectF QGeoTiledMapData::protectedRegion() const
{
    return d_ptr->protectRegion;
}

void QGeoTiledMapData::clearProtectedRegion()
{
    d_ptr->protectRegion = QRectF();
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
    QSize imgSize = screenRect.size().toSize();
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
            d_ptr->paintMapObject(painter, screenRect, obj, true);

            //now test whether any pixels in buffer have been painted
            for (int x = 0; x < imgSize.width(); x++) {
                bool pixelSet = false;

                for (int y = 0; y < imgSize.height(); y++)

                    if (buffer.pixelIndex(x, y) == 1) {
                        mapObjs.append(obj);
                        pixelSet = true;
                        break;
                    }

                if (pixelSet)
                    break;
            }
        }
    }

    return QList<QGeoMapObject*>();
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
        protectRegion(other.protectRegion),
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
    protectRegion = other.protectRegion;
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
    painter.setPen(QPen(QColor(Qt::black)));
    painter.setBrush(QBrush(Qt::black));
    painter.drawPixmap(rect, icon, QRectF(QPointF(0, 0), icon.size()));
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
