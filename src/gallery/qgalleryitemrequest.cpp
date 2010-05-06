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

#include "qgalleryitemrequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryItemRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemRequest)
public:
    QGalleryItemRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Item)
        , live(false)
    {
    }

    bool live;
    QStringList propertyNames;
    QVariant itemId;
};

/*!
    \class QGalleryItemRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryItemRequest class provides a request for an item from a
    gallery.
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
*/

QStringList QGalleryItemRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryItemRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
}

/*!
    \property QGalleryItemRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryItemRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryItemRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryItemRequest::itemId

    \brief The ID of a item a item request should return.
*/

QVariant QGalleryItemRequest::itemId() const
{
    return d_func()->itemId;
}

void QGalleryItemRequest::setItemId(const QVariant &id)
{
    d_func()->itemId = id;
}

/*!
    \property QGalleryItemRequest::item

    \brief The item returned by a request.
*/

QGalleryItemList *QGalleryItemRequest::item() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryItemRequest::itemChanged()

    Signals that the \l item property has changed.
*/

/*!
    \reimp
*/

void QGalleryItemRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemChanged();
}

#include "moc_qgalleryitemrequest.cpp"

QTM_END_NAMESPACE
