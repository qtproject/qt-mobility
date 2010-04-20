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

#include "qlandmarkcategoryremoverequest.h"
#include "qlandmarkcategoryid.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryRemoveRequest
    \brief The QLandmarkCategoryRemoveRequest class allows a client to asynchronously
    request that certain categories be removed from a landmark manager.

    For a QLandmarkCategoryRemoveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()) or an  overall operation error occurs(which may be
    retrieved by calling QLandmarkAbstractRequest::error()).

    \ingroup landmarks-request
*/

/*!
    Constructs a category remove request with the given \a parent.
*/
QLandmarkCategoryRemoveRequest::QLandmarkCategoryRemoveRequest(QObject *parent)
    : QLandmarkAbstractRequest(parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryRemoveRequest::~QLandmarkCategoryRemoveRequest()
{
}

/*!
    Returns the list of identifiers of categories which will be removed.
*/
QList<QLandmarkCategoryId> QLandmarkCategoryRemoveRequest::categoryIds() const
{
    return QList<QLandmarkCategoryId>();
}

/*!
    Sets the list of \a categoryIds of categories which will be removed.

    \sa setCategoryId()
*/
void QLandmarkCategoryRemoveRequest::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_UNUSED(categoryIds);
}

/*!
    Convenience function to set the \a categoryId of a sisngle category to be removed.

    \sa setCategoryIds()
*/
void QLandmarkCategoryRemoveRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_UNUSED(categoryId);
}

/*!
    Returns the mapping of input category id list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkCategoryRemoveRequest::errorMap() const
{
    return QMap<int, QLandmarkManager::Error>();
}

#include "moc_qlandmarkcategoryremoverequest.cpp"

QTM_END_NAMESPACE


