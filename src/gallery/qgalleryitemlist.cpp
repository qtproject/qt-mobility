/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgalleryitemlist_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGalleryResource

    \ingroup gallery

    \brief The QGalleryResource class provides a resource identifier for a
    gallery gallery item.
*/

/*!
    Constructs a null resource.
*/

QGalleryResource::QGalleryResource()
{
}

/*!
    Constructs a resource from a \a url.
*/

QGalleryResource::QGalleryResource(const QUrl &url)
    : m_url(url)
{
}

/*!
    Constructs a resource from a \a url and a set of \a attributes.
*/

QGalleryResource::QGalleryResource(const QUrl &url, QMap<int, QVariant> attributes)
    : m_url(url)
    , m_attributes(attributes)
{
}

/*!
    Constructs a copy of a \a resource.
*/

QGalleryResource::QGalleryResource(const QGalleryResource &resource)
    : m_url(resource.m_url)
    , m_attributes(resource.m_attributes)
{
}

/*!
    Destroys a resource.
*/

QGalleryResource::~QGalleryResource()
{
}

/*!
    Assigns the value of \a resource to another resource.
*/

QGalleryResource &QGalleryResource::operator =(const QGalleryResource &resource)
{
    m_url = resource.m_url;
    m_attributes = resource.m_attributes;

    return *this;
}

/*!
    Compares \a resource to another resource.

    Returns true if both resources are the same, and false otherwise.
*/

bool QGalleryResource::operator ==(const QGalleryResource &resource) const
{
    return m_url == resource.m_url && m_attributes == resource.m_attributes;
}

/*!
    Compares \a resource to another resource.

    Returns true if the resource are not the same, and false otherwise.
*/

bool QGalleryResource::operator !=(const QGalleryResource &resource) const
{
    return m_url != resource.m_url || m_attributes != resource.m_attributes;
}

/*!
    Returns the URL of a resource.
*/

QUrl QGalleryResource::url() const
{
    return m_url;
}

/*!
    Returns the attributes of a resource.
*/

QMap<int, QVariant> QGalleryResource::attributes() const
{
    return m_attributes;
}

/*!
    Returns the value of the resource attribute identified by \a key.
*/

QVariant QGalleryResource::attribute(int key) const
{
    return m_attributes.value(key);
}

/*!
    \class QGalleryItemList

    \ingroup gallery

    \brief The QGalleryItemList class provides an abstract interface for a
    list of gallery items from a gallery.
*/

/*!
    \enum QGalleryItemList::ItemStatusFlag

    This enumeration describes the current status of a gallery item.

    \value OutOfRange The item is outside the range of currently paged items.
    \value Reading New item data is currently being fetched.
    \value Writing Changes to item data are currently being written.
*/

/*!
    Constructs a new gallery gallery item list.

    The \a parent is passed to QObject.
*/

QGalleryItemList::QGalleryItemList(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryItemListPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/

QGalleryItemList::QGalleryItemList(QGalleryItemListPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a gallery gallery item list.
*/

QGalleryItemList::~QGalleryItemList()
{
    delete d_ptr;
}

/*!
    \fn QGalleryItemList::propertyNames() const

    Returns a list of property names an item list has meta-data values for.
*/

/*!
    \fn QGalleryItemList::propertyKey(const QString &propertyName) const

    Returns a integer key for a meta-data \a propertyName.
*/

/*!
    \fn QGalleryItemList::propertyAttributes(int key) const

    Returns the attributes of the property identified by \a key.
*/

/*!
    \property QGalleryItemList::cursorPosition

    \brief The current position of the cache cursor.
*/

int QGalleryItemList::cursorPosition() const
{
    return d_ptr->cursorPosition;
}

void QGalleryItemList::setCursorPosition(int position)
{
    d_ptr->cursorPosition = qMax(0, position);
}

/*!
    \property QGalleryItemList::minimumPagedItems

    \brief The minimum number of consecutive items loaded in a list.
*/

int QGalleryItemList::minimumPagedItems() const
{
    return d_ptr->cacheSize;
}

/*!
    \fn QGalleryItemList::count() const

    Returns the number of gallery items in a gallery item list.
*/

/*!
    \fn QGalleryItemList::id(int index) const

    Returns the id of the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::url(int index) const

    Returns the URL of the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::type(int index) const

    Returns the type of the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::resources(int index) const

    Returns a list of resources for the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::status(int index) const

    Returns the status of the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::metaData(int index, int key) const

    Returns the meta-data \a key value of the gallery item at \a index.
*/

/*!
    \fn QGalleryItemList::setMetaData(int index, int key, const QVariant &value)

    Sets the meta-data \a key \a value of the gallery item at \a index.
*/

/*!
    Returns the value of a meta-data \a property for the item at \a index in a list.
*/
QVariant QGalleryItemList::metaData(int index, const QString &property) const
{
    return metaData(index, propertyKey(property));
}

/*!
    Sets the \a value of a meta-data \a property for the item at \a index in a list.
*/
void QGalleryItemList::setMetaData(int index, const QString &property, const QVariant &value)
{
    setMetaData(index, propertyKey(property), value);
}

/*!
    \fn QGalleryItemList::inserted(int index, int count)

    Signals that \a count gallery items have been inserted into a gallery item list
    starting at \a index.
*/

/*!
    \fn QGalleryItemList::removed(int index, int count)

    Signals that \a count gallery items have been removed from a gallery item list
    starting at \a index.
*/

/*!
    \fn QGalleryItemList::moved(int from, int to, int count)

    Signals that \a count gallery items have been moved \a from one starting index
    \a to another.
*/

/*!
    \fn QGalleryItemList::metaDataChanged(int index, int count, const QList<int> &keys)

    Signals that the meta-data identified by \a keys of \a count gallery items
    starting at \a index has changed.
*/

#include "moc_qgalleryitemlist.cpp"

QTM_END_NAMESPACE
