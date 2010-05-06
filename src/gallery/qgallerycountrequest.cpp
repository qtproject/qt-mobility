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

#include "qgallerycountrequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryCountRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryCountRequest)
public:
    QGalleryCountRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Count)
        , count(0)
        , live(false)
    {
    }

    void _q_itemsChanged();

    int count;
    bool live;
    QString itemType;
    QVariant containerId;
    QGalleryFilter filter;
};

void QGalleryCountRequestPrivate::_q_itemsChanged()
{
    count = response->count();

    emit q_func()->countChanged();
}

/*!
    \class QGalleryCountRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryCountRequest class provides a request for the number of
    items in a gallery matching some criteria.
*/

/*!
    Constructs a new gallery count request.

    The \a parent is passed to QObject.
*/

QGalleryCountRequest::QGalleryCountRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCountRequestPrivate(0), parent)
{
}

/*!
    Contructs a new count request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryCountRequest::QGalleryCountRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCountRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a count request.
*/

QGalleryCountRequest::~QGalleryCountRequest()
{
}

/*!
    \property QGalleryCountRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryCountRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryCountRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryCountRequest::itemType

    \brief The type of item a request should count.
*/

QString QGalleryCountRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryCountRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryCountRequest::containerId

    \brief The ID of a container item a request should count the descendents
    of.

*/

QVariant QGalleryCountRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryCountRequest::setContainerId(const QVariant &id)
{
    d_func()->containerId = id;
}

/*!
    \property QGalleryCountRequest::filter

    \brief A filter used to restrict the items counted by a request.

*/

QGalleryFilter QGalleryCountRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryCountRequest::setFilter(const QGalleryFilter &filter)
{
    d_func()->filter = filter;
}

/*!
    \property QGalleryCountRequest::count

    \brief The number of gallery items matching a requests filtering criteria.
*/

int QGalleryCountRequest::count() const
{
    return d_func()->count;
}

/*!
    \fn QGalleryCountRequest::countChanged()

    Signals that the \l count property has changed.
*/

/*!
    \reimp
*/

void QGalleryCountRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryCountRequest);

    int count  = 0;

    if (response) {
        count = response->count();

        connect(response, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsChanged()));
        connect(response, SIGNAL(removed(int,int)), this, SLOT(_q_itemsChanged()));
    }

    if (d->count != count) {
        d->count = count;

        emit countChanged();
    }
}

#include "moc_qgallerycountrequest.cpp"

QTM_END_NAMESPACE
