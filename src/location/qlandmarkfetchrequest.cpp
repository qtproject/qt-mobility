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

#include "qlandmarkfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkfilter.h"
#include "qlandmarkidfilter.h"
#include "qlandmarksortorder.h"
#include "qlandmark.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkFetchRequest
    \brief The QLandmarkFetchRequest class allows a client to asynchronously
    request a list of landmarks from a landmark manager.

    For a QLandmarkFetchRequest, the resultsAvailable() signal will be emitted when the resultant
    landmarks (which may be retrieved by calling landmarks()), are updated, as well as if
    the overall operation error (which may be retrieved by calling error()) is updated.

    \ingroup landmarks-request
*/

/*!
    Creates a new landmark fetch request object with the given \a manager \a parent.
*/
QLandmarkFetchRequest::QLandmarkFetchRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkFetchRequest::~QLandmarkFetchRequest()
{
}

/*!
    Returns the filter which will be used to select the landmarks.

    By default, the filter's type will be a LandmarkFilter::DefaultFilter
    and thus match all landmarks.
*/
QLandmarkFilter QLandmarkFetchRequest::filter() const
{
    Q_D(const QLandmarkFetchRequest);
    return d->filter;
}

/*!
    Sets the \a filter which will be used to select landmarks.

    \sa setLandmarkIds()
*/
void QLandmarkFetchRequest::setFilter(const QLandmarkFilter &filter)
{
    Q_D(QLandmarkFetchRequest);
    d->filter = filter;
}

/*!
    This is a convenience function which will set a QLandmarkIdFilter
    based on the given list of landmark \a ids.

    \sa setFilter()
*/
void QLandmarkFetchRequest::setLandmarkIds(const QList<QLandmarkId> &ids)
{
    Q_D(QLandmarkFetchRequest);
    d->filter = QLandmarkIdFilter(ids);
}

/*!
    Returns the sort ordering which is used to sort the result.
*/
QList<QLandmarkSortOrder> QLandmarkFetchRequest::sorting() const
{
    Q_D(const QLandmarkFetchRequest);
    return d->sorting;
}

/*!
    Sets the sort ordering of the request to \a sorting.  This
    function will only have an effect on the results if invoked
    prior to calling \l QLandmarkAbstractRequest::start().
*/
void QLandmarkFetchRequest::setSorting(const QList<QLandmarkSortOrder> &sorting)
{
    Q_D(QLandmarkFetchRequest);
    d->sorting = sorting;
}

/*!
    Sets the sort ordering of the request to a single\a sorting.  This function
    will only have an effect on results if invoked prior to calling
    \l QLandmarkAbstractRequest::start()
*/
void QLandmarkFetchRequest::setSorting(const QLandmarkSortOrder &sorting)
{
    Q_D(QLandmarkFetchRequest);
    d->sorting.clear();
    d->sorting.append(sorting);
}

/*!
    Returns the list of landmarks which matched the
    filter.
*/
QList<QLandmark> QLandmarkFetchRequest::landmarks() const
{
    Q_D(const QLandmarkFetchRequest);
    return d->landmarks;
}

#include "moc_qlandmarkfetchrequest.cpp"

QTM_END_NAMESPACE
