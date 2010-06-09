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
#include "qgeomapcontainer.h"
#include "qgeomapcontainer_p.h"

QTM_BEGIN_NAMESPACE


/*!
    \class QGeoMapContainer
    \brief The QGeoMapContainer class groups QGeoMapObjects.
    \ingroup location

    A QGeoMapContainer is used to group QGeoMapObjects, thus allowing to operate on a whole group of objects
    without the need to iteratively visit each QGeoMapObject, e.g. for changing the visibility
*/


/*!
    Constructs a map container.
*/
QGeoMapContainer::QGeoMapContainer(const QGeoMapContainer* parent)
    : QGeoMapObject(parent), d_ptr(new QGeoMapContainerPrivate)
{
}

QGeoMapContainer::~QGeoMapContainer()
{
    delete d_ptr;
}

/*!
    Sets the \a visibility of the QGeoMapContainer. Even if the conatiner's
    visibility is set to true, the container (and all its contained objects)
    will not be visible if any of its parents is not visible.
    
    \sa QGeoMapObject::isVisible()
*/
void QGeoMapContainer::setVisibility(bool visibility)
{
    QGeoMapObject::setVisibility(visibility);
    setVisible(visibility);
    //now update children's isVisible status
    QListIterator<QGeoMapObject*> it(d_ptr->children);

    while (it.hasNext())
        it.next()->setVisible(visibility);
}

/*!
    Adds the given \a mapObject to the container.
    The container takes ownership of \a mapObject.
*/
void QGeoMapContainer::addMapObject(QGeoMapObject *mapObject)
{
    d_ptr->children.append(mapObject);
    d_ptr->zChildren.insertMulti(mapObject->zIndex(), mapObject);
}

/*!
    Removes the given \a mapObject to the container.
    The container relinquishes ownership of \a mapObject.
*/
void QGeoMapContainer::removeMapObject(QGeoMapObject *mapObject)
{
    d_ptr->children.removeAll(mapObject);
    d_ptr->zChildren.remove(mapObject->zIndex(), mapObject);
}

/*!
    Returns all QGeoMapObjects which are managed by the container.

    It is not guaranteed that modifications on the result will properly be reflected in the
    MapContainer.
    Please use addMapObject and removeMapObject to modify the list.

    \sa addMapObject()
    \sa removeMapObject()
*/
QList<QGeoMapObject*> QGeoMapContainer::getAllMapObjects() const
{
    return d_ptr->children;
}

QTM_END_NAMESPACE
