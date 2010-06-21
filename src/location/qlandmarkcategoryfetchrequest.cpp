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

#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFetchRequest
    \brief The QLandmarkCategoryFetchRequest class allows a client to asynchronously
    request a list of categories from a landmark manager.

    For a QLandmarkCategoryFetchRequest, the resultsAvailable() signal will be emitted
    as resultant categories are found (these are retrievable via the callings categories()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).

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
    return d->categories;
}

/*!
    Returns a list of identifiers for categories which will be retrieved
    by the fetch request.

    By default no category ids are set, this means the fetch request
    will fetch all categories.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryFetchRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    return d->categoryIds;
}

/*!
    Sets the categories to be retrieved by the request as a list of
    \a categoryIds.

    If \a categoryIds is empty, then the request will fetch all categories.
*/
void QLandmarkCategoryFetchRequest::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_D(QLandmarkCategoryFetchRequest);
    d->categoryIds = categoryIds;
}

/*!
    Convenience function to set a single category identified by \a categoryId to be fetched.
*/
void QLandmarkCategoryFetchRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryFetchRequest);
    d->categoryIds.clear();
    d->categoryIds.append(categoryId);
}

/*!
    Returns the matching scheme for the fetch request.

    By default the matching scheme is set to to \a MatchSubset
*/
QLandmarkCategoryFetchRequest::MatchingScheme QLandmarkCategoryFetchRequest::matchingScheme() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    return d->matchingScheme;
}

/*!
    Sets the \matchingScheme of the fetch request.
*/
void QLandmarkCategoryFetchRequest::setMatchingScheme(QLandmarkCategoryFetchRequest::MatchingScheme matchingScheme)
{
    Q_D(QLandmarkCategoryFetchRequest);
    d->matchingScheme = matchingScheme;
}

#include "moc_qlandmarkcategoryfetchrequest.cpp"

QTM_END_NAMESPACE
