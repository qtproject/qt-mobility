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

#include "qgalleryremoverequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryRemoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryRemoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Remove)
    {
    }

    QStringList propertyNames;
    QVariantList itemIds;
};

/*!
    \class QGalleryRemoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryRemoveRequest class provides a request which removes
    items from a gallery.
*/

/*!
    Constructs a new gallery remove request.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(0), parent)
{

}

/*!
    Contructs a new remove request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery remove request.
*/

QGalleryRemoveRequest::~QGalleryRemoveRequest()
{
}

/*!
    \property QGalleryRemoveRequest::propertyNames

    \brief A list of name of meta-data properties the \l currentItem list
    should contain values for.
*/

QStringList QGalleryRemoveRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryRemoveRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
}

/*!
    \property QGalleryRemoveRequest::itemId

    \brief The ID of an item to remove from a gallery.

    This is equivalent to \l itemIds with a single ID.  If there are
    multiple \l itemIds this will be null.
*/

QVariant QGalleryRemoveRequest::itemId() const
{
    Q_D(const QGalleryRemoveRequest);

    return d->itemIds.count() == 1
            ? d->itemIds.first()
            : QVariant();
}

void QGalleryRemoveRequest::setItemId(const QVariant &id)
{
    Q_D(QGalleryRemoveRequest);

    d->itemIds.clear();

    if (!id.isNull())
        d->itemIds.append(id);

    emit itemIdsChanged();
}

/*!
    \property QGalleryRemoveRequest::itemIds

    \brief A list of IDs of items to remove from a gallery.

    If the list only contains one ID this is equivalent to \l itemId.
*/

QVariantList QGalleryRemoveRequest::itemIds() const
{
    return d_func()->itemIds;
}

void QGalleryRemoveRequest::setItemIds(const QVariantList &ids)
{
    d_func()->itemIds = ids;

    emit itemIdsChanged();
}

/*!
    \fn QGalleryRemoveRequest::itemIdsChanged()

    Signals that the itemId and itemIds properties have changed.
*/

/*!
    \property QGalleryRemoveRequest::currentItem

    \brief A list containing the current item being removed by a request.
*/

QGalleryItemList *QGalleryRemoveRequest::currentItem() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryRemoveRequest::currentItemChanged()

    Signals the \l currentItem property has changed.
*/

/*!
    \reimp
*/

void QGalleryRemoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit currentItemChanged();
}

#include "moc_qgalleryremoverequest.cpp"

QTM_END_NAMESPACE
