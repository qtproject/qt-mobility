/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgalleryitemrequest.h"
#include "qgalleryabstractrequest_p.h"

#include "qgallerynullresultset_p.h"
#include "qgalleryresource.h"

QTM_BEGIN_NAMESPACE

class QGalleryItemRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemRequest)
public:
    QGalleryItemRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::ItemRequest)
        , autoUpdate(false)
        , resultSet(0)
        , internalResultSet(0)
    {
        internalResultSet = &nullResultSet;
    }

    void _q_itemsInserted(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsRemoved(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsMoved(int from, int to, int)
    {
        if (from == 0 || to == 0)
            resultSet->fetch(0);
    }

    void _q_currentItemChanged()
    {
        emit q_func()->itemChanged();

        if (!propertyKeys.isEmpty())
            emit q_func()->metaDataChanged(propertyKeys);
    }

    void _q_metaDataChanged(int index, int, const QList<int> &keys)
    {
        if (index == 0)
            emit q_func()->metaDataChanged(keys);
    }

    bool autoUpdate;
    QGalleryResultSet *resultSet;
    QGalleryResultSet *internalResultSet;
    QGalleryNullResultSet nullResultSet;
    QStringList propertyNames;
    QVariant itemId;
    QList<int> propertyKeys;
};

/*!
    \class QGalleryItemRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery
    \since 1.1

    \brief The QGalleryItemRequest class provides a request for the properties
    of a single item from a gallery.

    QGalleryItemRequest executes a query which returns information about the
    gallery item specified in \l itemId.  The query will return an \l itemUrl,
    an \l itemType, \l resources and \l {metaData()}{meta-data} values for the
    properties listed in \l propertyNames.

    When the request has finished and if the item could be found the \l valid
    property will be true, if not it will be false.

    If the \l autoUpdate property is true when the request is executed it will
    enter an \l Idle state on finishing and will refresh the queried
    information if the item changes.  If the gallery can't provide updates
    it will instead go immediately to the \l Finished state.  Automatic updates
    can be canceled by calling cancel() on a idle request.

    \sa QDocumentGallery
*/

/*!
    Constructs a new gallery item request.

    The \a parent is passed to QObject.
*/


QGalleryItemRequest::QGalleryItemRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryItemRequestPrivate(0), parent)
{
}
/*!
    Contructs a new item request for the given \a gallery.

    The \a parent is passed to QObject.
    \since 1.1
*/

QGalleryItemRequest::QGalleryItemRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryItemRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery item request.
*/

QGalleryItemRequest::~QGalleryItemRequest()
{
}

/*!
    \property QGalleryItemRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
    \since 1.1
*/

QStringList QGalleryItemRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryItemRequest::setPropertyNames(const QStringList &names)
{
    if (d_func()->propertyNames != names) {
        d_func()->propertyNames = names;

        emit propertyNamesChanged();
    }
}

/*!
    \fn QGalleryItemRequest::propertyNamesChanged()

    Signals that the value of \l propertyNames has changed.
    \since 1.1
*/

/*!
    \property QGalleryItemRequest::autoUpdate

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
    \since 1.1
*/

bool QGalleryItemRequest::autoUpdate() const
{
    return d_func()->autoUpdate;
}

void QGalleryItemRequest::setAutoUpdate(bool enabled)
{
    if (d_func()->autoUpdate != enabled) {
        d_func()->autoUpdate = enabled;

        emit autoUpdateChanged();
    }
}

/*!
    \fn QGalleryItemRequest::autoUpdateChanged()

    Signals that the value of \l autoUpdate has changed.
    \since 1.1
*/

/*!
    \property QGalleryItemRequest::itemId

    \brief the ID of an item the request should return the properties of.
    \since 1.1
*/

QVariant QGalleryItemRequest::itemId() const
{
    return d_func()->itemId;
}

void QGalleryItemRequest::setItemId(const QVariant &itemId)
{
    if (d_func()->itemId != itemId) {
        d_func()->itemId = itemId;

        emit itemIdChanged();
    }
}

/*!
    \fn QGalleryItemRequest::itemIdChanged()

    Signals that the value of the \l itemId property has changed.
    \since 1.1
*/

/*!
    Returns the result set containing the meta-data of a type.
    \since 1.1
*/

QGalleryResultSet *QGalleryItemRequest::resultSet() const
{
    return d_func()->resultSet;
}

/*!
    \fn QGalleryItemRequest::resultSetChanged(QGalleryResultSet *resultSet)

    Signals that the \a resultSet containing the meta-data of an item has
    changed.
    \since 1.1
*/

/*!
    \fn QGalleryItemRequest::itemChanged()

    Signals that the properties of an item have changed.
    \since 1.1
*/

/*!
    Returns the key of \a property.
    \since 1.1
*/

int QGalleryItemRequest::propertyKey(const QString &property) const
{
    return d_func()->internalResultSet->propertyKey(property);
}

/*!
    Returns the attributes of the property identified by \a key.
    \since 1.1
*/

QGalleryProperty::Attributes QGalleryItemRequest::propertyAttributes(int key) const
{
    return d_func()->internalResultSet->propertyAttributes(key);
}

/*!
    Returns the type of the property identified by \a key.
    \since 1.1
*/

QVariant::Type QGalleryItemRequest::propertyType(int key) const
{
    return d_func()->internalResultSet->propertyType(key);
}

/*!
    \property QGalleryItemRequest::valid

    \brief Whether the request currently holds valid type information.
    \since 1.1
*/

bool QGalleryItemRequest::isValid() const
{
    return d_func()->internalResultSet->isValid();
}

/*!
    \property QGalleryItemRequest::itemUrl

    \brief The URL of an item.
    \since 1.1
*/

QUrl QGalleryItemRequest::itemUrl() const
{
    return d_func()->internalResultSet->itemUrl();
}

/*!
    \property QGalleryItemRequest::itemType

    \brief the type of an item.
    \since 1.1
*/

QString QGalleryItemRequest::itemType() const
{
    return d_func()->internalResultSet->itemType();
}

/*!
    \property QGalleryItemRequest::resources

    \brief the resources of an item.
    \since 1.1
*/

QList<QGalleryResource> QGalleryItemRequest::resources() const
{
    return d_func()->internalResultSet->resources();
}

/*!
    Returns the value of a meta-data property identified by \a key.
    \since 1.1
*/

QVariant QGalleryItemRequest::metaData(int key) const
{
    return d_func()->internalResultSet->metaData(key);
}

/*!
    Sets the \a value of a meta-data property identified by \a key.

    Returns true if the value was changed; otherwise returns false.
    \since 1.1
*/

bool QGalleryItemRequest::setMetaData(int key, const QVariant &value)
{
    return d_func()->internalResultSet->setMetaData(key, value);
}

/*!
    Returns the value of a meta-data \a property.
    \since 1.1
*/

QVariant QGalleryItemRequest::metaData(const QString &property) const
{
    return d_func()->internalResultSet->metaData(
            d_func()->internalResultSet->propertyKey(property));
}

/*!
    Sets the \value of a meta-data \a property.

    Returns true if the value was changed; otherwise returns false.
    \since 1.1
*/

bool QGalleryItemRequest::setMetaData(const QString &property, const QVariant &value)
{
    return d_func()->internalResultSet->setMetaData(
            d_func()->internalResultSet->propertyKey(property), value);
}

/*!
    \fn QGalleryItemRequest::metaDataChanged(const QList<int> &keys)

    Signals that the values of meta-data properties identified by \a keys
    have changed.
    \since 1.1
*/

/*!
    \reimp
    \since 1.1
*/

void QGalleryItemRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryItemRequest);

    const bool wasValid = d->internalResultSet->isValid();

    d->resultSet = qobject_cast<QGalleryResultSet *>(response);
    d->propertyKeys.clear();

    if (d->resultSet) {
        d->internalResultSet = d->resultSet;

        connect(d->resultSet, SIGNAL(itemsInserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
        connect(d->resultSet, SIGNAL(itemsRemoved(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
        connect(d->resultSet, SIGNAL(itemsMoved(int,int,int)),
                this, SLOT(_q_itemsMoved(int,int,int)));
        connect(d->resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));
        connect(d->resultSet, SIGNAL(currentItemChanged()), this, SLOT(_q_currentItemChanged()));

        typedef QStringList::const_iterator iterator;
        for (iterator it = d->propertyNames.constBegin(), end = d->propertyNames.constEnd();
                it != end;
                ++it) {
            const int propertyKey = d->resultSet->propertyKey(*it);

            if (propertyKey != -1)
                d->propertyKeys.append(propertyKey);
        }
    } else {
        d->internalResultSet = &d->nullResultSet;
    }

    emit resultSetChanged(d->resultSet);

    if (d->internalResultSet->itemCount() > 0)
        d->internalResultSet->fetch(0);
    else if (wasValid)
        emit itemChanged();
}

#include "moc_qgalleryitemrequest.cpp"

QTM_END_NAMESPACE
