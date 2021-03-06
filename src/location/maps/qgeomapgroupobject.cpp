/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomapgroupobject.h"
#include "qgeomapgroupobject_p.h"

#include "qgeomapobject_p.h"
#include "qgeomapobjectengine_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

#include "qgeomapdata.h"
#include "qgeomapdata_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapGroupObject
    \brief The QGeoMapGroupObject class is a QGeoMapObject used to
    manager a group of other map objects.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping-objects

    The QGeoMapGroupObject class can be used to quickly add and remove
    groups of objects to a map.

    The map objects contained in the group will be ordered relative to
    one another in the usual manner, such that objects with higher z-values
    will be drawn over objects with lower z-values and objects with
    equal z-values will be drawn in insertion order.

    This ordering of the objects will be independent of the other
    objects that are added to the map, since the z-value and insertion
    order of the QGeoMapGroupObject is used to determine where the
    group is placed in the scene.
*/

/*!
    Constructs a new group object.
*/
QGeoMapGroupObject::QGeoMapGroupObject()
    : d_ptr(new QGeoMapGroupObjectPrivate(this)) {}

/*!
    Destroys this group object.
*/
QGeoMapGroupObject::~QGeoMapGroupObject()
{
    delete d_ptr;
}

/*!
    \reimp
    \since 1.1
*/
QGeoMapObject::Type QGeoMapGroupObject::type() const
{
    return QGeoMapObject::GroupType;
}

/*!
    Returns a bounding box which contains this map object.

    If this map object has children, the bounding box will be large
    enough to contain both this map object and all of its children.
    \since 1.1
*/
QGeoBoundingBox QGeoMapGroupObject::boundingBox() const
{
    QGeoBoundingBox bounds;

    if (d_ptr->children.size() == 0)
        return bounds;

    bounds = d_ptr->children.at(0)->boundingBox();

    for (int i = 1; i < d_ptr->children.size(); ++i)
        bounds = bounds.united(d_ptr->children.at(i)->boundingBox());

    return bounds;
}

/*!
    Returns whether \a coordinate is contained with the boundary of this
    map object.

    If this map object has children, this function will return whether
    \a coordinate is contained within the boundary of this map object or
    within the boundary of any of its children.
    \since 1.1
*/
bool QGeoMapGroupObject::contains(const QGeoCoordinate &coordinate) const
{
    for (int i = 0; i < d_ptr->children.size(); ++i)
        if (d_ptr->children.at(i)->contains(coordinate))
            return true;

    return false;
}

bool mapObjectLessThan(const QGeoMapObject* op1, const QGeoMapObject* op2)
{
    return op1->operator <(*op2);
}

/*!
    Adds \a childObject to the list of children of this map object.

    The children objects are drawn in order of the QGeoMapObject::zValue()
    value.  Children objects having the same z value will be drawn
    in the order they were added.

    The map object will take ownership of \a childObject.
    \since 1.1
*/
void QGeoMapGroupObject::addChildObject(QGeoMapObject *childObject)
{
    if (!childObject || d_ptr->children.contains(childObject))
        return;

    childObject->setMapData(mapData());
    childObject->d_func()->serial = d_func()->serial++;

    //binary search
    QList<QGeoMapObject*>::iterator i = qUpperBound(d_ptr->children.begin(),
                                                    d_ptr->children.end(),
                                                    childObject,
                                                    mapObjectLessThan);
    d_ptr->children.insert(i, childObject);

    connect(childObject, SIGNAL(zValueChanged(int)),
            d_ptr, SLOT(childChangedZValue(int)));

    emit childAdded(childObject);
}

/*!
    Removes \a childObject from the list of children of this map object.

    The map object will release ownership of \a childObject.
    \since 1.1
*/
void QGeoMapGroupObject::removeChildObject(QGeoMapObject *childObject)
{
    if (!childObject)
        return;

    if (d_ptr->children.removeAll(childObject) > 0) {

        disconnect(childObject, SIGNAL(zValueChanged(int)),
                   d_ptr, SLOT(childChangedZValue(int)));

        emit childRemoved(childObject);
        childObject->setMapData(0);

//        if (this->mapData()) {
//            QGeoMapObjectEngine *oe = this->mapData()->d_ptr->oe;
//            if (oe)
//                oe->removeObject(childObject);
//        }
    }

//    update();
}

/*!
    Returns the children of this object.
    \since 1.1
*/
QList<QGeoMapObject*> QGeoMapGroupObject::childObjects() const
{
    return d_ptr->children;
}

/*!
    Clears the children of this object.

    The child objects will be deleted.
    \since 1.1
*/
void QGeoMapGroupObject::clearChildObjects()
{
    for (int i = d_ptr->children.size() - 1; i >=0; --i) {
        QGeoMapObject* child = d_ptr->children[i];
        removeChildObject(child);
        delete child;
    }

    d_ptr->children.clear();
}

/*!
    Sets whether this group of objects is visible to \a visible.
    \since 1.2
*/
void QGeoMapGroupObject::setVisible(bool visible)
{
    for (int i = 0; i < d_ptr->children.size(); ++i)
        d_ptr->children[i]->setVisible(visible);

    QGeoMapObject::setVisible(visible);
}

/*!
    \reimp
    \since 1.1
*/
void QGeoMapGroupObject::setMapData(QGeoMapData *mapData)
{

    for (int i = 0; i < d_ptr->children.size(); ++i) {
        d_ptr->children[i]->setMapData(mapData);
        if (mapData)
            emit childAdded(d_ptr->children[i]);
    }

    QGeoMapObject::setMapData(mapData);
}

/*!
\fn void QGeoMapGroupObject::childAdded(QGeoMapObject *childObject)

    This signal will be emitted when the map object \a childObject
    is added to the group.
    \since 1.1
*/

/*!
\fn void QGeoMapGroupObject::childUpdated(QGeoMapObject *childObject)

    This signal will be emitted if the map object \a childObject has
    changed such that the corresponding QGeoMapObjectInfo class that handles
    the map-specific behaviours of the object needs to be informed of
    the change.

    The map object \a childObject must belong to this group.

    At the moment this is only emitted when the z-value of the
    child-objects are changed.
    \since 1.2
*/

/*!
\fn void QGeoMapGroupObject::childRemoved(QGeoMapObject *childObject)

    This signal will be emitted when the map object \a childObject
    is removed from the group.
    \since 1.1
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapGroupObjectPrivate::QGeoMapGroupObjectPrivate(QGeoMapGroupObject *p) :
    QObject(p),
    serial(1),
    q_ptr(p)
{}

QGeoMapGroupObjectPrivate::~QGeoMapGroupObjectPrivate()
{
    qDeleteAll(children);
}

void QGeoMapGroupObjectPrivate::childChangedZValue(int zValue)
{
    Q_UNUSED(zValue);
    QGeoMapObject *child = qobject_cast<QGeoMapObject*>(sender());
    if (!child)
        return;

    if (children.removeAll(child) > 0) {
        QList<QGeoMapObject*>::iterator i = qUpperBound(children.begin(),
                                                        children.end(),
                                                        child,
                                                        mapObjectLessThan);
        children.insert(i, child);
        emit q_ptr->childUpdated(child);
    }
}

#include "moc_qgeomapgroupobject.cpp"
#include "moc_qgeomapgroupobject_p.cpp"

QTM_END_NAMESPACE

