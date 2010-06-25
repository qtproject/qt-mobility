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

#include "gallerycountrequest.h"

#include "galleryfilter.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryQueryCount GalleryCountRequest

    \brief The GalleryQueryCount element allows you to fetch the number of
    items matching a gallery query.

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \sa GalleryQueryModel, GalleryItem
*/

GalleryCountRequest::GalleryCountRequest(QObject *parent)
    : QObject(parent)
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
    connect(&m_request, SIGNAL(countChanged()), this, SIGNAL(countChanged()));
}

/*!
    \qmlproperty QAbstractGallery GalleryQueryCount::gallery

    This property holds the gallery a query should return results from.
*/

/*!
    \qmlproperty enum GalleryQueryCount::state

    This property holds the state of a query.  It can be one of:

    \list
    \o Inactive The query has finished.
    \o Active The query is currently executing.
    \o Cancelling The query has been cancelled, but has yet reached the
    Inactive state.
    \o Idle The query has finished and is monitoring its result set for
    changes.
    \endlist
*/

/*!
    \qmlproperty enum GalleryQueryCount::result

    The property holds the result of a query. It can be one of:

    \list
    \o NoResult The query is still executing.
    \o Succeeded The query finished successfully.
    \o Cancelled The query was cancelled.
    \o NoGallery No gallery was set on the query.
    \o NotSupported Count queries are not supported by the \l gallery.
    \o ConnectionError The query failed due to a connection error.
    \o InvalidItemError The query failed because the value of \l scopeItemId
    is not a valid item ID.
    \o ItemTypeError The query failed because the value of \l itemType is not
    a valid item type.
    \o InvalidPropertyError The query failed because the \l filter refers to an
    invalid property.
    \o PropertyTypeError The query failed because the type of a value in the
    \l filter is incompatible with the property.
    \o UnsupportedFilterTypeError The query failed because the set \l filter
    is not supported by the \l gallery.
    \o UnsupportedFilterOptionError The query failed because an option of a
    \l filter is not supported by the \l gallery.
    \endlist
*/

/*!
    \qmlproperty int GalleryQueryCount::currentProgress

    This property holds the current progress value.
*/

/*!
    \qmlproperty int GalleryQueryCount::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty stringlist GalleryQueryCount::properties

    This property holds the item properties a query should return values for.
*/

/*!
    \qmlproperty bool GalleryQueryCount::live

    This property holds whether a query should refresh its results
    automatically.
*/

/*!
    \qmlproperty string GalleryQueryCount::itemType

    This property contains the type of item a query should return.
*/

/*!
    \qmlproperty GalleryFilter GalleryQueryCount::filter

    This property contains criteria to used to filter the results of a query.
*/

/*!
    \qmlproperty enum GalleryQueryCount::scope

    The property contains whether a query should count the direct descendants
    of the \l scopeItemId or all descendants.
*/

/*!
    \qmlproperty variant GalleryQueryCount::scopeItemId

    This property contains the id of an item that a query should return a
    count of the descendants of.
*/

/*!
    \qmlproperty int GalleryQueryCount::count

    This property holds the number of items matching a query.
*/

/*!
    \qmlmethod GalleryQueryCount::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryQueryCount::cancel()

    Cancels an executing query.
*/

/*!
    \qmlmethod GalleryQueryCount::clear()

    Clears the results of a query.
*/

/*!
    \qmlsignal GalleryQueryCount::onSucceeded()

    Signals that a query has finished successfully.
*/

/*!
    \qmlsignal GalleryQueryCount::onCancelled()

    Signals that a query was cancelled.
*/

/*!
    \qmlsignal GalleryQueryCount::onFailed(error)

    Signals that a query failed with the given \a error.
*/

/*!
    \qmlsignal GalleryQueryCount::onFinished(result)

    Signals that a query finished with the given \a result.
*/

void GalleryCountRequest::classBegin()
{
}

void GalleryCountRequest::componentComplete()
{
    reload();
}

void GalleryCountRequest::reload()
{
    m_request.setFilter(m_filter ? m_filter->filter() : QGalleryFilter());
    m_request.execute();
}


#include "moc_gallerycountrequest.cpp"

QTM_END_NAMESPACE
