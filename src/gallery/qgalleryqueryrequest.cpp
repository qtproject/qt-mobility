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

    QGalleryItemRequest executes a query which returns information about a set
    gallery item of gallery items matching some query criteria. For each item
    matching the query criteria the request will return an \l itemUrl, an
    \l itemType,  \l resources and \l {metaData()}{meta-data} values for the
    properties listed in \l propertyNames.

    The \l rootType property identifies the type of gallery item the request
    should return, if the root type has derivative types (i.e. an audio file is
    just a special case of a regular file) these will also be included in the
    result set.

    The \l rootItem property takes the ID of an item the query should only
    return the children of.  Depending on the \l scope of the query this may
    be {AllDescendents}{all descendents} or just the {DirectDescendents}
    {direct descendents} of the root item.

    The results of a query can be further limited by setting a \l filter on the
    request.  The request will evaluate the QGalleryFilter and only include
    items with meta-data matching the expression.

    The order the results are returned in can be specified in the
    sortPropertyNames property which takes an ordered list of property names.
    By default properties are sorted in ascending order, but this can be
    specified explicitly be prefixing the property name with a '+' character
    for ascending order and a '-' character for descending order.

    If the \l autoUpdate property is true when the request is executed it will
    enter an \l Idle state on finishing and will refresh the queried
    information if the items matching the query change.  If the gallery can't
    provide updates it will instead go immediately to the \l Finished state.
    Automatic updates can be canceled by calling cancel() on a idle request.

    Only one item in a query can be accessed at a time, so before
    information about an item can be accessed it must be selected using one of
    the fetch() functions.  When a new index is selected the result set will
    emit the currentIndexChanged() signal, and when the currently selected item
    changes the currentItemChanged() signal will be emitted.  If the
    currentIndex() contains a gallery item isValid() will return true, otherwise
    it will return false. Information identifying the current item in the
    request can be accessed using the itemId(), itemUrl() and itemType()
    functions.

    For each meta-data property returned by a query a unique key will be
    provided, this cab be queried by passing the property's name to the
    propertyKey() function.  Passing this key to the metaData() function will
    return the current item's value for that property.  Some queries may return
    items with editable meta-data values which can be changed using the
    setMetaData() function.  The attributes of a meta-data property such as
    whether it's writable can be queried from propertyAttributes() and the
    type of value that will returned by metaData() can be queried using
    propertyType().

    \sa QDocumentGallery, QGalleryQueryModel
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
    if (d_func()->propertyNames != names) {
        d_func()->propertyNames = names;

        emit propertyNamesChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::propertyNamesChanged();

    Signals that the value of \l propertyNames has changed.
*/

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
    if (d_func()->sortPropertyNames != names) {
        d_func()->sortPropertyNames = names;

        emit sortPropertyNamesChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::sortPropertyNamesChanged()

    Signals that the value of \l sortPropertyNames has changed.
*/

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
    if (d_func()->autoUpdate != enabled) {
        d_func()->autoUpdate = enabled;

        emit autoUpdateChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::autoUpdateChanged()

    Signals that the value of \l autoUpdate has changed.
*/

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
    const int boundedOffset = qMax(0, offset);
    if (d_func()->offset != boundedOffset) {
        d_func()->offset = boundedOffset;

        emit offsetChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::offsetChanged()

    Signals that the value of offset has changed.
*/

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
    const int boundedLimit = qMax(0, limit);
    if (d_func()->limit != boundedLimit) {
        d_func()->limit = boundedLimit;

        emit limitChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::limitChanged()

    Signals that the value of \l limit has changed.
*/

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
    if (d_func()->rootType != itemType) {
        d_func()->rootType = itemType;

        emit rootTypeChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::rootTypeChanged()

    Signals that the value of \l rootType has changed.
*/

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
    if (d_func()->rootItem != itemId) {
        d_func()->rootItem = itemId;

        emit rootItemChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::rootItemChanged()

    Signals that the value of \l rootItem has changed.
*/

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
    if (d_func()->scope != scope) {
        d_func()->scope = scope;

        emit scopeChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::scopeChanged()

    Signals that the value of \l scope has changed.
*/

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
    if (d_func()->filter != filter) {
        d_func()->filter = filter;

        emit filterChanged();
    }
}

/*!
    \fn QGalleryQueryRequest::filterChanged()

    Signals that the value of \l filter has changed.
*/


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
