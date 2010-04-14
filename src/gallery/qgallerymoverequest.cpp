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

#include "qgallerymoverequest.h"

#include "qgalleryabstractrequest_p.h"


class QGalleryMoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryMoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Move)
    {
    }

    QStringList propertyNames;
    QStringList itemIds;
    QString destinationId;
};

/*!
    \class QGalleryMoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryMoveRequest class provides a request which moves
    items within a gallery.
*/

/*!
    Constructs a new gallery move request.

    The \a parent is passed to QObject.
*/

QGalleryMoveRequest::QGalleryMoveRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryMoveRequestPrivate(0), parent)
{
}

/*!
    Contructs a new move request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryMoveRequest::QGalleryMoveRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryMoveRequestPrivate(gallery), parent)
{
}

/*!
*/

QGalleryMoveRequest::~QGalleryMoveRequest()
{
}

/*!
    \property QGalleryMoveRequest::propertyNames

    \brief A list of names of meta-data properties the \l currentItem list
    should contain values for.
*/

QStringList QGalleryMoveRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryMoveRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
}

/*!
    \property QGalleryMoveRequest::itemId

    \brief The ID of an item to copy within a gallery.

    This is equivalent to \l itemIds with a single ID.  If there are
    multiple \l itemIds this will be null.
*/

QString QGalleryMoveRequest::itemId() const
{
    Q_D(const QGalleryMoveRequest);

    return d->itemIds.count() == 1
            ? d->itemIds.first()
            : QString();
}

void QGalleryMoveRequest::setItemId(const QString &id)
{
    Q_D(QGalleryMoveRequest);

    d->itemIds.clear();

    if (!id.isNull())
        d->itemIds.append(id);

    emit itemIdsChanged();
}

/*!
    \property QGalleryMoveRequest::itemIds

    \brief A list of IDs of items to copy within a gallery.

    If the list only contains one ID this is equivalent to \l itemId.
*/

QStringList QGalleryMoveRequest::itemIds() const
{
    return d_func()->itemIds;
}

void QGalleryMoveRequest::setItemIds(const QStringList &ids)
{
    d_func()->itemIds = ids;

    emit itemIdsChanged();
}

/*!
    \fn QGalleryMoveRequest::itemIdsChanged()

    Signals the \l itemId and \l itemIds properties have changed.
*/

/*!
    \property QGalleryMoveRequest::destinationId

    \brief The ID of the container item a request will move items to.
*/

QString QGalleryMoveRequest::destinationId() const
{
    return d_func()->destinationId;
}

void QGalleryMoveRequest::setDestinationId(const QString &id)
{
    d_func()->destinationId = id;
}

/*!
    \property QGalleryMoveRequest::currentItem

    \brief A list containing the current item being moved by a request.
*/

QGalleryItemList *QGalleryMoveRequest::currentItem() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryMoveRequest::currentItemChanged()

    Signals the \l currentItem property has changed.
*/

/*!
    \reimp
*/

void QGalleryMoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit currentItemChanged();
}
