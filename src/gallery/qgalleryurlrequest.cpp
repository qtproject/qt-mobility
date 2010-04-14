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

#include "qgalleryurlrequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryUrlRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryUrlRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Url)
        , live(false)
    {
    }

    bool live;
    bool create;
    QStringList propertyNames;
    QUrl itemUrl;
};

/*!
    \class QGalleryUrlRequest

    \brief The QGalleryItemRequest class provides a request for an item
    identified by a URL from a gallery.
*/

/*!
    Constructs a new gallery url request.

    The \a parent is passed to QObject.
*/

QGalleryUrlRequest::QGalleryUrlRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryUrlRequestPrivate(0), parent)
{
}

/*!
    Contructs a new url request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryUrlRequest::QGalleryUrlRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryUrlRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery url request.
*/

QGalleryUrlRequest::~QGalleryUrlRequest()
{
}

/*!
    \property QGalleryUrlRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/

QStringList QGalleryUrlRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryUrlRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryUrlRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryUrlRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryUrlRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryUrlRequest::itemUrl

    \brief The url of the item a request should return.
*/

QUrl QGalleryUrlRequest::itemUrl() const
{
    return d_func()->itemUrl;
}

void QGalleryUrlRequest::setItemUrl(const QUrl &url)
{
    d_func()->itemUrl = url;
}

/*!
    \property QGalleryUrlRequest::create

    \biref Whether a gallery should create a new entry for the requested item
    if one does not already exist.
*/

bool QGalleryUrlRequest::create() const
{
    return d_func()->create;
}

void QGalleryUrlRequest::setCreate(bool create)
{
    d_func()->create = create;
}

/*!
    \property QGalleryUrlRequest::item

    \brief A list containing the item returned by the request.
*/

QGalleryItemList *QGalleryUrlRequest::item() const
{
    return d_func()->response;
}

/*!
    \fn void QGalleryUrlRequest::itemChanged()

    Signals that the \l item property has changed.
*/

void QGalleryUrlRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemChanged();
}

#include "moc_qgalleryurlrequest.cpp"

QTM_END_NAMESPACE
