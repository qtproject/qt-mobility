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

#include "galleryitemrequest.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryItem GalleryItemRequest
    \brief The GalleryItem element allows you to request a single item
    from a gallery

    This is element is part of the \bold {QtMobility.gallery 1.0} module.

    \sa GalleryFilterRequest
*/

GalleryItemRequest::GalleryItemRequest(QObject *parent)
    : QObject(parent)
    , m_items(0)
    , m_model(0)
{
    connect(&m_request, SIGNAL(succeeded()), this, SIGNAL(succeeded()));
    connect(&m_request, SIGNAL(cancelled()), this, SIGNAL(cancelled()));
    connect(&m_request, SIGNAL(stateChanged(QGalleryAbstractRequest::State)),
            this, SIGNAL(stateChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(progressChanged(int,int)), this, SIGNAL(progressChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(failed(int)), this, SIGNAL(failed(int)));
    connect(&m_request, SIGNAL(finished(int)), this, SIGNAL(finished(int)));

    connect(&m_request, SIGNAL(itemChanged(QGalleryItemList*)),
            this, SLOT(_q_itemsChanged(QGalleryItemList*)));
}

GalleryItemRequest::~GalleryItemRequest()
{
    delete m_model;
}

/*!
    \qmlproperty gallery GalleryItem::gallery

    This property holds the gallery a request should return results from.
*/

/*!
    \qmlproperty enum GalleryItem::state

    This property holds the state of a request.
*/

/*!
    \qmlproperty enum GalleryItem::result

    The property holds the result of a request.
*/

/*!
    \qmlproperty int GalleryItem::currentProgress

    This property holds the current progress value.
*/

/*!
    \qmlproperty int GalleryItem::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty stringlist GalleryItem::properties

    This property holds the item properties a request should return values for.
*/

/*!
    \qmlproperty bool GalleryItem::live

    This property holds whether a request should refresh its results
    automatically.
*/

/*!
    \qmlpropery galleryId GalleryItem::itemId

    This property holds the id of the item to return information about.
*/

/*!
    \qmlproperty model GalleryItem::model

    This property holds a model containing the results of a request.
*/

/*!
    \qmlmethod GalleryItem::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryItem::cancel()

    Cancels an executing request.
*/

/*!
    \qmlmethod GalleryItem::clear()

    Clears the results of a request.
*/

/*!
    \qmlsignal GalleryItem::onSucceeded()

    Signals that a request has finished successfully.
*/

/*!
    \qmlsignal GalleryItem::onCancelled()

    Signals that a request was cancelled.
*/

/*!
    \qmlsignal GalleryItem::onFailed(error)

    Signals that a request failed with the given \a error.
*/

/*!
    \qmlsignal GalleryItem::onFinished(result)

    Signals that a request finished with the given \a result.
*/

void GalleryItemRequest::classBegin()
{
}

void GalleryItemRequest::componentComplete()
{
    reload();
}

void GalleryItemRequest::_q_itemsChanged(QGalleryItemList *items)
{
    GalleryItemListModel *model = 0;

    if (items) {
        model = new GalleryItemListModel(this);
        model->setItemList(items);
    }

    qSwap(model, m_model);

    emit modelChanged();

    delete model;
}

#include "moc_galleryitemrequest.cpp"

QTM_END_NAMESPACE
