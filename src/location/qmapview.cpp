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

#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "qmapview.h"
#include "qmaptile.h"
#include "qmaproute.h"
#include "qmapobject.h"
#include "qmapobject_p.h"
#include "qmapline.h"
#include "qmaprect.h"
#include "qmappixmap.h"
#include "qmappolygon.h"
#include "qmapellipse.h"
#include "qmapmarker.h"
#include "qmapmarker_p.h"

#define RELEASE_INTERVAL 10000

#define DEFAULT_ZOOM_LEVEL 4
#define DEFAULT_ROUTE_DETAIL_LEVEL 20
#define MAX_Z_LAYER 65535

QTM_BEGIN_NAMESPACE

//TODO: there sometimes seems to be an infinite-loop issue with QList::clear() when map objects have been added and the zoom level is changed
//TODO: map object selection

QMapView::QMapView(QGraphicsItem* parent, Qt::WindowFlags wFlags)
        : QGraphicsWidget(parent, wFlags),
        releaseTimer(this),
        mapVer(MapVersion(MapVersion::Newest)),
        mapSchm(MapScheme(MapScheme::Normal_Day)),
        mapRes(MapResolution(MapResolution::Res_256_256)),
        mapFrmt(MapFormat(MapFormat::Png))
{
    numColRow = 1;
    geoEngine = NULL;
    currZoomLevel = DEFAULT_ZOOM_LEVEL;
    horPadding = 0;
    verPadding = 0;
    routeDetails = DEFAULT_ROUTE_DETAIL_LEVEL;

    panActive = false;
}

void QMapView::init(QGeoEngine* geoEngine, const QGeoCoordinateMaps& center)
{
    if (!geoEngine)
        return;

    //Is this map engine replacing an old one?
    if (this->geoEngine) {
        QObject::disconnect(geoEngine, SIGNAL(finished(QMapTileReply*)),
                            this, SLOT(tileFetched(QMapTileReply*)));
    }

    QObject::disconnect(&releaseTimer, SIGNAL(timeout()),
                        this, SLOT(releaseRemoteTiles()));

    this->geoEngine = geoEngine;

    QObject::connect(geoEngine, SIGNAL(finished(QMapTileReply*)),
                     this, SLOT(tileFetched(QMapTileReply*)), Qt::QueuedConnection);
    QObject::connect(&releaseTimer, SIGNAL(timeout()),
                     this, SLOT(releaseRemoteTiles()));

    currZoomLevel = DEFAULT_ZOOM_LEVEL;
    numColRow = (qint32) pow(2.0, currZoomLevel);
    viewPort = boundingRect();
    centerOn(center);

    releaseTimer.start(RELEASE_INTERVAL);
}

void QMapView::setVerticalPadding(quint32 /*range*/)
{
    //TODO: vertical padding
}

void QMapView::setHorizontalPadding(quint32 /*range*/)
{
    //TODO: horizontal padding
}

void QMapView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    painter->setClipRect(0, 0, boundingRect().width() + 1, boundingRect(). height() + 1);
    QRectF rect = option->exposedRect;
    rect.translate(viewPort.left(), viewPort.top());
    TileIterator it(*this, rect);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 index = getTileIndex(it.col(), it.row());

        if (mapTiles.contains(index)) {
            QPair<QPixmap, bool>& tileData = mapTiles[index];
            QPixmap& pixmap = tileData.first;
            QPointF tileTopLeft = it.tileRect().topLeft();
            tileTopLeft -= viewPort.topLeft();
            painter->drawPixmap(tileTopLeft, pixmap);
            painter->drawRect(tileTopLeft.x(), tileTopLeft.y(), mapRes.size.width(), mapRes.size.height());

            if (!tileData.second)
                requestTile(it.col(), it.row());
        } else
            requestTile(it.col(), it.row());
    }

    paintLayers(painter);
    painter->drawRect(boundingRect());
}

void QMapView::requestTile(quint32 col, quint32 row)
{
    if (!geoEngine)
        return;

    quint64 tileIndex = getTileIndex(col, row);

    if (pendingTiles.contains(tileIndex)) {
        return;
    }

    QMapTileRequest request;
    request.setVersion(mapVer);
    request.setScheme(mapSchm);
    request.setResolution(mapRes);
    request.setFormat(mapFrmt);
    request.setCol(col);
    request.setRow(row);
    request.setZoomLevel(currZoomLevel);

    QMapTileReply* reply = geoEngine->get(request);

    pendingTiles[getTileIndex(col, row)] = reply;
}

void QMapView::tileFetched(QMapTileReply* reply)
{
    if (!geoEngine)
        return; //This really should not be happening

    //Are we actually waiting for this tile?
    const QMapTileRequest& request = reply->request();
    quint64 tileIndex = getTileIndex(request.col(), request.row());

    if (!pendingTiles.contains(tileIndex)) {
        geoEngine->release(reply);
        return; //discard
    }

    //Not the reply we expected?
    if (reply != pendingTiles[tileIndex]) {
        geoEngine->release(reply);
        return; //discard
    }

    pendingTiles.remove(tileIndex);
    //has map configuration changed in the meantime?
    if (request.zoomLevel() != currZoomLevel ||
            request.format().id != mapFrmt.id ||
            request.resolution().id != mapRes.id ||
            request.scheme().id != mapSchm.id ||
            request.version().id != mapVer.id) {
        geoEngine->release(reply);
        return; //discard
    }

    QPixmap tile;
    tile.loadFromData(reply->rawData(), "PNG");
    mapTiles[tileIndex] = qMakePair(tile, true);
    this->update();
    geoEngine->release(reply);
}

void QMapView::cancelPendingTiles()
{
    QHashIterator<quint64, QMapTileReply*> it(pendingTiles);

    while (it.hasNext()) {
        it.next();
        geoEngine->cancel(it.value());
        geoEngine->release(it.value());
    }

    pendingTiles.clear();
}

void QMapView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF mapCoord = event->pos() + viewPort.topLeft();
    QGeoCoordinateMaps geoCoord = mapToGeo(mapCoord);

    if (event->button() == Qt::LeftButton) {
        panActive = true;
    }

    emit mapClicked(geoCoord, event);
}

void QMapView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        panActive = false;
    }
}

void QMapView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (panActive) {
        int deltaLeft = event->lastPos().x() - event->pos().x();
        int deltaTop  = event->lastPos().y() - event->pos().y();
        moveViewPort(deltaLeft, deltaTop);
    }
}

void QMapView::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (event->delta() > 0) //zoom in
        setZoomLevel(currZoomLevel + 1);
    else //zoom out
        setZoomLevel(((qint16) currZoomLevel) - 1);
}

QPointF QMapView::getTopLeftFromCenter(const QPointF& center)
{
    QPointF topLeft(center.x() - viewPort.width() / 2,
                    center.y() - viewPort.height() / 2);

    if (topLeft.x() < 0)
        topLeft.setX(0);
    if (topLeft.y() < 0)
        topLeft.setY(0);

    return topLeft;
}

void QMapView::setZoomLevel(int zoomLevel)
{
    if (!geoEngine)
        return;

    quint16 oldZoomLevel = currZoomLevel;

    if (zoomLevel > geoEngine->maxZoomLevel())
        currZoomLevel = geoEngine->maxZoomLevel();
    else if (zoomLevel < 0)
        currZoomLevel = 0;
    else
        currZoomLevel = zoomLevel;

    if (oldZoomLevel == currZoomLevel)
        return; //nothing to be done

    int tileWidth = mapRes.size.width();
    int tileHeight = mapRes.size.height();

    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    qreal scale = pow(2.0, ((qint16) currZoomLevel) - oldZoomLevel);

    if (scale < 1.0)
        scaledTiles = preZoomOut(scale);
    else
        scaledTiles = preZoomIn(scale);

    double ratioX = viewPort.center().x() / (numColRow * tileWidth);
    double ratioY = viewPort.center().y() / (numColRow * tileHeight);
    numColRow = (qint64) pow(2.0, currZoomLevel);
    QPointF newCenter(static_cast<qint64>(ratioX * numColRow * tileWidth),
                      static_cast<qint64>(ratioY * numColRow * tileHeight));
    viewPort.moveCenter(newCenter.toPoint());
    moveViewPort(0, 0);

    //reconstruct all object mappings
    //***************************************
    mapTiles.clear();
    mapTiles = scaledTiles;
    tileToObjectsMap.clear();
    QSetIterator<QMapObject*> it(mapObjects);

    while (it.hasNext()) {
        QMapObject* obj = it.next();
        obj->compMapCoords();
        addMapObjectToTiles(obj);
    }

    //***************************************

    cancelPendingTiles();
    update();
    emit zoomLevelChanged(oldZoomLevel, currZoomLevel);
}

QHash<quint64, QPair<QPixmap, bool> > QMapView::preZoomIn(qreal scale)
{
    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    TileIterator it(*this, viewPort);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 index = getTileIndex(it.col(), it.row());

        if (mapTiles.contains(index)) {
            QPixmap& pixmap = mapTiles[index].first;
            QPixmap scaledPixmap = pixmap.scaled(mapRes.size * scale);

            for (int i = 0; i < scale; i++)

                for (int j = 0; j < scale; j++) {
                    QPixmap scaledTile = scaledPixmap.copy(i * mapRes.size.width(),
                                                           j * mapRes.size.height(),
                                                           mapRes.size.width(),
                                                           mapRes.size.height()
                                                          );
                    quint32 scaledCol = it.col() * scale + i;
                    quint32 scaledRow = it.row() * scale + j;
                    quint64 scaledIndex = ((quint64) scaledRow) * numColRow * scale + scaledCol;
                    scaledTiles[scaledIndex] = qMakePair(scaledTile, false);
                }
        }
    }

    return scaledTiles;
}

QHash<quint64, QPair<QPixmap, bool> > QMapView::preZoomOut(qreal scale)
{
    QHash<quint64, QImage> combinedTiles;
    quint32 reverseScale = (quint32)(1.0 / scale);
    TileIterator it(*this, viewPort);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint32 scaledRow = it.row() / reverseScale;
        quint32 scaledCol = it.col() / reverseScale;
        quint64 scaledIndex = ((quint64) scaledRow) * (numColRow / reverseScale) + scaledCol;

        if (!combinedTiles.contains(scaledIndex)) {
            combinedTiles[scaledIndex] = QImage(mapRes.size, QImage::Format_ARGB32_Premultiplied);
        }

        QImage& combinedTile = combinedTiles[scaledIndex];
        QPainter painter(&combinedTile);
        quint64 index = getTileIndex(it.col(), it.row());

        if (mapTiles.contains(index)) {
            QPixmap& pixmap = mapTiles[index].first;
            QPixmap scaledPixmap = pixmap.scaled(mapRes.size * scale);
            painter.drawPixmap(mapRes.size.width() * scale * (it.col() % reverseScale),
                               mapRes.size.height() * scale * (it.row() % reverseScale),
                               scaledPixmap);
        }
    }

    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    QHashIterator<quint64, QImage> combinedIt(combinedTiles);

    while (combinedIt.hasNext()) {
        combinedIt.next();
        scaledTiles[combinedIt.key()] = qMakePair(QPixmap::fromImage(combinedIt.value()), false);
    }

    return scaledTiles;
}

void QMapView::centerOn(const QPointF& pos)
{
    viewPort.moveCenter(pos.toPoint());

    if (viewPort.top() < 0)
        viewPort.moveTop(0);
    if (viewPort.left() < 0)
        viewPort.moveLeft(0);

    update();
    emit centerChanged();
}

void QMapView::centerOn(qreal x, qreal y)
{
    centerOn(QPointF(x, y));
}

void QMapView::centerOn(const QGeoCoordinateMaps& geoPos)
{
    centerOn(geoToMap(geoPos));
}

QGeoCoordinateMaps QMapView::center() const
{
    return mapToGeo(viewPort.center());
}

QPointF QMapView::mapCenter() const
{
    return viewPort.center();
}

void QMapView::moveViewPort(int deltaX, int deltaY)
{
    if (!geoEngine)
        return;

    qreal pixelPerXAxis = numColRow *  mapRes.size.width();
    viewPort.translate(deltaX, deltaY);

    //have we gone past the left edge?
    while (viewPort.left() < 0) {
        viewPort.translate(pixelPerXAxis, 0);
    }

    //have we gone past the right edge?
    if (viewPort.left() >= pixelPerXAxis) {
        viewPort.moveLeft(((quint64) viewPort.left()) % ((quint64) pixelPerXAxis));
    }

    update();
    emit centerChanged();
}

void QMapView::resizeEvent(QGraphicsSceneResizeEvent* /*event*/)
{
    viewPort.setSize(boundingRect().size());
}

void QMapView::releaseRemoteTiles()
{
    if (panActive)
        return;

    QMutableHashIterator<quint64, QPair<QPixmap, bool> > it(mapTiles);

    while (it.hasNext()) {
        it.next();
        quint64 tileIndex = it.key();
        quint32 row = tileIndex / numColRow;
        quint32 col = tileIndex % numColRow;
        QRectF tileRect = getTileRect(col, row);
        quint64 pixelPerXAxis = static_cast<quint64>(numColRow) * mapRes.size.width();
        quint64 rightShift = static_cast<quint64>(viewPort.left()) / pixelPerXAxis;
        tileRect.translate(rightShift * pixelPerXAxis, 0);
        bool remove = true;

        while (tileRect.left() <= viewPort.right()) {
            if (tileRect.intersects(viewPort)) {
                remove = false;
                break;
            }

            tileRect.translate(pixelPerXAxis, 0);
        }

        if (remove)
            it.remove();
    }
}

QPointF QMapView::geoToMap(const QGeoCoordinateMaps& geoCoord) const
{
    return mercatorToMap(geoCoord.toMercator());
}

QPointF QMapView::mercatorToMap(const QPointF& mercatorCoord) const
{
    if (!geoEngine)
        return QPointF();

    return QPointF(static_cast<qint64>(mercatorCoord.x() * ((qreal) numColRow) * ((qreal) mapRes.size.width())),
                   static_cast<qint64>(mercatorCoord.y() * ((qreal) numColRow) * ((qreal) mapRes.size.height())));
}

QPointF QMapView::mapToMercator(const QPointF& mapCoord) const
{
    if (!geoEngine)
        return QPointF();

    return QPointF(mapCoord.x() / (((qreal) numColRow) * ((qreal) mapRes.size.width())),
                   mapCoord.y() / (((qreal) numColRow) * ((qreal) mapRes.size.height())));
}

QGeoCoordinateMaps QMapView::mapToGeo(const QPointF& mapCoord) const
{
    return QGeoCoordinateMaps::fromMercator(mapToMercator(mapCoord));
}

void QMapView::mapToTile(const QPointF& mapCoord, quint32* col, quint32* row) const
{
    *col = mapCoord.x() / mapRes.size.width();
    *row = mapCoord.y() / mapRes.size.height();
}

void QMapView::removeMapObject(const QMapObject* /*mapObject*/)
{
    //TODO: remove map object
}

const QMapPixmap* QMapView::addPixmap(const QGeoCoordinateMaps& /*topLeft*/, const QPixmap& /*pixmap*/, quint16 /*layerIndex*/)
{
    //TODO: add pixmap
    return NULL;
}

const QMapEllipse* QMapView::addEllipse(const QGeoCoordinateMaps& /*topLeft*/, const QGeoCoordinateMaps& /*bottomRight*/,
                                        const QPen& /*pen*/, const QBrush& /*brush*/, quint16 /*layerIndex*/)
{
    //TODO: add ellipse
    return NULL;
}

const QMapRect* QMapView::addRect(const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
                                  const QPen& pen, const QBrush& brush, quint16 layerIndex)
{
    QMapRect* rect = new QMapRect(this, topLeft, bottomRight, pen, brush, layerIndex);
    mapObjects.insert(rect);
    rect->compMapCoords();
    addMapObjectToTiles(rect);
    update();
    return rect;
}

const QMapPolygon* QMapView::addPolygon(const QList<QGeoCoordinateMaps>& polygon,
                                        const QPen& pen, const QBrush& brush, quint16 layerIndex)
{
    QMapPolygon* poly = new QMapPolygon(this, polygon, pen, brush, layerIndex);
    mapObjects.insert(poly);
    poly->compMapCoords();
    addMapObjectToTiles(poly);
    update();
    return poly;
}

const QMapMarker* QMapView::addMarker(const QGeoCoordinateMaps& point, const QString& text,
                                      const QFont& font, const QColor& fontColor,
                                      const QPixmap& icon, const QRectF& textRect,
                                      quint16 layerIndex)
{
    QMapMarker* marker = new QMapMarker(this, point, text, font, fontColor,
                                        icon, textRect, layerIndex);
    mapObjects.insert(marker);
    marker->compMapCoords();
    addMapObjectToTiles(marker);
    update();
    return marker;
}

const QMapLine* QMapView::addLine(const QGeoCoordinateMaps& point1, const QGeoCoordinateMaps& point2,
                                  const QPen& pen, quint16 layerIndex)
{
    QMapLine* line = new QMapLine(this, point1, point2, pen, layerIndex);
    mapObjects.insert(line);
    line->compMapCoords();
    addMapObjectToTiles(line);
    update();

    return line;
}

const QMapRoute* QMapView::addRoute(const QRoute& route, const QPen& pen,
                                    const QPixmap& endpointMarker, quint16 layerIndex)
{
    QMapRoute* mapRoute = new QMapRoute(this, route, pen, endpointMarker, layerIndex);
    mapObjects.insert(mapRoute);
    mapRoute->compMapCoords();
    addMapObjectToTiles(mapRoute);
    update();
    return mapRoute;
}

QRectF QMapView::getTileRect(quint32 col, quint32 row) const
{
    QPointF topLeft(((quint64) col) * mapRes.size.width(), ((quint64) row) * mapRes.size.height());
    return QRectF(topLeft, mapRes.size);
}

void QMapView::addMapObjectToTiles(QMapObject* mapObject)
{
    for (int i = 0; i < mapObject->d_ptr->intersectingTiles.count(); i++) {
        if (!tileToObjectsMap.contains(mapObject->d_ptr->intersectingTiles[i]))
            tileToObjectsMap[mapObject->d_ptr->intersectingTiles[i]] = QList<QMapObject*>();

        tileToObjectsMap[mapObject->d_ptr->intersectingTiles[i]].append(mapObject);
    }
}

void QMapView::paintLayers(QPainter* painter)
{
    TileIterator it(*this, viewPort);
    QMap<quint16, QSet<QMapObject*> > stackedObj;

    while (it.hasNext()) {
        it.next();
        quint64 tileIndex = getTileIndex(it.col(), it.row());

        if (tileToObjectsMap.contains(tileIndex)) {
            QList<QMapObject*>& mapObjects = tileToObjectsMap[tileIndex];

            for (int i = 0; i < mapObjects.size(); i++) {
                QMapObject* obj = mapObjects[i];

                if (!stackedObj.contains(obj->d_ptr->z))
                    stackedObj[obj->d_ptr->z] = QSet<QMapObject*>();

                if (!stackedObj[obj->d_ptr->z].contains(obj))
                    stackedObj[obj->d_ptr->z].insert(obj);
            }
        }
    }

    QMapIterator<quint16, QSet<QMapObject*> > mit(stackedObj);

    while (mit.hasNext()) {
        mit.next();
        QSetIterator<QMapObject*> sit(mit.value());

        while (sit.hasNext())
            sit.next()->paint(painter, viewPort);
    }
}

void QMapView::setScheme(const MapScheme& mapScheme)
{
    if (mapSchm.id == mapScheme.id)
        return; //nothing to do

    cancelPendingTiles();
    mapSchm = mapScheme;
    mapTiles.clear();
    update();
}

QLineF QMapView::connectShortest(const QGeoCoordinateMaps& point1, const QGeoCoordinateMaps& point2) const
{
    //order from west to east
    QGeoCoordinateMaps pt1;
    QGeoCoordinateMaps pt2;

    if (point1.longitude() < point2.longitude()) {
        pt1 = point1;
        pt2 = point2;
    } else {
        pt1 = point2;
        pt2 = point1;
    }

    QPointF mpt1 = geoToMap(pt1);
    QPointF mpt2 = geoToMap(pt2);

    if (pt2.longitude() - pt1.longitude() > 180.0) {
        mpt1.rx() += mapWidth();
        return QLineF(mpt2, mpt1);
    }

    return QLineF(mpt1, mpt2);
}

/*****************************************************************************
  TileIterator
 *****************************************************************************/
QMapView::TileIterator::TileIterator(const QMapView& mapView, const QRectF& viewPort)
        : hNext(true),
        viewPort(viewPort),
        numColRow(mapView.numColRow),
        mapRes(mapView.mapRes),
        currX(static_cast<qint64>(viewPort.left())),
        currY(static_cast<qint64>(viewPort.top())),
        rect(QPointF(), mapView.mapRes.size),
        valid(false)
{}

void QMapView::TileIterator::next()
{
    cl = (currX / mapRes.size.width()) % numColRow;
    qint64 left = (currX / mapRes.size.width()) * mapRes.size.width();
    rw = currY / mapRes.size.height();

    if (currY > 0) {
        qint64 top = (currY / mapRes.size.height()) * mapRes.size.height();
        rect.moveTopLeft(QPointF(left, top));
        valid = true;
    } else
        valid = false;

    currX += mapRes.size.width();
    qint64 nextLeft = (currX / mapRes.size.width()) * mapRes.size.width();

    if (nextLeft > viewPort.right()) {
        currX = viewPort.left();
        currY += mapRes.size.height();
    }

    qint64 nextTop = (currY / mapRes.size.height()) * mapRes.size.height();

    if (nextTop > viewPort.bottom())
        hNext = false;
}

#include "moc_qmapview.cpp"

QTM_END_NAMESPACE

