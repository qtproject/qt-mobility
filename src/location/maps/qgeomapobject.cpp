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
    QGraphicsGeoMap instancse, that is specified in terms of coordinates and
    distances.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

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
    Constructs a new map object associated with \a mapData.
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

    QTransform trans = d_ptr->mapData->d_ptr->latLonTrans.value(this);

    QRectF bounds = d_ptr->graphicsItem->boundingRect();
    QPolygonF poly = bounds * trans;
    QRectF latLonBounds = poly.boundingRect();

    QGeoCoordinate topLeft(latLonBounds.topLeft().y(),
                           latLonBounds.topLeft().x());
    QGeoCoordinate bottomRight(latLonBounds.bottomRight().y(),
                               latLonBounds.bottomRight().x());

    return QGeoBoundingBox(topLeft, bottomRight);
}

/*!
    Returns whether \a coordinate is contained with the boundary of this
    map object.
*/
bool QGeoMapObject::contains(const QGeoCoordinate &coordinate) const
{
    if (!d_ptr->graphicsItem || !d_ptr->mapData)
        return false;

    QTransform trans = d_ptr->mapData->d_ptr->latLonTrans.value(this);
    bool ok;
    QTransform inv = trans.inverted(&ok);
    if (!ok)
        return false;

    QPointF latLonPoint(coordinate.longitude(), coordinate.latitude());
    QPointF localPoint = latLonPoint * inv;

    return d_ptr->graphicsItem->contains(localPoint);
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

QGeoMapObject::CoordinateUnit QGeoMapObject::units() const
{
    return d_ptr->units;
}

void QGeoMapObject::setUnits(const CoordinateUnit &unit)
{
    d_ptr->units = unit;
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
      graphicsItem(0) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate()
{
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qgeomapobject.cpp"

QTM_END_NAMESPACE
