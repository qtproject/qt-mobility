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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkcategoryfetchbyidrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFetchByIdRequest
    \brief The QLandmarkCategoryFetchByIdRequest class allows a client to asynchronously
    request a list of categories by id from a landmark manager.

    For a QLandmarkCategoryFetchByIdRequest, the resultsAvailable() signal will be emitted
    as resultant categories are found (these are retrievable via the callings categories()),
    whenever individual items error out(individual errors may be retrieved by calling errorMap()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).


    \inmodule QtLocation

    \ingroup landmarks-request
*/

/*!
    Creates a new a request object with the given \a manager and \a parent.
*/
QLandmarkCategoryFetchByIdRequest::QLandmarkCategoryFetchByIdRequest(QLandmarkManager *manager, QObject *parent)
    :QLandmarkAbstractRequest(new QLandmarkCategoryFetchByIdRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryFetchByIdRequest::~QLandmarkCategoryFetchByIdRequest()
{
}

/*!
    Returns a list of identifiers of categories which are to be retrieed by this request.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryFetchByIdRequest::categoryIds() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryIds;
}

/*!
    Sets the \a categoryIds which are to be retrieved by this request.
*/
void QLandmarkCategoryFetchByIdRequest::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_D(QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds = categoryIds;
}

/*!
    Sets a single \a categoryId which is to be retrieved by this request.
*/
void QLandmarkCategoryFetchByIdRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryIds.clear();
    d->categoryIds.append(categoryId);
}

/*!
    Returns the list of categories fetched by this request
*/
QList<QLandmarkCategory> QLandmarkCategoryFetchByIdRequest::categories() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->categories;
}

/*!
    Returns the mapping of input landmark id list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkCategoryFetchByIdRequest::errorMap() const
{
    Q_D(const QLandmarkCategoryFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkcategoryfetchbyidrequest.cpp"

QTM_END_NAMESPACE
