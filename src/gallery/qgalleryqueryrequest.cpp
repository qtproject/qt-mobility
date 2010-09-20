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

#include "qgalleryqueryrequest.h"
#include "qgalleryabstractrequest_p.h"

#include "qgallerynullresultset_p.h"
#include "qgalleryresource.h"

QTM_BEGIN_NAMESPACE

class QGalleryQueryRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryQueryRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::QueryRequest)
        , offset(0)
        , limit(0)
        , scope(QGalleryQueryRequest::AllDescendants)
        , autoUpdate(false)
        , resultSet(0)
        , internalResultSet(0)
    {
        internalResultSet = &nullResultSet;
    }

    int offset;
    int limit;
    QGalleryQueryRequest::Scope scope;
    bool autoUpdate;
    QGalleryResultSet *resultSet;
    QGalleryResultSet *internalResultSet;
    QGalleryNullResultSet nullResultSet;
    QStringList propertyNames;
    QStringList sortPropertyNames;
    QString rootType;
    QVariant rootItem;
    QGalleryFilter filter;
};

/*!
    \class QGalleryQueryRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery

    \brief The QGalleryQueryRequest class provides a request for a set of
    items from a gallery.

*/

/*!
    \enum QGalleryQueryRequest::Scope

    Identifies the scope of query.

    \value AllDescendants The query will return all descendents of the scope
    item.
    \value DirectDescendants The query will return only direct descendents of
    the scope item.
*/

/*!
    Constructs a new gallery query request.

    The \a parent is passed to QObject.
*/


QGalleryQueryRequest::QGalleryQueryRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryQueryRequestPrivate(0), parent)
{
}
/*!
    Contructs a new query request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryQueryRequest::QGalleryQueryRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryQueryRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery query request.
*/

QGalleryQueryRequest::~QGalleryQueryRequest()
{
}
/*!
    \property QGalleryQueryRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/


QStringList QGalleryQueryRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryQueryRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryQueryRequest::sortPropertyNames

    \brief A list of names of meta-data properties a request should sort its
    results on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryQueryRequest::sortPropertyNames() const
{
    return d_func()->sortPropertyNames;
}

void QGalleryQueryRequest::setSortPropertyNames(const QStringList &names)
{
    d_func()->sortPropertyNames = names;
}

/*!
    \property QGalleryQueryRequest::autoUpdate

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryQueryRequest::autoUpdate() const
{
    return d_func()->autoUpdate;
}

void QGalleryQueryRequest::setAutoUpdate(bool enabled)
{
    d_func()->autoUpdate = enabled;
}

/*!
    \property QGalleryQueryRequest::offset

    \brief the offset of the first item a query should return.
*/

int QGalleryQueryRequest::offset() const
{
    return d_func()->offset;
}

void QGalleryQueryRequest::setOffset(int offset)
{
    d_func()->offset = qMax(0, offset);
}

/*!
    \property QGalleryQueryRequest::limit

    \brief the maximimum number of items a query should return.
*/

int QGalleryQueryRequest::limit() const
{
    return d_func()->limit;
}

void QGalleryQueryRequest::setLimit(int limit)
{
    d_func()->limit = qMax(0, limit);
}


/*!
    \property QGalleryQueryRequest::rootType

    \brief the root item type the results of a query should be restricted to.

*/

QString QGalleryQueryRequest::rootType() const
{
    return d_func()->rootType;
}

void QGalleryQueryRequest::setRootType(const QString &itemType)
{
    d_func()->rootType = itemType;
}

/*!
    \property QGalleryQueryRequest::rootItem

    \brief the ID of an item the query should return the descendents of.
*/

QVariant QGalleryQueryRequest::rootItem() const
{
    return d_func()->rootItem;
}

void QGalleryQueryRequest::setRootItem(const QVariant &itemId)
{
    d_func()->rootItem = itemId;
}


/*!
    \property QGalleryQueryRequest::scope

    \brief whether all descendants of the \l rootItem should be returned by
    a request or just the direct descendants.
*/

QGalleryQueryRequest::Scope QGalleryQueryRequest::scope() const
{
    return d_func()->scope;
}

void QGalleryQueryRequest::setScope(QGalleryQueryRequest::Scope scope)
{
    d_func()->scope = scope;
}

/*!
    \property QGalleryQueryRequest::filter

    \brief A filter identifying the items a request should return.

    If no filter is set the results of the request will be determined
    by the \l rootType and \l rootItem properties.
*/

QGalleryFilter QGalleryQueryRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryQueryRequest::setFilter(const QGalleryFilter &filter)
{
    d_func()->filter = filter;
}

/*!
    Returns the result set containing the results of a query.
*/

QGalleryResultSet *QGalleryQueryRequest::resultSet() const
{
    return d_func()->resultSet;
}

/*!
    \fn QGalleryQueryRequest::resultSetChanged(QGalleryResultSet *resultSet)

    Signals that the \a resultSet containing the results of a query have
    changed.
*/

/*!
    Returns the key of \a property.
*/

int QGalleryQueryRequest::propertyKey(const QString &property) const
{
    return d_func()->internalResultSet->propertyKey(property);
}

/*!
    Returns the attributes of the property identified by \a key.
*/

QGalleryProperty::Attributes QGalleryQueryRequest::propertyAttributes(int key) const
{
    return d_func()->internalResultSet->propertyAttributes(key);
}

/*!
    Returns the type of the property identified by \a key.
*/

QVariant::Type QGalleryQueryRequest::propertyType(int key) const
{
    return d_func()->internalResultSet->propertyType(key);
}

/*!
    Returns the number of items returned by a query.
*/

int QGalleryQueryRequest::itemCount() const
{
    return d_func()->internalResultSet->itemCount();
}

/*!
    \property QGalleryQueryRequest::itemId

    \brief The ID of the current item.
*/

QVariant QGalleryQueryRequest::itemId() const
{
    return d_func()->internalResultSet->itemId();
}

/*!
    \property QGalleryQueryRequest::itemUrl

    \brief The URL of the current item.
*/

QUrl QGalleryQueryRequest::itemUrl() const
{
    return d_func()->internalResultSet->itemUrl();
}

/*!
    \property QGalleryQueryRequest::itemType

    \brief he type of the current item.
*/

QString QGalleryQueryRequest::itemType() const
{
    return d_func()->internalResultSet->itemType();
}

/*!
    \property QGalleryQueryRequest::resources

    \brief The resources of the current item.
*/

QList<QGalleryResource> QGalleryQueryRequest::resources() const
{
    return d_func()->internalResultSet->resources();
}

/*!
    Returns the value of a meta-data property identified by \a key for the
    current item.
*/


QVariant QGalleryQueryRequest::metaData(int key) const
{
    return d_func()->internalResultSet->metaData(key);
}

/*!
    Sets the \a value of a meta-data property identified by \a key for the
    current item.

    Returns true if the value was changed; otherwise returns false.
*/

bool QGalleryQueryRequest::setMetaData(int key, const QVariant &value)
{
    return d_func()->internalResultSet->setMetaData(key, value);
}

/*!
    Returns the value of a meta-data \a property for the current item.
*/


QVariant QGalleryQueryRequest::metaData(const QString &property) const
{
    return d_func()->internalResultSet->metaData(
            d_func()->internalResultSet->propertyKey(property));
}

/*!
    Sets the \value of a meta-data \a property for the current item.

    Returns true if the value was changed; otherwise returns false.
*/

bool QGalleryQueryRequest::setMetaData(const QString &property, const QVariant &value)
{
    return d_func()->internalResultSet->setMetaData(
            d_func()->internalResultSet->propertyKey(property), value);
}

/*!
    \property QGalleryQueryRequest::currentIndex

    \brief The index of current item.
*/

int QGalleryQueryRequest::currentIndex() const
{
    return d_func()->internalResultSet->currentIndex();
}

/*!
    \fn QGalleryQueryRequest::currentItemChanged()

    Signals that the item the result set is positioned on has changed.
*/

/*!
    Seeks to the item at \a index.

    If \a relative is true the seek is peformed relative to the current index.

    Returns true if the position of the result set is valid after the seek; and
    false otherwise.
*/

bool QGalleryQueryRequest::seek(int index, bool relative)
{
    return d_func()->internalResultSet->fetch(relative
            ? d_func()->internalResultSet->currentIndex() + index
            : index);
}

/*!
    Seeks to the next item in the result set.

    Returns true if the position of the result set is valid after the seek; and
    false otherwise.
*/

bool QGalleryQueryRequest::next()
{
    return d_func()->internalResultSet->fetchNext();
}

/*!
    Seeks to the previous item in the result set.

    Returns true if the position of the result set is valid after the seek; and
    false otherwise.
*/

bool QGalleryQueryRequest::previous()
{
    return d_func()->internalResultSet->fetchPrevious();
}

/*!
    Seeks to the first item in the result set.

    Returns true if the position of the result set is valid after the seek; and
    false otherwise.
*/

bool QGalleryQueryRequest::first()
{
    return d_func()->internalResultSet->fetchFirst();
}

/*!
    Seeks to the last item in the result set.

    Returns true if the position of the result set is valid after the seek; and
    false otherwise.
*/

bool QGalleryQueryRequest::last()
{
    return d_func()->internalResultSet->fetchLast();
}

/*!
    \property QGalleryQueryRequest::valid

    \brief Whether the result set is currently positioned on a valid item.
*/

bool QGalleryQueryRequest::isValid() const
{
    return d_func()->internalResultSet->isValid();
}

/*!
    \reimp
*/

void QGalleryQueryRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryQueryRequest);

    d->resultSet = qobject_cast<QGalleryResultSet *>(response);

    if (d->resultSet) {
        d->internalResultSet = d->resultSet;

        connect(d->resultSet, SIGNAL(currentItemChanged()), this, SIGNAL(currentItemChanged()));
    } else {
        d->internalResultSet = &d->nullResultSet;
    }

    emit resultSetChanged(d->resultSet);
}

#include "moc_qgalleryqueryrequest.cpp"

QTM_END_NAMESPACE
