/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFetchRequest
    \brief The QLandmarkCategoryFetchRequest class allows a client to asynchronously
    request a list of categories from a landmark manager.

    For a QLandmarkCategoryFetchRequest, the resultsAvailable() signal will be emitted
    as resultant categories are found (these are retrievable by calling categories()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
*/

/*!
    Creates a new category fetch request object with the given \a manager and \a parent.
*/
QLandmarkCategoryFetchRequest::QLandmarkCategoryFetchRequest(QLandmarkManager *manager, QObject *parent)
    :QLandmarkAbstractRequest(new QLandmarkCategoryFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryFetchRequest::~QLandmarkCategoryFetchRequest()
{
}

/*!
    Returns the list of categories that have been found during the
    request.
*/
QList<QLandmarkCategory> QLandmarkCategoryFetchRequest::categories() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->categories;
}

/*!
    Returns the sorting of the categories.
    By default the sorting is case insensitive and in ascending order
    according to the category name.
*/
QLandmarkNameSort QLandmarkCategoryFetchRequest::sorting() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->sorting;
}

/*!
    Sets \a nameSort to specify the sorting of the returned categories.
*/
void QLandmarkCategoryFetchRequest::setSorting(const QLandmarkNameSort &nameSort)
{
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting = nameSort;
}

/*!
    Returns the maximum number of categories to be returned.  By default the limit
    is -1 indicating that all categories should be retrieved.
*/
int QLandmarkCategoryFetchRequest::limit() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->limit;
}

/*!
    Sets the maximum number of categories to be returned to \a limit.
    A limit of -1 will retrieve all categories.

    (A limit of 0 will retrieve no categories.)
 */
void QLandmarkCategoryFetchRequest::setLimit(int limit)
{
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->limit = limit;
}

/*!
    Returns the index offset for the request.  By default the offset is 0.
*/
int QLandmarkCategoryFetchRequest::offset() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->offset;
}

/*!
    Sets the index \a offset for the request.
*/
void QLandmarkCategoryFetchRequest::setOffset(int offset) {
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->offset = offset;
}

#include "moc_qlandmarkcategoryfetchrequest.cpp"

QTM_END_NAMESPACE
