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
#include "qmapobject.h"
#include "qmapobject_p.h"

QTM_BEGIN_NAMESPACE

QMapObjectPrivate::QMapObjectPrivate()
        : intersectingTiles(QList<quint64>()), mapView(NULL),
        objType(QMapObject::NullObject), z(0)
{
}

/*!
    \class QMapObject
    \brief The QMapObject class is the base of all map objects
    \ingroup location

    This is the abstract base class of all internal map objects. Map objects store all information
    that is needed to generate concrete Qt:QGraphicsItem objects when they become covered by the
    view port.
*/

/*!
    \enum QMapObject::QMapObjectType
    Describes the type of the map object.
    \value MarkerObject The object is a marker.
    \value LineObject The object is a line.
    \value RectObject The object is a rectangle.
    \value EllipseObject The object is an ellipse.
    \value PolygonObject The object is a polygon.
    \value PixmapObject The object is a pixmap.
    \value NullObject The object is invalid.
*/

/*!
    Constructs the map object with the given \a type and layer \a z.
*/
QMapObject::QMapObject(QMapObject::MapObjectType type, quint16 z)
        : d_ptr(new QMapObjectPrivate)
{
    Q_D(QMapObject);
    d->objType = type;
    d->z = z;
}

/*!
    \internal
*/
QMapObject::QMapObject(QMapObjectPrivate &dd, MapObjectType type, quint16 z)
        : d_ptr(&dd)
{
    Q_D(QMapObject);
    d->objType = type;
    d->z = z;
}

/*!
    Destroys the map object
*/
QMapObject::~QMapObject()
{
    Q_D(QMapObject);
    delete d;
}

/*!
    Returns the z index of this map object, as used for layering.
    Objects with higher z indices are stacked on top of objects
    with lower z indices.
*/
quint16 QMapObject::zValue() const
{
    Q_D(const QMapObject);
    return d->z;
}

QMapObject::MapObjectType QMapObject::type() const
{
    Q_D(const QMapObject);
    return d->objType;
}

/*!
    \fn bool QMapObject::intersects(const QRectF& rect) const = 0;
    Computes whether this map object intersects with or completely lies
    in the given \a rect.  \a rect is defined in terms of map(pixel)
    coordinates.

    Returns true if there is an intersection, otherwise returns false
*/

/*!
    \fn void QMapObject::compMapCoords() = 0

    Computes the appropriate map coordinates (in pixels) according to the
    current QMapView setup.
*/

/*!
    Updates(internally) the set of tiles this object intersects with
    when viewed with viewport defined by \a box.

    This function operates using map(pixel) coordinates.
*/
void QMapObject::compIntersectingTiles(const QRectF& box)
{
    Q_D(QMapObject);

    if (!d->mapView)
        return;

    QMapView::TileIterator it(*d->mapView, box);

    while (it.hasNext()) {
        it.next();

        if (it.isValid() && intersects(it.tileRect()))
            d->intersectingTiles.append(it.index());
    }
}

/*!
    \fn void QMapObject::paint(QPainter* painter, const QRectF& viewPort)
*/

/*!
    Returns the rectangle covered by \a line.

    This function operates using map(pixel) coordinates.
*/
QRectF QMapObject::boundingRect(const QLineF& line)
{
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;

    if (line.x1() < line.x2()) {
        left = line.x1();
        right = line.x2();
    } else {
        left = line.x2();
        right = line.x1();
    }

    if (line.y1() < line.y2()) {
        top = line.y1();
        bottom = line.y2();
    } else {
        top = line.y2();
        bottom = line.y1();
    }

    return QRectF(QPoint(left, top), QPoint(right, bottom));
}

/*!
    Returns true if \a rect and \a line intersect, otherwise
    returns false.

    This function operates on map(pixel) coordinates.
*/
bool QMapObject::intersect(const QRectF& rect, const QLineF& line)
{
    //check all for edges of the tile
    QPointF tmp;
    QLineF leftEdge(rect.topLeft(), rect.bottomLeft());
    QLineF topEdge(rect.topLeft(), rect.topRight());
    QLineF rightEdge(rect.topRight(), rect.bottomRight());
    QLineF bottomEdge(rect.bottomLeft(), rect.bottomRight());

    if (line.intersect(leftEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(topEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(rightEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(bottomEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    }

    //no intersection so far, maybe line lies completely within rect?
    QRectF box = boundingRect(line);

    if (rect.contains(box))
        return true;

    return false;
}

void QMapObject::setParentView(QMapView *mapView)
{
    Q_D(QMapObject);
    d->mapView = mapView;
};

QTM_END_NAMESPACE
