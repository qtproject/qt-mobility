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

#include "galleryfilterrequest.h"

#include "galleryfilter.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryFilterRequest GalleryFilterRequest
    \brief The GalleryFilterRequest element allows you to query a list of items
    from a gallery.

    This is element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    import Qt 4.7
    import QtMobility.gallery 1.0

    Rectangle {
        width: 1024
        height: 768

        GalleryFilterRequest {
            id: request;
            gallery: DocumentGallery {}

            itemType: "Image"
            properties: ["thumbnailImage"]
            filter: GalleryFilter {
                property: "fileName";
                value: "*.jpg";
                matchFlags: GalleryFilter.MatchExactly
            }
        }

        GridView {
            anchors.fill: parent
            cellWidth: 128
            cellHeight: 128

            model: request.model

            delegate: Image {
                pixmap: thumbnailImage
            }
        }
    }
    \endqml

    \sa GalleryItemRequest
*/

GalleryFilterRequest::GalleryFilterRequest(QObject *parent)
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

    connect(&m_request, SIGNAL(itemsChanged(QGalleryItemList*)),
            this, SLOT(_q_itemsChanged(QGalleryItemList*)));
}

GalleryFilterRequest::~GalleryFilterRequest()
{
    delete m_model;
}

void GalleryFilterRequest::classBegin()
{
}

void GalleryFilterRequest::componentComplete()
{
    reload();
}

/*!
    \qmlproperty gallery GalleryFilterRequest::gallery

    This property holds the gallery a query should return results from.
*/

/*!
    \qmlproperty enum GalleryFilterRequest::state

    This property holds the state of a query.
*/

/*!
    \qmlproperty enum GalleryFilterRequest::result

    The property holds the result of a query.
*/

/*!
    \qmlproperty int GalleryFilterRequest::currentProgress

    This property holds the current progress value.
*/

/*!
    \qmlproperty int GalleryFilterRequest::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty stringlist GalleryFilterRequest::properties

    This property holds the item properties a query should return values for.
*/

/*!
    \qmlproperty stringlist GalleryFilterRequest::sortProperties

    This property holds the properties the results of a query should be sorted
    on.
*/

/*!
    \qmlproperty bool GalleryFilterRequest::live

    This property holds whether a query should refresh its results
    automatically.
*/

/*!
    \qmlproperty int GalleryFilterRequest::cursorPosition

    This property holds the offset of a query's internal cache.
*/

/*!
    \qmlproperty int GalleryFilterRequest::minimumPagedItems

    This property contains the minimum number of consectutive items a query
    should retain in it's internal cache.
*/

/*!
    \qmlproperty string GalleryFilterRequest::itemType

    This property contains the type of item a query should return.
*/

/*!
    \qmlproperty filter GalleryFilterRequest::filter

    This property contains criteria to used to filter the results of a query.
*/

/*!
    \qmlproperty galleryId GalleryFilterRequest::containerId

    This property contains the id of a container item that a query should
    return the contents of.
*/

/*!
    \qmlproperty model GalleryFilterRequest::model

    This property holds a model containing the results of a query.
*/

/*!
    \qmlmethod GalleryFilterRequest::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryFilterRequest::cancel()

    Cancels an executing query.
*/

/*!
    \qmlmethod GalleryFilterRequest::clear()

    Clears the results of a query.
*/

/*!
    \qmlsignal GalleryFilterRequest::onSucceeded()

    Signals that a query has finished successfully.
*/

/*!
    \qmlsignal GalleryFilterRequest::onCancelled()

    Signals that a query was cancelled.
*/

/*!
    \qmlsignal GalleryFilterRequest::onFailed(error)

    Signals that a query failed with the given \a error.
*/

/*!
    \qmlsignal GalleryFilterRequest::onFinished(result)

    Signals that a query finished with the given \a result.
*/

void GalleryFilterRequest::reload()
{
    m_request.setFilter(m_filter ? m_filter->filter() : QGalleryFilter());
    if (m_items)
        m_request.setInitialCursorPosition(m_items->cursorPosition());
    m_request.execute();
}

void GalleryFilterRequest::_q_itemsChanged(QGalleryItemList *items)
{
    GalleryItemListModel *model = 0;

    if (items)
        model = new GalleryItemListModel(items, this);

    qSwap(model, m_model);

    emit modelChanged();
    emit cursorPositionChanged();

    delete model;
}

#include "moc_galleryfilterrequest.cpp"

QTM_END_NAMESPACE
