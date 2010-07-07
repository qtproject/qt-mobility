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

QTM_BEGIN_NAMESPACE

class QGalleryQueryRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryQueryRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Query)
        , initialCursorPosition(0)
        , minimumPagedItems(200)
        , scope(QGalleryAbstractRequest::AllDescendants)
        , live(false)
    {
    }

    int initialCursorPosition;
    int minimumPagedItems;
    QGalleryAbstractRequest::Scope scope;
    bool live;
    QStringList propertyNames;
    QStringList sortPropertyNames;
    QString itemType;
    QVariant scopeItemId;
    QGalleryFilter filter;
};

/*!
    \class QGalleryQueryRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryQueryRequest class provides a request for a set of
    items from a gallery.

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
    \property QGalleryQueryRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryQueryRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryQueryRequest::setLive(bool live)
{
    d_func()->live = live;
}
/*!
    \property QGalleryQueryRequest::initialCursorPosition

    \brief The index of the first item a request should return.

    By default this is 0.
*/


int QGalleryQueryRequest::initialCursorPosition() const
{
    return d_func()->initialCursorPosition;
}

void QGalleryQueryRequest::setInitialCursorPosition(int index)
{
    d_func()->initialCursorPosition = index;
}
/*!
    \property QGalleryQueryRequest::minimumPagedItems

    \brief The minimum number of consecutive items the list returned by a
    request should cache.

    By default this is 200.
*/


int QGalleryQueryRequest::minimumPagedItems() const
{
    return d_func()->minimumPagedItems;
}

void QGalleryQueryRequest::setMinimumPagedItems(int size)
{
    d_func()->minimumPagedItems = size;
}
/*!
    \property QGalleryQueryRequest::itemType

    \brief The type of items a request should return.
*/


QString QGalleryQueryRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryQueryRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryQueryRequest::scope

    \brief whether all descendants of the scopeItemId should be returned by
    a request or just the direct descendants.
*/

QGalleryAbstractRequest::Scope QGalleryQueryRequest::scope() const
{
    return d_func()->scope;
}

void QGalleryQueryRequest::setScope(QGalleryAbstractRequest::Scope scope)
{
    d_func()->scope = scope;
}

/*!
    \property QGalleryQueryRequest::scopeItemId

    \brief The ID of an item a request should return the descendants of.

    \sa scope
*/

QVariant QGalleryQueryRequest::scopeItemId() const
{
    return d_func()->scopeItemId;
}

void QGalleryQueryRequest::setScopeItemId(const QVariant &id)
{
    d_func()->scopeItemId = id;
}

/*!
    \property QGalleryQueryRequest::filter

    \brief A filter identifying the items a request should return.

    If no filter is set the results of the request will be determined
    by the \l itemType and \l scopeItemId properties.
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
    \property QGalleryQueryRequest::items

    \brief The items returned by a request.
*/


QGalleryItemList *QGalleryQueryRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryQueryRequest::itemsChanged(QGalleryItemList *items)

    Signals that the list of \a items returned by a request has changed.
*/

/*!
    \reimp
*/

void QGalleryQueryRequest::setResponse(QGalleryAbstractResponse *response)
{
    emit itemsChanged(response);
}

#include "moc_qgalleryqueryrequest.cpp"

QTM_END_NAMESPACE
