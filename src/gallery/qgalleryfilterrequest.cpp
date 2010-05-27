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

#include "qgalleryfilterrequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryFilterRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryFilterRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Filter)
        , initialCursorPosition(0)
        , minimumPagedItems(200)
        , live(false)
    {
    }

    int initialCursorPosition;
    int minimumPagedItems;
    bool live;
    QStringList propertyNames;
    QStringList sortPropertyNames;
    QString itemType;
    QVariant containerId;
    QGalleryFilter filter;
};

/*!
    \class QGalleryFilterRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryFilterRequest class provides a request for a set of
    items from a gallery.

*/
/*!
    Constructs a new gallery filter request.

    The \a parent is passed to QObject.
*/


QGalleryFilterRequest::QGalleryFilterRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryFilterRequestPrivate(0), parent)
{
}
/*!
    Contructs a new filter request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryFilterRequest::QGalleryFilterRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryFilterRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery filter request.
*/

QGalleryFilterRequest::~QGalleryFilterRequest()
{
}
/*!
    \property QGalleryFilterRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/


QStringList QGalleryFilterRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryFilterRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryFilterRequest::sortPropertyNames

    \brief A list of names of meta-data properties a request should sort its
    results on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryFilterRequest::sortPropertyNames() const
{
    return d_func()->sortPropertyNames;
}

void QGalleryFilterRequest::setSortPropertyNames(const QStringList &names)
{
    d_func()->sortPropertyNames = names;
}

/*!
    \property QGalleryFilterRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryFilterRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryFilterRequest::setLive(bool live)
{
    d_func()->live = live;
}
/*!
    \property QGalleryFilterRequest::initialCursorPosition

    \brief The index of the first item a request should return.

    By default this is 0.
*/


int QGalleryFilterRequest::initialCursorPosition() const
{
    return d_func()->initialCursorPosition;
}

void QGalleryFilterRequest::setInitialCursorPosition(int index)
{
    d_func()->initialCursorPosition = index;
}
/*!
    \property QGalleryFilterRequest::minimumPagedItems

    \brief The minimum number of consecutive items the list returned by a
    request should cache.

    By default this is 200.
*/


int QGalleryFilterRequest::minimumPagedItems() const
{
    return d_func()->minimumPagedItems;
}

void QGalleryFilterRequest::setMinimumPagedItems(int size)
{
    d_func()->minimumPagedItems = size;
}
/*!
    \property QGalleryFilterRequest::itemType

    \brief The type of items a request should return.
*/


QString QGalleryFilterRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryFilterRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryFilterRequest::containerId

    \brief The ID of a container item a request should return the descendents
    of.

*/

QVariant QGalleryFilterRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryFilterRequest::setContainerId(const QVariant &id)
{
    d_func()->containerId = id;
}
/*!
    \property QGalleryFilterRequest::filter

    \brief A filter identifying the items a request should return.

    If no filter is set the results of the request will be determined soley
    by the \l itemType property.
*/

QGalleryFilter QGalleryFilterRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryFilterRequest::setFilter(const QGalleryFilter &filter)
{
    d_func()->filter = filter;
}
/*!
    \property QGalleryFilterRequest::items

    \brief The items returned by a request.
*/


QGalleryItemList *QGalleryFilterRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryFilterRequest::itemsChanged(QGalleryItemList *items)

    Signals that the list of \a items returned by a request has changed.
*/

/*!
    \reimp
*/

void QGalleryFilterRequest::setResponse(QGalleryAbstractResponse *response)
{
    emit itemsChanged(response);
}

#include "moc_qgalleryfilterrequest.cpp"

QTM_END_NAMESPACE
