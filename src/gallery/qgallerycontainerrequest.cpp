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

#include "qgallerycontainerrequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryContainerRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryContainerRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Container)
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
};

/*!
    \class QGalleryContainerRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryContainerRequest class provides a request for the
    immediate children of an item in a gallery.
*/

/*!
    Constructs a new gallery container request.

    The \a parent is passed to QObject.
*/


QGalleryContainerRequest::QGalleryContainerRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryContainerRequestPrivate(0), parent)
{
}
/*!
    Contructs a new container request for the given \a gallery.

    The \a parent is passed to QObject.
*/


QGalleryContainerRequest::QGalleryContainerRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryContainerRequestPrivate(gallery), parent)
{
}
/*!
    Destroys a gallery container request.
*/


QGalleryContainerRequest::~QGalleryContainerRequest()
{
}
/*!
    \property QGalleryContainerRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/


QStringList QGalleryContainerRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryContainerRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryContainerRequest::sortPropertyNames

    \brief A list of names of meta-data properties a request should sort its
    results on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryContainerRequest::sortPropertyNames() const
{
    return d_func()->sortPropertyNames;
}

void QGalleryContainerRequest::setSortPropertyNames(const QStringList &names)
{
    d_func()->sortPropertyNames = names;
}
/*!
    \property QGalleryContainerRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryContainerRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryContainerRequest::setLive(bool live)
{
    d_func()->live = live;
}
/*!
    \property QGalleryContainerRequest::initialCursorPosition

    \brief The index of the first item a request should return.

    By default this is 0.
*/

int QGalleryContainerRequest::initialCursorPosition() const
{
    return d_func()->initialCursorPosition;
}

void QGalleryContainerRequest::setInitialCursorPosition(int index)
{
    d_func()->initialCursorPosition = index;
}
/*!
    \property QGalleryContainerRequest::minimumPagedItems

    \brief The minimum number of consecutive items the list returned by a
    request should cache.

    By default this is 200.
*/


int QGalleryContainerRequest::minimumPagedItems() const
{
    return d_func()->minimumPagedItems;
}

void QGalleryContainerRequest::setMinimumPagedItems(int size)
{
    d_func()->minimumPagedItems = size;
}
/*!
    \property QGalleryContainerRequest::itemType

    \brief The type of items a request should return.

    If this is not set items of all types will be returned.  If no filter is
    set all items of this type will be returned.
*/


QString QGalleryContainerRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryContainerRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}
/*!
    \property QGalleryContainerRequest::containerId

    \brief The ID of container item a item request should return the
    immediate children of.
*/


QVariant QGalleryContainerRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryContainerRequest::setContainerId(const QVariant &id)
{
    d_func()->containerId = id;
}

/*!
    \property QGalleryContainerRequest::items

    \brief The items returned by a request.
*/

QGalleryItemList *QGalleryContainerRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryContainerRequest::itemsChanged()
*/

/*!
    \reimp
*/
void QGalleryContainerRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemsChanged();
}

#include "moc_qgallerycontainerrequest.cpp"

QTM_END_NAMESPACE
