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

#include "qgeomapobject.h"
#include "qgeomapobject_p.h"
#include "qgeomapobjectengine_p.h"
#include "qgeomapdata.h"
#include "qgeomapdata_p.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "projwrapper_p.h"

#include <QtAlgorithms>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapObject
    \brief The QGeoMapObject class is graphical item for display in
    QGraphicsGeoMap instances, that is specified in terms of coordinates and
    distances.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    QGeoMapObject itself can be used to facilitate the display of any
    arbitrary QGraphicsItem on a map. To this end, it supports different
    coordinate systems and the concept of an "origin point" for translation
    into map coordinates.

    For example, the following code creates a QGraphicsEllipseItem and a
    QGeoMapObject to display it. The EllipseItem extends from the origin point,
    out 20 meters to the east and 30 metres south.

    \code
    QGraphicsEllipseItem *ellipseItem = new QGraphicsEllipseItem;
    ellipseItem->setRect(0, 0, 20, 30);

    QGeoMapObject *mapObject = new QGeoMapObject;
    mapObject->setGraphicsItem(ellipseItem);
    mapObject->setUnits(QGeoMapObject::MeterUnit);
    mapObject->setOrigin(QGeoCoordinate(-27.5796, 153.1));
    \endcode

    Normally, the GraphicsItem will be transformed into map coordinates using
    a bilinear interpolation. Another option is the ExactTransform, which
    converts the GraphicsItem exactly into map coordinates, but is only available
    for certain subclasses. Other interpolation methods may be provided in
    future for greater accuracy near poles and in different map projections,
    without the limitations of ExactTransform.

    Calling setUnits() or setting the units property will result in the
    default value of transformType being restored. See QGeoMapObject::transformType
    for more details.

    QGeoMapObject instances can also be grouped into heirarchies in order to
    simplify the process of creating compound objects and managing groups of
    objects.
*/

/*!
    \enum QGeoMapObject::Type

    Describes the type of a map object.

    \value NullType
        An empty QGeoMapObject.
    \value GroupType
        A QGeoMapObject used to organize other map objects into groups.
    \value RectangleType
        A QGeoMapObject used to display a rectangular region.
    \value CircleType
        A QGeoMapObject used to display a circular region.
    \value PolylineType
        A QGeoMapObject used to display a multi-segment line.
    \value PolygonType
        A QGeoMapObject used to display a polygonal region.
    \value PixmapType
        A QGeoMapObject used to display a pixmap on a map.
    \value TextType
        A QGeoMapObject used to display text on a map
    \value RouteType
        A QGeoMapObject used to display a route.
*/

/*!
    \enum QGeoMapObject::CoordinateUnit

    Describes the units of measurement used for a map object's
    graphics item.

    \value PixelUnit
        Units are in pixels on the screen. Pixel coordinate (0,0) is
        translated to the origin coordinate.
    \value MeterUnit
        Units are in meters on the ground -- a local Transverse Mercator
        coordinate system (on the WGS84 ellipsoid) is used for translation,
        centered on the origin coordinate.
    \value RelativeArcSecondUnit
        Units are in arc seconds relative to the origin coordinate (along the
        WGS84 ellipsoid).
    \value AbsoluteArcSecondUnit
        Units are in arc seconds on WGS84, origin ignored.
*/

/*!
    \enum QGeoMapObject::TransformType

    Describes the type of transformation applied to change this object's
    coordinate system into screen coordinates.

    \value BilinearTransform
        This object's bounding box is taken, and transformed at each of its
        corners into screen coordinates. A bilinear interpolation is then used
        to draw the rest of the object's GraphicsItem.
    \value ExactTransform
        Individual key points on the object are transformed and the GraphicsItem
        is constructed in direct pixel coordinates. This is only available for
        certain subclasses, depending on the implementation of QGeoMapData used.
*/

/*!
    Constructs a new map object associated with \a mapData.

    The object will be in pixel coordinates, with exact transform.
*/
QGeoMapObject::QGeoMapObject(QGeoMapData *mapData)
    : d_ptr(new QGeoMapObjectPrivate())
{
    setMapData(mapData);

    connect(this, SIGNAL(originChanged(QGeoCoordinate)),
            this, SIGNAL(mapNeedsUpdate()));
    connect(this, SIGNAL(selectedChanged(bool)),
            this, SIGNAL(mapNeedsUpdate()));
    connect(this, SIGNAL(visibleChanged(bool)),
            this, SIGNAL(mapNeedsUpdate()));
    connect(this, SIGNAL(zValueChanged(int)),
            this, SIGNAL(mapNeedsUpdate()));
    connect(this, SIGNAL(graphicsItemChanged(QGraphicsItem*)),
            this, SIGNAL(mapNeedsUpdate()));
}

/*!
    Destroys this map object.
*/
QGeoMapObject::~QGeoMapObject()
{
    setMapData(0);
    delete d_ptr;
}

/*!
    Causes the QGeoMapData containing this object to be updated.
*/
void QGeoMapObject::update()
{
    emit mapNeedsUpdate();
}

/*!
    Returns the type of this map object.
*/
QGeoMapObject::Type QGeoMapObject::type() const
{
    if (d_ptr->graphicsItem)
        return QGeoMapObject::CustomType;
    else
        return QGeoMapObject::NullType;
}

/*!
    \property QGeoMapObject::zValue
    \brief This property holds the z-value of the map object.

    The z-value determines the order in which the objects are drawn on the
    screen.  Objects with the same value will be drawn in the order that
    they were added to the map or map object.

    This is the same behaviour as QGraphicsItem.
*/
void QGeoMapObject::setZValue(int zValue)
{
    if (d_ptr->zValue != zValue) {
        d_ptr->zValue = zValue;
        if (d_ptr->graphicsItem)
            d_ptr->graphicsItem->setZValue(zValue);
        emit zValueChanged(d_ptr->zValue);
        if (d_ptr->mapData && d_ptr->mapData->d_ptr->oe) {
            QGeoMapObjectEngine *e = d_ptr->mapData->d_ptr->oe;
            e->rebuildScenes();
        }
        emit mapNeedsUpdate();
    }
}

int QGeoMapObject::zValue() const
{
    return d_ptr->zValue;
}

/*!
    \property QGeoMapObject::visible
    \brief This property holds whether the map object is visible.

    If this map object is not visible then none of the childObjects() will
    be displayed either.
*/
void QGeoMapObject::setVisible(bool visible)
{
    if (d_ptr->isVisible != visible) {
        d_ptr->isVisible = visible;
        if (d_ptr->graphicsItem)
            d_ptr->graphicsItem->setVisible(visible);
        emit visibleChanged(d_ptr->isVisible);
    }
}

bool QGeoMapObject::isVisible() const
{
    return d_ptr->isVisible;
}

/*!
    \property QGeoMapObject::selected
    \brief This property holds whether the map object is selected.
*/
void QGeoMapObject::setSelected(bool selected)
{
    if (d_ptr->isSelected != selected) {
        d_ptr->isSelected = selected;
        emit selectedChanged(d_ptr->isSelected);
    }
}

bool QGeoMapObject::isSelected() const
{
    return d_ptr->isSelected;
}

/*!
    Returns a bounding box which contains this map object.
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    if (!d_ptr->graphicsItem || !d_ptr->mapData)
        return QGeoBoundingBox();

    QGeoMapObjectEngine *e = d_ptr->mapData->d_ptr->oe;

    e->updateTransforms();
    QTransform trans = e->latLonTrans.value(this);

    QRectF bounds = d_ptr->graphicsItem->boundingRect();
    QPolygonF poly = bounds * trans;

    QRectF latLonBounds = poly.boundingRect();
    QPointF topLeft = latLonBounds.bottomLeft();
    if (topLeft.x() > 180.0 * 3600.0)
        topLeft.setX(topLeft.x() - 360.0 * 3600.0);
    if (topLeft.x() < -180.0 * 3600.0)
        topLeft.setX(topLeft.x() + 360.0 * 3600.0);

    QPointF bottomRight = latLonBounds.topRight();
    if (bottomRight.x() > 180.0 * 3600.0)
        bottomRight.setX(bottomRight.x() - 360.0 * 3600.0);
    if (bottomRight.x() < -180.0 * 3600.0)
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);

    QGeoCoordinate tlc(topLeft.y() / 3600.0, topLeft.x() / 3600.0);
    QGeoCoordinate brc(bottomRight.y() / 3600.0, bottomRight.x() / 3600.0);

    return QGeoBoundingBox(tlc, brc);
}

/*!
    Returns whether \a coordinate is contained with the boundary of this
    map object.
*/
bool QGeoMapObject::contains(const QGeoCoordinate &coordinate) const
{
    if (!d_ptr->graphicsItem || !d_ptr->mapData)
        return false;

    QGeoMapObjectEngine *e = d_ptr->mapData->d_ptr->oe;

    e->updateTransforms();
    QPointF latLonPoint(coordinate.longitude()*3600.0, coordinate.latitude()*3600.0);

    if (e->latLonExact.contains(this)) {
        QList<QGraphicsItem*> items = e->latLonExact.values(this);
        foreach (QGraphicsItem *item, items) {
            if (item->contains(latLonPoint))
                return true;
        }
        return false;
    } else {
        QList<QTransform> transList = e->latLonTrans.values(this);
        foreach (QTransform trans, transList) {
            bool ok;
            QTransform inv = trans.inverted(&ok);
            if (!ok)
                continue;

            QPointF localPoint = latLonPoint * inv;

            if (d_ptr->graphicsItem->contains(localPoint))
                return true;
        }
        return false;
    }
}

/*!
    \internal
*/
bool QGeoMapObject::operator<(const QGeoMapObject &other) const
{
    return d_ptr->zValue < other.d_ptr->zValue;
}

/*!
    \internal
*/
bool QGeoMapObject::operator>(const QGeoMapObject &other) const
{
    return d_ptr->zValue > other.d_ptr->zValue;
}

/*!
    Associates the QGeoMapData instance \a mapData with this map object.

    This will create an appropriate QGeoMapObjectInfo instance for
    this QGeoMapObject and will connect the appropriate signals to it
    so that it can be kept up to date.
*/
void QGeoMapObject::setMapData(QGeoMapData *mapData)
{
    if (d_ptr->mapData == mapData)
        return;

    if (d_ptr->mapData) {
        disconnect(this, SIGNAL(mapNeedsUpdate()),
                   d_ptr->mapData->d_ptr, SLOT(updateSender()));
    }

    d_ptr->mapData = mapData;
    if (!d_ptr->mapData)
        return;

    connect(this, SIGNAL(mapNeedsUpdate()),
            d_ptr->mapData->d_ptr, SLOT(updateSender()));
}

/*!
    Returns the QGeoMapData instance associated with this object.

    Will return 0 if not QGeoMapData instance has been set.
*/
QGeoMapData* QGeoMapObject::mapData() const
{
    return d_ptr->mapData;
}

/*!
    \property QGeoMapObject::graphicsItem
    \brief This property holds the GraphicsItem that will be drawn on the map.

    The GraphicsItem's coordinates are in the units specified by the
    QGeoMapObject::units property.

    To offset the coordinates of the GraphicsItem (for items that can only
    be constructed about their parent position), use the QGraphicsItem::setTransform
    method with a QTransform containing the desired translation.
*/
QGraphicsItem * const QGeoMapObject::graphicsItem() const
{
    return d_ptr->graphicsItem;
}

void QGeoMapObject::setGraphicsItem(QGraphicsItem *item)
{
    if (item == d_ptr->graphicsItem)
        return;

    d_ptr->graphicsItem = item;
    item->setZValue(this->zValue());
    emit graphicsItemChanged(item);
}

/*!
    \property QGeoMapObject::transformType
    \brief This property holds the transformation type used to draw the object.

    \sa QGeoMapObject::TransformType
*/
QGeoMapObject::TransformType QGeoMapObject::transformType() const
{
    return d_ptr->transType;
}

void QGeoMapObject::setTransformType(const TransformType &type)
{
    d_ptr->transType = type;
    emit mapNeedsUpdate();
}

/*!
    \property QGeoMapObject::origin
    \brief This property holds the origin of the object's coordinate system.

    How the origin coordinate is used depends on the selected coordinate
    system, see QGeoMapObject::TransformType for more details.
*/
QGeoCoordinate QGeoMapObject::origin() const
{
    return d_ptr->origin;
}

void QGeoMapObject::setOrigin(const QGeoCoordinate &origin)
{
    if (origin == d_ptr->origin)
        return;

    d_ptr->origin = origin;
    emit originChanged(origin);
}

/*!
    \property QGeoMapObject::units
    \brief This property holds the units of measurement for the object.

    Note that setting this property will reset the transformType property to
    the default for the units given. For PixelUnit, this is ExactTransform,
    and for all others, BilinearTransform.

    \sa QGeoMapObject::CoordinateUnit
*/
QGeoMapObject::CoordinateUnit QGeoMapObject::units() const
{
    return d_ptr->units;
}

void QGeoMapObject::setUnits(const CoordinateUnit &unit)
{
    d_ptr->units = unit;

    if (unit == QGeoMapObject::PixelUnit)
        setTransformType(QGeoMapObject::ExactTransform);
    else
        setTransformType(QGeoMapObject::BilinearTransform);

    emit mapNeedsUpdate();
}

/*!
\fn void QGeoMapObject::zValueChanged(int zValue)

    This signal is emitted when the z value of the map object 
    has changed.

    The new value is \a zValue.
*/

/*!
\fn void QGeoMapObject::visibleChanged(bool visible)

    This signal is emitted when the visible state of the map object 
    has changed.

    The new value is \a visible.
*/

/*!
\fn void QGeoMapObject::selectedChanged(bool selected)

    This signal is emitted when the selected state of the map object
    has changed.

    The new vlaue is \a selected.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapObjectPrivate::QGeoMapObjectPrivate()
    : zValue(0),
      isVisible(true),
      isSelected(false),
      mapData(0),
      units(QGeoMapObject::PixelUnit),
      transType(QGeoMapObject::ExactTransform),
      graphicsItem(0) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate()
{
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qgeomapobject.cpp"

QTM_END_NAMESPACE
