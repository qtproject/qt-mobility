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
    Q_DECLARE_PUBLIC(QGalleryUrlRequest)
public:
    QGalleryUrlRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Url)
        , create(false)
    {
    }

    void _q_itemsInserted(int index, int count)
    {
        if (index == 0 && count == 1) {
            itemId = response->id(0);
            itemType = response->type(0);

            emit q_func()->itemChanged();
        } else {
            qWarning("QGalleryUrlRequest::insert: Response contained an irregular number of items."
                     "Index: %d, Count %d", index, count);
        }
    }

    bool create;
    QUrl itemUrl;
    QVariant itemId;
    QString itemType;
};

/*!
    \class QGalleryUrlRequest

    \brief The QGalleryUrlRequest class provides a request for an item
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

    \brief Whether a gallery should create a new entry for the requested item
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
    \property QGalleryUrlRequest::itemId

    \brief The ID of the requested item.
*/

QVariant QGalleryUrlRequest::itemId() const
{
    return d_func()->itemId;
}

/*!
    \property QGalleryUrlRequest::itemType

    \brief The type of the requested item.
*/

QString QGalleryUrlRequest::itemType() const
{
    return d_func()->itemType;
}

/*!
    \fn void QGalleryUrlRequest::itemChanged()

    Signals that the \l itemId and \l itemType properties have changed.
*/

/*!
    \reimp
*/

void QGalleryUrlRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryUrlRequest);

    if (response && response->count() > 0) {
        d->itemId = response->id(0);
        d->itemType = response->type(0);

        emit itemChanged();
    } else {
        if (response)
            connect(response, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));

        if (!d->itemId.isNull() || !d->itemType.isNull()) {
            d->itemId = QVariant();
            d->itemType = QString();

            emit itemChanged();
        }
    }
}

#include "moc_qgalleryurlrequest.cpp"

QTM_END_NAMESPACE
