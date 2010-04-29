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
#include "qmaptileservice.h"

#define RELEASE_INTERVAL 10000
#define DEFAULT_ZOOM_LEVEL 4
#define DEFAULT_ROUTE_DETAIL_LEVEL 20

//TODO: get rid of this
#define TILE_WIDTH 256
#define TILE_HEIGHT 256

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
    Destructor. Destroys the map view.
*/
QMapView::~QMapView()
{
    Q_D(QMapView);
    delete d;
}

/*!
    Initializes a the map view with a given \a mapService and centers
    the map at \a center.
*/
void QMapView::init(QMapTileService* mapService, const QGeoCoordinate& center)
{
    Q_D(QMapView);

    if (!mapService)
        return;

    //Is this map engine replacing an old one?
    if (d->mapService) {
        QObject::disconnect(mapService, SIGNAL(finished(QMapTileReply*)),
                            this, SLOT(tileFetched(QMapTileReply*)));
    }

    QObject::disconnect(&d->releaseTimer, SIGNAL(timeout()),
                        this, SLOT(releaseRemoteTiles()));

    d->mapService = mapService;

    QObject::connect(d->mapService, SIGNAL(finished(QMapTileReply*)),
                     this, SLOT(tileFetched(QMapTileReply*)), Qt::QueuedConnection);
    QObject::connect(&d->releaseTimer, SIGNAL(timeout()),
                     this, SLOT(releaseRemoteTiles()));

    d->currZoomLevel = DEFAULT_ZOOM_LEVEL;
    d->numColRow = (qint32) pow(2.0, d->currZoomLevel);
    d->viewPort = boundingRect();
    d->routeDetails = DEFAULT_ROUTE_DETAIL_LEVEL;
    setCenter(center);

    d->releaseTimer.start(RELEASE_INTERVAL);
}

/*!
    \fn QMapView::mapClicked(QGeoCoordinate geoCoord, QGraphicsSceneMouseEvent* mouseEvent)

    This signal is emitted when the map receieves a \a mouseEvent (clicked) at
    \a geoCoord.
*/

/*!
    \fn QMapView::zoomLevelChanged(quint16 oldZoomLevel, quint16 newZoomLevel)

    This signal is emitted after the map has changed its zoom level.
*/

/*!
    \fn QMapViewer::centerChanged()

    This signal is emitted when the center of the viewport onto the map has changed.
*/

/*!
    \fn QMapView::mapObjectSelected(QMapObject* mapObject)

    This signal is emitted when a \a mapObject was selected by left-clicking on it.
*/

/*!
    This method sets the horizontal \a range (in pixels) beyond the immediate limits of
    the view port, for which map tiles should also be loaded. This can make
    for a smoother panning experience.
*/
void QMapView::setHorizontalPadding(quint32 range)
{
    Q_D(QMapView);
    d->horizontalPadding = range;
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
    return d->numColRow * TILE_WIDTH;
}

/*!
    Returns the height of the logical map space in pixels -
    i.e. the pixel range from 90N to 90S.
*/
quint64 QMapView::mapHeight() const
{
    Q_D(const QMapView);
    return d->numColRow * TILE_HEIGHT;
}

void QMapView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    Q_D(QMapView);
    painter->setClipRect(0, 0, boundingRect().width() + 1, boundingRect().height() + 1);
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
            //painter->drawRect(tileTopLeft.x(), tileTopLeft.y(), TILE_WIDTH, TILE_HEIGHT);

            if (!tileData.second)
                d->requestTile(it.col(), it.row());
        } else
            d->requestTile(it.col(), it.row());
    }

    d->paintLayers(painter);
    //painter->drawRect(boundingRect());
}

/*!
    This slot is called when a requested map tile has become available.
    Internally, this slot is connected to QGeoEngine::finished(QMapTileReply*).
*/
void QMapView::tileFetched(QMapTileReply* reply)
{
    Q_D(QMapView);

    if (!d->mapService)
        return; //This really should not be happening
    if (!reply)
        return;

    //Are we actually waiting for this tile?
    //TODO: check if expected tile
    //const QGeoMapTileRequest& request = reply->request();
    quint64 tileIndex = getTileIndex(reply->col(), reply->row());

    if (!d->pendingTiles.contains(tileIndex)) {
        delete reply;
        return; //discard
    }

    ////Not the reply we expected?
    //if (reply != d->pendingTiles[tileIndex]) {
    //    delete reply;
    //    return; //discard
    //}

    d->pendingTiles.remove(tileIndex);

    ////has map configuration changed in the meantime?
    //if (request.zoomLevel() != d->currZoomLevel ||
    //        request.format().id != d->mapFormat.id ||
    //        request.resolution().id != d->mapResolution.id ||
    //        request.scheme().id != d->mapSchmeme.id ||
    //        request.version().id != d->mapVersion.id) {
    //    delete reply;
    //    return; //discard
    //}

    QPixmap tile;
    tile.loadFromData(reply->data(), "PNG");

    if(!tile.isNull() && !tile.size().isEmpty()) {
        d->mapTiles[tileIndex] = qMakePair(tile, true);
        this->update();
    }
    delete reply;
}

void QMapView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_D(QMapView);
    QPointF mapCoord = event->pos() + d->viewPort.topLeft();
    QGeoCoordinate geoCoord = translateFromViewport(mapCoord);

    if (event->button() == Qt::LeftButton && d->pannable)
        d->panActive = true;

    emit mapClicked(geoCoord, event);
}

void QMapView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_D(QMapView);

    if (event->button() == Qt::LeftButton)
        d->panActive = false;

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
        pan(deltaLeft, deltaTop);
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
    if (d->mapService)
        return d->mapService->maxZoomLevel();
    return 0;
}
/*!
    Returns the current zoom level.
*/
quint16 QMapView::getZoomLevel() const
{
    Q_D(const QMapView);
    return d->currZoomLevel;
}

/*!
    Sets the current zoom level.
*/
void QMapView::setZoomLevel(int level)
{
    Q_D(QMapView);

    if (!d->mapService)
        return;

    quint16 oldZoomLevel = d->currZoomLevel;

    if (level > d->mapService->maxZoomLevel())
        d->currZoomLevel = d->mapService->maxZoomLevel();
    else if (level < 0)
        d->currZoomLevel = 0;
    else
        d->currZoomLevel = level;

    if (oldZoomLevel == d->currZoomLevel)
        return; //nothing to be done

    int tileWidth = TILE_WIDTH;
    int tileHeight = TILE_HEIGHT;

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
    pan(0, 0);

    //reconstruct all object mappings
    //***************************************
    d->mapTiles.clear();
    d->mapTiles = scaledTiles;
    d->tileToObjects.clear();
    d->mapObjects.reconstructObjects(d->tileToObjects);
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

    d->requestMissingMapTiles();
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
    Centers the view port on the given rect center.
*/
void QMapView::centerOn(QGeoBoundingBox rect)
{
    setCenter(rect.getCenter());
}

/*
    Centers the view port on the given \a geoPos.
*/
void QMapView::setCenter(const QGeoCoordinate& coord)
{
    centerOn(translateToViewport(coord));
}

/*!
    Returns the geo coordinate at the center of the map's current view port.
*/
QGeoCoordinate QMapView::getCenter() const
{
    Q_D(const QMapView);
    return translateFromViewport(d->viewPort.center());
}

/*!
    Returns the map coordinate (in pixels) at the center of the map's current view port.
*/
QPointF QMapView::getViewportCenter() const
{
    Q_D(const QMapView);
    return d->viewPort.center();
}

/*!
    Moves the view port relative to its current position,
    with \a deltaX specifying the number of pixels the view port is moved along the x-axis
    and \a deltaY specifying the number of pixels the view port is moved along the y-axis.
*/
void QMapView::pan(int deltaX, int deltaY)
{
    Q_D(QMapView);

    if (!d->mapService)
        return;

    qreal pixelPerXAxis = d->numColRow * TILE_WIDTH;
    d->viewPort.translate(deltaX, deltaY);

    //have we gone past the left edge?
    while (d->viewPort.left() < 0) {
        d->viewPort.translate(pixelPerXAxis, 0);
    }

    //have we gone past the right edge?
    if (d->viewPort.left() >= pixelPerXAxis)
        d->viewPort.moveLeft(((quint64) d->viewPort.left()) % ((quint64) pixelPerXAxis));

    d->requestMissingMapTiles();
    update();
    emit centerChanged();
}

void QMapView::pan(int dragX, int dragY, int dropX, int dropY)
{
    pan(dropX-dragX, dropY-dragY);
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
    QRectF paddedViewPort = QRectF(d->viewPort.x()-d->horizontalPadding,
                                    d->viewPort.y()-d->verticalPadding,
                                    d->viewPort.width()+(2*d->horizontalPadding),
                                    d->viewPort.height()+(2*d->verticalPadding));

    while (it.hasNext()) {
        it.next();
        quint64 tileIndex = it.key();
        quint32 row = tileIndex / d->numColRow;
        quint32 col = tileIndex % d->numColRow;
        QRectF tileRect = getTileRect(col, row);
        if (tileRect.intersects(paddedViewPort)) continue;
        it.remove();
    }
}

/*!
    Converts a \a geoCoordinate to a map coordinate (in pixels).
*/
QPointF QMapView::translateToViewport(const QGeoCoordinate& coord) const
{
    float lng = coord.longitude();
    float lat = coord.latitude();

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

    if (!d->mapService)
        return QPointF();

    return QPointF(static_cast<qint64>(mercatorCoordinate.x() * ((qreal) d->numColRow) * ((qreal) TILE_WIDTH)),
                   static_cast<qint64>(mercatorCoordinate.y() * ((qreal) d->numColRow) * ((qreal) TILE_HEIGHT)));
}

/*!
    Computes the normalized mercator coordinate for the given \a mapCoordinate (in pixels).
*/
QPointF QMapView::mapToMercator(const QPointF& mapCoordinate) const
{
    Q_D(const QMapView);

    if (!d->mapService)
        return QPointF();

    return QPointF(mapCoordinate.x() / (((qreal) d->numColRow) * ((qreal) TILE_WIDTH)),
                   mapCoordinate.y() / (((qreal) d->numColRow) * ((qreal) TILE_HEIGHT)));
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/*!
    Converts a \a mapCoordinate (in pixels) into its corresponding geo coordinate.
*/
QGeoCoordinate QMapView::translateFromViewport(const QPointF& point) const
{
    QPointF mercCoord = mapToMercator(point);
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
    *col = mapCoordinate.x() / TILE_WIDTH;
    *row = mapCoordinate.y() / TILE_HEIGHT;
}

/*!
    Removes the given \a mapObject from the map.
    The \a mapObject is deleted in the process.
*/
void QMapView::removeMapObject(QMapObject* mapObject)
{
    Q_D(QMapView);
    d->mapObjects.removeMapObject(mapObject);

    for (int i = 0; i < mapObject->d_ptr->intersectingTiles.count(); i++) {
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
    d->mapObjects.addMapObject(mapObject);
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
    QPointF topLeft(((quint64) col) * TILE_WIDTH, ((quint64) row) * TILE_HEIGHT);
    return QRectF(topLeft, QSize(TILE_WIDTH, TILE_HEIGHT));
}

/*!
    Convenience method for getTopmostMapObject(const QPointF& mapCoordinate).
*/
QMapObject* QMapView::getTopmostMapObject(const QGeoCoordinate& geoCoordinate)
{
    return getTopmostMapObject(translateToViewport(geoCoordinate));
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

    for (int i = 0; i < objects.count(); i++) {
        QMapObject* obj = objects[i];

        if (obj->intersects(rect)) {
            if (!selected || selected->zValue() < obj->zValue())
                selected = obj;
        }
    }

    return selected;
}

/*!
    Sets the map scheme.
*/
void QMapView::setScheme(QMapTileServiceNokia::MapScheme mapScheme)
{
    Q_D(QMapView);

    if (d->mapSchmeme == mapScheme)
        return; //nothing to do

    d->cancelPendingTiles();
    d->mapSchmeme = mapScheme;
    d->mapTiles.clear();
    update();
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
    Returns the current heading.
*/
quint16 QMapView::getHeading() const
{
    Q_D(const QMapView);
    return d->heading;
}

/*!
    Sets the current heading.
*/
void QMapView::setHeading(quint16 heading)
{
    //TODO: Map view heading
    Q_D(QMapView);
    d->heading = heading;
}

/*!
    Returns the current tilt.
*/
quint16 QMapView::getTilt() const
{
    Q_D(const QMapView);
    return d->heading;
}

/*!
    Sets the current tilt.
*/
void QMapView::setTilt(quint16 tilt)
{
    //TODO: Map view tilt
    Q_D(QMapView);
    d->tilt = tilt;
}

/*!
    Returns the current map view bounds.
*/
QRectF QMapView::getViewBounds() const
{
    Q_D(const QMapView);
    return d->viewPort;
}

/*!
    Sets hotspot.
*/
void QMapView::setHotSpot(QPointF& /*hotspot*/)
{
    //TODO: Implement hotspot support
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
            currX(static_cast<qint64>(viewPort.left())),
            currY(static_cast<qint64>(viewPort.top())),
            rect(QPointF(), QSize(TILE_WIDTH, TILE_HEIGHT)),
            valid(false)
    {}

    quint32 cl;
    quint32 rw;
    quint64 tileIndex;
    bool hasNext;
    QRectF viewPort;
    quint64 numColRow;
    //MapResolution mapRes;
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
    Destroys the TileIterator.
*/
QMapView::TileIterator::~TileIterator()
{
    Q_D(QMapView::TileIterator);
    delete d;
}

/*!
    Moves iterator to next tile.
*/
void QMapView::TileIterator::next()
{
    Q_D(QMapView::TileIterator);
    d->cl = (d->currX / TILE_WIDTH) % d->numColRow;
    qint64 left = (d->currX / TILE_WIDTH) * TILE_WIDTH;
    d->rw = d->currY / TILE_HEIGHT;
    d->tileIndex = ((quint64) d->rw) * d->numColRow + d->cl;

    if (d->currY > 0) {
        qint64 top = (d->currY / TILE_HEIGHT) * TILE_HEIGHT;
        d->rect.moveTopLeft(QPointF(left, top));
        d->valid = true;
    } else
        d->valid = false;

    d->currX += TILE_WIDTH;
    qint64 nextLeft = (d->currX / TILE_WIDTH) * TILE_WIDTH;

    if (nextLeft > d->viewPort.right()) {
        d->currX = d->viewPort.left();
        d->currY += TILE_HEIGHT;
    }

    qint64 nextTop = (d->currY / TILE_HEIGHT) * TILE_HEIGHT;

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

quint64 QMapView::TileIterator::index() const
{
    Q_D(const QMapView::TileIterator);
    return d->tileIndex;
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
