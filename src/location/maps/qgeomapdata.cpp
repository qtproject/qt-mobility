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

#include "qgeomapdata.h"
#include "qgeomapdata_p.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>

#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeomapobject.h"
#include "qgeomappixmapobject.h"
#include "qgeomapgroupobject.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomapoverlay.h"

#include "qgeomapobject_p.h"
#include "projwrapper_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapData

    \brief The QGeoMapData class are used as a bridge between QGraphicsGeoMap and
    QGeoMappingManager.


    \inmodule QtLocation

    \ingroup maps-impl

    Instances of QGeoMapData are created with
    QGeoMappingManager::createMapData(), and are used internally by
    QGraphicsGeoMap to manage the state of the map and the associated
    QGeoMapObject instances.

    Plugin implementers will need to provide implementations of
    coordinateToScreenPosition(const QGeoCoordinate &coordinate) and
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition).

    The other virtual functions can be overridden. If the screen position to
    coordinate tranformations are expensive then overriding these functions may
    allow optimizations based on caching parts of the geometry information.

    Subclasses should override createMapObjectInfo() so that QGeoMapObjectInfo
    instances will be created for each QGeoMapObject type in order to provide
    the QGeoMapData subclass specific behaviours for the map objects.
 */

/*!
    Constructs a new map data object, which makes use of the functionality provided by \a engine.
*/
QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine)
    : d_ptr(new QGeoMapDataPrivate(this, engine))
{
    if (engine->supportedConnectivityModes().length() > 0)
        setConnectivityMode(engine->supportedConnectivityModes().at(0));
    else
        setConnectivityMode(QGraphicsGeoMap::NoConnectivity);
}

/*!
  \internal
*/
QGeoMapData::QGeoMapData(QGeoMapDataPrivate *dd) : d_ptr(dd) {}

/*!
    Destroys this map data object.
*/
QGeoMapData::~QGeoMapData()
{
    Q_D(QGeoMapData);
    delete d;
}

/*!
    This function is run after the QGeoMapData instance has been 
    constructed.

    Any subclasses which override this function should make sure that
    QGeoMapData::init() is called within the body of the overridding function.
*/
void QGeoMapData::init()
{
    d_ptr->containerObject = new QGeoMapGroupObject();
    d_ptr->containerObject->setMapData(this);
}

/*!
    Returns the mapping engine that this map data object is associated with.
*/
QGeoMappingManagerEngine* QGeoMapData::engine() const
{
    return d_ptr->engine;
}

/*!
    Returns the QGeoMapObject which acts as the parent to all QGeoMapObject
    instances which are added to the map by the user.
*/
QGeoMapObject* QGeoMapData::containerObject()
{
    return d_ptr->containerObject;
}

/*!
    Sets the size of the map viewport to \a size.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
void QGeoMapData::setWindowSize(const QSizeF &size)
{
    if (d_ptr->windowSize == size)
        return;

    d_ptr->windowSize = size;
    d_ptr->pixelsOutOfDate = true;

    if (!d_ptr->blockPropertyChangeSignals)
        emit windowSizeChanged(d_ptr->windowSize);
}

/*!
    \property QGeoMapData::windowSize
    Returns the size of the map viewport.

    The size will be adjusted by the associated QGraphicsGeoMap as it resizes.
*/
QSizeF QGeoMapData::windowSize() const
{
    return d_ptr->windowSize;
}

/*!
    Sets the zoom level of the map to \a zoomLevel.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If \a zoomLevel is less than minimumZoomLevel() then minimumZoomLevel()
    will be used, and if \a zoomLevel is  larger than
    maximumZoomLevel() then maximumZoomLevel() will be used.
*/
void QGeoMapData::setZoomLevel(qreal zoomLevel)
{
    zoomLevel = qMin(zoomLevel, d_ptr->engine->maximumZoomLevel());
    zoomLevel = qMax(zoomLevel, d_ptr->engine->minimumZoomLevel());

    if (d_ptr->zoomLevel == zoomLevel)
        return;

    d_ptr->zoomLevel = zoomLevel;
    d_ptr->zoomOutOfDate = true;

    if (!d_ptr->blockPropertyChangeSignals)
        emit zoomLevelChanged(d_ptr->zoomLevel);
}

/*!
    \property QGeoMapData::zoomLevel

    Returns the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapData::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.

    Subclasses should call QGeoMapData::setCenter() when the pan has completed.
*/
void QGeoMapData::pan(int dx, int dy)
{
    QPointF pos = coordinateToScreenPosition(center());
    setCenter(screenPositionToCoordinate(QPointF(pos.x() + dx, pos.y() + dy)));
}

/*!
    Centers the map viewport on the coordinate \a center.
*/
void QGeoMapData::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->center == center)
        return;

    d_ptr->center = center;
    d_ptr->pixelsOutOfDate = true;

    if (!d_ptr->blockPropertyChangeSignals)
        emit centerChanged(d_ptr->center);
}

/*!
    \property QGeoMapData::center

    Returns the coordinate of the point in the center of the map viewport.
*/
QGeoCoordinate QGeoMapData::center() const
{
    return d_ptr->center;
}

/*!
    Changes the type of map data to display to \a mapType.
*/
void QGeoMapData::setMapType(QGraphicsGeoMap::MapType mapType)
{
    if (d_ptr->mapType == mapType)
        return;

    d_ptr->mapType = mapType;

    if (!d_ptr->blockPropertyChangeSignals)
        emit mapTypeChanged(mapType);
}

/*!
    \property QGeoMapData::mapType

    Returns the type of map data which is being displayed.
*/
QGraphicsGeoMap::MapType QGeoMapData::mapType() const
{
    return d_ptr->mapType;
}

/*!
    Changes the connectivity mode of this map to \a connectivityMode
*/
void QGeoMapData::setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (d_ptr->connectivityMode == connectivityMode)
        return;

    d_ptr->connectivityMode = connectivityMode;

    if (!d_ptr->blockPropertyChangeSignals)
        emit connectivityModeChanged(connectivityMode);
}

/*!
    \property QGeoMapData::connectivityMode

    Returns the connectivity mode for this map.
*/
QGraphicsGeoMap::ConnectivityMode QGeoMapData::connectivityMode() const
{
    return d_ptr->connectivityMode;
}

/*!
    Returns the map objects associated with this map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjects() const
{
    return d_ptr->containerObject->childObjects();
}

/*!
    Adds \a mapObject to the list of map objects managed by this map.

    The children objects are drawn in order of the QGeoMapObject::zValue()
    value.  Children objects having the same z value will be drawn
    in the order they were added.

    The map will take ownership of the \a mapObject.
*/
void QGeoMapData::addMapObject(QGeoMapObject *mapObject)
{
    d_ptr->addObject(mapObject);
}

/*!
    Removes \a mapObject from the list of map objects managed by this map.
    The map will release ownership of the \a mapObject.
*/
void QGeoMapData::removeMapObject(QGeoMapObject *mapObject)
{
    d_ptr->removeObject(mapObject);
}

/*!
    Clears the map objects associated with this map.

    The map objects will be deleted.
*/
void QGeoMapData::clearMapObjects()
{
    d_ptr->clearObjects();
}

/*!
    \fn QGeoBoundingBox QGeoMapData::viewport() const
    Returns a bounding box corresponding to the physical area displayed
    in the viewport of the map.

    The bounding box which is returned is defined by the upper left and
    lower right corners of the visible area of the map.
*/

/*!
    \fn void QGeoMapData::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)

    Attempts to fit the bounding box \a bounds into the viewport of the map.

    This method will change the zoom level to the maximum zoom level such
    that all of \a bounds is visible within the resulting viewport.

    If \a preserveViewportCenter is false the map will be centered on the
    bounding box \a bounds before the zoom level is changed, otherwise the
    center of the map will not be changed.
*/

/*!
    Returns the list of visible map objects managed by this map which
    contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    QList<QGeoMapObject*> results;
    QSet<QGeoMapObject*> considered;

    d_ptr->updateTransforms();

    QList<QGraphicsItem*> pixelItems;
    pixelItems = d_ptr->pixelScene->items(QRectF(screenPosition - QPointF(1,1),
                                                 screenPosition + QPointF(1,1)),
                                          Qt::IntersectsItemShape,
                                          Qt::AscendingOrder);

    foreach (QGraphicsItem *item, pixelItems) {
        QGeoMapObject *object = d_ptr->pixelItems.value(item);
        Q_ASSERT(object);

        if (object->isVisible() && !considered.contains(object)) {
            bool contains = false;

            if (d_ptr->pixelExact.contains(object)) {
                foreach (QGraphicsItem *item, d_ptr->pixelExact.values(object)) {
                    if (item->shape().contains(screenPosition)) {
                        contains = true;
                        break;
                    }
                }
            } else {
                QList<QTransform> trans = d_ptr->pixelTrans.values(object);

                foreach (QTransform t, trans) {
                    bool ok;
                    QTransform inv = t.inverted(&ok);
                    if (ok) {
                        QPointF testPt = screenPosition * inv;
                        if (object->graphicsItem()->shape().contains(testPt)) {
                            contains = true;
                            break;
                        }
                    }
                }
            }

            if (contains)
                results << object;

            considered.insert(object);
        }
    }

    return results;
}

/*!
    Returns the list of visible map objects managed by this map which are displayed at
    least partially within the on screen rectangle \a screenRect.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    QList<QGeoMapObject*> results;
    QSet<QGeoMapObject*> considered;

    d_ptr->updateTransforms();

    QList<QGraphicsItem*> pixelItems = d_ptr->pixelScene->items(screenRect,
                                                                Qt::IntersectsItemShape,
                                                                Qt::AscendingOrder);

    foreach (QGraphicsItem *item, pixelItems) {
        QGeoMapObject *object = d_ptr->pixelItems.value(item);
        Q_ASSERT(object);

        if (object->isVisible() && !considered.contains(object)) {
            QList<QTransform> trans = d_ptr->pixelTrans.values(object);
            bool contains = false;

            foreach (QTransform t, trans) {
                bool ok;
                QTransform inv = t.inverted(&ok);
                if (ok) {
                    QPolygonF testPoly = screenRect * inv;

                    QPainterPath testPath;
                    testPath.moveTo(testPoly[0]);
                    testPath.lineTo(testPoly[1]);
                    testPath.lineTo(testPoly[2]);
                    testPath.lineTo(testPoly[3]);
                    testPath.closeSubpath();

                    if (object->graphicsItem()->shape().intersects(testPath))
                        contains = true;
                }
            }

            if (contains)
                results << object;

            considered.insert(object);
        }
    }

    return results;
}

/*!
    Returns the list of visible map objects manager by this widget which
    are displayed at least partially within the viewport of the map.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInViewport() const
{
    return mapObjectsInScreenRect(QRectF(0.0,
                                         0.0,
                                         d_ptr->windowSize.width(),
                                         d_ptr->windowSize.height()));
}

/*!
\fn QPointF QGeoMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const

    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
*/

/*!
\fn QGeoCoordinate QGeoMapData::screenPositionToCoordinate(const QPointF &screenPosition) const

    Returns the coordinate corresponding to the point in the viewport at \a
    screenPosition.

    An invalid QGeoCoordinate will be returned if \a screenPosition is invalid
    or is not within the current viewport.
*/

/*!
    Paints the map and everything associated with it on \a painter, using the
    options \a option.

    This will paint the map with paintMap(), then the map overlays with
    QGeoMapOverlay::paint(), then the map objects with paintObjects(), and
    finally paintProviderNotices().
*/
void QGeoMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    paintMap(painter, option);

    for (int i = 0; i < d_ptr->overlays.size(); ++i)
        d_ptr->overlays[i]->paint(painter, option);

    paintObjects(painter, option);

    paintProviderNotices(painter, option);
}

/*!
    Paints the map on \a painter, using the options \a option.

    The map overlays, map objects and the provider notices (such as copyright
    and powered by notices) are painted in separate methods, which are combined
    in the paint() method.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Paints the map objects on \a painter, using the options \a option.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF target = option ? option->rect : QRectF(QPointF(0,0), d_ptr->windowSize);

    d_ptr->updateTransforms();

    QList<QGraphicsItem*> items = d_ptr->pixelScene->items(target, Qt::IntersectsItemShape,
                                                           Qt::DescendingOrder);
    QSet<QGeoMapObject*> objsDone;

    QTransform baseTrans = painter->transform();

    foreach (QGraphicsItem *item, items) {
        QGeoMapObject *object = d_ptr->pixelItems.value(item);
        Q_ASSERT(object);
        if (object->isVisible() && !objsDone.contains(object)) {
            if (d_ptr->pixelExact.contains(object)) {
                foreach (QGraphicsItem *it, d_ptr->pixelExact.values(object)) {
                    painter->setTransform(baseTrans);

                    QStyleOptionGraphicsItem *style = new QStyleOptionGraphicsItem;
                    it->paint(painter, style);
                }
            } else {
                foreach (QTransform trans, d_ptr->pixelTrans.values(object)) {
                    painter->setTransform(trans * baseTrans);

                    QStyleOptionGraphicsItem *style = new QStyleOptionGraphicsItem;
                    object->graphicsItem()->paint(painter, style);
                }
            }
            objsDone.insert(object);
        }
    }

    painter->setTransform(baseTrans);
}

/*!
    Paints the provider notices on \a painter, using the options \a option.

    The provider notices are things like the copyright and powered by notices.

    The provider may not want the client developers to be able to move the
    notices from their standard positions and so we have not provided API
    support for specifying the position of the notices at this time.

    If support for hinting at the position of the notices is to be provided by
    plugin parameters, the suggested parameter keys are
    "mapping.notices.copyright.alignment" and
    "mapping.notices.poweredby.alignment", with type Qt::Alignment.

    The default implementation does not paint anything.
*/
void QGeoMapData::paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
}

/*!
    Returns the map overlays associated with this map.
*/
QList<QGeoMapOverlay*> QGeoMapData::mapOverlays() const
{
    return d_ptr->overlays;
}

/*!
    Adds \a overlay to the list of map overlays associated with this map.

    The overlays will be drawn in the order in which they were added.

    The map will take ownership of \a overlay.
*/
void QGeoMapData::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    overlay->setMapData(this);
    d_ptr->overlays.append(overlay);
}

/*!
    Removes \a overlay from the list of map overlays associated with this map.

    The map will release ownership of \a overlay.
*/
void QGeoMapData::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.removeAll(overlay);
}

/*!
    Clears the map overlays associated with this map.

    The map overlays will be deleted.
*/
void QGeoMapData::clearMapOverlays()
{
    qDeleteAll(d_ptr->overlays);
    d_ptr->overlays.clear();
}


/*!
    Sets whether changes to properties will trigger their corresponding signals to \a block.

    By default the QGeoMapData implementations of the property functions are used
    which cause the property notification signals to be emitted immediately.

    Calling this function with \a block set to false will prevent these
    signals from being called, which will allow a subclass to defer the
    emission of the signal until a later time.

    If this function needs to be called it should be used as soon as possible,
    preferably in the constructor of the QGeoMapData subclass.
*/
void QGeoMapData::setBlockPropertyChangeSignals(bool block)
{
    d_ptr->blockPropertyChangeSignals = block;
}

/*!
\fn void QGeoMapData::windowSizeChanged(const QSizeF &windowSize)

    This signal is emitted when the size of the window which contains 
    the map has changed.

    The new value is \a windowSize.
*/

/*!
\fn void QGeoMapData::zoomLevelChanged(qreal zoomLevel)

    This signal is emitted when the zoom level of the map has changed.

    The new value is \a zoomLevel.
*/

/*!
\fn void QGeoMapData::centerChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the center of the map has changed.

    The new value is \a coordinate.
*/

/*!
\fn void QGeoMapData::mapTypeChanged(QGraphicsGeoMap::MapType mapType)

    This signal is emitted when the type of the map has changes.

    The value is \a mapType.
*/

/*!
\fn void QGeoMapData::connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)

    This signal is emitted when the connectivity mode used to fetch the 
    map data has changed.

    The new value is \a connectivityMode.
*/

/*!
\fn void QGeoMapData::updateMapDisplay(const QRectF &target)

    This signal is emitted when the region \a target of the window which 
    contains the map needs to be updated.


    If \a target is empty then the entire map will be updated.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMapData *parent, QGeoMappingManagerEngine *engine)
    : QObject(0),
      q_ptr(parent),
      engine(engine),
      containerObject(0),
      zoomLevel(-1.0),
      zoomOutOfDate(true),
      pixelsOutOfDate(true),
      windowSize(0, 0),
      blockPropertyChangeSignals(false),
      pixelScene(new QGraphicsScene),
      latLonScene(new QGraphicsScene)
{}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    if (containerObject)
        delete containerObject;
    qDeleteAll(overlays);
}

void QGeoMapDataPrivate::addObject(QGeoMapObject *object)
{
    containerObject->addChildObject(object);
    QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(object);

    zoomOutOfDate = true;
    pixelsOutOfDate = true;
    emit q_ptr->updateMapDisplay();
}

void QGeoMapDataPrivate::removeObject(QGeoMapObject *object)
{
    containerObject->removeChildObject(object);

    QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(object);
    if (group) {
        zoomOutOfDate = true;
        pixelsOutOfDate = true;
        emit q_ptr->updateMapDisplay();
    } else {
        QList<QRectF> rectsToUpdate;
        foreach (QGraphicsItem *item, pixelItems.keys(object))
            rectsToUpdate << item->boundingRect();

        latLonTrans.remove(object);
        foreach (QGraphicsItem *item, latLonItems.keys(object)) {
            latLonItems.remove(item);
            latLonScene->removeItem(item);
            delete item;
        }

        pixelTrans.remove(object);
        foreach (QGraphicsItem *item, pixelItems.keys(object)) {
            pixelItems.remove(item);
            pixelScene->removeItem(item);
            delete item;
        }

        foreach (QRectF rect, rectsToUpdate)
            emit q_ptr->updateMapDisplay(rect);
    }
}

void QGeoMapDataPrivate::clearObjects()
{
    foreach (QGeoMapObject *obj, containerObject->childObjects())
        removeObject(obj);

    containerObject->clearChildObjects();
}

/*
  A quick note about how the transforms are set up:

  Each QGeoMapObject has its own "local" coordinate system, whether this
  a pixel system or meters or whatever.

  For local systems that are not in pixels, we do a two-stage transform --
  first we transform the object to arc-seconds, then we transform from
  arc-seconds to pixels on the screen. This is necessary as QGeoMapData
  subclasses only provide a mapping from lat/lon coordinates to screen pixels
  and no other source.

  For local systems that are in pixels, we simply translate to the screen
  coordinates. However, we still generate a transform to arc-seconds for these
  objects, to speed up the drawing process.

  QGeoMapDataPrivate has two sets of 3 fields that are used here:
    - xxxTrans
    - xxxScene
    - xxxItems
  (where xxx = [latLon, pixel])

  xxxTrans = a multi-hash of transforms associated with each QGeoMapObject
             (a given map object may appear at more than one coordinate if
              it wraps over the dateline)
  xxxScene = a QGraphicsScene filled with bounding boxes for each object, one
             for each entry in xxxTrans
  xxxItems = a hash taking the items in the xxxScene and giving back the original
             QGeoMapObject they were created for

  the "latLon" entries describe the transformations from local coordinates to
  arc-seconds, and the "pixel" entries describe the transformations from local
  coordinates to pixels on screen.

  Items within latLonScene have their coordinates in arc-seconds, and items
  within pixelScene have theirs in pixels.

*/

void QGeoMapDataPrivate::updateLatLonTransforms(QGeoMapGroupObject *group)
{
    if (!group)
        group = containerObject;

    foreach (QGeoMapObject *object, group->childObjects()) {
        QGeoMapGroupObject *subGroup = qobject_cast<QGeoMapGroupObject*>(object);
        if (subGroup)
            updateLatLonTransforms(subGroup);
        else
            updateLatLonTransform(object);
    }
}

static QGraphicsPolygonItem *polyCopy(const QGraphicsPolygonItem *polyItem)
{
    QGraphicsPolygonItem *pi = new QGraphicsPolygonItem;
    pi->setBrush(polyItem->brush());
    pi->setPen(polyItem->pen());
    pi->setVisible(polyItem->isVisible());
    pi->setFillRule(polyItem->fillRule());
    pi->setOpacity(polyItem->opacity());
    pi->setPolygon(polyItem->polygon());
    pi->setGraphicsEffect(polyItem->graphicsEffect());
    return pi;
}

static QGraphicsPathItem *pathCopy(const QGraphicsPathItem *pathItem)
{
    QGraphicsPathItem *pi = new QGraphicsPathItem;
    pi->setBrush(pathItem->brush());
    pi->setPen(pathItem->pen());
    pi->setVisible(pathItem->isVisible());
    pi->setOpacity(pathItem->opacity());
    pi->setGraphicsEffect(pathItem->graphicsEffect());
    pi->setPath(pathItem->path());
    return pi;
}

bool QGeoMapDataPrivate::exactMetersToSeconds(const QGeoCoordinate &origin,
                                              QGeoMapObject *object,
                                              QGraphicsItem *item,
                                              QList<QPolygonF> &polys)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        QPolygonF poly = polyItem->polygon() * polyItem->transform();

        ProjPolygon p(poly, localSys);
        p.convert(wgs84);
        QPolygonF wgs = p.toPolygonF(3600.0);

        latLonExact.remove(object);
        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(wgs);
        latLonExact.insertMulti(object, pi);
        polys << wgs.boundingRect();

        QPolygonF westPoly = wgs * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << westPoly.boundingRect();

        QPolygonF eastPoly = wgs * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << eastPoly.boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        QPainterPath path = pathItem->path() * pathItem->transform();

        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element e = path.elementAt(i);

            ProjCoordinate c(e.x, e.y, 0.0, localSys);
            Q_ASSERT(c.convert(wgs84));

            path.setElementPositionAt(i, c.x() * 3600.0, c.y() * 3600.0);
        }

        latLonExact.remove(object);


        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(path.boundingRect());

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(westPath.boundingRect());

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(eastPath.boundingRect());

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed"
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

bool QGeoMapDataPrivate::exactSecondsToSeconds(const QGeoCoordinate &origin,
                                               QGeoMapObject *object,
                                               QGraphicsItem *item,
                                               QList<QPolygonF> &polys)
{
    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    QTransform toAbs;
    if (object->units() == QGeoMapObject::RelativeArcSecondUnit) {
        double ox = origin.longitude() * 3600.0;
        double oy = origin.latitude() * 3600.0;
        toAbs.translate(ox, oy);
    }

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        if (polyItem->polygon().isEmpty() || polyItem->polygon().size() < 3)
            return false;

        QPolygonF poly = polyItem->polygon() * polyItem->transform();
        poly = poly * toAbs;

        latLonExact.remove(object);
        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(poly);
        latLonExact.insertMulti(object, pi);
        polys << poly.boundingRect();

        QPolygonF westPoly = poly * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << westPoly.boundingRect();

        QPolygonF eastPoly = poly * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << eastPoly.boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        if (pathItem->path().isEmpty() || pathItem->path().elementCount() < 2)
            return false;

        QPainterPath path = pathItem->path() * pathItem->transform();
        path = path * toAbs;

        latLonExact.remove(object);
        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(path.boundingRect());

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(westPath.boundingRect());

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << QPolygonF(eastPath.boundingRect());

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed "
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

void QGeoMapDataPrivate::bilinearMetersToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    ProjPolygon p(local, localSys);
    if (!p.convert(wgs84)) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: projection is singular");
        return;
    }
    QPolygonF wgs = p.toPolygonF(3600.0);

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    latLon = item->transform() * latLon;
}

void QGeoMapDataPrivate::bilinearPixelsToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QPointF pixelOrigin = this->coordinateToScreenPosition(origin.longitude(),
                                                           origin.latitude());

    QPolygonF wgs;
    foreach (const QPointF &pt, local) {
        const QGeoCoordinate coord =
                q_ptr->screenPositionToCoordinate(pt + pixelOrigin);
        const QPointF lpt(coord.longitude() * 3600.0, coord.latitude() * 3600.0);
        wgs.append(lpt);
    }

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    latLon = item->transform() * latLon;
}

void QGeoMapDataPrivate::updateLatLonTransform(QGeoMapObject *object)
{
    QGeoCoordinate origin = object->origin();

    QGraphicsItem *item = object->graphicsItem();

    // skip any objects without graphicsitems
    if (!item)
        return;

    QRectF localRect = item->boundingRect();

    // skip any objects with invalid bounds
    if (!localRect.isValid() || localRect.isEmpty() || localRect.isNull())
        return;

    QPolygonF local = item->boundingRect() * item->transform();
    QList<QPolygonF> polys;

    latLonTrans.remove(object);

    if (object->transformType() == QGeoMapObject::BilinearTransform ||
            object->units() == QGeoMapObject::PixelUnit) {
        QTransform latLon;

        if (object->units() == QGeoMapObject::MeterUnit) {
            bilinearMetersToSeconds(origin, item, local, latLon);
        } else if (object->units() == QGeoMapObject::RelativeArcSecondUnit) {
            latLon.translate(origin.longitude() * 3600.0, origin.latitude() * 3600.0);
        } else if (object->units() == QGeoMapObject::PixelUnit) {
            bilinearPixelsToSeconds(origin, item, local, latLon);
        }

        polys << latLon.map(object->graphicsItem()->boundingRect());
        latLonTrans.insertMulti(object, latLon);

        QTransform latLonWest;
        latLonWest.translate(360.0 * 3600.0, 0.0);
        latLonWest = latLon * latLonWest;

        polys << latLonWest.map(object->graphicsItem()->boundingRect());
        latLonTrans.insertMulti(object, latLonWest);

        QTransform latLonEast;
        latLonEast.translate(-360.0 * 3600.0, 0.0);
        latLonEast = latLon * latLonEast;

        polys << latLonEast.map(object->graphicsItem()->boundingRect());
        latLonTrans.insertMulti(object, latLonEast);

    } else if (object->transformType() == QGeoMapObject::ExactTransform) {
        if (object->units() == QGeoMapObject::MeterUnit) {
            if (!exactMetersToSeconds(origin, object, item, polys))
                return;
        } else if (object->units() == QGeoMapObject::AbsoluteArcSecondUnit ||
                   object->units() == QGeoMapObject::RelativeArcSecondUnit) {
            if (!exactSecondsToSeconds(origin, object, item, polys))
                return;
        } else {
            qWarning("QGeoMapData: unknown units for map object");
            return;
        }
    }

    QList<QGraphicsItem*> items = latLonItems.keys(object);
    foreach (QGraphicsItem *item, items) {
        latLonScene->removeItem(item);
        latLonItems.remove(item);
        delete item;
    }

    foreach (QPolygonF poly, polys) {
        QGraphicsItem *item = new QGraphicsPolygonItem(poly);
        item->setZValue(object->zValue());
        item->setVisible(true);
        latLonItems.insert(item, object);
        latLonScene->addItem(item);
    }
}

QPolygonF QGeoMapDataPrivate::latLonViewport()
{
    QPolygonF view;
    QGeoBoundingBox viewport = q_ptr->viewport();
    QGeoCoordinate c, c2;
    double offset = 0.0;

    c = viewport.bottomLeft();
    view << QPointF(c.longitude() * 3600.0, c.latitude() * 3600.0);
    c2 = viewport.bottomRight();
    if (c2.longitude() < c.longitude())
        offset = 360.0 * 3600.0;
    view << QPointF(c2.longitude() * 3600.0 + offset, c2.latitude() * 3600.0);
    c = viewport.topRight();
    view << QPointF(c.longitude() * 3600.0 + offset, c.latitude() * 3600.0);
    c = viewport.topLeft();
    view << QPointF(c.longitude() * 3600.0, c.latitude() * 3600.0);

    return view;
}

void QGeoMapDataPrivate::updatePixelTransforms(QGeoMapGroupObject *group)
{
    if (group == 0)
        group = containerObject;

    QPolygonF view = latLonViewport();

    QList<QGraphicsItem*> itemsInView;
    itemsInView = latLonScene->items(view, Qt::IntersectsItemShape,
                                     Qt::AscendingOrder);

    pixelTrans.clear();
    pixelItems.clear();
    if (pixelScene)
        delete pixelScene;
    pixelScene = new QGraphicsScene();

    QSet<QGeoMapObject*> done;

    foreach (QGraphicsItem *latLonItem, itemsInView) {
        QGeoMapObject *object = latLonItems.value(latLonItem);
        Q_ASSERT(object);
        if (!done.contains(object)) {
            updatePixelTransform(object);
            done.insert(object);
        }
    }
}

QPointF QGeoMapDataPrivate::coordinateToScreenPosition(double lon, double lat) const
{
    QGeoCoordinate c(lat, lon);
    return q_ptr->coordinateToScreenPosition(c);
}

void QGeoMapDataPrivate::pixelShiftToScreen(const QGeoCoordinate &origin,
                                            QGeoMapObject *object,
                                            QList<QPolygonF> &polys)
{
    // compute the transform as an origin shift
    QList<QPointF> origins;
    origins << QPointF(origin.longitude(), origin.latitude());
    origins << QPointF(origin.longitude() + 360.0, origin.latitude());
    origins << QPointF(origin.longitude() - 360.0, origin.latitude());

    foreach (QPointF o, origins) {
        QTransform pixel = object->graphicsItem()->transform();
        QPointF pixelOrigin = coordinateToScreenPosition(o.x(), o.y());
        pixel.translate(pixelOrigin.x(), pixelOrigin.y());
        pixelTrans.insertMulti(object, pixel);
        polys << pixel.map(object->graphicsItem()->boundingRect());
    }
}

QPolygonF QGeoMapDataPrivate::polyToScreen(const QPolygonF &poly)
{
    QPolygonF r;
    foreach (QPointF pt, poly) {
        double x = pt.x() / 3600.0;
        double y = pt.y() / 3600.0;
        QPointF pixel = this->coordinateToScreenPosition(x, y);
        r.append(pixel);
    }
    return r;
}

void QGeoMapDataPrivate::bilinearSecondsToScreen(const QGeoCoordinate &origin,
                                                 QGeoMapObject *object,
                                                 QList<QPolygonF> &polys)
{
    QList<QTransform> latLons = latLonTrans.values(object);

    // compute the transform by linearising from the lat/lon space
    foreach (QTransform latLon, latLons) {
        QTransform pixel;

        QGraphicsItem *item = object->graphicsItem();
        QPolygonF local = item->boundingRect();
        QPolygonF latLonPoly = latLon.map(local);

        QPolygonF pixelPoly = polyToScreen(latLonPoly);

        // QTransform expects the last vertex (closing vertex) to be dropped
        local.remove(4);
        pixelPoly.remove(4);

        bool ok = QTransform::quadToQuad(local, pixelPoly, pixel);
        if (!ok) {
            qWarning("QGeoMapData: bilinear transform to screen from arc-seconds "
                     "failed: could not compute transformation matrix");
            return;
        }

        pixelTrans.insertMulti(object, pixel);

        polys << pixelPoly;
    }
}

void QGeoMapDataPrivate::exactPixelMap(const QGeoCoordinate &origin,
                                       QGeoMapObject *object,
                                       QList<QPolygonF> &polys)
{
    QList<QGraphicsItem*> latLonItems = latLonExact.values(object);

    pixelExact.remove(object);

    foreach (QGraphicsItem *latLonItem, latLonItems) {
        QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(latLonItem);
        if (polyItem) {
            QPolygonF poly = polyItem->polygon();
            QPolygonF pixelPoly = polyToScreen(poly);

            QGraphicsPolygonItem *pi = polyCopy(polyItem);
            pi->setPolygon(pixelPoly);
            pixelExact.insertMulti(object, pi);
            polys << pixelPoly.boundingRect();
        }

        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(latLonItem);
        if (pathItem) {
            QPainterPath path = pathItem->path();

            for (int i = 0; i < path.elementCount(); ++i) {
                QPainterPath::Element e = path.elementAt(i);

                double x = e.x;
                x /= 3600.0;
                double y = e.y;
                y /= 3600.0;

                QPointF pixel = this->coordinateToScreenPosition(x, y);

                path.setElementPositionAt(i, pixel.x(), pixel.y());
            }

            QGraphicsPathItem *pi = pathCopy(pathItem);
            pi->setPath(path);
            pixelExact.insertMulti(object, pi);
            polys << QPolygonF(path.boundingRect());
        }
    }
}

void QGeoMapDataPrivate::updatePixelTransform(QGeoMapObject *object)
{
    QGeoCoordinate origin = object->origin();
    QGraphicsItem *item = object->graphicsItem();

    // skip any objects without graphicsitems
    if (!item)
        return;

    QList<QGraphicsItem*> items = pixelItems.keys(object);
    foreach (QGraphicsItem *item, items) {
        pixelItems.remove(item);
        pixelScene->removeItem(item);
        delete item;
    }

    QList<QPolygonF> polys;

    pixelTrans.remove(object);
    if (object->transformType() == QGeoMapObject::BilinearTransform) {
        bilinearSecondsToScreen(origin, object, polys);
    } else if (object->transformType() == QGeoMapObject::ExactTransform) {
        if (object->units() == QGeoMapObject::PixelUnit) {
            pixelShiftToScreen(origin, object, polys);
        } else {
            exactPixelMap(origin, object, polys);
        }
    }

    foreach (QPolygonF poly, polys) {
        QGraphicsPolygonItem *item = new QGraphicsPolygonItem(poly);
        item->setZValue(object->zValue());
        item->setVisible(true);
        pixelItems.insert(item, object);
        pixelScene->addItem(item);
    }
}

void QGeoMapDataPrivate::forceUpdate(const QRectF &target)
{
    emit q_ptr->updateMapDisplay(target);
}

void QGeoMapDataPrivate::forceUpdate(QGeoMapObject *obj)
{
    updateLatLonTransform(obj);

    const QRectF view = latLonViewport().boundingRect();

    bool needsPixelUpdate = false;
    foreach (QGraphicsItem *item, latLonItems.keys(obj)) {
        if (item->boundingRect().intersects(view)) {
            needsPixelUpdate = true;
            break;
        }
    }

    if (needsPixelUpdate) {
        /* make a list of rects that need updating but don't fire the signal
           until after we've run updatePixelTransform */
        QList<QRectF> rectsToUpdate;
        foreach (QGraphicsItem *item, pixelItems.keys(obj))
            rectsToUpdate << item->boundingRect();

        updatePixelTransform(obj);

        foreach (QGraphicsItem *item, pixelItems.keys(obj))
            emit q_ptr->updateMapDisplay(item->boundingRect());

        foreach (QRectF rect, rectsToUpdate)
            emit q_ptr->updateMapDisplay(rect);
    }
}

// update the transform tables as necessary
void QGeoMapDataPrivate::updateTransforms()
{
    if (zoomOutOfDate)
        updateLatLonTransforms();
    if (pixelsOutOfDate)
        updatePixelTransforms();

    zoomOutOfDate = false;
    pixelsOutOfDate = false;
}

// ensures the sender is up to date on the map
void QGeoMapDataPrivate::updateSender()
{
    QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(sender());
    if (group) {
        zoomOutOfDate = true;
        pixelsOutOfDate = true;
        emit q_ptr->updateMapDisplay();
    } else {
        QGeoMapObject *obj = qobject_cast<QGeoMapObject*>(sender());
        if (obj)
            forceUpdate(obj);
    }
}

#include "moc_qgeomapdata.cpp"
#include "moc_qgeomapdata_p.cpp"

QTM_END_NAMESPACE
