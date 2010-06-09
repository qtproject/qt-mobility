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
#include "qgeomapcontainer.h"

QTM_BEGIN_NAMESPACE


/*!
    \class QGeoMapObject
    \brief The QGeoMapObject class A MapObject is the base class for all visual objects shown on a map.
    \ingroup location

    QGeoMapObjects are responsible to identify UI relevant state changes and automatically trigger a redraw.
*/

/*!
    Constructs a map container.
*/
QGeoMapObject::QGeoMapObject(const QGeoMapContainer* parent)
    : d_ptr(new QGeoMapObjectPrivate)
{
    d_ptr->parent = parent;
    d_ptr->visibility = true;

    if (parent)
        d_ptr->isVisible = parent->isVisible();
    else
        d_ptr->isVisible = true;
}

QGeoMapObject::~QGeoMapObject()
{
    delete d_ptr;
}

/*!
    Sets the \a visibility of the QGeoMapObject. Even if the object's visibility is set to true,
    the object will not be visible if any of its parents is not visible.
    
    \sa isVisible()
*/
void QGeoMapObject::setVisibility(bool visibility)
{
    d_ptr->visibility = visibility;
}

/*!
    Returns the \a QGeoBoundingBox covering the entire shape of the QGeoMapObject
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    return d_ptr->boundingBox;
}

/*!
    Returns the z-Index relative to the container. The z-Index is used to specify
    the stacking order in a QGeoMapContainer. Objects with the highest
    values are placed on top.
    The overall stack order in a map is defined hierarchically:
    Level in the QGeoMapContainer hierarchy > z-Index (in one
    QGeoMapContainer) > order in the container's list of QGeoMapObjects.
*/
int QGeoMapObject::zIndex() const
{
    return d_ptr->zIndex;
}

/*!
    Returns the visibility of the QGeoMapObject. Even if the object's visibility is set to true,
    the object will not be visible if any of its parents is not visible.
    
    \sa isVisible()
*/
bool QGeoMapObject::visibility() const
{
    return d_ptr->visibility;
}

/*!
    Returns true if and only if the object's visibility as well
    as all parents' visibilities are true and if the object is attached to a map.
*/
bool QGeoMapObject::isVisible() const
{
    //TODO: AND with map attached
    return d_ptr->isVisible;
}

/*!
    Returns the direct parent of this QGeoMapObject according to the
    QGeoMapContainer hierarchy.
*/
const QGeoMapContainer* QGeoMapObject::parent() const
{
    return d_ptr->parent;
}

/*!
    Sets the z-Index of this map object to \a z.

    \sa zIndex()
*/
void QGeoMapObject::setZIndex(int z)
{
    d_ptr->zIndex = z;
    //TODO: update parent's z-ordered list
}

/*!
    Sets the isVisible status of this object. This method is usually
    called by the parent when its status has changed.
*/
void QGeoMapObject::setVisible(bool visible)
{
    d_ptr->isVisible = d_ptr->visibility & visible;
}

QTM_END_NAMESPACE
