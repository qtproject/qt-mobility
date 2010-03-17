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

#define PI 3.14159265358979323846
#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "qmapview.h"
#include "qmapview_p.h"
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

QTM_BEGIN_NAMESPACE

//TODO: there sometimes seems to be an infinite-loop issue with QList::clear() when map objects have been added and the zoom level is changed

/*!
    \class QMapView
    \brief The QMapView class displays a map. It supports panning, zooming, different map formats,
    and the adding of map objects (overlays).
    \ingroup location

    The map is a 2D Mercator projection of geo coordinates.
    The map allows seamless continual panning along the x-axis.
    The map is logically 2^zoomLevel * width(map tile) pixels wide, and
    2^zoomLevel * height(map tile) high, with the top left corner being (0,0).
    This logical 2D map space is spit equally into 2^zoomLevel map tiles along each axis.
    The QMapView provides a view port onto that logical map space that is always
    QMapView::width() wide and QMapView::height() high.

    Instead of using a full-blown quad tree, QMapView exploits the fact that the map is made
    up of map tiles. Internally, it uses efficient hash maps (tileToObjectsMap, itemToObjectMap)
    to map each internal QMapObject to its corresponding tiles.
    These mappings are then used to determine which map objects need to be shown in the current
    view port.
*/

/*!
    Constructor.
*/
QMapView::QMapView(QGraphicsItem* parent, Qt::WindowFlags wFlags)
        : QGraphicsWidget(parent, wFlags), d_ptr(new QMapViewPrivate(this))
{
}

/*!
    Initializes a the map view with a given \a geoEngine and centers
    the map at \a center.
*/
void QMapView::init(QGeoEngine* geoEngine, const QGeoCoordinate& center)
{
    Q_D(QMapView);

    if (!geoEngine)
        return;

    //Is this map engine replacing an old one?
    if (d->geoEngine) {
        QObject::disconnect(geoEngine, SIGNAL(finished(QMapTileReply*)),
                            this, SLOT(tileFetched(QMapTileReply*)));
    }

    QObject::disconnect(&d->releaseTimer, SIGNAL(timeout()),
                        this, SLOT(releaseRemoteTiles()));

    d->geoEngine = geoEngine;

    QObject::connect(d->geoEngine, SIGNAL(finished(QMapTileReply*)),
                     this, SLOT(tileFetched(QMapTileReply*)), Qt::QueuedConnection);
    QObject::connect(&d->releaseTimer, SIGNAL(timeout()),
                     this, SLOT(releaseRemoteTiles()));

    d->currZoomLevel = DEFAULT_ZOOM_LEVEL;
    d->numColRow = (qint32) pow(2.0, d->currZoomLevel);
    d->viewPort = boundingRect();
    d->routeDetails = DEFAULT_ROUTE_DETAIL_LEVEL;
    centerOn(center);

    d->releaseTimer.start(RELEASE_INTERVAL);
}

/*!
    This method sets the horizontal \a range (in pixels) beyond the immediate limits of
    the view port, for which map tiles should also be loaded. This can make
    for a smoother panning experience.
*/
void QMapView::setHorizontalPadding(quint32 range)
{
    Q_D(QMapView);
    d->horizontalPadding = range;
    //TODO: horizontal padding
}
/*!
    Returns the horizontal range beyond the immediate limits of
    the view port, for which map tiles should also be loaded.
*/
quint32 QMapView::horizontalPadding() const
{
    Q_D(const QMapView);
    return d->horizontalPadding;
}
/*!
    This method sets the vertical \a range (in pixels) beyond the immediate limits of
    the view port, for which map tiles should also be loaded. This can make
    for a smoother panning experience.
*/
void QMapView::setVerticalPadding(quint32 range)
{
    Q_D(QMapView);
    d->verticalPadding = range;
    //TODO: vertical padding
}
/*!
* @return The vertical range beyond the immediate limits of
*         the view port, for which map tiles should also be loaded.
*/
quint32 QMapView::verticalPadding() const
{
    Q_D(const QMapView);
    return d->verticalPadding;
}

/*!
    Returns the width of the logical map space in pixels -
    i.e. the pixel range from 180W to 180E.
*/
quint64 QMapView::mapWidth() const
{
    Q_D(const QMapView);
    return d->numColRow * d->mapResolution.size.width();
}

/*!
    Returns the height of the logical map space in pixels -
    i.e. the pixel range from 90N to 90S.
*/
quint64 QMapView::mapHeight() const
{
    Q_D(const QMapView);
    return d->numColRow * d->mapResolution.size.height();
}

void QMapView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    Q_D(QMapView);
    painter->setClipRect(0, 0, boundingRect().width() + 1, boundingRect(). height() + 1);
    QRectF rect = option->exposedRect;
    rect.translate(d->viewPort.left(), d->viewPort.top());
    TileIterator it(*this, rect);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 index = getTileIndex(it.col(), it.row());

        if (d->mapTiles.contains(index)) {
            QPair<QPixmap, bool>& tileData = d->mapTiles[index];
            QPixmap& pixmap = tileData.first;
            QPointF tileTopLeft = it.tileRect().topLeft();
            tileTopLeft -= d->viewPort.topLeft();
            painter->drawPixmap(tileTopLeft, pixmap);
            painter->drawRect(tileTopLeft.x(), tileTopLeft.y(), d->mapResolution.size.width(), d->mapResolution.size.height());

            if (!tileData.second)
                d->requestTile(it.col(), it.row());
        } else
            d->requestTile(it.col(), it.row());
    }

    d->paintLayers(painter);
    painter->drawRect(boundingRect());
}

/*!
    This slot is called when a requested map tile has become available.
    Internally, this slot is connected to QGeoEngine::finished(QMapTileReply*).
*/
void QMapView::tileFetched(QMapTileReply* reply)
{
    Q_D(QMapView);

    if (!d->geoEngine)
        return; //This really should not be happening

    //Are we actually waiting for this tile?
    const QMapTileRequest& request = reply->request();
    quint64 tileIndex = getTileIndex(request.col(), request.row());

    if (!d->pendingTiles.contains(tileIndex)) {
        d->geoEngine->release(reply);
        return; //discard
    }

    //Not the reply we expected?
    if (reply != d->pendingTiles[tileIndex]) {
        d->geoEngine->release(reply);
        return; //discard
    }

    d->pendingTiles.remove(tileIndex);
    //has map configuration changed in the meantime?
    if (request.zoomLevel() != d->currZoomLevel ||
            request.format().id != d->mapFormat.id ||
            request.resolution().id != d->mapResolution.id ||
            request.scheme().id != d->mapSchmeme.id ||
            request.version().id != d->mapVersion.id) {
        d->geoEngine->release(reply);
        return; //discard
    }

    QPixmap tile;
    tile.loadFromData(reply->rawData(), "PNG");
    d->mapTiles[tileIndex] = qMakePair(tile, true);
    this->update();
    d->geoEngine->release(reply);
}

void QMapView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_D(QMapView);
    QPointF mapCoord = event->pos() + d->viewPort.topLeft();
    QGeoCoordinate geoCoord = mapToGeo(mapCoord);

    if (event->button() == Qt::LeftButton && d->pannable) {
        d->panActive = true;
    }

    emit mapClicked(geoCoord, event);
}

void QMapView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_D(QMapView);

    if (event->button() == Qt::LeftButton) {
        d->panActive = false;
    }

    if (event->button() == Qt::LeftButton) {
        QPointF mapCoord = event->pos() + d->viewPort.topLeft();
        QMapObject* selected = getTopmostMapObject(mapCoord);

        if (selected)
            emit mapObjectSelected(selected);
    }
}

void QMapView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    Q_D(QMapView);

    if (d->panActive) {
        int deltaLeft = event->lastPos().x() - event->pos().x();
        int deltaTop  = event->lastPos().y() - event->pos().y();
        moveViewPort(deltaLeft, deltaTop);
    }
}

void QMapView::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    Q_D(QMapView);

    if (event->delta() > 0) //zoom in
        setZoomLevel(d->currZoomLevel + 1);
    else //zoom out
        setZoomLevel(((qint16) d->currZoomLevel) - 1);
}

/*!
    Returns the maximum zoom level.
*/
quint16 QMapView::maxZoomLevel() const
{
    Q_D(const QMapView);
    if (d->geoEngine)
        return d->geoEngine->maxZoomLevel();
    return 0;
}
/*!
    Returns the current zoom level.
*/
quint16 QMapView::zoomLevel() const
{
    Q_D(const QMapView);
    return d->currZoomLevel;
}

/*!
    Sets the current zoom level.
*/
void QMapView::setZoomLevel(int zoomLevel)
{
    Q_D(QMapView);

    if (!d->geoEngine)
        return;

    quint16 oldZoomLevel = d->currZoomLevel;

    if (zoomLevel > d->geoEngine->maxZoomLevel())
        d->currZoomLevel = d->geoEngine->maxZoomLevel();
    else if (zoomLevel < 0)
        d->currZoomLevel = 0;
    else
        d->currZoomLevel = zoomLevel;

    if (oldZoomLevel == d->currZoomLevel)
        return; //nothing to be done

    int tileWidth = d->mapResolution.size.width();
    int tileHeight = d->mapResolution.size.height();

    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    qreal scale = pow(2.0, ((qint16) d->currZoomLevel) - oldZoomLevel);

    if (scale < 1.0)
        scaledTiles = d->preZoomOut(scale);
    else
        scaledTiles = d->preZoomIn(scale);

    double ratioX = d->viewPort.center().x() / (d->numColRow * tileWidth);
    double ratioY = d->viewPort.center().y() / (d->numColRow * tileHeight);
    d->numColRow = (qint64) pow(2.0, d->currZoomLevel);
    QPointF newCenter(static_cast<qint64>(ratioX * d->numColRow * tileWidth),
                      static_cast<qint64>(ratioY * d->numColRow * tileHeight));
    d->viewPort.moveCenter(newCenter.toPoint());
    moveViewPort(0, 0);

    //reconstruct all object mappings
    //***************************************
    d->mapTiles.clear();
    d->mapTiles = scaledTiles;
    d->tileToObjects.clear();
    QSetIterator<QMapObject*> it(d->mapObjects);

    while (it.hasNext()) {
        QMapObject* obj = it.next();
        obj->compMapCoords();
        d->addMapObjectToTiles(obj);
    }

    //***************************************

    d->cancelPendingTiles();
    update();
    emit zoomLevelChanged(oldZoomLevel, d->currZoomLevel);
}

/*!
    Centers the view port on the given map coordinate \a pos.
*/
void QMapView::centerOn(const QPointF& pos)
{
    Q_D(QMapView);
    d->viewPort.moveCenter(pos.toPoint());

    if (d->viewPort.top() < 0)
        d->viewPort.moveTop(0);
    if (d->viewPort.left() < 0)
        d->viewPort.moveLeft(0);

    update();
    emit centerChanged();
}

/*
    Centers the view port on the given map \a x and \a y coordinate (in pixels).
*/
void QMapView::centerOn(qreal x, qreal y)
{
    centerOn(QPointF(x, y));
}

/*
    Centers the view port on the given \a geoPos.
*/
void QMapView::centerOn(const QGeoCoordinate& geoPos)
{
    centerOn(geoToMap(geoPos));
}

/*!
    Returns the geo coordinate at the center of the map's current view port.
*/
QGeoCoordinate QMapView::center() const
{
    Q_D(const QMapView);
    return mapToGeo(d->viewPort.center());
}

/*!
    Returns the map coordinate (in pixels) at the center of the map's current view port.
*/
QPointF QMapView::mapCenter() const
{
    Q_D(const QMapView);
    return d->viewPort.center();
}

/*!
    Moves the view port relative to its current position,
    with \a deltaX specifying the number of pixels the view port is moved along the x-axis
    and \a deltaY specifying the number of pixels the view port is moved along the y-axis.
*/
void QMapView::moveViewPort(int deltaX, int deltaY)
{
    Q_D(QMapView);

    if (!d->geoEngine)
        return;

    qreal pixelPerXAxis = d->numColRow * d->mapResolution.size.width();
    d->viewPort.translate(deltaX, deltaY);

    //have we gone past the left edge?
    while (d->viewPort.left() < 0) {
        d->viewPort.translate(pixelPerXAxis, 0);
    }

    //have we gone past the right edge?
    if (d->viewPort.left() >= pixelPerXAxis) {
        d->viewPort.moveLeft(((quint64) d->viewPort.left()) % ((quint64) pixelPerXAxis));
    }

    update();
    emit centerChanged();
}

void QMapView::resizeEvent(QGraphicsSceneResizeEvent* /*event*/)
{
    Q_D(QMapView);
    d->viewPort.setSize(boundingRect().size());
}

/*!
    Releases all map tiles that are not currently covered by the view port.
*/
void QMapView::releaseRemoteTiles()
{
    Q_D(QMapView);

    if (d->panActive)
        return;

    QMutableHashIterator<quint64, QPair<QPixmap, bool> > it(d->mapTiles);

    while (it.hasNext()) {
        it.next();
        quint64 tileIndex = it.key();
        quint32 row = tileIndex / d->numColRow;
        quint32 col = tileIndex % d->numColRow;
        QRectF tileRect = getTileRect(col, row);
        quint64 pixelPerXAxis = static_cast<quint64>(d->numColRow) * d->mapResolution.size.width();
        quint64 rightShift = static_cast<quint64>(d->viewPort.left()) / pixelPerXAxis;
        tileRect.translate(rightShift * pixelPerXAxis, 0);
        bool remove = true;

        while (tileRect.left() <= d->viewPort.right()) {
            if (tileRect.intersects(d->viewPort)) {
                remove = false;
                break;
            }

            tileRect.translate(pixelPerXAxis, 0);
        }

        if (remove)
            it.remove();
    }
}

/*!
    Converts a \a geoCoordinate to a map coordinate (in pixels).
*/
QPointF QMapView::geoToMap(const QGeoCoordinate& geoCoordinate) const
{
    float lng = geoCoordinate.longitude();
    float lat = geoCoordinate.latitude();

    lng = lng / 360.0f + 0.5f;

    lat = 0.5f - (log(tan((PI / 4.0f) + (PI / 2.0f) * lat / 180.0f)) / PI) / 2.0f;
    lat = qMax(0.0f, lat);
    lat = qMin(1.0f, lat);

    return mercatorToMap(QPointF(lng, lat));
}

/*!
    Converts a normalized \a mercatorCoordinate to a map coordinate (in pixels).
*/
QPointF QMapView::mercatorToMap(const QPointF& mercatorCoordinate) const
{
    Q_D(const QMapView);

    if (!d->geoEngine)
        return QPointF();

    return QPointF(static_cast<qint64>(mercatorCoordinate.x() * ((qreal) d->numColRow) * ((qreal) d->mapResolution.size.width())),
                   static_cast<qint64>(mercatorCoordinate.y() * ((qreal) d->numColRow) * ((qreal) d->mapResolution.size.height())));
}

/*!
    Computes the normalized mercator coordinate for the given \a mapCoordinate (in pixels).
*/
QPointF QMapView::mapToMercator(const QPointF& mapCoordinate) const
{
    Q_D(const QMapView);

    if (!d->geoEngine)
        return QPointF();

    return QPointF(mapCoordinate.x() / (((qreal) d->numColRow) * ((qreal) d->mapResolution.size.width())),
                   mapCoordinate.y() / (((qreal) d->numColRow) * ((qreal) d->mapResolution.size.height())));
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/*!
    Converts a \a mapCoordinate (in pixels) into its corresponding geo coordinate.
*/
QGeoCoordinate QMapView::mapToGeo(const QPointF& mapCoordinate) const
{
    QPointF mercCoord = mapToMercator(mapCoordinate);
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
    Determines the \a col and \a row index of the tile that the given \a mapCoordinate
    (in pixels) lies on.
*/
void QMapView::mapToTile(const QPointF& mapCoordinate, quint32* col, quint32* row) const
{
    Q_D(const QMapView);
    *col = mapCoordinate.x() / d->mapResolution.size.width();
    *row = mapCoordinate.y() / d->mapResolution.size.height();
}

/*!
    Removes the given \a mapObject from the map.
    The \a mapObject is deleted in the process.
*/
void QMapView::removeMapObject(QMapObject* mapObject)
{
    Q_D(QMapView);
    QHash<quint64, QList<QMapObject*> > tileToObjectsMap; //!< Map tile to map object hash map.
    d->mapObjects.remove(mapObject);

    for (int i = 0; i < mapObject->d_ptr->intersectingTiles.count(); i++)
    {
        if (tileToObjectsMap.contains(mapObject->d_ptr->intersectingTiles[i]))
            tileToObjectsMap[mapObject->d_ptr->intersectingTiles[i]].removeAll(mapObject);
        if (d->tileToObjects.contains(mapObject->d_ptr->intersectingTiles[i]))
            d->tileToObjects[mapObject->d_ptr->intersectingTiles[i]].removeAll(mapObject);
    }

    delete mapObject;
}

/*!
    Adds \a mapObject to the QMapView. The QMapView takes ownership of the \a mapObject.
*/
void QMapView::addMapObject(QMapObject* mapObject)
{
    Q_D(QMapView);
    mapObject->setParentView(this);
    d->mapObjects.insert(mapObject);
    mapObject->compMapCoords();
    d->addMapObjectToTiles(mapObject);
    update();
}

/*!
    Returns the bounding box of the map tile given by its \a col and \a row index
    for the current zoom level and map configuration.
*/
QRectF QMapView::getTileRect(quint32 col, quint32 row) const
{
    Q_D(const QMapView);
    QPointF topLeft(((quint64) col) * d->mapResolution.size.width(), ((quint64) row) * d->mapResolution.size.height());
    return QRectF(topLeft, d->mapResolution.size);
}

/*!
    Convenience method for getTopmostMapObject(const QPointF& mapCoordinate).
*/
QMapObject* QMapView::getTopmostMapObject(const QGeoCoordinate& geoCoordinate)
{
    return getTopmostMapObject(geoToMap(geoCoordinate));
}

/*!
    Returns the map object with the highest z-index that intersects
    the given \a mapCoordinate. If more than object with the same (highest)
    z-index intersect that point, the first one found is returned.
*/
QMapObject* QMapView::getTopmostMapObject(const QPointF& mapCoordinate)
{
    Q_D(QMapView);
    QMapObject* selected = 0;
    QRectF rect(mapCoordinate.x(), mapCoordinate.y(), 1, 1);
    quint32 col;
    quint32 row;
    mapToTile(mapCoordinate, &col, &row);
    quint64 tileIndex = getTileIndex(col, row);

    if (!d->tileToObjects.contains(tileIndex))
        return 0;

    QList<QMapObject*>& objects = d->tileToObjects[tileIndex];

    for (int i = 0; i < objects.count(); i++)
    {
        QMapObject* obj = objects[i];

        if (obj->intersects(rect))
        {
            if (!selected || selected->zValue() < obj->zValue())
                selected = obj;
        }
    }

    return selected;
}

/*!
    Sets the map scheme.
*/
void QMapView::setScheme(const MapScheme& mapScheme)
{
    Q_D(QMapView);

    if (d->mapSchmeme.id == mapScheme.id)
        return; //nothing to do

    d->cancelPendingTiles();
    d->mapSchmeme = mapScheme;
    d->mapTiles.clear();
    update();
}

QLineF QMapView::connectShortest(const QGeoCoordinate& point1, const QGeoCoordinate& point2) const
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

    QPointF mpt1 = geoToMap(pt1);
    QPointF mpt2 = geoToMap(pt2);

    if (pt2.longitude() - pt1.longitude() > 180.0) {
        mpt1.rx() += mapWidth();
        return QLineF(mpt2, mpt1);
    }

    return QLineF(mpt1, mpt2);
}

/*!
    Sets whether the map is pannable.
*/
void QMapView::setPannable(bool isPannable)
{
    Q_D(QMapView);
    d->pannable = isPannable;
}

/*!
    Returns whether the map is pannable.
*/
bool QMapView::isPannable() const
{
    Q_D(const QMapView);
    return d->pannable;
}

/*!
    A route can consist of 10,000+ individual legs. Drawing them all into the
    view port (e.g. when the map is zoomed to a continent scale) can be prhobitively
    expensive. Therefore, the map view will suppress some route legs according to the
    current zoom level.

    This method sets the minimum manhattan distance in \a pixels between two consecutive visible route way points.
    In other words, specifying \a 0 here, will force the map view to always draw all individual
    route legs that are in the current view port. Higher values will allow for much faster rendering,
    especially at far out zoom levels, but the route may appear less smooth.<br>
    The default value is 20 pixels. At QGeoEngine::maxZoomLevel(), all route legs are always shown.
*/
void QMapView::setRouteDetailLevel(quint32 pixels)
{
    Q_D(QMapView);
    d->routeDetails = pixels;
}

/*!
    Returns the minum manhattan distance between two consecutive visible route way points.
    \sa setRouteDetailLevel()
*/
quint32 QMapView::routeDetailLevel() const
{
    Q_D(const QMapView);
    return d->routeDetails;
}

/*!
    Maps a two-dimensional map tile index (\a col, \a row) onto a one-dimensional one.
*/
quint64 QMapView::getTileIndex(quint32 col, quint32 row) const
{
    Q_D(const QMapView);
    return ((quint64) row) * d->numColRow + col;
}

/*!
    Returns the current map format.
*/
MapFormat QMapView::format() const
{
    Q_D(const QMapView);
    return d->mapFormat;
}

/*!
    Returns the current map scheme.
*/
MapScheme QMapView::scheme() const
{
    Q_D(const QMapView);
    return d->mapSchmeme;
}

/*!
    Returns the current map version.
*/
MapVersion QMapView::version() const
{
    Q_D(const QMapView);
    return d->mapVersion;
}

/*!
    Returns the current map resolution.
*/
MapResolution QMapView::resolution() const
{
    Q_D(const QMapView);
    return d->mapResolution;
}

/*****************************************************************************
  TileIterator
 *****************************************************************************/

class QMapView::TileIteratorPrivate
{
public:
    TileIteratorPrivate(const QMapViewPrivate* mapViewPrivate, const QRectF& viewPort)
        : hasNext(true), viewPort(viewPort),
        numColRow(mapViewPrivate->numColRow),
        mapRes(mapViewPrivate->mapResolution),
        currX(static_cast<qint64>(viewPort.left())),
        currY(static_cast<qint64>(viewPort.top())),
        rect(QPointF(), mapViewPrivate->mapResolution.size),
        valid(false)
    {
    }

    quint32 cl;
    quint32 rw;
    bool hasNext;
    QRectF viewPort;
    quint64 numColRow;
    MapResolution mapRes;
    qint64 currX;
    qint64 currY;
    QRectF rect;
    bool valid;
};

/*!
    \class QMapView::TileIterator
    \brief The QMapView::TileIterator can be used to iterate through all map tiles that are
    covered by a specified view port
    \ingroup location

    The iteration goes row by row
    (top-down), with each row being walked from left to right.
*/

/*!
    Constructs a TileIterator with its associated \a mapView and \a viewPort.
*/
QMapView::TileIterator::TileIterator(const QMapView& mapView, const QRectF& viewPort)
    : d_ptr(new QMapView::TileIteratorPrivate(mapView.d_ptr, viewPort))
{}

/*!
    Moves iterator to next tile.
*/
void QMapView::TileIterator::next()
{
    Q_D(QMapView::TileIterator);
    d->cl = (d->currX / d->mapRes.size.width()) % d->numColRow;
    qint64 left = (d->currX / d->mapRes.size.width()) * d->mapRes.size.width();
    d->rw = d->currY / d->mapRes.size.height();

    if (d->currY > 0) {
        qint64 top = (d->currY / d->mapRes.size.height()) * d->mapRes.size.height();
        d->rect.moveTopLeft(QPointF(left, top));
        d->valid = true;
    } else
        d->valid = false;

    d->currX += d->mapRes.size.width();
    qint64 nextLeft = (d->currX / d->mapRes.size.width()) * d->mapRes.size.width();

    if (nextLeft > d->viewPort.right()) {
        d->currX = d->viewPort.left();
        d->currY += d->mapRes.size.height();
    }

    qint64 nextTop = (d->currY / d->mapRes.size.height()) * d->mapRes.size.height();

    if (nextTop > d->viewPort.bottom())
        d->hasNext = false;
}

/*!
    Returns True (at least one more tile is available), False (last tile has been reached)
*/
bool QMapView::TileIterator::hasNext() const
{
    Q_D(const QMapView::TileIterator);
    return d->hasNext;
}

/*!
    Returns whether the current tile is valid,
    invalid tiles occur beyond the north and south pole.
*/
bool QMapView::TileIterator::isValid() const
{
    Q_D(const QMapView::TileIterator);
    return d->valid;
}

/*!
    Returns the column index of the current map tile.
*/
quint32 QMapView::TileIterator::col() const
{
    Q_D(const QMapView::TileIterator);
    return d->cl;
}

/*!
    Returns the row index of the current map tile.
*/
quint32 QMapView::TileIterator::row() const
{
    Q_D(const QMapView::TileIterator);
    return d->rw;
}

/*!
    Returns the bounding box of the map tile (in map pixel coordinates).
*/
QRectF QMapView::TileIterator::tileRect() const
{
    Q_D(const QMapView::TileIterator);
    return d->rect;
}

#include "moc_qmapview.cpp"

QTM_END_NAMESPACE
