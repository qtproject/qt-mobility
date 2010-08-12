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

#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeomapobject.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomapoverlay.h"

#include "qgeomapobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapData

    \brief The QGeoMapData class are used as a bridge between QGeoMapWidget and
    QGeoMappingManager.


    \inmodule QtLocation

    \ingroup maps-impl

    Instances of QGeoMapData are created with
    QGeoMappingManager::createMapData(), and are used internally by
    QGeoMappingWidget to manage the state of the map and the associated
    QGeoMapObject instances.

    Plugin implementers will need to provide implementations of
    coordinateToScreenPosition(const QGeoCoordinate &coordinate) and
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition).

     The other virtual functions can be overriden.  If the screen position to
     coordinate tranformations are expensive then overriding these functions
     may allow optimizations based on caching parts of the geometry information.
 */

/*!
    Constructs a new map data object, which stores the map data required by
    \a widget and makes use of the functionality provided by \a engine.
*/
QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : d_ptr(new QGeoMapDataPrivate(this, engine, geoMap)) {}

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

    if (d->engine)
        d->engine->removeMapData(this);

    delete d;
}

/*!
    Returns the QGraphicsGeoMap instance that this map data object is associated with.
*/
QGraphicsGeoMap* QGeoMapData::geoMap() const
{
    return d_ptr->geoMap;
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

    The size will be adjusted by the associated QGeoMapWidget as it resizes.
*/
void QGeoMapData::setViewportSize(const QSizeF &size)
{
    d_ptr->viewportSize = size;
}

/*!
    Returns the size of the map viewport.

    The size will be adjusted by the associated QGeoMapWidget as it resizes.
*/
QSizeF QGeoMapData::viewportSize() const
{
    return d_ptr->viewportSize;
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
    d_ptr->zoomLevel = zoomLevel;
}

/*!
    Returns the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapData::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

void QGeoMapData::startPanning() {}

void QGeoMapData::stopPanning() {}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.
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
    d_ptr->center = center;
}

/*!
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
    d_ptr->mapType = mapType;
}

/*!
    Returns the type of map data which is being displayed.
*/
QGraphicsGeoMap::MapType QGeoMapData::mapType() const
{
    return d_ptr->mapType;
}

/*!
*/
QDeclarativeListProperty<QGeoMapObject> QGeoMapData::mapObjects()
{
    return d_ptr->containerObject->childObjects();
}

/*!
*/
int QGeoMapData::mapObjectCount() const
{
    return d_ptr->containerObject->childObjectCount();
}

/*!
    Adds \a mapObject to the list of map objects managed by this map.
    The map will take ownership of the \a mapObject.
*/
void QGeoMapData::addMapObject(QGeoMapObject *mapObject)
{
    d_ptr->containerObject->addChildObject(mapObject);
}

/*!
    Removes \a mapObject from the list of map objects managed by this map.
    The map will release ownership of the \a mapObject.
*/
void QGeoMapData::removeMapObject(QGeoMapObject *mapObject)
{
    d_ptr->containerObject->removeChildObject(mapObject);
}

/*!
*/
QGeoMapObject* QGeoMapData::mapObject(int index) const
{
    return d_ptr->containerObject->childObject(index);
}

/*!
*/
void QGeoMapData::clearMapObjects()
{
    d_ptr->containerObject->clearChildObjects();
}

/*!
    Returns the list of map objects managed by this map which
    contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition)
{
    QList<QGeoMapObject*> results;

    QGeoCoordinate coord = screenPositionToCoordinate(screenPosition);
    int childObjectCount = d_ptr->containerObject->childObjectCount();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObject(i);
        if (object->contains(coord))
            results.append(object);
    }

    return results;
}

/*!
    Returns the list of map objects managed by this map which are displayed at
    least partially within the on screen rectangle \a screenRect.
*/
QList<QGeoMapObject*> QGeoMapData::mapObjectsInScreenRect(const QRectF &screenRect)
{
    QList<QGeoMapObject*> results;

    // TODO - find a way to disambiguate rectangles at poles
    QGeoCoordinate topLeft = screenPositionToCoordinate(screenRect.topLeft());
    QGeoCoordinate bottomRight = screenPositionToCoordinate(screenRect.bottomRight());

    QGeoBoundingBox bounds(topLeft, bottomRight);

    int childObjectCount = d_ptr->containerObject->childObjectCount();
    for (int i = 0; i < childObjectCount; ++i) {
        QGeoMapObject *object = d_ptr->containerObject->childObject(i);
        if (bounds.intersects(object->boundingBox()))
            results.append(object);
    }

    return results;
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
*/
void QGeoMapData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    for (int i = 0; i < d_ptr->overlays.size(); ++i)
        d_ptr->overlays[i]->paint(painter, option);
}

/*!
*/
QList<QGeoMapOverlay*> QGeoMapData::mapOverlays() const
{
    return d_ptr->overlays;
}

/*!
*/
void QGeoMapData::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.append(overlay);
}

/*!
*/
void QGeoMapData::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay)
        return;

    d_ptr->overlays.removeAll(overlay);
}

/*!
*/
void QGeoMapData::clearMapOverlays()
{
    qDeleteAll(d_ptr->overlays);
    d_ptr->overlays.clear();
}

/*!
*/
void QGeoMapData::setupMapObject(QGeoMapObject *mapObject) {}

/*******************************************************************************
*******************************************************************************/

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMapData *parent, QGeoMappingManagerEngine *engine, QGraphicsGeoMap *geoMap)
        : q_ptr(parent),
        engine(engine),
        geoMap(geoMap),
        zoomLevel(-1.0)
{
    Q_Q(QGeoMapData);
    containerObject = new QGeoMapObject(q);
}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    delete containerObject;
    qDeleteAll(overlays);
}

void QGeoMapDataPrivate::setObjectInfo(QGeoMapObject *object, QGeoMapObjectInfo *info)
{
    object->d_ptr->info = info;
}

#include "moc_qgeomapdata.cpp"

QTM_END_NAMESPACE
