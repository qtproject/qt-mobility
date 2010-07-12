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

#include <QtAlgorithms>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapObject
    \brief The QGeoMapObject class is graphical item for display in
    QGeoMapWidget instancse, that is specified in terms of coordinates and
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

    \value ContainerType
        A basic QGeoMapObject.
    \value RectangleType
        A QGeoMapObject used to display a rectangular region.
    \value CircleType
        A QGeoMapObject used to display a circular region.
    \value PolylineType
        A QGeoMapObject used to display a multi-segment line.
    \value PolygonType
        A QGeoMapObject used to display a polygonal region.
    \value MarkerType
        A QGeoMapObject used to display a map marker.
*/

/*!
    Constructs a new map object with the specified \a parent.
*/
QGeoMapObject::QGeoMapObject(QGeoMapObject *parent)
    : d_ptr(new QGeoMapObjectPrivate(this, parent)) {}

/*!
  \internal
*/
QGeoMapObject::QGeoMapObject(QGeoMapObjectPrivate *dd)
    : d_ptr(dd) {}

/*!
    Destroys this map object.
*/
QGeoMapObject::~QGeoMapObject()
{
    Q_D(QGeoMapObject);
    delete d;
}

/*!
    Returns the type of this map object.
*/
QGeoMapObject::Type QGeoMapObject::type() const
{
    Q_D(const QGeoMapObject);
    return d->type;
}

/*!
    Sets the z-value of this map object to \a zValue.

    The z-value determines the order in which the objects are drawn on the
    screen.  Objects with the same value will be drawn in the order that
    they were added to the map or map object.

    This is the same behaviour as QGraphicsItem.
*/
void QGeoMapObject::setZValue(int zValue)
{
    Q_D(QGeoMapObject);
    int oldZValue = d->zValue;
    d->zValue = zValue;
    emit zValueChanged(zValue, oldZValue);
}

/*!
    Returns the z-value of this map object.

    The z-value determines the order in which the objects are drawn on the
    screen.  Objects with the same value will be drawn in the order that
    they were added to the map or map object.

    This is the same behaviour as QGraphicsItem.
*/
int QGeoMapObject::zValue() const
{
    Q_D(const QGeoMapObject);
    return d->zValue;
}

/*!
    Sets whether this map object is \a visible.

    If this map object is not visible then none of the childObjects() will
    be displayed either.
*/
void QGeoMapObject::setVisible(bool visible)
{
    Q_D(QGeoMapObject);
    d->isVisible = visible;
}

/*!
    Returns whether this map object is visible.

    If this map object is not visible then none of the childObjects() will
    be displayed either.
*/
bool QGeoMapObject::isVisible() const
{
    Q_D(const QGeoMapObject);
    return d->isVisible;
}

/*!
    Returns a bounding box which contains this map object.

    If this map object has children, the bounding box will be large
    enough to contain both this map object and all of its children.
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    Q_D(const QGeoMapObject);

    QGeoBoundingBox bounds = d_ptr->boundingBox;

    if (d->children.size() == 0)
        return bounds;

    bounds = d->children.at(0)->boundingBox();

    for (int i = 1; i < d->children.size(); ++i)
        bounds = bounds.united(d->children.at(i)->boundingBox());

    return bounds;
}

/*!
    Returns whether \a coordinate is contained with the boundary of this
    map object.

    If this map object has children, this function will return whether
    \a coordinate is contained within the boundary of this map object or
    within the boundary of any of its children.
*/
bool QGeoMapObject::contains(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoMapObject);

    if (d->children.size() == 0)
        return d_ptr->boundingBox.contains(coordinate);

    for (int i = 0; i < d->children.size(); ++i)
        if (d->children.at(i)->contains(coordinate))
            return true;

    return false;
}

/*!
    Returns the parent of this map object.

    Returns 0 if this map object has no parent.
*/
QGeoMapObject* QGeoMapObject::parentObject() const
{
    Q_D(const QGeoMapObject);
    return d->parent;
}

/*!
    Adds \a childObject to the list of children of this map object.

    If \a childObject is 0 it will not be added.

    The map object will take ownership of \a childObject.
*/
void QGeoMapObject::addChildObject(QGeoMapObject *childObject)
{
    if (!childObject)
        return;

    childObject->d_ptr->parent = this;
    //binary search
    QList<QGeoMapObject*>::iterator i = qUpperBound(d_ptr->children.begin(), d_ptr->children.end(), childObject);
    d_ptr->children.insert(i, childObject);

    connect(childObject, SIGNAL(destroyed(QObject*)),
            this, SLOT(childObjectDestroyed(QObject*)));
    emit childObjectAdded(childObject);
}

void QGeoMapObject::childObjectDestroyed(QObject *obj)
{
    Q_D(QGeoMapObject);
    d->children.removeAll(static_cast<QGeoMapObject*>(obj));
}

/*!
    Removes \a childObject from the list of children of this map object.

    This method does nothing if \a childObject is not contained in this
    map objects list of children.

    The map object will release ownership of \a childObject.
*/
void QGeoMapObject::removeChildObject(QGeoMapObject *childObject)
{
    Q_D(QGeoMapObject);
    if (childObject) {
        if (d->children.removeAll(childObject) > 0) {
            childObject->d_ptr->parent = 0;
            emit childObjectRemoved(childObject);
        }
    }
}

/*!
    Returns the children of this map object.
    The children are ordered ascendingly on their zValues.
*/
QList<QGeoMapObject*> QGeoMapObject::childObjects() const
{
    Q_D(const QGeoMapObject);
    return d->children;
}

bool QGeoMapObject::operator<(const QGeoMapObject &other) const
{
    return d_ptr->zValue < other.d_ptr->zValue;
}

bool QGeoMapObject::operator>(const QGeoMapObject &other) const
{
    return d_ptr->zValue > other.d_ptr->zValue;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapObjectPrivate::QGeoMapObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
    : type(QGeoMapObject::ContainerType),
    parent(parent),
    q_ptr(impl) {}

QGeoMapObjectPrivate::QGeoMapObjectPrivate(const QGeoMapObjectPrivate &other)
    : type(other.type),
    parent(other.parent),
    children(other.children),
    zValue(other.zValue),
    isVisible(other.isVisible),
    boundingBox(other.boundingBox),
    q_ptr(other.q_ptr) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate() {
    if (parent) {
        Q_Q(QGeoMapObject);
        parent->removeChildObject(q);
    }

    for (int i = 0; i < children.size(); ++i) {
        children.at(i)->d_ptr->parent = 0;
        delete children.at(i);
    }

    children.clear();
}

QGeoMapObjectPrivate& QGeoMapObjectPrivate::operator= (const QGeoMapObjectPrivate &other)
{
    type = other.type;
    parent = other.parent;
    children = other.children;
    zValue = other.zValue;
    isVisible = other.isVisible;
    boundingBox = other.boundingBox;
    q_ptr = other.q_ptr;

    return *this;
}

#include "moc_qgeomapobject.cpp"

QTM_END_NAMESPACE
