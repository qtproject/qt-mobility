
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapGroup

    \brief The MapGroup element aggregates a set of map objects.
    \inherits QGeoMapGroupObject

    \ingroup qml-location-maps

    Grouping is provided to make it easier to quickly add, remove, show 
    and hide groups of objects.

    It also allows users to specify an ordering of objects local to the 
    group via the z-values and insertion order of the objects in the group.

    The MapGroup element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapObject::QDeclarativeGeoMapObject()
        : QGeoMapGroupObject() {}

QDeclarativeGeoMapObject::~QDeclarativeGeoMapObject() {}

/*!
    \qmlproperty list<QGeoMapObject> MapGroup::objects
    \default

    This property holds the list of objects which make up the group.
*/

QDeclarativeListProperty<QGeoMapObject> QDeclarativeGeoMapObject::objects()
{
    return QDeclarativeListProperty<QGeoMapObject>(this,
            0,
            child_append,
            child_count,
            child_at,
            child_clear);
}

void QDeclarativeGeoMapObject::child_append(QDeclarativeListProperty<QGeoMapObject> *prop, QGeoMapObject *mapObject)
{
    static_cast<QDeclarativeGeoMapObject*>(prop->object)->addChildObject(mapObject);
}

int QDeclarativeGeoMapObject::child_count(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    return static_cast<QDeclarativeGeoMapObject*>(prop->object)->childObjects().size();
}

QGeoMapObject *QDeclarativeGeoMapObject::child_at(QDeclarativeListProperty<QGeoMapObject> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapObject*>(prop->object)->childObjects().at(index);
}

void QDeclarativeGeoMapObject::child_clear(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    static_cast<QDeclarativeGeoMapObject*>(prop->object)->clearChildObjects();
}

/*!
    \qmlproperty int MapGroup::zValue

    This property holds the z-value of the group.

    Map objects are drawn in z-value order, and objects with the 
    same z-value will be drawn in insertion order.

    The objects inside the group are drawn according to the z-values 
    and insertion order of the other elements of the group.  This 
    is indpendent of the z-value and insertion order of the group 
    element itself.
*/

/*!
    \qmlproperty bool MapGroup::visible

    This property holds a boolean corresponding to whether or not the 
    group is visible.
*/

/*!
    \qmlproperty bool MapGroup::selected

    This property holds a boolean corresponding to whether or not the 
    group is selected.
*/

#include "moc_qdeclarativegeomapobject_p.cpp"

QTM_END_NAMESPACE

